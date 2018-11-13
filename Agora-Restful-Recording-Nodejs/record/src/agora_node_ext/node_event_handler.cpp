/*
* Copyright (c) 2017 Agora.io
* All rights reserved.
* Proprietry and Confidential -- Agora.io
*/

/*
*  Created by Wang Yongli, 2017
*/

#include "node_event_handler.h"
#include "node_log.h"
#include <stdio.h>
#include "node_uid.h"
#include "agora_node_recording.h"
#include "uv.h"
#include "node_async_queue.h"
namespace agora {
    namespace recording {

#define FUNC_TRACE 

        NodeEventHandler::NodeEventHandler(NodeRecordingSdk *pEngine)
            : m_engine(pEngine)
        {
        }

        NodeEventHandler::~NodeEventHandler() 
        {
            for (auto& handler : m_callbacks) {
                delete handler.second;
            }
        }

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

#define CHECK_NAPI_OBJ(obj) \
    if (obj.IsEmpty()) \
        break;

#define NODE_SET_OBJ_PROP_UINT32(obj, name, val) \
    { \
        Local<Value> propName = String::NewFromUtf8(isolate, name, NewStringType::kInternalized).ToLocalChecked(); \
        CHECK_NAPI_OBJ(propName); \
        Local<Value> propVal = v8::Uint32::New(isolate, val); \
        CHECK_NAPI_OBJ(propVal); \
        obj->Set(isolate->GetCurrentContext(), propName, propVal); \
    }

#define NODE_SET_OBJ_PROP_NUMBER(obj, name, val) \
    { \
        Local<Value> propName = String::NewFromUtf8(isolate, name, NewStringType::kInternalized).ToLocalChecked(); \
        CHECK_NAPI_OBJ(propName); \
        Local<Value> propVal = v8::Number::New(isolate, val); \
        CHECK_NAPI_OBJ(propVal); \
        obj->Set(isolate->GetCurrentContext(), propName, propVal); \
    }

        void NodeEventHandler::onJoinChannelSuccess_node(const char* channel, uid_t id)
        {
            FUNC_TRACE;
            MAKE_JS_CALL_2(RTC_EVENT_JOIN_CHANNEL, string, channel, uid, id);
        }

        void NodeEventHandler::onJoinChannelSuccess(const char* channel, uid_t uid)
        {
            FUNC_TRACE;
            std::string channelName = channel;
            cout << "about to async call" << endl;
            node_async_call::async_call([this, channelName, uid]() {
                cout << "about to call js" << endl;
                this->onJoinChannelSuccess_node(channelName.c_str(), uid);
            });
        }

        // void NodeEventHandler::onLeaveChannel_node(const RtcStats& stats)
        // {
        //     FUNC_TRACE;
        //     MAKE_JS_CALL_0(RTC_EVENT_LEAVE_CHANNEL);
        // }

        // void NodeEventHandler::onLeaveChannel(const RtcStats& stats)
        // {
        //     FUNC_TRACE;
        //     node_async_call::async_call([this, stats] {
        //         this->onLeaveChannel_node(stats);
        //     });
        // }

        // void NodeEventHandler::onUserJoined_node(uid_t uid, int elapsed)
        // {
        //     FUNC_TRACE;
        //     MAKE_JS_CALL_2(RTC_EVENT_USER_JOINED, uid, uid, int32, elapsed);
        // }

        // void NodeEventHandler::onUserJoined(uid_t uid, int elapsed)
        // {
        //     FUNC_TRACE;
        //     node_async_call::async_call([this, uid, elapsed] {
        //         this->onUserJoined_node(uid, elapsed);
        //     });
        // }

        // void NodeEventHandler::onUserOffline_node(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
        // {
        //     FUNC_TRACE;
        //     MAKE_JS_CALL_2(RTC_EVENT_USER_OFFLINE, uid, uid, int32, reason);
        // }

        // void NodeEventHandler::onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason)
        // {
        //     FUNC_TRACE;
        //     node_async_call::async_call([this, uid, reason] {
        //         this->onUserOffline_node(uid, reason);
        //     });
        // }


        void NodeEventHandler::addEventHandler(const std::string& eventName, Persistent<Object>& obj, Persistent<Function>& callback)
        {
            FUNC_TRACE;
            NodeEventCallback *cb = new NodeEventCallback();;
            cb->js_this.Reset(Isolate::GetCurrent(), obj);
            cb->callback.Reset(Isolate::GetCurrent(), callback);
            m_callbacks.emplace(eventName, cb);
        }
    }
}
