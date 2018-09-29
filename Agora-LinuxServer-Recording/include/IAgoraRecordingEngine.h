#pragma once

namespace agora {
namespace recording {

typedef unsigned char uchar_t;
typedef unsigned int uint_t;
typedef unsigned int uid_t;

enum ERROR_CODE_TYPE {
    ERR_OK = 0,
    //1~1000
    ERR_FAILED = 1,
    ERR_INVALID_ARGUMENT = 2,
    ERR_INTERNAL_FAILED = 3,
};

enum WARN_CODE_TYPE {
    WARN_NO_AVAILABLE_CHANNEL = 103,
    WARN_LOOKUP_CHANNEL_TIMEOUT = 104,
    WARN_LOOKUP_CHANNEL_REJECTED = 105,
    WARN_OPEN_CHANNEL_TIMEOUT = 106,
    WARN_OPEN_CHANNEL_REJECTED = 107,
};

enum CHANNEL_PROFILE_TYPE
{
    CHANNEL_PROFILE_COMMUNICATION = 0,
    CHANNEL_PROFILE_LIVE_BROADCASTING = 1,
};

enum USER_OFFLINE_REASON_TYPE
{
    USER_OFFLINE_QUIT = 0,
    USER_OFFLINE_DROPPED = 1,
    USER_OFFLINE_BECOME_AUDIENCE = 2,
};

enum REMOTE_VIDEO_STREAM_TYPE
{
    REMOTE_VIDEO_STREAM_HIGH = 0,
    REMOTE_VIDEO_STREAM_LOW = 1,
};


typedef struct VideoMixingLayout
{
    struct Region {
        uid_t uid;
        double x;//[0,1]
        double y;//[0,1]
        double width;//[0,1]
        double height;//[0,1]
        int zOrder; //optional, [0, 100] //0 (default): bottom most, 100: top most

        //  Optional
        //  [0, 1.0] where 0 denotes throughly transparent, 1.0 opaque
        double alpha;

        int renderMode;//RENDER_MODE_HIDDEN: Crop, RENDER_MODE_FIT: Zoom to fit
        Region()
        :uid(0)
        , x(0)
        , y(0)
        , width(0)
        , height(0)
        , zOrder(0)
        , alpha(1.0)
        , renderMode(1)
        {}

    };
    int canvasWidth;
    int canvasHeight;
    const char* backgroundColor;//e.g. "#C0C0C0" in RGB
    int regionCount;
    const Region* regions;
    const char* appData;
    int appDataLength;
    VideoMixingLayout()
    :canvasWidth(0)
    , canvasHeight(0)
    , backgroundColor(NULL)
    , regionCount(0)
    , regions(NULL)
    , appData(NULL)
    , appDataLength(0)
    {}
} VideoMixingLayout;

typedef struct UserJoinInfos {
    const char* recordingDir;
    //new attached info add below

    UserJoinInfos():
    recordingDir(NULL)
    {}
}UserJoinInfos;


class IRecordingEngineEventHandler {
public:
    virtual ~IRecordingEngineEventHandler() {}

    virtual void onError(int error) = 0;
    virtual void onWarning(int warn) = 0;

    virtual void onJoinChannelSuccess(const char * channelId, uid_t uid) = 0;
    virtual void onLeaveChannel() = 0;

    virtual void onUserJoined(uid_t uid, UserJoinInfos &infos) = 0;
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) = 0;
};

typedef struct RecordingConfig {
    CHANNEL_PROFILE_TYPE channelProfile;
    bool isAudioOnly;
    bool isMixingEnabled;
    char * decryptionMode;
    char * secret;
    int idleLimitSec;
    char * appliteDir;
//    char * appliteLogDir;
    char * recordFileRootDir;
    int lowUdpPort;
    int highUdpPort;

    RecordingConfig(): channelProfile(CHANNEL_PROFILE_COMMUNICATION),
        isAudioOnly(false),
        isMixingEnabled(false),
        decryptionMode(NULL),
        secret(NULL),
        idleLimitSec(300),
        appliteDir(NULL),
//        appliteLogDir(NULL),
        recordFileRootDir(NULL),
        lowUdpPort(0),
        highUdpPort(0)
    {}
} RecordingConfig;

class IRecordingEngine{
public:

    /**
     *  create a new recording engine instance
     *
     *  @param appId        The App ID issued to the application developers by Agora.io.
     *  @param eventHandler the callback interface
     *
     *  @return a recording engine instance pointer
     */
    static IRecordingEngine* createAgoraRecordingEngine(const char * appId, IRecordingEngineEventHandler *eventHandler);

    virtual ~IRecordingEngine() {}

    /**
     *  This method lets the recording engine join a channel, and start recording
     *
     *  @param channelKey This parameter is optional if the user uses a static key, or App ID. In this case, pass NULL as the parameter value. More details refer to http://docs-origin.agora.io/en/user_guide/Component_and_Others/Dynamic_Key_User_Guide.html
     *  @param channelId  A string providing the unique channel id for the AgoraRTC session
     *  @param uid        The uid of recording client
     *  @param config     The config of current recording
     *
     *  @return 0: Method call succeeded. <0: Method call failed.
     */
    virtual int joinChannel(const char * channelKey, const char *channelId, uid_t uid, const RecordingConfig &config) = 0;



    /**
     *  set the layout of video mixing
     *
     *  @param layout layout setting
     *
     *  @return 0: Method call succeeded. <0: Method call failed.
     */
    virtual int setVideoMixingLayout(const VideoMixingLayout &layout) = 0;

    /**
     *  Stop recording
     *
     *  @return 0: Method call succeeded. <0: Method call failed.
     */
    virtual int leaveChannel() = 0;

    /**
     *  release recording engine
     *
     *  @return 0: Method call succeeded. <0: Method call failed.
     */
    virtual int release() = 0;
};

}
}
