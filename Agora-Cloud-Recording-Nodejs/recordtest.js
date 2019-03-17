const AgoraCloudRecord = require('./AgoraCloudRecord')

class RecordingTest {
    constructor() {
        let recorder = new AgoraCloudRecord()
        this.recorder = recorder
        recorder.on('RecordConnecting', recording_id => {
            console.log(`record connecting: ${recording_id}`)
        })
        recorder.on('RecordStarted', recording_id => {
            console.log(`record started: ${recording_id}`)
        })
        recorder.on('RecordStopped', recording_id => {
            console.log(`record stopped: ${recording_id}`)
        })
        recorder.on('RecordUploaded', (recording_id, file_name) => {
            console.log(`record uploaded: ${recording_id} ${file_name}`)
        })
        recorder.on('RecordBackedUp', (recording_id, file_name) => {
            console.log(`record backed up: ${recording_id} ${file_name}`)
        })
        recorder.on('RecordUpdateProgress', (recording_id, progress, recording_playlist_filename) => {
            console.log(`record update progress: ${recording_id} ${progress} ${recording_playlist_filename}`)
        })
        recorder.on('RecordFailure', (recording_id, code, msg) => {
            console.log(`record failure: ${recording_id} ${code} ${msg}`)
        })
        recorder.on('UploadFailure', (recording_id, code, msg) => {
            console.log(`upload failure: ${recording_id} ${code} ${msg}`)
        })
        recorder.on('RecordFatalError', (recording_id, code) => {
            console.log(`record fatal error: ${recording_id} ${code}`)
        })
    }

    start(appid, cname, token, uid, config, storageConfig) {
        return new Promise((resolve, reject) => {
            this.recorder.StartCloudRecording(appid, cname, token, uid, config, storageConfig)
        })
    }
}

let test1 = new RecordingTest()
test1.start("aab8b8f5a8cd4469a63042fcfafe7063", "agoratest", "", 1000, {    
}, require('./storage.json'))

