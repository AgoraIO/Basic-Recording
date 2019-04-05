#include <csignal>
#include <cstdint>
#include <iostream>
#include <sstream> 
#include <string>
#include <vector>
#include <algorithm>

#include "IAgoraLinuxSdkCommon.h"
#include "IAgoraRecordingEngine.h"
#include "AgoraSdk.h"
#include "node_async_queue.h"

#include "base/atomic.h"
#include "base/log.h" 
#include "opt_parser.h" 
namespace agora {

#define MAKE_JS_CALL_0(ev) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 0, nullptr);\
        }

#define MAKE_JS_CALL_1(ev, type, param) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            Local<Value> argv[1]{ napi_create_##type##_(isolate, param)\
                                };\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 1, argv);\
        }

#define MAKE_JS_CALL_2(ev, type1, param1, type2, param2) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            Local<Value> argv[2]{ napi_create_##type1##_(isolate, param1),\
                                  napi_create_##type2##_(isolate, param2)\
                                };\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 2, argv);\
        }

#define MAKE_JS_CALL_3(ev, type1, param1, type2, param2, type3, param3) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            Local<Value> argv[3]{ napi_create_##type1##_(isolate, param1),\
                                  napi_create_##type2##_(isolate, param2),\
                                  napi_create_##type3##_(isolate, param3) \
                                };\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 3, argv);\
        }

#define MAKE_JS_CALL_4(ev, type1, param1, type2, param2, type3, param3, type4, param4) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            Local<Value> argv[4]{ napi_create_##type1##_(isolate, param1),\
                                  napi_create_##type2##_(isolate, param2),\
                                  napi_create_##type3##_(isolate, param3), \
                                  napi_create_##type4##_(isolate, param4), \
                                };\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 4, argv);\
        }

#define MAKE_JS_CALL_5(ev, type1, param1, type2, param2, type3, param3, type4, param4, type5, param5) \
        auto it = m_callbacks.find(ev); \
        if (it != m_callbacks.end()) {\
            Isolate *isolate = Isolate::GetCurrent();\
            HandleScope scope(isolate);\
            Local<Value> argv[5]{ napi_create_##type1##_(isolate, param1),\
                                  napi_create_##type2##_(isolate, param2),\
                                  napi_create_##type3##_(isolate, param3), \
                                  napi_create_##type4##_(isolate, param4), \
                                  napi_create_##type5##_(isolate, param5), \
                                };\
            NodeEventCallback& cb = *it->second;\
            cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 5, argv);\
        }

AgoraSdk::AgoraSdk(): IRecordingEngineEventHandler() {
  m_engine = NULL;
  m_stopped.store(false);
  m_storage_dir = "./";
}

AgoraSdk::~AgoraSdk() {
  if (m_engine) {
    m_engine->leaveChannel();
    m_engine->release();
  }
}

bool AgoraSdk::stopped() const {
  return m_stopped;
}

bool AgoraSdk::release() {
  if (m_engine) {
    m_engine->release();
    m_engine = NULL;
  }

  return true;
}

bool AgoraSdk::createChannel(const string &appid, const string &channelKey, const string &name,
    uint32_t uid, 
    agora::recording::RecordingConfig &config) 
{
    if ((m_engine = agora::recording::IRecordingEngine::createAgoraRecordingEngine(appid.c_str(), this)) == NULL)
        return false;

    if(linuxsdk::ERR_OK != m_engine->joinChannel(channelKey.c_str(), name.c_str(), uid, config))
        return false;

    m_config = config;
    return true;
}

bool AgoraSdk::leaveChannel() {
  if (m_engine) {
    m_engine->leaveChannel();
    m_stopped = true;
  }

  return true;
}

int AgoraSdk::startService() {
  if (m_engine) 
    return m_engine->startService();

  return 1;
}

int AgoraSdk::stopService() {
  if (m_engine) 
    return m_engine->stopService();

  return 1;
}

void AgoraSdk::updateMixLayout(agora::linuxsdk::VideoMixingLayout &layout) {
    m_layout = layout;
    setVideoMixLayout();
}

