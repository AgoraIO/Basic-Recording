/*
 * Copyright (c) 2018 Agora.io
 */

#ifndef I_CLOUD_RECORDING_H // NOLINT
#define I_CLOUD_RECORDING_H // NOLINT

namespace agora {
namespace cloud_recording {
/** */
typedef const char* RecordingId;
typedef unsigned int uid_t;

/**
 * AudioProfile to specify the recording audio quality.
 * kAudioProfileMusicStandard is used if not specified.
 */
enum AudioProfile {
  kAudioProfileMusicStandard,
  kAudioProfileMusicHighQuality,
  kAudioProfileMusicHighQualityStereo
};

/**
 * VideoStreamType to specify the recording video quality.
 * kVideoStreamTypeHigh is used if not specified.
 */
enum VideoStreamType {
  kVideoStreamTypeHigh,
  kVideoStreamTypeLow
};

/**
 * StreamType used to specify the kind of media to be recorded.
 * kStreamTypeAudioVideo is used if not specified.
 */
enum RecordingStreamType {
  kStreamTypeAudioOnly,
  kStreamTypeVideoOnly,
  kStreamTypeAudioVideo
};

/**
 * MixingLayoutType used to specify the layout of users video after
 * transcoding.
 * kMixedVideolayoutTypeVertical is used if not specified.
 */
enum MixedVideoLayoutType {
  kMixedVideoLayoutTypeFloat,
  kMixedVideoLayoutTypeBestFit,
  kMixedVideolayoutTypeVertical
};

struct MixedVideoLayout {
  MixedVideoLayoutType layout;
  uid_t max_resolution_uid;
};

/** Channel type, communication by default.
 */
enum ChannelType {
  kChannelTypeCommunication,
  kChannelTypeLive
};

/** Cloud storage vendor, Qiniu by default.
 */
enum CloudStorageVendor {
  kCloudStorageVendorQiniu,
  kCloudStorageVendorAws,
  kCloudStorageVendorAliyun
};

/**
 * Cloud storage configuration.
 */
struct CloudStorageConfig {
  CloudStorageVendor vendor;
  unsigned int region;
  char* bucket;
  char* access_key;
  char* secret_key;
};

/**
 * Cloud recording decryption mode.
 */
enum DecryptionMode {
  kModeNone,
  kModeAes128Xts,
  kModeAes128Ecb,
  kModeAes256Xts
};

/**
 * Cloud recording transcoding configuration.
 */
struct TranscodingConfig {
 public:
  TranscodingConfig() :
    width(360),
    height(640),
    fps(15),
    bitrate(500),
    max_resolution_uid(0),
    layout(kMixedVideoLayoutTypeFloat) {
  }
  /**
   * Mixed video width after transcoding.
   * 360 by default.
   */
  unsigned int width;
  /**
   * Mixed video hight after transcoding.
   * 640 by default.
   */
  unsigned int height;
  /** video frames per seconds.
   * 15 by default.
   */
  unsigned int fps;
  /** transcoding bitrate.
   * 500 kbps by default
   */
  unsigned int bitrate;
  /** Max resolution uid.
   */
  uid_t max_resolution_uid;
  /** Mixed video layout type, kMixedVideolayoutTypeVertical by default.
   */
  MixedVideoLayoutType layout;
};

/** Recording configuration. */
struct RecordingConfig {
 public:
  RecordingConfig() :
    recording_stream_types(kStreamTypeAudioVideo),
    decryption_mode(kModeNone),
    secret(0),
    channel_type(kChannelTypeCommunication),
    audio_profile(kAudioProfileMusicStandard),
    video_stream_type(kVideoStreamTypeHigh),
    max_idle_time(30) {
    }
  /**
   * Recording stream type. kStreamTypeAudioVideo by default.
   */
  RecordingStreamType recording_stream_types;
  /**
   * Recording decryption mode, kModeNone by defualt.
   */
  DecryptionMode decryption_mode;
  /**
   * Recording decryption secret.
   */
  char* secret;
  /**
   * Recording channel type, kChannelTypeCommunication by default.
   */
  ChannelType channel_type;
  /** Recording audio profile, kAudioProfileMusicStandard by default.
   */
  AudioProfile audio_profile;
  /**
   * Recording Video stream type, kVideoStreamTypeHigh by default.
   */
  VideoStreamType video_stream_type;
  /** Maximum idle time in seconds. If all users leaves channel, the
   * recording instance will still stay in the channel till idle time 
   * expired. After timeout, recording instance will leave the channel 
   * automatically. By default, it is 30 seconds. if it is set to 0, default
   * value will be used. 
   */
  unsigned int max_idle_time;
  /**
   * Recording transcoding configuration.
   */
  TranscodingConfig transcoding_config;
  /**
   * Set recording transcoding configuration width.
   */
  void SetTranscodingWidth(unsigned int width) {
    transcoding_config.width = width;
  }
  /**
   * Set recording transcoding configuration height.
   */
  void SetTranscodingHeight(unsigned int height) {
    transcoding_config.height = height;
  }
  /**
   * Set recording transcoding FPS.
   */
  void SetTranscodingFPS(unsigned int fps) {
    transcoding_config.fps = fps;
  }
  /**
   * Set recording transcoding bitrate.
   */
  void SetTranscodingBitrate(unsigned int bitrate) {
    transcoding_config.bitrate = bitrate;
  }
  /**
   * Set recording transcoding mixed video layout type.
   */
  void SetTranscodingMixedVideoLayout(MixedVideoLayoutType type,
      uid_t max_resolution_uid) {
    transcoding_config.max_resolution_uid = max_resolution_uid;
    transcoding_config.layout = type;
  }
};

/** Cloud Recording Error Code.
 */
enum RecordingErrorCode {
  kRecordingErrorOk = 0,
  kRecordingErrorConnectError = 430,
  kRecordingErrorDisconnected,
  kRecordingErrorInvalidParameter,
  kRecordingErrorInvalidOperation,
  kRecordingErrorNoUsers,
  kRecordingErrorNoRecordedData,
  kRecordingErrorRecorderInit,
  kRecordingErrorRecorderFailed,
  kRecordingErrorUploaderInit,
  kRecordingErrorUploaderFailed,
  kRecordingErrorBackupFailed,
  kRecordingErrorRecorderExit,
  kRecordingErrorGeneralError,
};

/** Cloud Recorder event observer
*/
class ICloudRecorderObserver {
 public:
  virtual ~ICloudRecorderObserver() {}
  /**
   * Recording client is connecting to cloud.
   */
  virtual void OnRecordingConnecting(RecordingId recording_id) = 0;
  /**
   * Cloud started to record.
   */
  virtual void OnRecordingStarted(RecordingId recording_id) = 0;
  /**
   * Cloud already completed recording and start to upload files to third party
   * cloud storage.
   */
  virtual void OnRecordingStopped(RecordingId recording_id) = 0;
  /** 
   * Cloud completed uploading.
   */
  virtual void OnRecordingUploaded(RecordingId recording_id,
      const char* file_name) = 0;
  /**
   * Cloud failed to upload files to third party cloud storage due to any
   * error, and backup files to backup server.
   */
  virtual void OnRecordingBackedUp(RecordingId recording_id,
      const char* file_name) = 0;
  /**
   * uploading progress update
   */
  virtual void OnRecordingUploadingProgress(RecordingId recording_id,
      unsigned int progress, const char* recording_playlist_filename) = 0;
  /** 
   * Recording Error occurred.
   */
  virtual void OnRecorderFailure(RecordingId recording_id,
      RecordingErrorCode code, const char* msg) = 0;
  /**
   * Uploading error occur. 
   */
  virtual void OnUploaderFailure(RecordingId recording_id,
      RecordingErrorCode code, const char* msg) = 0;
  /**
   * Fatal error, both recording and uploading terminate.
   */
  virtual void OnRecordingFatalError(RecordingId recording_id,
      RecordingErrorCode code) = 0;
};

/**
 * ICloudRecorderObserver helper class.
 */
class CloudRecorderObserver : public ICloudRecorderObserver {
 public:
  void OnRecordingConnecting(RecordingId recording_id) override {
    (void)recording_id;
  }

