const AgoraRecordingSDK = require("./AgoraRecordSdk");
const path = require("path");
const fs = require("fs");

let rec = new AgoraRecordingSDK();
let layout = {
    "canvasWidth": 640,
    "canvasHeight": 480,
    "backgroundColor": "#00ff00",
    "regions": []
}
rec.setMixLayout(layout);
rec.on("joinchannel", function (channel, uid) {
    console.log(`channel joined ${channel} ${uid}`);
});
rec.on("error", function (err, stat) {
    console.log(`err ${err} ${stat}`);
});
rec.on("userleave", function(uid) {
    console.log(`userleave ${uid}`);
    layout.regions = layout.regions.filter(function(region){
        return region.uid !== uid
    })
    rec.setMixLayout(layout);
});
rec.on("userjoin", function (uid) {
    console.log(`userjoin ${uid}`);
    let region = {
        "x": 0,
		"y": 0,
		"width": 320,
		"height": 240,
		"zOrder": 1,
		"alpha": 1,
		"uid": uid
    }
    switch(layout.regions.length) {
        case 0:
            region.x = 0;
            region.y = 0;
            break;
        case 1:
            region.x = 320;
            region.y = 0;
            break;
        case 2:
            region.x = 0;
            region.y = 240;
            break;
        case 3:
            region.x = 320;
            region.y = 240;
        default:
            break;
    }
    layout.regions.push(region)
    rec.setMixLayout(layout);
});
let storageDir = path.resolve(__dirname, `./output`);

//create output folder
fs.mkdir(storageDir, {recursive: true}, err => {
    //join channel
    rec.joinChannel(null, "agoratest", 0, "aab8b8f5a8cd4469a63042fcfafe7063", storageDir);
})

setTimeout(() => {
    console.log("leaving channel...");
    rec.leaveChannel();
}, 1000 * 50);