agora::linuxsdk::VideoMixingLayout AgoraSdk::getMixLayout() {
    return m_layout;
}

//Customize the layout of video under video mixing model
int AgoraSdk::setVideoMixLayout()
{
    // recording::RecordingConfig *pConfig = getConfigInfo();
    // size_t max_peers = pConfig->channelProfile == linuxsdk::CHANNEL_PROFILE_COMMUNICATION ? 7:17;
    if(m_layout.regionCount == 0) return 0;

    cout << "color: " << m_layout.backgroundColor << endl;
    return setVideoMixingLayout(m_layout);
}

int AgoraSdk::setVideoMixingLayout(const agora::linuxsdk::VideoMixingLayout &layout)
{
   int result = -agora::linuxsdk::ERR_INTERNAL_FAILED;
   if(m_engine)
      result = m_engine->setVideoMixingLayout(layout);
   return result;
}

const agora::recording::RecordingEngineProperties* AgoraSdk::getRecorderProperties(){
    return m_engine->getProperties();
}

void AgoraSdk::onErrorImpl(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code) {
    cerr << "Error: " << error <<",with stat_code:"<< stat_code << endl;
    m_engine->stoppedOnError();
    agora::recording::node_async_call::async_call([this, error, stat_code]() {
        MAKE_JS_CALL_2(REC_EVENT_ERROR, int32, error, int32, stat_code);
    });
}

void AgoraSdk::onWarningImpl(int warn) {
    cerr << "warn: " << warn << endl;
    //  leaveChannel();
}

void AgoraSdk::onJoinChannelSuccessImpl(const char * channelId, agora::linuxsdk::uid_t uid) {
    cout << "join channel Id: " << channelId << ", with uid: " << uid << endl;
    string channelName = channelId;
    agora::recording::node_async_call::async_call([this, channelName, uid]() {
        MAKE_JS_CALL_2(REC_EVENT_JOIN_CHANNEL, string, channelName.c_str(), uid, uid);
    });
}

void AgoraSdk::onLeaveChannelImpl(agora::linuxsdk::LEAVE_PATH_CODE code) {
    cout << "leave channel with code:" << code << endl;
    agora::recording::node_async_call::async_call([this]() {
        MAKE_JS_CALL_0(REC_EVENT_LEAVE_CHANNEL);
    });
}

void AgoraSdk::onUserJoinedImpl(unsigned uid, agora::linuxsdk::UserJoinInfos &infos) {
    cout << "User " << uid << " joined, RecordingDir:" << (infos.storageDir? infos.storageDir:"NULL") <<endl;
    if(infos.storageDir)
    {
        m_storage_dir = std::string(infos.storageDir);
        m_logdir = m_storage_dir;
    }

    m_peers.push_back(uid);
    
    //When the user joined, we can re-layout the canvas
    setVideoMixLayout();

    agora::recording::node_async_call::async_call([this, uid]() {
        MAKE_JS_CALL_1(REC_EVENT_USER_JOIN, uid, uid);
    });
}


void AgoraSdk::onUserOfflineImpl(unsigned uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason) {
    cout << "User " << uid << " offline, reason: " << reason << endl;
    m_peers.erase(std::remove(m_peers.begin(), m_peers.end(), uid), m_peers.end());
    
    //When the user is offline, we can re-layout the canvas
    setVideoMixLayout();

    agora::recording::node_async_call::async_call([this, uid]() {
        MAKE_JS_CALL_1(REC_EVENT_USER_LEAVE, uid, uid);
    });
}

void AgoraSdk::onActiveSpeaker(uid_t uid) {
    agora::recording::node_async_call::async_call([this, uid]() {
        MAKE_JS_CALL_1(REC_EVENT_ACTIVE_SPEAKER, uid, uid);
    });
}

