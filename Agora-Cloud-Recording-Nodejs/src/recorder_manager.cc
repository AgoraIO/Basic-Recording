#include "recorder_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <utility>
#include <sstream>
#include "node_napi_api.h"
#include "node_async_queue.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

uv_async_t async;

static Nan::Persistent<v8::Function> constructor;

using namespace v8;

#define MAKE_JS_CALL_0(ev)                                                   \
    auto it = m_callbacks.find(ev);                                          \
    if (it != m_callbacks.end())                                             \
    {                                                                        \
        Isolate *isolate = Isolate::GetCurrent();                            \
        HandleScope scope(isolate);                                          \
        NodeEventCallback &cb = *it->second;                                 \
        cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 0, nullptr); \
    }

#define MAKE_JS_CALL_1(ev, type, param)                                   \
    auto it = m_callbacks.find(ev);                                       \
    if (it != m_callbacks.end())                                          \
    {                                                                     \
        Isolate *isolate = Isolate::GetCurrent();                         \
        HandleScope scope(isolate);                                       \
        Local<Value> argv[1]{napi_create_##type##_(isolate, param)};      \
        NodeEventCallback &cb = *it->second;                              \
        cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 1, argv); \
    }

#define MAKE_JS_CALL_2(ev, type1, param1, type2, param2)                  \
    auto it = m_callbacks.find(ev);                                       \
    if (it != m_callbacks.end())                                          \
    {                                                                     \
        Isolate *isolate = Isolate::GetCurrent();                         \
        HandleScope scope(isolate);                                       \
        Local<Value> argv[2]{napi_create_##type1##_(isolate, param1),     \
                             napi_create_##type2##_(isolate, param2)};    \
        NodeEventCallback &cb = *it->second;                              \
        cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 2, argv); \
    }

#define MAKE_JS_CALL_3(ev, type1, param1, type2, param2, type3, param3)   \
    auto it = m_callbacks.find(ev);                                       \
    if (it != m_callbacks.end())                                          \
    {                                                                     \
        Isolate *isolate = Isolate::GetCurrent();                         \
        HandleScope scope(isolate);                                       \
        Local<Value> argv[3]{napi_create_##type1##_(isolate, param1),     \
                             napi_create_##type2##_(isolate, param2),     \
                             napi_create_##type3##_(isolate, param3)};    \
        NodeEventCallback &cb = *it->second;                              \
        cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 3, argv); \
    }

#define MAKE_JS_CALL_4(ev, type1, param1, type2, param2, type3, param3, type4, param4) \
    auto it = m_callbacks.find(ev);                                                    \
    if (it != m_callbacks.end())                                                       \
    {                                                                                  \
        Isolate *isolate = Isolate::GetCurrent();                                      \
        HandleScope scope(isolate);                                                    \
        Local<Value> argv[4]{                                                          \
            napi_create_##type1##_(isolate, param1),                                   \
            napi_create_##type2##_(isolate, param2),                                   \
            napi_create_##type3##_(isolate, param3),                                   \
            napi_create_##type4##_(isolate, param4),                                   \
        };                                                                             \
        NodeEventCallback &cb = *it->second;                                           \
        cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 4, argv);              \
    }

#define MAKE_JS_CALL_5(ev, type1, param1, type2, param2, type3, param3, type4, param4, type5, param5) \
    auto it = m_callbacks.find(ev);                                                                   \
    if (it != m_callbacks.end())                                                                      \
    {                                                                                                 \
        Isolate *isolate = Isolate::GetCurrent();                                                     \
        HandleScope scope(isolate);                                                                   \
        Local<Value> argv[5]{                                                                         \
            napi_create_##type1##_(isolate, param1),                                                  \
            napi_create_##type2##_(isolate, param2),                                                  \
            napi_create_##type3##_(isolate, param3),                                                  \
            napi_create_##type4##_(isolate, param4),                                                  \
            napi_create_##type5##_(isolate, param5),                                                  \
        };                                                                                            \
        NodeEventCallback &cb = *it->second;                                                          \
        cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 5, argv);                             \
    }

#define MAKE_JS_CALL_6(ev, type1, param1, type2, param2, type3, param3, type4, param4, type5, param5, type6, param6) \
    auto it = m_callbacks.find(ev);                                                                                  \
    if (it != m_callbacks.end())                                                                                     \
    {                                                                                                                \
        Isolate *isolate = Isolate::GetCurrent();                                                                    \
        HandleScope scope(isolate);                                                                                  \
        Local<Value> argv[6]{                                                                                        \
            napi_create_##type1##_(isolate, param1),                                                                 \
            napi_create_##type2##_(isolate, param2),                                                                 \
            napi_create_##type3##_(isolate, param3),                                                                 \
            napi_create_##type4##_(isolate, param4),                                                                 \
            napi_create_##type5##_(isolate, param5),                                                                 \
            napi_create_##type6##_(isolate, param6),                                                                 \
        };                                                                                                           \
        NodeEventCallback &cb = *it->second;                                                                         \
        cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 6, argv);                                            \
    }

