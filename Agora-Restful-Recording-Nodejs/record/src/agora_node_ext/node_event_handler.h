/*
* Copyright (c) 2017 Agora.io
* All rights reserved.
* Proprietry and Confidential -- Agora.io
*/

/*
*  Created by Wang Yongli, 2017
*/

#ifndef AGORA_NODE_EVENT_HANDLER_H
#define AGORA_NODE_EVENT_HANDLER_H

#include "IAgoraRecordingEngine.h"
#include "agora_node_ext.h"
#include <unordered_map>
#include <string>
#include <uv.h>
#include "node_napi_api.h"
namespace agora {
    namespace recording {
#define RTC_EVENT_JOIN_CHANNEL "joinchannel"
        class NodeRecordingSdk;
        class NodeEventHandler : public IRecordingEngineEventHandler
        {
        public:
            struct NodeEventCallback
            {
                Persistent<Function> callback;
                Persistent<Object> js_this;
            };
        public:
            NodeEventHandler(NodeRecordingSdk* pEngine);
            ~NodeEventHandler();
            virtual void onJoinChannelSuccess(const char* channel, uid_t uid) override;
            // virtual void onRejoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override;
            virtual void onWarning(int warn) {
                
            };
            virtual void onError(int err, agora::linuxsdk::STAT_CODE_TYPE statcode) {
                
            };
            // virtual void onLeaveChannel(const RtcStats& stats) override;
            // virtual void onUserJoined(uid_t uid, int elapsed) override;
            // virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override;
            virtual void onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code) {
                // onLeaveChannelImpl(code);
            }

            virtual void onUserJoined(agora::linuxsdk::uid_t uid, agora::linuxsdk::UserJoinInfos &infos) {
                // onUserJoinedImpl(uid, infos);
            }
            virtual void onUserOffline(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason) {
                // onUserOfflineImpl(uid, reason);
            }

            virtual void audioFrameReceived(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const {
                // audioFrameReceivedImpl(uid, frame);
            }
            virtual void videoFrameReceived(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const {
                // videoFrameReceivedImpl(uid, frame);
            }
            
            void addEventHandler(const std::string& eventName, Persistent<Object>& obj, Persistent<Function>& callback);

        private:
            void onJoinChannelSuccess_node(const char* channel, uid_t uid) ;
            // void onRejoinChannelSuccess_node(const char* channel, uid_t uid, int elapsed) ;
            // void onLeaveChannel_node(const RtcStats& stats) ;
            // void onUserJoined_node(uid_t uid, int elapsed) ;
            // void onUserOffline_node(uid_t uid, USER_OFFLINE_REASON_TYPE reason) ;
            
        private:
            std::unordered_map<std::string, NodeEventCallback*> m_callbacks;
            NodeRecordingSdk* m_engine;
        };
    }
}

#endif
