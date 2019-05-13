# Agora Restful Recording Nodejs

*Read this in other languages: [中文](README_zh.md)*

This sample application works as a simple restful server to manage recording in an easy way.

## Prerequisites
- Ubuntu 12.04+ x64 or CentOS 6.5+ x64 (CentOS 7+ recommended)
- GCC 4.4+
- ARS IP (public IP)
- 1MB+ bandwidth for each simultaneous recording channel
- Server access for `qos.agoralab.co`
- NodeJS 8.9+

	**Note:** If server access is denied, the Agora SDK may fail to transfer the required data.

## Architecture
![Architecture](https://github.com/AgoraIO/Basic-Recording/blob/master/Agora-Restful-Recording-Nodejs/architecture.png)

## Quick Start
### Backgound Knowledge
This project presumes you have basic ideas of how Agora Recording SDK works, if not please read [here](https://github.com/AgoraIO/Basic-Recording/tree/master/Agora-LinuxServer-Recording) carefully before start.

### Integrate the Agora Recording SDK

The Agora Recording SDK is not included in the repository. To make everything work, you need to do the following, 

* Download the [Agora Recording SDK for Linux](https://www.agora.io/en/download/).
* Unzip the downloaded SDK package.
* Copy the unziped folder to `record/src/sdk` folder
* Run `npm install -g node-gyp` to install addon build tool
* In terminal change to `record` folder, run `build.sh` (run `build_debug.sh` for debugging purpose), if everything goes smoothly, you should see a file named `agorasdk.node` under `record` folder.


## Run Recording Sample
### Quick Start 
* Change to `record` folder
* Ensure `agorasdk.node` has been properly compiled based on previous steps
* Change the code in `sdkdemo.js`, replace the placeholder with the appid you registered from Agora Official Website

```
fs.mkdir(storageDir, {recursive: true}, err => {
	//join channel
	rec.joinChannel(null, "agoratest", 0, YOUR_APP_ID, storageDir);
})
```
* Start a Video/Audio conversation using your Agora Video SDK integration with your appid, change the channel name to `agoratest`
* Run `node sdkdemo.js`, and you should see your recording files under `output` folder

### Layout Adjustment
The Layout is controlled via function `setMixedLayout`.
In this demo we layout user images from top-left to bottom-right.

The demo re-layout the images when new user arrives or existing user leaves.

```
rec.on("userjoin", function(uid) {
    //rearrange layout when user leaves
    console.log(`userleave ${uid}`);
    layout.regions = layout.regions.filter(function(region){
        return region.uid !== uid
    })
    rec.setMixLayout(layout);
});
```

```
rec.on("userleave", function(uid) {
    //rearrange layout when user leaves
    console.log(`userleave ${uid}`);
    //... adding user to layout
    rec.setMixLayout(layout);
});
```

## Run Restful Server Sample
### Quick Start
* Ensure `agorasdk.node` in `record` folder has been properly compiled based on previous steps
* Change to `server` folder
* Run `npm install` to install server dependencies
* Run `node app.js` to start the restful server

### Predefined APIs
#### Start Recording

- `http://localhost:3000/recorder/v1/start`
  
Method:

- POST

Parameters:
 

|Name|Mandatory|Type|Desc|
|:----    |:---|:----- |-----   |
|appid |Y  |string |agora appid   |
|channel |Y  |string | channel name    |
|key     |N  |string | key if certificate is enalbed for your appid    |

Example:
```
curl -i -X POST -H "Content-type: application/json" --data '{"appid":"XXXXX","channel":"XXXX","key":"XXXX"}' 127.0.0.1:3000/recorder/v1/start 
```

Sample Response:

```
{
    "success": true,
    "sid": "ec8711fb-cd98-4411-a430-a0e8de2c6e98"
}
```

Response Properties:

|Name|Type|Desc|
|:----|:----- |-----   |
|success  |bool |operation result   |
|sid  |string | sid of this operation, need this to stop recorder|

#### Stop Recording

- `http://localhost:3000/recorder/v1/stop`
  
Method:

- POST

Parameters:
 

|Name|Mandatory|Type|Desc|
|:----    |:---|:----- |-----   |
|sid |Y  |string |sid when you start the recorder   |

Sample Response:

```
{
    "success": true
}
```

Response Properties:

|Name|Type|Desc|
|:----|:----- |-----   |
|success  |bool |operation result   |

## Resources
- See full API documentation in the [Document Center](https://docs.agora.io/en/)
- [File bugs about this sample](https://github.com/AgoraIO/Basic-Recording/issues)
- See [detailed Agora Linux Recording guides](https://docs.agora.io/en/2.3.1/addons/Recording/Quickstart%20Guide/recording_cpp?platform=C%2B%2B)

## License
This software is licensed under the MIT License (MIT). [View the license](LICENSE.md).