void AgoraSdk::audioFrameReceivedImpl(unsigned int uid, const agora::linuxsdk::AudioFrame *pframe) const 
{
  char uidbuf[65];
  snprintf(uidbuf, sizeof(uidbuf),"%u", uid);
  std::string info_name = m_storage_dir + std::string(uidbuf) /*+ timestamp_per_join_*/;

  const uint8_t* data = NULL; 
  uint32_t size = 0;
  if (pframe->type == agora::linuxsdk::AUDIO_FRAME_RAW_PCM) {
    info_name += ".pcm";  

    agora::linuxsdk::AudioPcmFrame *f = pframe->frame.pcm;
    data = f->pcmBuf_;
    size = f->pcmBufSize_;

    cout << "User " << uid << ", received a raw PCM frame ,channels:" << f->channels_ <<endl;

  } else if (pframe->type == agora::linuxsdk::AUDIO_FRAME_AAC) {
    info_name += ".aac";

    cout << "User " << uid << ", received an AAC frame" << endl;

    agora::linuxsdk::AudioAacFrame *f = pframe->frame.aac;
    data = f->aacBuf_;
    size = f->aacBufSize_;
  }

  FILE *fp = fopen(info_name.c_str(), "a+b");
  if(fp == NULL) {
      cout << "failed to open: " << info_name;
      cout<< " ";
      cout << "errno: " << errno;
      cout<< endl;
      return;
  }

  ::fwrite(data, 1, size, fp);
  ::fclose(fp);
}

