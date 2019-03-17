const AgoraCloudRecord = require("../record/AgoraCloudRecord");
const path = require("path");
const fs = require("fs");
const uuidv4 = require('uuid/v4');
const storageConfig = require('./storage.json')

class RecordManager{
    constructor() {
        this.recorders = {};
    }

    //find existing recorder
    find(sid) {
        return this.recorders[sid];
    }

    start(key, appid, channel, uid, recordConfig) {
        return new Promise((resolve, reject) => {
            let sdk = new AgoraCloudRecord();
            let recorder = {
                appid: appid,
                channel: channel,
                sdk: sdk,
                sid: sdk.GetRecordingId(),
            };
            this.recorders[recorder.sid] = recorder 
            this.subscribeEvents(recorder)
            sdk.once('RecordStarted', recording_id => {
                console.log(`record started: ${recording_id}`)
                resolve(recorder)
            })
            sdk.StartCloudRecording(appid, channel, key || "", uid || 0, recordConfig, storageConfig)
        });
    }

    subscribeEvents(recorder) {
        let { sdk, sid, appid, channel } = recorder;
        sdk.on('RecordConnecting', recording_id => {
            console.log(`record connecting: ${recording_id}`)
        })
        sdk.on('RecordStopped', recording_id => {
            console.log(`record stopped: ${recording_id}`)
        })
        sdk.on('RecordUploaded', (recording_id, file_name) => {
            console.log(`record uploaded: ${recording_id} ${file_name}`)
        })
        sdk.on('RecordBackedUp', (recording_id, file_name) => {
            console.log(`record backed up: ${recording_id} ${file_name}`)
        })
        sdk.on('RecordUpdateProgress', (recording_id, progress, recording_playlist_filename) => {
            console.log(`record update progress: ${recording_id} ${progress} ${recording_playlist_filename}`)
        })
        sdk.on('RecordFailure', (recording_id, code, msg) => {
            console.log(`record failure: ${recording_id} ${code} ${msg}`)
        })
        sdk.on('UploadFailure', (recording_id, code, msg) => {
            console.log(`upload failure: ${recording_id} ${code} ${msg}`)
        })
        sdk.on('RecordFatalError', (recording_id, code) => {
            console.log(`record fatal error: ${recording_id} ${code}`)
        })
    }

    stop(sid) {
        let recorder = this.recorders[sid];
        if(recorder) {
            let {appid, channel} = recorder;
            console.log(`stop recorder ${appid} ${channel} ${sid}`)
            this.onCleanup(sid);
        } else {
            throw new Error('recorder not exists');
        }
    }

    onCleanup(sid) {
        let recorder = this.recorders[sid];
        if(recorder) {
            let {sdk} = recorder;
            console.log(`releasing ${sid}`)
            sdk.Release()
            delete this.recorders[`${sid}`];
        } else {
            throw new Error('recorder not exists');
        }
    }
}



module.exports = new RecordManager();