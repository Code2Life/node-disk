{ 
  "targets": [ 
    { 
      "target_name": "node_disk", 
      "sources": [
          "src/node_disk.h",
          "src/main.cc"
      ],
	  "cflags" : [
        "-std=c++11",
        "-D_GLIBCXX_USE_CXX11_ABI=0"
      ],
	  "conditions": [
        ["OS=='win'", {
          "sources": [
            "src/node_disk_win.cc",
          ],
          "defines": [
            "UNICODE",
            "WIN32_LEAN_AND_MEAN",
            "NOMINMAX"
          ],
          "msvs_settings": {
            "VCCLCompilerTool": {
              "ExceptionHandling": "1"
            }
          }
        }],
        ["OS not in ['win']", {
          "sources": [
            "src/node_disk_posix.cc"
          ],
          "cflags!": [ "-fno-exceptions" ],
          "cflags_cc!": [ "-fno-exceptions" ]
        }],
        ["OS=='mac'", {
          "xcode_settings": {
            "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
          }
        }]
      ]
    }
  ]
}