#define COMMAND_JS_CALLBACK_SUCCESS(cid)                                       \
    auto it = m_commands.find(cid);                                            \
    if (it != m_commands.end())                                                \
    {                                                                          \
        Isolate *isolate = Isolate::GetCurrent();                              \
        HandleScope scope(isolate);                                            \
        CommandPrivData &pdata = *it->second;                                  \
        NodeEventCallback *cb = pdata.success;                                 \
        cb->callback.Get(isolate)->Call(cb->js_this.Get(isolate), 0, nullptr); \
    }

#define COMMAND_JS_CALLBACK_FAIL(cid, type1, param1, type2, param2)         \
    auto it = m_commands.find(cid);                                         \
    if (it != m_commands.end())                                             \
    {                                                                       \
        Isolate *isolate = Isolate::GetCurrent();                           \
        HandleScope scope(isolate);                                         \
        Local<Value> argv[2]{napi_create_##type1##_(isolate, param1),       \
                             napi_create_##type2##_(isolate, param2)};      \
        CommandPrivData &pdata = *it->second;                               \
        NodeEventCallback *cb = pdata.fail;                                 \
        cb->callback.Get(isolate)->Call(cb->js_this.Get(isolate), 2, argv); \
    }

namespace agora
{
namespace cloud_recording
{
void RecorderManager::Init(Local<Object> exports)
{
    Nan::HandleScope scope;

    // Prepare constructor template
    v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
    tpl->SetClassName(Nan::New("RecorderManager").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    Nan::SetPrototypeMethod(tpl, "StartCloudRecording", StartCloudRecording);
    Nan::SetPrototypeMethod(tpl, "StopCloudRecording", StopCloudRecording);
    Nan::SetPrototypeMethod(tpl, "onEvent", onEvent);
    Nan::SetPrototypeMethod(tpl, "Release", Release);

    constructor.Reset(tpl->GetFunction());
    exports->Set(Nan::New("RecorderManager").ToLocalChecked(), tpl->GetFunction());
}

void RecorderManager::New(const Nan::FunctionCallbackInfo<Value> &args)
{
    RecorderManager *obj = new RecorderManager();
    obj->Wrap(args.This());
    args.GetReturnValue().Set(args.This());
}

void RecorderManager::StopCloudRecording(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
    RecorderManager *instance = ObjectWrap::Unwrap<RecorderManager>(args.Holder());
    bool result = instance->controller_->StopCloudRecording();
    napi_set_int_result(args, result);
}

void RecorderManager::Release(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
    RecorderManager *instance = ObjectWrap::Unwrap<RecorderManager>(args.Holder());
    bool keepRecordingInBackground = false;
    napi_get_value_bool_(args[0], keepRecordingInBackground);
    instance->controller_->Release(keepRecordingInBackground);
    napi_set_int_result(args, true);
}

void RecorderManager::StartCloudRecording(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
    do
    {
        Isolate *isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);

        NodeString s_appid, s_cname, s_token;
        RecordingConfig recordConfig;
        CloudStorageConfig storageConfig;
        uid_t uid;
        napi_status status = napi_get_value_nodestring_(args[0], s_appid);
        CHECK_NAPI_STATUS(status);
        status = napi_get_value_nodestring_(args[1], s_cname);
        CHECK_NAPI_STATUS(status);
        status = napi_get_value_nodestring_(args[2], s_token);
        CHECK_NAPI_STATUS(status);
        status = napi_get_value_uint32_(args[3], uid);
        CHECK_NAPI_STATUS(status);

        NodeString s_secret;
        Local<Object> recordConfigJson = args[4]->ToObject(isolate);
        uint32_t recording_stream_types, decryption_mode, channel_type, audio_profile, video_stream_type;
        napi_get_object_property_uint32_(isolate, recordConfigJson, "recording_stream_types", recording_stream_types);
        switch (recording_stream_types)
        {
        case 1:
            recordConfig.recording_stream_types = kStreamTypeAudioOnly;
            break;
        case 2:
            recordConfig.recording_stream_types = kStreamTypeVideoOnly;
            break;
        default:
            recordConfig.recording_stream_types = kStreamTypeAudioVideo;
        }
        napi_get_object_property_uint32_(isolate, recordConfigJson, "decryption_mode", decryption_mode);
        switch (decryption_mode)
        {
        case 1:
            recordConfig.decryption_mode = kModeAes128Xts;
            break;
        case 2:
            recordConfig.decryption_mode = kModeAes128Ecb;
            break;
        case 3:
            recordConfig.decryption_mode = kModeAes256Xts;
            break;
        default:
            recordConfig.decryption_mode = kModeNone;
        }
        napi_get_object_property_nodestring_(isolate, recordConfigJson, "secret", s_secret);
        if (s_secret != nullptr)
        {
            string secret = (string)s_secret;
            recordConfig.secret = const_cast<char *>(secret.c_str());
        }
        napi_get_object_property_uint32_(isolate, recordConfigJson, "channel_type", channel_type);
        switch (channel_type)
        {
        case 0:
            recordConfig.channel_type = kChannelTypeCommunication;
            break;
        case 1:
            recordConfig.channel_type = kChannelTypeLive;
            break;
        }
        napi_get_object_property_uint32_(isolate, recordConfigJson, "video_stream_type", video_stream_type);
        switch (video_stream_type)
        {
        case 1:
            recordConfig.video_stream_type = kVideoStreamTypeLow;
            break;
        default:
            recordConfig.video_stream_type = kVideoStreamTypeHigh;
        }
        napi_get_object_property_uint32_(isolate, recordConfigJson, "audio_profile", audio_profile);
        switch (audio_profile)
        {
        case 1:
            recordConfig.audio_profile = kAudioProfileMusicHighQuality;
            break;
        case 2:
            recordConfig.audio_profile = kAudioProfileMusicHighQualityStereo;
            break;
        default:
            recordConfig.audio_profile = kAudioProfileMusicStandard;
        }
        napi_get_object_property_uint32_(isolate, recordConfigJson, "max_idle_time", recordConfig.max_idle_time);

        Local<Name> keyName = String::NewFromUtf8(args.GetIsolate(), "transcoding_config", NewStringType::kInternalized).ToLocalChecked();
        Local<Value> keyValue = recordConfigJson->Get(args.GetIsolate()->GetCurrentContext(), keyName).ToLocalChecked();
        if (keyValue->IsObject())
        {
            TranscodingConfig transcodingConfig;
            Local<Object> transcodingConfigJson = keyValue->ToObject(isolate);
            uint32_t layout;
            napi_get_object_property_uint32_(isolate, transcodingConfigJson, "width", transcodingConfig.width);
            napi_get_object_property_uint32_(isolate, transcodingConfigJson, "height", transcodingConfig.height);
            napi_get_object_property_uint32_(isolate, transcodingConfigJson, "fps", transcodingConfig.fps);
            napi_get_object_property_uint32_(isolate, transcodingConfigJson, "bitrate", transcodingConfig.bitrate);
            napi_get_object_property_uint32_(isolate, transcodingConfigJson, "max_resolution_uid", transcodingConfig.max_resolution_uid);

            napi_get_object_property_uint32_(isolate, transcodingConfigJson, "layout", layout);
            switch (layout)
            {
            case 1:
                transcodingConfig.layout = kMixedVideoLayoutTypeBestFit;
                break;
            case 2:
                transcodingConfig.layout = kMixedVideolayoutTypeVertical;
                break;
            default:
                transcodingConfig.layout = kMixedVideoLayoutTypeFloat;
            }
            recordConfig.transcoding_config = transcodingConfig;
        }

        NodeString s_bucket, s_access_key, s_secret_key;
        Local<Object> storageConfigJson = args[5]->ToObject(isolate);
        uint32_t vendor;
        napi_get_object_property_uint32_(isolate, storageConfigJson, "vendor", vendor);
        switch (vendor)
        {
        case 0:
            storageConfig.vendor = kCloudStorageVendorQiniu;
            break;
        case 1:
            storageConfig.vendor = kCloudStorageVendorAws;
            break;
        }

        napi_get_object_property_nodestring_(isolate, storageConfigJson, "access_key", s_access_key);
        string access_key = (string)s_access_key;
        storageConfig.access_key = const_cast<char *>(access_key.c_str());

        napi_get_object_property_nodestring_(isolate, storageConfigJson, "secret_key", s_secret_key);
        string secret_key = (string)s_secret_key;
        storageConfig.secret_key = const_cast<char *>(secret_key.c_str());

        napi_get_object_property_nodestring_(isolate, storageConfigJson, "bucket", s_bucket);
        string bucket = (string)s_bucket;
        storageConfig.bucket = const_cast<char *>(bucket.c_str());

        napi_get_object_property_uint32_(isolate, storageConfigJson, "region", storageConfig.region);

        string appid = (string)s_appid;
        string cname = (string)s_cname;
        string token;
        if (s_token != nullptr)
        {
            token = (string)s_token;
        }
        else
        {
            token = string("");
        }

        RecorderManager *instance = ObjectWrap::Unwrap<RecorderManager>(args.Holder());
        bool result = instance->controller_->StartCloudRecording(appid.c_str(), cname.c_str(), token.c_str(), uid, recordConfig, storageConfig);
        napi_set_int_result(args, result);
    } while (false);
}

void RecorderManager::onEvent(const Nan::FunctionCallbackInfo<v8::Value> &args)
{
    do
    {
        RecorderManager *instance = ObjectWrap::Unwrap<RecorderManager>(args.Holder());
        NodeString eventName;
        napi_status status = napi_get_value_nodestring_(args[0], eventName);
        // std::cout << eventName << std::endl;
        if (!args[1]->IsFunction())
        {
            cout << "Function expected" << endl;
            break;
        }

        Local<Function> callback = args[1].As<Function>();

        if (callback.IsEmpty())
        {
            cout << "Function expected" << endl;
            break;
        }

        Persistent<Function> persist;
        persist.Reset(args.GetIsolate(), callback);
        Local<Object> obj = args.This();
        Persistent<Object> persistObj;
        persistObj.Reset(args.GetIsolate(), obj);
        instance->addEventHandler((char *)eventName, persistObj, persist);
    } while (false);
}

void RecorderManager::addEventHandler(const std::string &eventName, Persistent<Object> &obj, Persistent<Function> &callback)
{
    NodeEventCallback *cb = new NodeEventCallback();
    cb->js_this.Reset(Isolate::GetCurrent(), obj);
    cb->callback.Reset(Isolate::GetCurrent(), callback);
    m_callbacks.emplace(eventName, cb);
}

RecorderManager::RecorderManager()
{

    controller_ = CreateCloudRecorderInstance(this);
}

RecorderManager::~RecorderManager()
{
    // LbecQuit();
}

void RecorderManager::OnRecordingConnecting(RecordingId recording_id)
{
    agora::lb_linux_sdk::node_async_call::async_call([this, recording_id]() {
        MAKE_JS_CALL_1(RECORD_CONNECTING, string, recording_id);
    });
}

void RecorderManager::OnRecordingStarted(RecordingId recording_id)
{
    agora::lb_linux_sdk::node_async_call::async_call([this, recording_id]() {
        MAKE_JS_CALL_1(RECORD_STARTED, string, recording_id);
    });
}

void RecorderManager::OnRecordingStopped(RecordingId recording_id)
{
    agora::lb_linux_sdk::node_async_call::async_call([this, recording_id]() {
        MAKE_JS_CALL_1(RECORD_STOPPED, string, recording_id);
    });
}

void RecorderManager::OnRecordingUploaded(RecordingId recording_id,
                                          const char *file_name)
{
    agora::lb_linux_sdk::node_async_call::async_call([this, recording_id, file_name]() {
        MAKE_JS_CALL_2(RECORD_UPLOADED, string, recording_id, string, file_name);
    });
}

void RecorderManager::OnRecordingBackedUp(RecordingId recording_id,
                                          const char *file_name)
{
    agora::lb_linux_sdk::node_async_call::async_call([this, recording_id, file_name]() {
        MAKE_JS_CALL_2(RECORD_BACKEDUP, string, recording_id, string, file_name);
    });
}

void RecorderManager::OnRecordingUploadingProgress(RecordingId recording_id,
                                                   unsigned int progress, const char *recording_playlist_filename)
{
    agora::lb_linux_sdk::node_async_call::async_call([this, recording_id, progress, recording_playlist_filename]() {
        MAKE_JS_CALL_3(RECORD_UPDATE_PROGRESS, string, recording_id, uint32, progress, string, recording_playlist_filename);
    });
}

void RecorderManager::OnRecorderFailure(RecordingId recording_id,
                                        RecordingErrorCode code, const char *msg)
{
    agora::lb_linux_sdk::node_async_call::async_call([this, recording_id, code, msg]() {
        MAKE_JS_CALL_3(RECORD_FAILURE, string, recording_id, uint32, code, string, msg);
    });
}

void RecorderManager::OnUploaderFailure(RecordingId recording_id,
                                        RecordingErrorCode code, const char *msg)
{
    agora::lb_linux_sdk::node_async_call::async_call([this, recording_id, code, msg]() {
        MAKE_JS_CALL_3(UPLOAD_FAILURE, string, recording_id, uint32, code, string, msg);
    });
}

void RecorderManager::OnRecordingFatalError(RecordingId recording_id,
                                            RecordingErrorCode code)
{
    agora::lb_linux_sdk::node_async_call::async_call([this, recording_id, code]() {
        MAKE_JS_CALL_2(RECORD_FATAL_ERROR, string, recording_id, uint32, code);
    });
}

} // namespace cloud_recording
} // namespace agora