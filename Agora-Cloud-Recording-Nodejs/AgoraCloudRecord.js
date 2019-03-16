const node = require('./agorasdk')
const EventEmitter = require('events')

class AgoraCloudRecord extends EventEmitter {
    constructor() {
        super()
        this.sdk = new node.RecorderManager()
        this.subscribeEvents()
    }

    StartCloudRecording(appid, cname, token, uid, config, storageConfig) {
        return this.sdk.StartCloudRecording(appid, cname, token, uid, config, storageConfig)
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