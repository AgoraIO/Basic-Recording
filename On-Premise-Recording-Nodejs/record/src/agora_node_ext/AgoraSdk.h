#include <csignal>
#include <cstdint>
#include <iostream>
#include <sstream> 
#include <string>
#include <vector>
#include <algorithm>
#include "node_napi_api.h"
#include "node_uid.h"

#include "IAgoraLinuxSdkCommon.h"
#include "IAgoraRecordingEngine.h"

#include "base/atomic.h"
#include "opt_parser.h" 

namespace agora {

using std::string;
using std::cout;
using std::cerr;
using std::endl;

using agora::base::opt_parser;
using agora::linuxsdk::VideoFrame;
using agora::linuxsdk::AudioFrame;


struct MixModeSettings {
    int m_height;
    int m_width;
    bool m_videoMix;
    MixModeSettings():
        m_height(0),
        m_width(0),
        m_videoMix(false)
    {};
};


class AgoraSdk : virtual public agora::recording::IRecordingEngineEventHandler {
#define REC_EVENT_JOIN_CHANNEL "joinchannel"
#define REC_EVENT_LEAVE_CHANNEL "leavechannel"
#define REC_EVENT_ERROR "error"
#define REC_EVENT_USER_JOIN "userjoin"
#define REC_EVENT_USER_LEAVE "userleave"
#define REC_EVENT_ACTIVE_SPEAKER "activespeaker"
#define REC_EVENT_CONN_LOST "connectionlost"
#define REC_EVENT_CONN_INTER "connectioninterrupt"
#define REC_EVENT_STREAM_CHANGED "receivingstreamstatuschanged"
#define REC_EVENT_FIRST_VIDEO_FRAME "firstremotevideodecoded"
#define REC_EVENT_FIRST_AUDIO_FRAME "firstremoteaudioframe"
#define RTC_EVENT_AUDIO_VOLUME_INDICATION "audiovolumeindication"
#define REC_EVENT_REMOTE_VIDEO_STREAM_STATE_CHANGED "remoteVideoStreamStateChanged"
#define REC_EVENT_REMOTE_AUDIO_STREAM_STATE_CHANGED "remoteAudioStreamStateChanged"
#define REC_EVENT_REJOIN_SUCCESS "rejoinChannelSuccess"
#define REC_EVENT_CONN_STATE_CHANGED "connectionStateChanged"
#define REC_EVENT_REMOTE_VIDEO_STATS "remoteVideoStats"
#define REC_EVENT_REMOTE_AUDIO_STATS "remoteAudioStats"
#define REC_EVENT_RECORDING_STATS "recordingStats"
#define REC_EVENT_LOCAL_USER_REGISTER "localUserRegistered"
#define REC_EVENT_USER_INFO_UPDATED "userInfoUpdated"
    public:
            struct NodeEventCallback
            {
                Persistent<Function> callback;
                Persistent<Object> js_this;
            };
    public:
        AgoraSdk();
        virtual ~AgoraSdk();

        virtual bool createChannel(const string &appid, const string &channelKey, const string &name,  agora::linuxsdk::uid_t uid,
                agora::recording::RecordingConfig &config);
        virtual int setVideoMixLayout();
        virtual bool leaveChannel();
        virtual bool release();
        virtual bool stopped() const;
        virtual void updateMixModeSetting(int width, int height, bool isVideoMix) {
            m_mixRes.m_width = width;
            m_mixRes.m_height = height;
            m_mixRes.m_videoMix = isVideoMix;
        }
        virtual const agora::recording::RecordingEngineProperties* getRecorderProperties();
        virtual void updateStorageDir(const char* dir) { m_storage_dir = dir? dir:"./"; }

        virtual int startService();
        virtual int stopService();

