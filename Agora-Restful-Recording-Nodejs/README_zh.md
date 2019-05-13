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
录制的布局通过方法 `setMixedLayout`调整。关于布局，详情参考:[video_mixing_layout](https://docs.agora.io/cn/Recording/API%20Reference/recording_cpp/structagora_1_1linuxsdk_1_1_video_mixing_layout.html)

当有用户加入频道或者频道内用户退出时都会更新布局

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

## 运行Restful Server
### 快速开始
* 确保 `agorasdk.node` 在 `record` 已经被正确编译
* 切换到`server` 目录
* 运行 `npm install`安装依赖库
* 运行 `node app.js` 

### 预定义 APIs
#### 开始录制

- `http://localhost:3000/recorder/v1/start`
  
方法:

- POST

参数:
 

|参数名|是否必须|类型|描述|
|:----    |:---|:----- |-----   |
|appid |是  |string |Agora AppId |
|channel |是  |string | 频道名    |
|key     |否 |string |取决于你是否开启了App ID Certificate  |

参考：
```
curl -i -X POST -H "Content-type: application/json" --data '{"appid":"XXXXX","channel":"XXXX","key":"XXXX"}' 127.0.0.1:3000/recorder/v1/start 
```

Sample 应答:

```
{
    "success": true,
    "sid": "ec8711fb-cd98-4411-a430-a0e8de2c6e98"
}
```

应答 属性:

|名称|类型|描述|
|:----|:----- |-----   |
|success  |bool |执行结果   |
|sid  |string | 录制sessionId，用于结束录制 |

#### 结束录制

- `http://localhost:3000/recorder/v1/stop`
  
方法:

- POST

参数:
 

|名称|是否必须|类型|描述|
|:----    |:---|:----- |-----   |
|sid |是  |string |开启录制时的sessionId  |

Sample 应答:

```
{
    "success": true
}
```

应答 属性:

|名称|类型|描述|
|:----|:----- |-----   |
|success  |bool |执行结果   |

## 联系我们
- 完整的 API 文档见 [文档中心](https://docs.agora.io/en/)
- 如果在集成中遇到问题, 你可以到[开发者社区](https://github.com/AgoraIO/Basic-Recording/issues)提问
- 详情请参考 [录制快速入门](https://docs.agora.io/en/2.3.1/addons/Recording/Quickstart%20Guide/recording_cpp?platform=C%2B%2B)

