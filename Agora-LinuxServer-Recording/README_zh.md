
# Agora-LinuxServer-Recording

*Read this in other languages: [English](README.md)*

## 准备工作

请确保满足以下操作系统要求:
- Ubuntu 12.04 x64 或更高版本
- CentOS 6.5 x64 或更高版本
- gcc 4.4 或更高版本。
- 与公网互通，并能够访问`qos.agoralab.co`

## 快速开始

### 创建Agora账号并获取App ID
- 在[Agora开发者中心](https://dashboard.agora.io/cn/signup/) 注册账号
- 在[我的主页]-->[项目管理]创建自己的项目，获取到 AppID
- 该AppID用于运行Agora应用程序
 
 **Note：** 该AppID用于运行你的Agora项目，请妥善保存，勿外泄。

### 下载Agora Recording SDK
- 下载[录制SDK](https://docs.agora.io/cn/Agora%20Platform/downloads)
- 解压下载的SDK包
- 将解压后的文件/文件夹拷贝到相应的项目应用中：

	SDK 路径|Sample 路径
	----|----
	`bin/AgoraCoreService` 文件|拷贝到`bin` 文件夹
	`libs/librecorder.a` 文件|拷贝到 `libs` 文件夹
	`samples/agorasdk` 文件夹|拷贝到 `samples` 文件夹
	`samples/base` 文件夹|拷贝到 `samples` 文件夹
	`samples/cpp` 文件夹|拷贝到 `samples` 文件夹
	`samples/java` 文件夹| 拷贝到 `samples` 文件夹
	
- 将下面项目中下面的文件替换成下载的SDK包里的文件
  - `include` 文件夹
  - `tools` 文件夹
  - `base` 文件夹

### 编译运行

1. 确保以下的UDP端口是打开的(这些端口用来与Agora服务器之间通信)
 - 4001
 - 4030
 - 1080
 - 8000
 - 25000
 
 2. 确保本地端口没有被防火墙禁掉。如果启用了防火墙，则需要放开Agora录制SDK设定的端口范围。你可以为多个录制进程统一配置较大的端口范围（Agora 建议 40000 ~ 41000 或更大）。此时，录制 SDK 会在指定范围内分配录制端口。设置端口范围，你需要配置参数 lowUdpPort 和highUdpPort。
	
	端口类型|范围
	----|----
	接收码流端口|40000 - 41000
	Low UDP Port|40000
	High UDP Port|41000
	
3. 进入`samples/cpp`目录下，执行以下命令进行编译,编译成功后你讲看到`record_local`可执行程序
       `make`

4. 运行`./record_local`，即可看到相关用法，填上相应的参数即可进行录制
	```
	./recorder_local --appId APPID --uid 0 --channel mychannel --appliteDir PATH-TO-PROJECT/bin/AgoraCoreService
	```

## 快速通道
**当前sample**
 - 将`libs`文件夹内的`libRecordEngine.tar.gz`解压
 - 进入`samples`,执行`make`编译，然后运行编译生成的`./Record_local`, 即可看到相关用法

**解压下载的最新版[录制SDK](https://docs.agora.io/cn/Agora%20Platform/downloads)**
- C++版：进入`samples/cpp`后，执行`make`编译，然后运行编译生成的`./record_local`, 即可看到相关用法
- Java版：进入`samples/java`后，按照文件夹中的ReadMe编译，编译生成的文件在目录`samples/java/bin`中，进入该目录执行`java -Djava.library.path=io/agora/recording/ RecordingSample`，即可看到相关用法

## 联系我们

- 详细信息请参考 [录制快速入门](https://docs.agora.io/cn/Recording/product_recording?platform=All%20Platforms)
- 完整的 API 文档见 [文档中心](https://docs.agora.io/cn/)
- 如果在集成中遇到问题, 你可以到 [开发者社区](https://dev.agora.io/cn/) 提问
- 如果有售前咨询问题, 可以拨打 400 632 6626，或加入官方Q群 12742516 提问
- 如果需要售后技术支持, 你可以在 [Agora Dashboard](https://dashboard.agora.io) 提交工单
