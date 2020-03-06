const agora = require("./agorasdk");
const path = require("path");
const fs = require("fs")
const EventEmitter = require("events")

class AgoraRecordSdk extends EventEmitter {
    constructor() {
        super();
        this.recording = new agora.NodeRecordingSdk();
        this.initEventHandler();
    }

    initEventHandler() {
        let self = this;
    
        let fire = (...args) => {
          setImmediate(() => {
            this.emit(...args)
          })
        }

        this.onEvent("joinchannel", (channel, uid) => {
            fire('joinchannel', channel, uid);
        });
        this.onEvent("error", (err, stat) => {
            fire('error', err, stat);
        });
        this.onEvent("userjoin", (err, stat) => {
            fire('userjoin', err, stat);
        });
        this.onEvent("userleave", (err, stat) => {
            fire('userleave', err, stat);
        });
        this.onEvent("activespeaker", uid => {
            fire("activespeaker", uid)
        });
        this.onEvent("connectionlost", () => {
            fire("connectionlost")
        })
        this.onEvent("connectioninterrupt", () => {
            fire("connectioninterrupt")
        })
        this.onEvent("receivingstreamstatuschanged", (receivingAudio, receivingVideo) => {
            fire("receivingstreamstatuschanged", receivingAudio, receivingVideo)
        })
        this.onEvent("firstremotevideodecoded", (uid, width, height, elapsed) => {
            fire("firstremotevideodecoded", uid, width, height, elapsed)
        })
        this.onEvent("firstremoteaudioframe", (uid, elapsed) => {
            fire("firstremoteaudioframe", uid, elapsed)
        })
        this.onEvent("audiovolumeindication", (speakers, speakerNum) => {
            fire("audiovolumeindication", speakers, speakerNum)
        })
        this.onEvent("remoteVideoStreamStateChanged", (uid, state, reason) => {
            fire("remoteVideoStreamStateChanged", uid, state, reason)
        })
        this.onEvent("remoteAudioStreamStateChanged", (uid, state, reason) => {
            fire("remoteAudioStreamStateChanged", uid, state, reason)
        })
        this.onEvent("rejoinChannelSuccess", (channel, uid) => {
            fire("rejoinChannelSuccess", channel, uid)
        })
        this.onEvent("connectionStateChanged", (state, reason) => {
            fire("connectionStateChanged", state, reason)
        })
        this.onEvent("remoteVideoStats", (uid, stats) => {
            fire("remoteVideoStats", uid, stats)
        })
        this.onEvent("remoteAudioStats", (uid, stats) => {
            fire("remoteAudioStats", uid, stats)
        })
        this.onEvent("recordingStats", (stats) => {
            fire("recordingStats", stats)
        })
        this.onEvent("localUserRegistered", (uid, account) => {
            fire("localUserRegistered", uid, account)
        })
        this.onEvent("userInfoUpdated", (uid, info) => {
            fire("userInfoUpdated", uid, info)
        })
    }

    joinChannel(key, name, uid, appid, storeFolder) {
        return new Promise((resolve, reject) => {
            let binPath = path.join(__dirname, "./src/sdk/bin/");
            fs.access(storeFolder, fs.constants.W_OK, (err) => {
                if(err) {
                    throw "folder not writable"
                }
                const json = {
                    Recording_Dir: `${storeFolder}`
                };
                const cfgPath = path.join(storeFolder, '/cfg.json')
                fs.writeFile(cfgPath, JSON.stringify(json), err => {
                    this.recording.joinChannel(key || null, name, binPath, appid, uid, cfgPath);
                    this.once("error", err => {
                        reject(err);
                    })
                    this.once("joinchannel", () => {
                        resolve();
                    });
                });
            });
    
        });
    }

    setMixLayout(layout) {
        return this.recording.setMixLayout(layout);
    }

    onEvent(event, cb) {
        return this.recording.onEvent(event, cb);
    };

    leaveChannel() {
        return this.recording.leaveChannel();
    }

    release() {
        return this.recording.release();
    }
}

module.exports = AgoraRecordSdk;