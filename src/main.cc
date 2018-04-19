#include <uv.h>
#include <v8.h>
#include <node.h>
#include "node_disk.h"

#if defined _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

using namespace v8;

struct async_req {
	uv_work_t req;
	string input;
	DISK_INFO* output;
	Isolate* isolate;
	Persistent<Function> callback;
};

void DoAsync(uv_work_t* r) {
	async_req* req = reinterpret_cast<async_req*>(r->data);
	DISK_INFO *info = new DISK_INFO();
	bool success = DoGetDiskInfo(*info, req->input);
	req->output = info;
}

void AfterAsync(uv_work_t* r) {
	async_req* req = reinterpret_cast<async_req*>(r->data);
	Isolate* isolate = req->isolate;
	HandleScope scope(isolate);

	Local<Object> result = Object::New(req->isolate);
	
	DISK_INFO* info = req->output;

	int i = 0;
	Local<Array> volumes = Array::New(req->isolate);
	for (auto it = info->partition.begin(); it != info->partition.end(); it++) {
		Local<Object> vol_detail = Object::New(req->isolate);
		vol_detail->Set(String::NewFromUtf8(req->isolate, "name"), String::NewFromUtf8(req->isolate, it->volume.c_str()));
		vol_detail->Set(String::NewFromUtf8(req->isolate, "total"), String::NewFromUtf8(req->isolate, it->totalSize.c_str()));
		vol_detail->Set(String::NewFromUtf8(req->isolate, "free"), String::NewFromUtf8(req->isolate, it->freeSize.c_str()));
		volumes->Set(i++, vol_detail);
	}

	result->Set(String::NewFromUtf8(req->isolate, "total"), String::NewFromUtf8(req->isolate, info->totalSize.c_str()));
	result->Set(String::NewFromUtf8(req->isolate, "free"), String::NewFromUtf8(req->isolate, info->freeSize.c_str()));
	result->Set(String::NewFromUtf8(req->isolate, "volumes"), volumes);
	
	Local<Value> argv[2] = { Null(isolate), result };

	TryCatch try_catch(isolate);

	Local<Object> global = isolate->GetCurrentContext()->Global();
	Local<Function> callback = Local<Function>::New(isolate, req->callback);

	callback->Call(global, 2, argv);
	req->callback.Reset();
	delete req;

	if (try_catch.HasCaught()) {
		node::FatalException(isolate, try_catch);
	}
}

void GetDiskInfo(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();

	async_req* req = new async_req;
	req->req.data = req;

	if (args.Length() != 2 || !args[0]->IsString() || !args[1]->IsFunction()) {
		node::ErrnoException(isolate, NULL, "Parameter error", NULL);
		return;
	}

	String::Utf8Value param(args[0]->ToString());
	req->input = std::string(*param);
	req->isolate = isolate;

	Local<Function> callback = Local<Function>::Cast(args[1]);
	req->callback.Reset(isolate, callback);

	uv_queue_work(uv_default_loop(),
		&req->req,
		DoAsync,
		(uv_after_work_cb)AfterAsync);
	args.GetReturnValue().Set(Boolean::New(isolate, true));
}

void init(Local<Object> exports) {
	NODE_SET_METHOD(exports, "getDiskInfo", GetDiskInfo);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init)