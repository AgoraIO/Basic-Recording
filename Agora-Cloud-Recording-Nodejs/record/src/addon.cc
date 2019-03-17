#include <nan.h>
#include "recorder_manager.h"

using namespace v8;
using namespace agora::cloud_recording;

void InitAll(Local<Object> exports){
  RecorderManager::Init(exports);
}

NODE_MODULE(addon, InitAll)
