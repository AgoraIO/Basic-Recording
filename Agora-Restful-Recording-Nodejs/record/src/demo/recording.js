const EventEmitter = require("events");
const agora = require("../build/Debug/agorasdk");
const path = require("path");
//require("./webgl-utils")
//require("./AgoraRender2")

class Recording {
    constructor() {
        //super();
        console.log(agora);
        this.recording = new agora.NodeRecordingSdk();
        //'this.streams = {};
    }

    joinChannel(key, name, chan_info, uid, appliteDir, appid, rootDir) {
        return this.recording.joinChannel(key, name, appliteDir, appid, uid, rootDir);
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

};
let rec = new Recording();
let binPath = path.join(__dirname, "./");
let storageDir = path.join(__dirname, "./20180615/cfg.json");
rec.setMixLayout({
    "canvasWidth": 640,
    "canvasHeight": 480,
    "backgroundColor": "#00ff00",
    "regions": [{
        "x": 0,
		"y": 0,
		"width": 320,
		"height": 240,
		"zOrder": 1,
		"alpha": 1,
		"uid": 1637410260
    },{
        "x": 320,
		"y": 0,
		"width": 320,
		"height": 240,
		"zOrder": 1,
		"alpha": 1,
		"uid": 1637410260
    }]
});
rec.onEvent("joinchannel", function (channel, uid) {
    console.log(`channel joined ${channel} ${uid}`);
});
rec.onEvent("error", function (err, stat) {
    console.log(`err ${err} ${stat}`);
});
rec.onEvent("userjoin", function (uid) {
    console.log(`userjoin ${uid}`);
});
rec.joinChannel(null, "agoratest", null, 0, binPath, "aab8b8f5a8cd4469a63042fcfafe7063", storageDir);

setTimeout(() => {
    rec.leaveChannel();
}, 1000 * 5);



module.exports = Recording;
