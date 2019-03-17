const node = require('./agorasdk')
const EventEmitter = require('events')

class AgoraCloudRecord extends EventEmitter {
    constructor() {
        super()
        this.sdk = new node.RecorderManager()
        this.subscribeEvents()
    }

    GetRecordingId() {
        return this.sdk.GetRecordingId();
    }

    StartCloudRecording(appid, cname, token, uid, recordConfig, storageConfig) {
        if(!appid) {
            throw new Error('appid is mandatory')
        }

        if(!cname) {
            throw new Error('cname is mandatory')
        }

        token = token || ""
        uid = parseInt(uid)
        if(isNaN(uid) && uid <= 0) {
            throw new Error('invalid uid')
        }

        let recordConfigTemplate = {
            recording_stream_type: 0,
            decryption_mode: 0,
            secret: "",
            channel_type: 0,
            video_stream_type: 0,
            audio_profile: 0,
            max_idle_time: 15,
            transcoding_config: null
        }

        let storageConfigTemplate = {
            vendor: 0,
            access_key: "",
            secret_key: "",
            bucket: "",
            region: 0
        }

        return this.sdk.StartCloudRecording(
            appid, cname, token, uid,
            Object.assign(recordConfigTemplate, recordConfig),
            Object.assign(storageConfigTemplate, storageConfig)
        )
    }

    StopCloudRecording() {
        return this.sdk.StopCloudRecording()
    }

    Release(keepRecordingInBackground) {
        return this.sdk.Release(keepRecordingInBackground || false)
    }

    subscribeEvents() {
        let fire = (...args) => {
            setImmediate(() => {
                this.emit(...args)
            })
        }
        this.sdk.onEvent('RecordConnecting', recording_id => {
            fire('RecordConnecting', recording_id)
        })
        this.sdk.onEvent('RecordStarted', recording_id => {
            fire('RecordStarted', recording_id)
        })
        this.sdk.onEvent('RecordStopped', recording_id => {
            fire('RecordStopped', recording_id)
        })
        this.sdk.onEvent('RecordUploaded', (recording_id, file_name) => {
            fire('RecordUploaded', recording_id, file_name)
        })
        this.sdk.onEvent('RecordBackedUp', (recording_id, file_name) => {
            fire('RecordBackedUp', recording_id, file_name)
        })
        this.sdk.onEvent('RecordUpdateProgress', (recording_id, progress, recording_playlist_filename) => {
            fire('RecordUpdateProgress', recording_id, progress, recording_playlist_filename)
        })
        this.sdk.onEvent('RecordFailure', (recording_id, code, msg) => {
            fire('RecordFailure', recording_id, code, msg)
        })
        this.sdk.onEvent('UploadFailure', (recording_id, code, msg) => {
            fire('UploadFailure', recording_id, code, msg)
        })
        this.sdk.onEvent('RecordFatalError', (recording_id, code) => {
            fire('RecordFatalError', recording_id, code)
        })
    }
}

module.exports = AgoraCloudRecord