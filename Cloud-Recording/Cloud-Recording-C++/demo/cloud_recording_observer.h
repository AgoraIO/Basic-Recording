// Copyright (c) 2018 Agora.io, Inc.
//

#pragma once  // NOLINT(build/header_guard)

#include <stdio.h>
#include <stdarg.h>

#include "IAgoraCloudRecording.h"   // NOLINT
namespace agora {
namespace cloud_recording {

class CloudRecordingObserver : public ICloudRecorderObserver {
 public:
    virtual ~CloudRecordingObserver() {}
    void OnRecordingConnecting(RecordingId recording_id) override;

    void OnRecordingStarted(RecordingId recording_id) override;

    void OnRecordingStopped(RecordingId recording_id) override;

    void OnRecordingUploaded(RecordingId recording_id,
        const char* file_name) override;

    void OnRecordingBackedUp(RecordingId recording_id,
        const char* file_name) override;

    void OnRecordingUploadingProgress(RecordingId recording_id,
        uint32_t progress,
        const char* recording_playlist_filename) override;

    void OnRecorderFailure(RecordingId recording_id,
        RecordingErrorCode code, const char* msg)
      override;
    void OnUploaderFailure(RecordingId recording_id,
        RecordingErrorCode code, const char* msg) override;
    void OnRecordingFatalError(RecordingId recording_id,
        RecordingErrorCode code) override;
};

static void cr_log(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

inline void CloudRecordingObserver::OnRecordingConnecting(
    RecordingId recording_id) {
  cr_log("%s, %s\n", __FUNCTION__, recording_id);
}

inline void CloudRecordingObserver::OnRecordingStarted(RecordingId rid) {
  cr_log("%s, %s\n", __FUNCTION__, rid);
}

inline void CloudRecordingObserver::OnRecordingStopped(
    RecordingId rid) {
  cr_log("%s, %s\n", __FUNCTION__, rid);
}

inline void CloudRecordingObserver::OnRecordingUploaded(RecordingId rid,
    const char* file_name) {
  cr_log("%s, %s,%s\n", __FUNCTION__, rid, file_name);
}

inline void CloudRecordingObserver::OnRecordingBackedUp(RecordingId rid,
    const char* file_name) {
  cr_log("%s, %s, %s\n", __FUNCTION__, rid, file_name);
}

inline void CloudRecordingObserver::OnRecordingUploadingProgress(
    RecordingId id,
    uint32_t progress,
    const char* recording_playlist_filename) {
  (void)id;
  cr_log("%s, %s, progress : %d, file name : %s\n", __FUNCTION__, id, progress,
      recording_playlist_filename);
}

inline void CloudRecordingObserver::OnRecorderFailure(RecordingId rid,
    RecordingErrorCode code, const char* msg) {
  (void)msg;
  (void)rid;
  switch (code) {
    case kRecordingErrorRecorderInit:
      cr_log("Cloud recording recorder init fail.");
      break;
    case kRecordingErrorRecorderFailed:
      cr_log("Cloud recording recorder failed.");
      break;
    default:
      break;
  }
}

inline void CloudRecordingObserver::OnUploaderFailure(RecordingId rid,
    RecordingErrorCode code, const char* msg) {
  (void)msg;
  (void)rid;
  switch (code) {
    case kRecordingErrorUploaderInit:
      cr_log("Cloud recording uploader init fail.");
      break;
    case kRecordingErrorUploaderFailed:
      cr_log("Cloud recording uploader failed.");
      break;
    default:
      break;
  }
}

inline void CloudRecordingObserver::OnRecordingFatalError(RecordingId id,
    RecordingErrorCode code) {
  (void)id;
  switch (code) {
    case kRecordingErrorDisconnected:
      cr_log("Cloud recording connection lost.\n");
      break;
    case kRecordingErrorInvalidParameter:
      cr_log("Recording parameter not right, please have a check.\n");
      break;
    case kRecordingErrorInvalidOperation:
      cr_log("Current operation is not supported.\n");
      break;
    case kRecordingErrorConnectError:
      cr_log("Can't connect to cloud recording\n");
      break;
    case kRecordingErrorNoRecordedData:
      cr_log("No recorded data.\n");
      break;
    case kRecordingErrorBackupFailed:
      cr_log("cloud recording backup failed.\n");
      break;
    case kRecordingErrorRecorderExit:
      cr_log("Recorder exit.Wait uploading\n");
      break;
    case kRecordingErrorNoUsers:
      cr_log("No users in channel.\n");
      break;
    case kRecordingErrorGeneralError:
      cr_log("Error occur.\n");
      break;
    default:
      break;
  }
}
}  // namespace cloud_recording
}  // namespace agora