        virtual int setVideoMixingLayout(const agora::linuxsdk::VideoMixingLayout &layout);
        virtual agora::recording::RecordingConfig* getConfigInfo() { return &m_config;}
        void addEventHandler(const std::string& eventName, Persistent<Object>& obj, Persistent<Function>& callback);
        void updateMixLayout(agora::linuxsdk::VideoMixingLayout &layout);
        void emitError(int err, int stat_code);
        agora::linuxsdk::VideoMixingLayout getMixLayout();
    protected:
        virtual void onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code) {
            onErrorImpl(error, stat_code);
        }
        virtual void onWarning(int warn) {
            onWarningImpl(warn);
        }

        virtual void onJoinChannelSuccess(const char * channelId, agora::linuxsdk::uid_t uid) {
            onJoinChannelSuccessImpl(channelId, uid);
        }
        virtual void onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code) {
            onLeaveChannelImpl(code);
        }

        virtual void onUserJoined(agora::linuxsdk::uid_t uid, agora::linuxsdk::UserJoinInfos &infos) {
            onUserJoinedImpl(uid, infos);
        }
        virtual void onUserOffline(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason) {
            onUserOfflineImpl(uid, reason);
        }

        virtual void onActiveSpeaker(uid_t uid);

        virtual void audioFrameReceived(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const {
            audioFrameReceivedImpl(uid, frame);
        }
        virtual void videoFrameReceived(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const {
            videoFrameReceivedImpl(uid, frame);
        }

        virtual void onAudioVolumeIndication(const agora::linuxsdk::AudioVolumeInfo* speakers, unsigned int speakerNum);
        virtual void onFirstRemoteVideoDecoded(uid_t uid, int width, int height, int elapsed);
        virtual void onFirstRemoteAudioFrame(uid_t uid, int elapsed);
        virtual void onReceivingStreamStatusChanged(bool receivingAudio, bool receivingVideo);
        virtual void onConnectionLost();
        virtual void onConnectionInterrupted();

        /**
         * 3.0.0
         */
        virtual void onRemoteVideoStreamStateChanged(uid_t uid, linuxsdk::RemoteStreamState state, linuxsdk::RemoteStreamStateChangedReason reason);
        virtual void onRemoteAudioStreamStateChanged(uid_t, agora::linuxsdk::RemoteStreamState, agora::linuxsdk::RemoteStreamStateChangedReason);
        virtual void onRejoinChannelSuccess(const char* channelId, uid_t uid);
        virtual void onConnectionStateChanged(agora::linuxsdk::ConnectionStateType state, agora::linuxsdk::ConnectionChangedReasonType reason);
        virtual void onRemoteVideoStats(agora::linuxsdk::uid_t uid, const agora::linuxsdk::RemoteVideoStats& stats);
        virtual void onRemoteAudioStats(agora::linuxsdk::uid_t uid, const agora::linuxsdk::RemoteAudioStats& stats);
        virtual void onRecordingStats(const agora::linuxsdk::RecordingStats& stats);
        virtual void onLocalUserRegistered(uid_t uid, const char* userAccount);
        virtual void onUserInfoUpdated(uid_t uid, const agora::linuxsdk::UserInfo& info);
    protected:
        void onErrorImpl(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code);
        void onWarningImpl(int warn);

        void onJoinChannelSuccessImpl(const char * channelId, agora::linuxsdk::uid_t uid);
        void onLeaveChannelImpl(agora::linuxsdk::LEAVE_PATH_CODE code);

        void onUserJoinedImpl(agora::linuxsdk::uid_t uid, agora::linuxsdk::UserJoinInfos &infos);
        void onUserOfflineImpl(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason);

        void audioFrameReceivedImpl(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const;
        void videoFrameReceivedImpl(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const;
        void onAudioVolumeIndication_node(const agora::linuxsdk::AudioVolumeInfo* sperkers, unsigned int speakerNumber);
    protected:
        atomic_bool_t m_stopped;
        std::vector<agora::linuxsdk::uid_t> m_peers;
        std::string m_logdir;
        std::string m_storage_dir;
        MixModeSettings m_mixRes;
        agora::recording::RecordingConfig m_config;
        agora::recording::IRecordingEngine *m_engine;
        std::unordered_map<std::string, NodeEventCallback*> m_callbacks;
        agora::linuxsdk::VideoMixingLayout m_layout;
};


}
