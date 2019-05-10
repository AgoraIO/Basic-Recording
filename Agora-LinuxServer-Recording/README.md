# Agora Linux Server Recording

*Read this in other languages: [中文](README_zh.md)*

This sample application for the Agora Recording SDK enables recording on your Linux server.

## Prerequisites
- Ubuntu 12.04+ x64 or CentOS 6.5+ x64 (CentOS 7+ recommended)
- GCC 4.4+
- ARS IP (public IP)
- 1MB+ bandwidth for each simultaneous recording channel
- Server access for `qos.agoralab.co` 

	**Note:** If server access is denied, the Agora SDK may fail to transfer the required data.

## Quick Start

This section shows you how to prepare and build the sample application.

- [Create an Account and Obtain an App ID](#create-an-account-and-obtain-an-app-id)
- [Integrate the Agora Video SDK](#integrate-the-agora-video-sdk)
- [Build the Sample Application](#build-the-sample-application) 


### Create an Account and Obtain an App ID
To build and run the sample application, first obtain an app ID: 

1. Create a developer account at [agora.io](https://dashboard.agora.io/signin/). Once you finish the sign-up process, you are redirected to the dashboard.
2. In the dashboard tree on the left, navigate to **Projects** > **Project List**.
3. Save the app ID from the Agora dashboard. The app ID is needed to run the sample application.

### Integrate the Agora Video SDK

To open and build the sample application, first integrate the Agora SDK. 

1. Download the [Agora Recording SDK for Linux](https://www.agora.io/en/download/).
2. Unzip the downloaded SDK package.
3. Copy the following files/folders from the downloaded SDK package to the sample application:

	SDK Path|Sample Application Path
	----|----
	`bin/AgoraCoreService` file|Copy to`bin` folder
	`libs/librecorder.a` file|Copy to `libs` folder
	`samples/agorasdk` folder|Copy to `samples` folder
	`samples/cpp` folder|Copy to `samples` folder
	`samples/java` folder| Copy to `samples` folder

4. Replace the following folders in the sample application with the folders from the downloaded SDK package.

	- `include` folder
	- `tools` folder
	- `base` folder

5. To add the required `libRecordEngine.a` file, decompress the `libRecordEngine.tar.gz` file in the `libs` directory of your sample application. 


### Build the Sample Application 

1. Ensure the following UDP ports are open. (Required for communication with the Agora servers.)
	- 4001
	- 4030
	- 1080
	- 8000
	- 25000
2. Ensure local ports are not limited by firewalls. If limitations cannot be avoided, apply the following port settings:

	Port type|Setting
	----|----
	Receiving port|4000 - 41000
	Low UDP Port|40000
	High UDP Port|41000

3. Navigate to the `samples/cpp` directory. Replace `PATH-TO-PROJECT` with the path to the sample application on your Linux server.

	```
	cd PATH-TO-PROJECT/samples/cpp
	```
	
4. Execute the `make` command to create a `record_local` application within the directory.

	```
	make
	```

5. Run `./record_local`, using the app ID and path to the sample app directory.
	- Replace `APPID` with the app ID from the [earlier step](#create-an-account-and-obtain-an-app-id).
	- Replace `PATH-TO-PROJECT` with the path to the sample app directory on the Linux server.

	```
	./recorder_local --appId APPID --uid 0 --channel mychannel --appliteDir PATH-TO-PROJECT/bin/AgoraCoreService
	```

**Note:** To view additional Agora SDK help commands, run `cmds`.

```
cmds ./Record_local
```	

## Resources
- A detailed code walkthrough for this sample is available in [Steps to Create this Sample](./guide.md).
- See full API documentation in the [Document Center](https://docs.agora.io/en/)
- [File bugs about this sample](https://github.com/AgoraIO/Basic-Recording/issues)
- See [detailed Agora Linux Recording guides](https://docs.agora.io/en/Recording/recording_integrate_cpp?platform=CPP)

## License
This software is licensed under the MIT License (MIT). [View the license](LICENSE.md).
