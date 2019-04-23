# Agora Cloud Restful Recording Nodejs

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
This project presumes you have basic ideas of how Agora Cloud Recording works, if not please read [here](https://github.com/AgoraIO/Basic-Recording/tree/master/Agora-LinuxServer-Recording) carefully before start.

### Integrate the Agora Cloud Recording SDK and preparation

The Agora Cloud Recording SDK is not included in the repository. To make everything work, you need to do the following, 

* Contact support@agora.io to get the Cloud Recording SDK C++ Version
* Unzip the downloaded SDK package.
* Copy the unziped folder to `record/src/sdk` folder
* Copy the .so file under  `record/src/sdk/lib` folder to  `/usr/lib` folder
* Input your bucket information to the `storage.json` file under the `server` folder.
* Install `libevent-2.1.8-stable`. [Download link](https://github.com/libevent/libevent/releases/download/release-2.1.8-stable/libevent-2.1.8-stable.tar.gz). More tutorial can be found [here](http://libevent.org/)
* Run `npm install nana -g` to install nan pkg
* Run `apt-get install uuid-dev` to install uuid pkg
* Run `npm install -g node-gyp` to install addon build tool
* In terminal change to `record` folder, run `build.sh` (run `build_debug.sh` for debugging purpose), if everything goes smoothly, you should see a file named `agorasdk.node` under `record` folder.


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
|uid |Y  |integer | uid    |
|key     |N  |string | key if certificate is enalbed for your appid    |

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
- [File bugs about this sample](https://github.com/AgoraIO/Basic-Recording/issues)

## License
This software is licensed under the MIT License (MIT). [View the license](LICENSE.md).
