const AgoraRecordingSDK = require("./AgoraRecordSdk");
const path = require("path");

let rec = new AgoraRecordingSDK();
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
		"uid": 227894999
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
rec.joinChannel(null, "agoratest", 0, "aab8b8f5a8cd4469a63042fcfafe7063", storageDir);

setTimeout(() => {
    // rec.leaveChannel();
}, 1000 * 5);
