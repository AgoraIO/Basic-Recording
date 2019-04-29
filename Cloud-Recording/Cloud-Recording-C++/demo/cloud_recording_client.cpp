/** Copyright (c) 2018
 * All rights reserved
 * Confidential and Proprietary -- Agora.io, Inc.
 */

#include <signal.h>
#include <string>
#include <iostream>
#include <memory>
#include <atomic>
#include <thread>
#include <list>
#include <utility>
#include <functional>
#include <map>
#include <mutex>

#include "IAgoraCloudRecording.h"
#include "cloud_recording_observer.h"  // NOLINT
#include "opt_parser.h"  // NOLINT

using agora::cloud_recording::CloudStorageConfig;
using agora::cloud_recording::RecordingConfig;
using agora::cloud_recording::ICloudRecorder;
using agora::cloud_recording::RecordingId;
using agora::cloud_recording::RecordingErrorCode;
using agora::cloud_recording::CloudRecordingObserver;
class SigHandler {
 public:
  static void RegisterSigHandler(int sig, std::function<void(int)> handler);
  static void DeregisterSigHandler(int sig);

 private:
  static void SignalHandler(int sig_no);

 private:
  static std::map<int, std::function<void(int)>> handlers;
  static std::mutex handler_mutex;
};

std::map<int, std::function<void(int)>> SigHandler::handlers;
std::mutex SigHandler::handler_mutex;

void SigHandler::RegisterSigHandler(int sig, std::function<void(int)> handler) {
  handlers[sig] = std::move(handler);
  signal(sig, SignalHandler);
}

void SigHandler::DeregisterSigHandler(int sig_no) {
  auto it = handlers.find(sig_no);
  if (it != handlers.end()) {
    handlers.erase(it);
  }
}

void SigHandler::SignalHandler(int sig_no) {
  auto it = handlers.find(sig_no);
  if (it != handlers.end()) {
    it->second(sig_no);
  }
}

class CloudRecordingClient : CloudRecordingObserver {
 public:
  CloudRecordingClient();
  ~CloudRecordingClient();
  void Run(int argc, char *argv[]);

 private:
  void OnRecordingStarted(RecordingId recording_id) override;
  void OnRecordingUploaded(RecordingId recording_id,
      const char* file_name) override;
  void OnRecordingBackedUp(RecordingId recording_id,
      const char* file_name) override;
  void OnRecorderFailure(RecordingId recording_id,
      RecordingErrorCode code, const char* msg) override;
  void OnRecordingFatalError(RecordingId recording_id,
      RecordingErrorCode code) override;

  bool CheckChannelParam(const std::string& app_id, const std::string& cname,
      uint32_t uid);

  bool CheckConfigParam(uint32_t stream_type, uint32_t channel_type,
      uint32_t audio_profile, uint32_t mixed_video_layout,
      uint32_t decryption_mode, uint32_t video_stream_type);

  bool CheckStorageParam(uint32_t vender, uint32_t region,
      const std::string& bucket, const std::string& secret_key,
      const std::string& access_key);

  void sig_handler(int sig_no);

 private:
  std::unique_ptr<ICloudRecorder, void(*)(ICloudRecorder*)> recorder_;
  std::atomic_bool recording_started;
  std::atomic_bool stopped_;
};

CloudRecordingClient::CloudRecordingClient()
  : CloudRecordingObserver(),
    recorder_(agora::cloud_recording::CreateCloudRecorderInstance(this),
      [](ICloudRecorder* recorder) {
          recorder->Release();
          }),
    recording_started(false),
    stopped_(false) {
}

CloudRecordingClient::~CloudRecordingClient() {}

bool CloudRecordingClient::CheckChannelParam(const std::string& app_id,
    const std::string& channel_name, uint32_t uid) {
  if (app_id.length() == 0) {
    std::cout << "Invalid app id."<< std::endl;
    return false;
  }

  if (channel_name.length() == 0) {
    std::cout << "Invalid channel name" << std::endl;
    return false;
  }

  if (uid == 0) {
    std::cout << "uid can't be 0." << std::endl;
    return false;
  }
  return true;
}

bool CloudRecordingClient::CheckConfigParam(uint32_t stream_type,
    uint32_t channel_type, uint32_t audio_profile,
    uint32_t mixed_video_layout, uint32_t decryption_mode,
    uint32_t video_stream_type) {
  if (stream_type > 2) {
    std::cout << "Invalid stream type, it must be less or "
        "equal to 2." << std::endl;
    return false;
  }

  if (channel_type > 1) {
    std::cout << "Invalid channel type, it must be 0 or 1." << std::endl;
    return false;
  }

  if (audio_profile > 2) {
    std::cout << "Invalid audio profile, it must be less than 2." << std::endl;
    return false;
  }

  if (mixed_video_layout > 2) {
    std::cout << "Invalid mixed video layout. it must be less than 3." <<
      std::endl;
    return false;
  }

  if (decryption_mode > 3) {
    std::cout << "Invalid decryption mode. it must be less than 4." <<
      std::endl;
    return false;
  }

  if (video_stream_type > 1) {
    std::cout << "Invalid video stream type. it must be less than 2." <<
      std::endl;
    return false;
  }

  return true;
}