  void OnRecordingStarted(RecordingId recording_id) override {
    (void)recording_id;
  }

  void OnRecordingStopped(RecordingId recording_id) override {
    (void)recording_id;
  }

  void OnRecordingUploaded(RecordingId recording_id,
      const char* file_name) override {
    (void) recording_id;
    (void) file_name;
  }

  void OnRecordingBackedUp(RecordingId recording_id,
      const char* file_name) override {
    (void)recording_id;
    (void)file_name;
  }

  void OnRecordingUploadingProgress(RecordingId recording_id,
      unsigned int progress, const char* recording_playlist_filename) override {
    (void)recording_id;
    (void)progress;
    (void)recording_playlist_filename;
  }

  void OnRecorderFailure(RecordingId recording_id,
      RecordingErrorCode code,
      const char* msg) override {
    (void)recording_id;
    (void)code;
    (void)msg;
  }

  void OnUploaderFailure(RecordingId recording_id,
      RecordingErrorCode code,
      const char* msg) override {
    (void)recording_id;
    (void) code;
    (void)msg;
  }

  void OnRecordingFatalError(RecordingId recording_id,
      RecordingErrorCode code) override {
    (void)recording_id;
    (void) code;
  }
};

/** This interface is the access point for a cloud recorder. Access to this 
* interface is acquired through the \ref CreateCloudRecorderInstance() 
* function. It is possible to create multiple cloud recorder instances to 
* perform simultaneous recordings.
*/
class ICloudRecorder {
 public:
  virtual RecordingId GetRecordingId() = 0;
  /** This method initializes recording instance and start recording.
  */
  virtual int StartCloudRecording(
    const char* appId,
    const char* channel_name,
    const char* token,
    const uid_t uid,
    const RecordingConfig& recording_config,
    const CloudStorageConfig &storage_config) = 0;

  /** To stop recording.
   * After stop recording, cloud will upload recorded media files to
   * cloud storage.
   */
  virtual int StopCloudRecording() = 0;

  /**  To release ICloudRecorder instance. if recording is in process, then
   * both recording and uploading will be stopped. if recording already
   * stopped, then all recorded
   * media files will be uploaded to cloud storage.
   * 
   * if cancel is true, then both recording and uploading in cloud side would be
   * stopped.
   */
  virtual void Release(bool cancel = false) = 0;

 protected:
  virtual ~ICloudRecorder() {}
};

/** This function creates a new cloud recorder and associates it with the
 * calling application. When you no longer need the cloud recorder, it must 
 * release it by calling \ref ICloudRecorder::release(). This will return 
 * the underlying resources to the system.
 * It is possible to create multiple cloud recorder instances to perform 
 * simultaneous recordings.
 */
ICloudRecorder* CreateCloudRecorderInstance(ICloudRecorderObserver *observer);
}  // namespace cloud_recording
}  // namespace agora
#endif //  I_CLOUD_RECORDING_H  NOLINT