void AgoraSdk::videoFrameReceivedImpl(unsigned int uid, const agora::linuxsdk::VideoFrame *pframe) const {
  char uidbuf[65];
  snprintf(uidbuf, sizeof(uidbuf),"%u", uid);
  const char * suffix=".vtmp";
  if (pframe->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) {
    agora::linuxsdk::VideoYuvFrame *f = pframe->frame.yuv;
    suffix=".yuv";

    cout << "User " << uid << ", received a yuv frame, width: "
        << f->width_ << ", height: " << f->height_ ;
    cout<<",ystride:"<<f->ystride_<< ",ustride:"<<f->ustride_<<",vstride:"<<f->vstride_;
    cout<< endl;
  } else if(pframe->type == agora::linuxsdk::VIDEO_FRAME_JPG) {
    suffix=".jpg";
    agora::linuxsdk::VideoJpgFrame *f = pframe->frame.jpg;

    cout << "User " << uid << ", received an jpg frame, timestamp: "
    << f->frame_ms_ << endl;

    struct tm date;
    time_t t = time(NULL);
    localtime_r(&t, &date);
    char timebuf[128];
    sprintf(timebuf, "%04d%02d%02d%02d%02d%02d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
    std::string file_name = m_storage_dir + std::string(uidbuf) + "_" + std::string(timebuf) + suffix;
    FILE *fp = fopen(file_name.c_str(), "w");
    if(fp == NULL) {
        cout << "failed to open: " << file_name;
        cout<< " ";
        cout << "errno: " << errno;
        cout<< endl;
        return;
    }

    ::fwrite(f->buf_, 1, f->bufSize_, fp);
    ::fclose(fp);
    return;
  } else {
    suffix=".h264";
    agora::linuxsdk::VideoH264Frame *f = pframe->frame.h264;

    cout << "User " << uid << ", received an h264 frame, timestamp: "
        << f->frame_ms_ << ", frame no: " << f->frame_num_ << endl;
  }

  std::string info_name = m_storage_dir + std::string(uidbuf) /*+ timestamp_per_join_ */+ std::string(suffix);
  FILE *fp = fopen(info_name.c_str(), "a+b");
  if(fp == NULL) {
        cout << "failed to open: " << info_name;
        cout<< " ";
        cout << "errno: " << errno;
        cout<< endl;
        return;
  }


  //store it as file
  if (pframe->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) {
      agora::linuxsdk::VideoYuvFrame *f = pframe->frame.yuv;
      ::fwrite(f->buf_, 1, f->bufSize_, fp);
  }
  else {
      agora::linuxsdk::VideoH264Frame *f = pframe->frame.h264;
      ::fwrite(f->buf_, 1, f->bufSize_, fp);
  }
  ::fclose(fp);

}

void AgoraSdk::addEventHandler(const std::string& eventName, Persistent<Object>& obj, Persistent<Function>& callback)
{
    NodeEventCallback *cb = new NodeEventCallback();;
    cb->js_this.Reset(Isolate::GetCurrent(), obj);
    cb->callback.Reset(Isolate::GetCurrent(), callback);
    m_callbacks.emplace(eventName, cb);
}

void AgoraSdk::emitError(int err, int stat_code) {
    agora::recording::node_async_call::async_call([this, err, stat_code]() {
        MAKE_JS_CALL_2(REC_EVENT_ERROR, int32, err, int32, stat_code);
    });
}

//added 2.3.3
void AgoraSdk::onAudioVolumeIndication_node(const agora::linuxsdk::AudioVolumeInfo* speakers, unsigned int speakerNumber) {
    auto it = m_callbacks.find(RTC_EVENT_AUDIO_VOLUME_INDICATION);
    if (it != m_callbacks.end()) {
        Isolate *isolate = Isolate::GetCurrent();
        HandleScope scope(isolate);
        Local<v8::Array> arrSpeakers = v8::Array::New(isolate, speakerNumber);
        for(int i = 0; i < speakerNumber; i++) {
            Local<Object> obj = Object::New(isolate);
            obj->Set(napi_create_string_(isolate, "uid"), napi_create_uid_(isolate, speakers[i].uid));
            obj->Set(napi_create_string_(isolate, "volume"), napi_create_uint32_(isolate, speakers[i].volume));
            arrSpeakers->Set(i, obj);
        }

        Local<Value> argv[2]{ arrSpeakers,
                            napi_create_uint32_(isolate, speakerNumber)
                            };
        NodeEventCallback& cb = *it->second;
        cb.callback.Get(isolate)->Call(cb.js_this.Get(isolate), 2, argv);
    }
}

void AgoraSdk::onAudioVolumeIndication(const agora::linuxsdk::AudioVolumeInfo* speakers, unsigned int speakerNum){
    if (speakers) {
        agora::linuxsdk::AudioVolumeInfo* localSpeakers = new agora::linuxsdk::AudioVolumeInfo[speakerNum];
        for(int i = 0; i < speakerNum; i++) {
            agora::linuxsdk::AudioVolumeInfo tmp = speakers[i];
            localSpeakers[i].uid = tmp.uid;
            localSpeakers[i].volume = tmp.volume;
        }
        agora::recording::node_async_call::async_call([this, localSpeakers, speakerNum] {
            this->onAudioVolumeIndication_node(localSpeakers, speakerNum);
            delete []localSpeakers;
        });
    }
}

void AgoraSdk::onFirstRemoteVideoDecoded(uid_t uid, int width, int height, int elapsed) {
    agora::recording::node_async_call::async_call([this, uid, width, height, elapsed]() {
        MAKE_JS_CALL_4(REC_EVENT_FIRST_VIDEO_FRAME, uid, uid, int32, width, int32, height, int32, elapsed);
    });
}

void AgoraSdk::onFirstRemoteAudioFrame(uid_t uid, int elapsed) {
    agora::recording::node_async_call::async_call([this, uid, elapsed]() {
        MAKE_JS_CALL_2(REC_EVENT_FIRST_AUDIO_FRAME, uid, uid, int32, elapsed);
    });
}

void AgoraSdk::onReceivingStreamStatusChanged(bool receivingAudio, bool receivingVideo) {
    agora::recording::node_async_call::async_call([this, receivingAudio, receivingVideo]() {
        MAKE_JS_CALL_2(REC_EVENT_STREAM_CHANGED, bool, receivingAudio, bool, receivingVideo);
    });
}

void AgoraSdk::onConnectionLost() {
    agora::recording::node_async_call::async_call([this]() {
        MAKE_JS_CALL_0(REC_EVENT_CONN_LOST);
    });
}

void AgoraSdk::onConnectionInterrupted() {
    agora::recording::node_async_call::async_call([this]() {
        MAKE_JS_CALL_0(REC_EVENT_CONN_INTER);
    });
}

}