bool CloudRecordingClient::CheckStorageParam(uint32_t vender, uint32_t region,
      const std::string& bucket, const std::string& secret_key,
      const std::string& access_key) {
  if (vender > 2) {
    std::cout << "Invalid vendor id. it must be less than 2." << std::endl;
    return false;
  }

  (void)region;

  if (bucket.length() == 0) {
    std::cout << "Invalid bucket." << std::endl;
    return false;
  }

  if (secret_key.length() == 0) {
    std::cout << "Invalid secret_key." << std::endl;
    return false;
  }

  if (access_key.length() == 0) {
    std::cout << "Invalid access_key" << std::endl;
    return false;
  }
  return true;
}

void CloudRecordingClient::sig_handler(int sig_no) {
  (void)sig_no;
  if (sig_no == SIGINT || sig_no == SIGTERM || sig_no == SIGQUIT) {
    std::cout << __FUNCTION__ << ", sig : " << sig_no << std::endl;
    stopped_.store(true);
    return;
  } else if (sig_no == SIGUSR1) {
    if (recorder_ && recording_started) {
      std::cout << __FUNCTION__ << std::endl;
      recorder_->StopCloudRecording();
    }
  }
}

void CloudRecordingClient::Run(int argc, char* argv[]) {
  opt_parser parser;
  std::string app_id;
  std::string channel_name;
  std::string token;
  std::string secret;
  std::string bucket;
  std::string access_key;
  std::string secret_key;

  uint32_t decryption_mode = 0;
  uint32_t uid = 0;
  uint32_t recording_stream_types =
    agora::cloud_recording::kStreamTypeAudioVideo;
  uint32_t video_stream_type = agora::cloud_recording::kVideoStreamTypeHigh;
  uint32_t channel_type = agora::cloud_recording::kChannelTypeCommunication;
  uint32_t audio_profile =
    agora::cloud_recording::kAudioProfileMusicStandard;
  uint32_t max_resolution_uid = 0;
  uint32_t mixed_video_layout =
    agora::cloud_recording::kMixedVideoLayoutTypeFloat;
  uint32_t region;
  uint32_t vender = agora::cloud_recording::kCloudStorageVendorQiniu;
  uint32_t width = 360;
  uint32_t height = 640;
  uint32_t fps = 15;
  uint32_t bitrate = 500;
  uint32_t max_idle_time = 30;

  if (!recorder_) {
    std::cout << "Create ICloudRecorder failed." << std::endl;
    return;
  }
  SigHandler::RegisterSigHandler(SIGINT,
      std::bind(&CloudRecordingClient::sig_handler, this,
        std::placeholders::_1));
  SigHandler::RegisterSigHandler(SIGTERM,
      std::bind(&CloudRecordingClient::sig_handler, this,
        std::placeholders::_1));
  SigHandler::RegisterSigHandler(SIGUSR1,
      std::bind(&CloudRecordingClient::sig_handler, this,
        std::placeholders::_1));
  SigHandler::RegisterSigHandler(SIGQUIT,
      std::bind(&CloudRecordingClient::sig_handler, this,
        std::placeholders::_1));

  parser.add_long_opt("appId", &app_id, "app id", opt_parser::require_argu);
  parser.add_long_opt("channelName", &channel_name, "channel name",
      opt_parser::require_argu);
  parser.add_long_opt("uid", &uid, "User id", opt_parser::require_argu);
  parser.add_long_opt("token", &token, "channel token/Optional");
  parser.add_long_opt("recordingStreamType", &recording_stream_types,
      "stream types (Optional: 0 for audio only,  1 for video only, 2 for"
      " audio and video, default 2)");
  parser.add_long_opt("videoStreamType", &video_stream_type, "video"
      " stream type(Optional: 0 for high video, 1 for low video, default 0."
      " Only works when recordingStreamType is not 0.)");
  parser.add_long_opt("decryption_mode", &decryption_mode,
      "decryption mode(Optional, 0 for none, 1 for aes-128-xts,"
      "2 for aes-128-ecb, 3 for aes-256-xts, no decryption by default.)");
  parser.add_long_opt("secret", &secret, "secret(Optional, emptry"
      " by default)");
  parser.add_long_opt("channelType", &channel_type, "channel type "
      "(Optional 0 for communication, 1 for live, default 0)");
  parser.add_long_opt("audioProfile", &audio_profile, "audio profile, "
      "(Optional 0 for single channel mono, 1 for single channel music, "
      "2 for multi channel music. default 0)");
  parser.add_long_opt("mixWidth", &width, "transcoding mixing width "
      "(Optional, defualt 360)");
  parser.add_long_opt("mixHeight", &height, "transcoding mixing height "
      "(Optional, default 640)");
  parser.add_long_opt("fps", &fps,
      "transcoding fps(Optional, default 15)");
  parser.add_long_opt("bitrate", &bitrate, "transcoding bitrate"
      "(Optional, 500 by default)");
  parser.add_long_opt("maxResolutionUid", &max_resolution_uid,
      "transcoding max resolution uid(Optional)");
  parser.add_long_opt("mixedVideoLayoutType", &mixed_video_layout,
      "mixed video layout mode(Optional, 0 for float, 1 for BestFit, 2 for "
      "vertical, 0 by defualt)");
  parser.add_long_opt("maxIdleTime", &max_idle_time, "max idle time"
      "(Optional, 30 by default)");
  parser.add_long_opt("vendor", &vender, "cloud storage vendor("
      "0 for Qiniu, 1 for AWS, 2 for Aliyun)", opt_parser::require_argu);
  parser.add_long_opt("region", &region, "cloud storage region",
      opt_parser::require_argu);
  parser.add_long_opt("bucket", &bucket, "cloud storage bucket",
      opt_parser::require_argu);
  parser.add_long_opt("accessKey", &access_key,
      "cloud storage access key", opt_parser::require_argu);
  parser.add_long_opt("secretKey", &secret_key,
      "cloud storage secret key", opt_parser::require_argu);

  if (!parser.parse_opts(argc, argv)) {
    std::cout << "Invalid parameters." << std::endl;
    parser.print_usage(argv[0], std::cout);
    return;
  }

  if (!CheckChannelParam(app_id, channel_name, uid)) {
    parser.print_usage(argv[0], std::cout);
    return;
  }

  if (!CheckConfigParam(recording_stream_types, channel_type, audio_profile,
        mixed_video_layout, decryption_mode, video_stream_type)) {
    parser.print_usage(argv[0], std::cout);
    return;
  }

  if (!CheckStorageParam(vender, region, bucket, secret_key, access_key)) {
    parser.print_usage(argv[0], std::cout);
    return;
  }

  CloudStorageConfig config;
  config.vendor = agora::cloud_recording::CloudStorageVendor(vender);
  config.region = region;
  config.bucket = const_cast<char*>(bucket.c_str());
  config.access_key = const_cast<char*>(access_key.c_str());
  config.secret_key = const_cast<char*>(secret_key.c_str());

  RecordingConfig recording_config;
  recording_config.audio_profile =
    agora::cloud_recording::AudioProfile(audio_profile);
  recording_config.channel_type =
    agora::cloud_recording::ChannelType(channel_type);
  recording_config.decryption_mode =
    agora::cloud_recording::DecryptionMode(decryption_mode);
  recording_config.secret = const_cast<char*>(secret.c_str());
  recording_config.max_idle_time = max_idle_time;
  recording_config.recording_stream_types =
    agora::cloud_recording::RecordingStreamType(recording_stream_types);
  recording_config.video_stream_type =
    agora::cloud_recording::VideoStreamType(video_stream_type);
  recording_config.transcoding_config.bitrate = bitrate;
  recording_config.transcoding_config.fps = fps;
  recording_config.transcoding_config.height = height;
  recording_config.transcoding_config.width = width;
  recording_config.transcoding_config.max_resolution_uid = max_resolution_uid;
  recording_config.transcoding_config.layout =
    agora::cloud_recording::MixedVideoLayoutType(mixed_video_layout);

  recorder_->StartCloudRecording(app_id.c_str(), channel_name.c_str(),
        token.c_str(), uid, recording_config, config);

  while (!stopped_.load()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  printf("Stopped\n");
}

void CloudRecordingClient::OnRecordingStarted(RecordingId recording_id) {
  (void)recording_id;
  CloudRecordingObserver::OnRecordingStarted(recording_id);
  recording_started.store(true);
}

void CloudRecordingClient::OnRecordingUploaded(RecordingId recording_id,
    const char* file_name) {
  CloudRecordingObserver::OnRecordingUploaded(recording_id, file_name);
  stopped_.store(true);
}

void CloudRecordingClient::OnRecordingBackedUp(RecordingId recording_id,
    const char* file_name) {
  CloudRecordingObserver::OnRecordingBackedUp(recording_id, file_name);
  stopped_.store(true);
}

void CloudRecordingClient::OnRecorderFailure(RecordingId recording_id,
    RecordingErrorCode code, const char* msg) {
  (void)recording_id;
  (void)msg;
  if (code == agora::cloud_recording::kRecordingErrorDisconnected) {
    stopped_.store(true);
  }
}

void CloudRecordingClient::OnRecordingFatalError(RecordingId recording_id,
    RecordingErrorCode code) {
  CloudRecordingObserver::OnRecordingFatalError(recording_id, code);
  stopped_.store(true);
}

int main(int argc, char* argv[]) {
  CloudRecordingClient client;
  client.Run(argc, argv);
  return 0;
}
