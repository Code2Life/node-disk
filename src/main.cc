#include <uv.h>
#include <v8.h>
#include <node.h>
#include "node_disk.h"

#if defined _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

DiskInfo *disk = new DiskInfo();

struct async_req {
	uv_work_t req;
	int input;
	int output;
	v8::Isolate* isolate;
	v8::Persistent<v8::Function> callback;
	node::async_context context;
};

void DoAsync(uv_work_t* r) {
	//todo
	DISK_INFO *info = new DISK_INFO();
	disk->GetDiskInfo(*info);

	async_req* req = reinterpret_cast<async_req*>(r->data);
	// Simulate CPU intensive process...
#if defined _WIN32
	Sleep(1000);
#else
	sleep(1);
#endif
	req->output = req->input * 2;
}

void AfterAsync(uv_work_t* r) {
	async_req* req = reinterpret_cast<async_req*>(r->data);
	v8::Isolate* isolate = req->isolate;
	v8::HandleScope scope(isolate);

	v8::Local<v8::Value> argv[2] = {
		v8::Null(isolate),
		v8::Integer::New(isolate, req->output)
	};

	v8::TryCatch try_catch(isolate);

	v8::Local<v8::Object> global = isolate->GetCurrentContext()->Global();
	v8::Local<v8::Function> callback =
		v8::Local<v8::Function>::New(isolate, req->callback);


	// 也可以直接调用: callback->Call(global, 2, argv);
	
	v8::Local<v8::Value> ret =
		node::MakeCallback(isolate, global, callback, 2, argv, req->context)
		.ToLocalChecked();
	assert(!ret.IsEmpty());

	// cleanup
	node::EmitAsyncDestroy(isolate, req->context);
	req->callback.Reset();
	delete req;

	if (try_catch.HasCaught()) {
		node::FatalException(isolate, try_catch);
	}
}

void GetDiskInfo(const v8::FunctionCallbackInfo<v8::Value>& args) {
	v8::Isolate* isolate = args.GetIsolate();

	async_req* req = new async_req;
	req->req.data = req;

	req->input = args[0]->IntegerValue();
	req->output = 0;
	req->isolate = isolate;
	req->context = node::EmitAsyncInit(isolate, v8::Object::New(isolate), "test");

	v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[1]);
	req->callback.Reset(isolate, callback);

	uv_queue_work(uv_default_loop(),
		&req->req,
		DoAsync,
		(uv_after_work_cb)AfterAsync);
}

void init(v8::Local<v8::Object> exports) {
	NODE_SET_METHOD(exports, "getDiskInfo", GetDiskInfo);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, init)