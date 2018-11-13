const agora = require("./agorasdk");
const path = require("path");

class AgoraRecordSdk {
    constructor() {
        this.recording = new agora.NodeRecordingSdk();
    }

    joinChannel(key, name, uid, appid, cfgPath) {
        let binPath = path.join(__dirname, "./src/sdk/bin/");
        return this.recording.joinChannel(key, name, binPath, appid, uid, cfgPath);
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
}

module.exports = AgoraRecordSdk;