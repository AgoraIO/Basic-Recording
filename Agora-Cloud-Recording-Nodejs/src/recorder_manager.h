#pragma once // NOLINT(build/header_guard)

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/event.h>

#include <map>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#include <unordered_map>
#include <uuid/uuid.h>
#include <nan.h>
#include <node.h>
#include <iostream>
#include <node_object_wrap.h>
#include "IAgoraCloudRecording.h"

using std::cout;
using std::endl;
using v8::Function;
using v8::Local;
using v8::Object;
using v8::Persistent;

namespace agora
{
namespace cloud_recording
{
class RecorderManager : public Nan::ObjectWrap,
                        public ICloudRecorderObserver
{
#define RECORD_CONNECTING "RecordConnecting"
#define RECORD_STARTED "RecordStarted"
#define RECORD_STOPPED "RecordStopped"
#define RECORD_UPLOADED "RecordUploaded"
#define RECORD_BACKEDUP "RecordBackedUp"
#define RECORD_UPDATE_PROGRESS "RecordUpdateProgress"
#define RECORD_FAILURE "RecordFailure"
#define UPLOAD_FAILURE "UploadFailure"
#define RECORD_FATAL_ERROR "RecordFatalError"
  public:
    struct NodeEventCallback
    {
        Persistent<Function> callback;
        Persistent<Object> js_this;
    };
    void addEventHandler(const std::string &eventName, Persistent<Object> &obj, Persistent<Function> &callback);
    static void Init(v8::Local<v8::Object> exports);
    static void New(const Nan::FunctionCallbackInfo<v8::Value> &info);
    static void StartCloudRecording(
        const Nan::FunctionCallbackInfo<v8::Value> &args);
    static void onEvent(const Nan::FunctionCallbackInfo<v8::Value> &args);

    void OnRecordingConnecting(RecordingId recording_id);
    void OnRecordingStarted(RecordingId recording_id);
    void OnRecordingStopped(RecordingId recording_id);
    void OnRecordingUploaded(RecordingId recording_id,
        const char* file_name);
    void OnRecordingBackedUp(RecordingId recording_id,
        const char* file_name);
    void OnRecordingUploadingProgress(RecordingId recording_id,
        unsigned int progress, const char* recording_playlist_filename);
    void OnRecorderFailure(RecordingId recording_id,
        RecordingErrorCode code, const char* msg);
    void OnUploaderFailure(RecordingId recording_id,
        RecordingErrorCode code, const char* msg);
    void OnRecordingFatalError(RecordingId recording_id,
        RecordingErrorCode code);

  private:
    RecorderManager();
    virtual ~RecorderManager();

  public:
  private:
    ICloudRecorder *controller_;
    std::unordered_map<std::string, NodeEventCallback *> m_callbacks;
};
/*
  * to return int value for JS call.
  */
#define napi_set_int_result(args, result) (args).GetReturnValue().Set(Integer::New(args.GetIsolate(), (result)))

/**
  * to return bool value for JS call
  */
#define napi_set_bool_result(args, result) (args).GetReturnValue().Set(v8::Boolean::New(args.GetIsolate(), (result)))

/*
  * to return string value for JS call
  */
#define napi_set_string_result(args, data)                                                                          \
    Local<Value> tmp = String::NewFromUtf8(args.GetIsolate(), data, NewStringType::kInternalized).ToLocalChecked(); \
    args.GetReturnValue().Set(tmp);

} // namespace cloud_recording
} // namespace agora
