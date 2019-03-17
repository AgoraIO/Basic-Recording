{
    "targets": 
    [
        {
            "target_name":  "agorasdk",
            "sources":      [
                "src/node_uid.h",
                "src/node_uid.cpp",
                "src/node_napi_api.h",
                "src/node_napi_api.cpp",
                "src/node_async_queue.cpp",
                "src/node_async_queue.h",
                "src/recorder_manager.cc",
                "src/addon.cc",
            ],
            "include_dirs": [ "<!(node -e \"require('nan')\")", "src/sdk/include/" ],
            "libraries":    ["-Wl,-rpath,libagora-cloud-recording-client.so", "-lagora-cloud-recording-client", "-lpthread"],
            "cflags":       [ "-std=c++11" ]
        }
    ]
}
