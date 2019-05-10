# Agora Restful Recording Nodejs

*Read this in other languages: [English](README.md)*

该文档介绍如何以Restful API的形式快速实现录制。

## 准备工作
请确保满足以下操作系统要求:
- Ubuntu 12.04 x64 或更高版本 或者 CentOS 6.5 x64 或更高版本(推荐CentOS 7)
- GCC 4.4+
- NodeJS 8.9+
- 与公网互通，并且能够访问`qos.agoralab.co`
- 每一个录制频道至少1MB+的带宽

## 架构
![架构](https://github.com/AgoraIO/Basic-Recording/blob/master/Agora-Restful-Recording-Nodejs/architecture.png)

## 快速开始
### 背景知识
该项目依赖于你已经了解了如何使用Agora录制SDK。或者再开始之前阅读[这里](https://github.com/AgoraIO/Basic-Recording/tree/master/Agora-LinuxServer-Recording) 

### 下载 Agora 录制 SDK

* 下载[Agora 录制 SDK](https://www.agora.io/en/download/).
* 解压下载的SDK包.
* 把解压后的子文件夹拷贝到目录 `record/src/sdk` 
* 运行`npm install -g node-gyp` 安装编译工具
* 切换到目录 `record`, 运行`build.sh` (运行`build_debug.sh`可编译debug版)。如果一切运行顺利，你会在`record`目录内看到`agorasdk.node`


## 运行录制 Sample
### 快速开始
* 进入目录`record` 
* 确保 `agorasdk.node`已经正确的编译
* 更改目录下文件 `sdkdemo.js`, 替换里面的App ID

```
fs.mkdir(storageDir, {recursive: true}, err => {
	//join channel
	rec.joinChannel(null, "agoratest", 0, YOUR_APP_ID, storageDir);
})
```
* 运行 `node sdkdemo.js`, 你会看到生成的`output`文件夹。该演示Demo仅运行50秒便退出声网频道

### 调整布局
录制的布局通过方法 `setMixedLayout`调整
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
