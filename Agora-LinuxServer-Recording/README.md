# Agora Linux Server Recording

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

## Steps to Create the Sample

The key code for the sample application is in the `main.cpp` file. The most relevant code for the Agora Recording SDK is in the following sections:

- [Add Import Statements](#add-import-statements)
- [Add Global Variables](#add-global-variables)
- [Start and Stop Service Methods](#start-and-stop-service-methods)
- [Create the Main Method](#create-the-main-method)

### Add Import Statements

Add the C++ libraries for variable definitions and streaming.

Library|Description
----|----
`<csignal>`|Signal handling library
`<cstdint>`|Defines a set of integral type aliases
`<iostream>`|Defines the standard input/output stream objects
`<sstream>`|Provides string stream classes
`<string>`|Defines string types, character traits, and a set of converting functions
`<vector>`|Defines the vector container class
`<algorithm>`|Defines a collection of functions designed to be used on ranges of elements

``` c++
#include <csignal>
#include <cstdint>
#include <iostream>
#include <sstream> 
#include <string>
#include <vector>
#include <algorithm>
```

Add the Agora SDK libraries.

Library|Description
----|----
`IAgoraLinuxSdkCommon.h`|Defines Agora variable types and classes
`IAgoraRecordingEngine.h`|Defines the Agora recording engine class
`base/atomic.h`|Defines the Agora namespace
`base/log.h`|Agora logging library
`base/opt_parser.h`|Agora communication helper library
`agorasdk/AgoraSdk.h`|Agora Recording SDK

``` c++
#include "IAgoraLinuxSdkCommon.h"
#include "IAgoraRecordingEngine.h"

#include "base/atomic.h"
#include "base/log.h" 
#include "base/opt_parser.h" 
#include "agorasdk/AgoraSdk.h" 
```

### Add Namespaces and Global Variables

Define standard and Agora classes, and specify namespaces to use in the code.

``` c++
using std::string;
using std::cout;
using std::cerr;
using std::endl;

using agora::base::opt_parser;
using agora::linuxsdk::VideoFrame;
using agora::linuxsdk::AudioFrame;

```

Define global variables to determine signal and service starts and stops.

Variable|Description
---|---
`g_bSignalStop`|Used to define if a signal is stopped
`g_bSignalStartService`|Used to define if a service has started
`g_bSignalStopService`|Used to define if a service has stopped


``` c++
atomic_bool_t g_bSignalStop;
atomic_bool_t g_bSignalStartService;
atomic_bool_t g_bSignalStopService;
```

### Start and Stop Service Methods

The `start_service()` and `stop_service()` methods update the global `g_bSignalStartService` and `g_bSignalStopService` variables.

``` c++
void start_service(int signo) {
    (void)signo;
    g_bSignalStartService = true;
}

void stop_service(int signo) {
    (void)signo;
    g_bSignalStopService = true;
}
```

### Create the Main Method

The `main()` function is called when the application initializes.

``` c++
int main(int argc, char * const argv[]) {
  
  ...
  
}
```

- [Define Variables](#define-variables)
- [Set the Signal Event Handlers](#set-the-signal-event-handlers)
- [Set Up the Parser Object](#set-up-the-parser-object)
- [Check Configuration Settings](#check-configuration-settings)
- [Set the Recording Configuration](#set-the-recording-configuration)
- [Set Up the Agora Recorder](#set-up-the-agora-recorder)

#### Define Variables

Define Agora variables for the Agora SDK engine.

Variable|Description
----|----
`uid`|User ID
`appId`|App ID
`channelKey`|Channel key
`name`|Channel name
`channelProfile`|Channel profile

``` c++
  uint32_t uid = 0;
  string appId;
  string channelKey;
  string name;
  uint32_t channelProfile = 0;
```

Define the decryption mode `decryptionMode` and decryption key `secret`.

``` c++
  string decryptionMode;
  string secret;
```

Define the resolution for the video mix and the idle time limit (in seconds).

``` c++
  string mixResolution("360,640,15,500");

  int idleLimitSec=5*60;//300s
```

Define the paths for the application.

Variable|Description
---|---
`applitePath`|Application path
`appliteLogPath`|Path to save application logs
`recordFileRootDir`|Directory to save the recording files
`cfgFilePath`|Path to the configuration file
`proxyServer`|IP and port for the proxy server

``` c++
  string applitePath;
  string appliteLogPath;
  string recordFileRootDir = "";
  string cfgFilePath = "";
  string proxyServer;
```

Define the variables for the low and high UDP ports.

``` c++
  int lowUdpPort = 0;//40000;
  int highUdpPort = 0;//40004;
```

Define the audio, video, and mixing variables for the settings in the Agora parser.

``` c++
  bool isAudioOnly=0;
  bool isVideoOnly=0;
  bool isMixingEnabled=0;
  bool mixedVideoAudio=0;
```

Define the audio, video, and stream format types.

``` c++
  uint32_t getAudioFrame = agora::linuxsdk::AUDIO_FORMAT_DEFAULT_TYPE;
  uint32_t getVideoFrame = agora::linuxsdk::VIDEO_FORMAT_DEFAULT_TYPE;
  uint32_t streamType = agora::linuxsdk::REMOTE_VIDEO_STREAM_HIGH;
```

Define the video snapshot interval `captureInterval` (in seconds). Set the trigger mode to automatic.

``` c++
  int captureInterval = 5;
  int triggerMode = agora::linuxsdk::AUTOMATICALLY_MODE;
```

Define the following recording configuration parameters: width, height, frames per second, and KB per second.

``` c++
  int width = 0;
  int height = 0;
  int fps = 0;
  int kbps = 0;
```

Define signal variables, which track the communication signal of the recording.

``` c++
  g_bSignalStop = false;
  g_bSignalStartService = false;
  g_bSignalStopService = false;
```

#### Set the Signal Event Handlers

Use the `signal()` method to set signal event handlers.

Event|Description
---|---
`SIGQUIT`|The signal is terminated.
`SIGABRT`|The signal is aborted.
`SIGINT`|The signal is interrupted.
`SIGPIPE`|Broken pipe signal. Passing `SIG_IGN` as a handler ignores the broken pipe signal.

``` c++
  signal(SIGQUIT, signal_handler);
  signal(SIGABRT, signal_handler);
  signal(SIGINT, signal_handler);
  signal(SIGPIPE, SIG_IGN);
```

#### Set Up the Parser Object

Define the `parser` object, using the `parser.add_long_opt()` method to apply the app ID, user ID, channel name, application path, channel key, and channel profile. 

``` c++
  opt_parser parser;

  parser.add_long_opt("appId", &appId, "App Id/must", agora::base::opt_parser::require_argu);
  parser.add_long_opt("uid", &uid, "User Id default is 0/must", agora::base::opt_parser::require_argu);

  parser.add_long_opt("channel", &name, "Channel Id/must", agora::base::opt_parser::require_argu);
  parser.add_long_opt("appliteDir", &applitePath, "directory of app lite 'AgoraCoreService', Must pointer to 'Agora_Recording_SDK_for_Linux_FULL/bin/' folder/must",
          agora::base::opt_parser::require_argu);

  parser.add_long_opt("channelKey", &channelKey, "channelKey/option");
  parser.add_long_opt("channelProfile", &channelProfile, "channel_profile:(0:COMMUNICATION),(1:broadcast) default is 0/option");
```

Set the audio, video, and mix settings.

``` c++
  parser.add_long_opt("isAudioOnly", &isAudioOnly, "Default 0:A/V, 1:AudioOnly (0:1)/option");
  parser.add_long_opt("isVideoOnly", &isVideoOnly, "Default 0:A/V, 1:VideoOnly (0:1)/option");
  parser.add_long_opt("isMixingEnabled", &isMixingEnabled, "Mixing Enable? (0:1)/option");
  parser.add_long_opt("mixResolution", &mixResolution, "change default resolution for vdieo mix mode/option");
  parser.add_long_opt("mixedVideoAudio", &mixedVideoAudio, "mixVideoAudio:(0:seperated Audio,Video) (1:mixed Audio & Video), default is 0 /option");
```

Set the decryption mode and decryption key `secret`.

``` c++
  parser.add_long_opt("decryptionMode", &decryptionMode, "decryption Mode, default is NULL/option");
  parser.add_long_opt("secret", &secret, "input secret when enable decryptionMode/option");
```

Apply the idle time limit and root directory for the recording files.

``` c++
  parser.add_long_opt("idle", &idleLimitSec, "Default 300s, should be above 3s/option");
  parser.add_long_opt("recordFileRootDir", &recordFileRootDir, "recording file root dir/option");
```

Set the low and high UDP ports.

``` c++
  parser.add_long_opt("lowUdpPort", &lowUdpPort, "default is random value/option");
  parser.add_long_opt("highUdpPort", &highUdpPort, "default is random value/option");
```

Apply the audio and video frame settings.

``` c++
  parser.add_long_opt("getAudioFrame", &getAudioFrame, "default 0 (0:save as file, 1:aac frame, 2:pcm frame, 3:mixed pcm frame) (Can't combine with isMixingEnabled) /option");
  parser.add_long_opt("getVideoFrame", &getVideoFrame, "default 0 (0:save as file, 1:h.264, 2:yuv, 3:jpg buffer, 4:jpg file, 5:jpg file and video file) (Can't combine with isMixingEnabled) /option");
```

Set the video snapshot interval, configuration file path, and proxy server.

``` c++
  parser.add_long_opt("captureInterval", &captureInterval, "default 5 (Video snapshot interval (second))");
  parser.add_long_opt("cfgFilePath", &cfgFilePath, "config file path / option");
  parser.add_long_opt("proxyServer", &proxyServer, "proxyServer:format ip:port, eg,\"127.0.0.1:1080\"/option");
```

Set the video stream type and trigger mode.

``` c++
  parser.add_long_opt("streamType", &streamType, "remote video stream type(0:STREAM_HIGH,1:STREAM_LOW), default is 0/option");
  parser.add_long_opt("triggerMode", &triggerMode, "triggerMode:(0: automatically mode, 1: manually mode) default is 0/option");
```

#### Check Configuration Settings

Ensure the `parser` settings, `appID`, and channel `name` are all valid. If any of these are invalid, terminate the application.

``` c++
  if (!parser.parse_opts(argc, argv) || appId.empty() || name.empty()) {
    std::ostringstream sout;
    parser.print_usage(argv[0], sout);
    cout<<sout.str()<<endl;
    return -1;
  }
```

If the trigger mode is set to manual, add additional signal event listeners to start and stop the service.
 
``` c++
  if(triggerMode == agora::linuxsdk::MANUALLY_MODE) {
      signal(SIGUSR1, start_service);
      signal(SIGUSR2, stop_service);
  }
```

Check if the recording file directory and configuration file path are empty:

If the directories are not empty, log an error using the `LOG()` method and terminate the application.


``` c++
  if(!recordFileRootDir.empty() && !cfgFilePath.empty()){
    LOG(ERROR,"Client can't set both recordFileRootDir and cfgFilePath");
    return -1;
  }
```

If the directories are empty and mixing is enabled, set the mix resolution. If the resolution is set to a valid resolution, terminate the application.

``` c++
  if(recordFileRootDir.empty() && cfgFilePath.empty())
      recordFileRootDir = ".";

  //Once recording video under video mixing model, client needs to config width, height, fps and kbps
  if(isMixingEnabled && !isAudioOnly) {
     if(4 != sscanf(mixResolution.c_str(), "%d,%d,%d,%d", &width,
                  &height, &fps, &kbps)) {
        LOG(ERROR, "Illegal resolution: %s", mixResolution.c_str());
        return -1;
     }
  }
```

#### Set the Recording Configuration

Add a log to track the users that join the channel, and define the Agora SDK `recorder` and configuration.

``` c++
  LOG(INFO, "uid %" PRIu32 " from vendor %s is joining channel %s",
          uid, appId.c_str(), name.c_str());
          
  agora::AgoraSdk recorder;
  agora::recording::RecordingConfig config;
```

Set the idle time limit and channel profile.

``` c++
  config.idleLimitSec = idleLimitSec;
  config.channelProfile = static_cast<agora::linuxsdk::CHANNEL_PROFILE_TYPE>(channelProfile);
```

Apply the video, audio, and mix settings.
  
``` c++
  config.isVideoOnly = isVideoOnly;
  config.isAudioOnly = isAudioOnly;
  config.isMixingEnabled = isMixingEnabled;
  config.mixResolution = (isMixingEnabled && !isAudioOnly)? const_cast<char*>(mixResolution.c_str()):NULL;
  config.mixedVideoAudio = mixedVideoAudio;
```

Set the application directory, recording file directory, configuration file path, and proxy server.

``` c++
  config.appliteDir = const_cast<char*>(applitePath.c_str());
  config.recordFileRootDir = const_cast<char*>(recordFileRootDir.c_str());
  config.cfgFilePath = const_cast<char*>(cfgFilePath.c_str());
  config.proxyServer = proxyServer.empty()? NULL:const_cast<char*>(proxyServer.c_str());
```

Set the decryption mode and decryption key `secret`.

``` c++
  config.secret = secret.empty()? NULL:const_cast<char*>(secret.c_str());
  config.decryptionMode = decryptionMode.empty()? NULL:const_cast<char*>(decryptionMode.c_str());
```

Set the low and high UDP ports and the video capture interval.

``` c++
  config.lowUdpPort = lowUdpPort;
  config.highUdpPort = highUdpPort;
  config.captureInterval = captureInterval;
```

Set the audio, video, and stream format types and the trigger mode.

``` c++
  config.decodeAudio = static_cast<agora::linuxsdk::AUDIO_FORMAT_TYPE>(getAudioFrame);
  config.decodeVideo = static_cast<agora::linuxsdk::VIDEO_FORMAT_TYPE>(getVideoFrame);
  config.streamType = static_cast<agora::linuxsdk::REMOTE_VIDEO_STREAM_TYPE>(streamType);
  config.triggerMode = static_cast<agora::linuxsdk::TRIGGER_MODE_TYPE>(triggerMode);
```

#### Set Up the Agora Recorder

Set the mix mode for the `recorder` and create an Agora video channel using the `recorder.createChannel()` method. If the channel creation fails, terminate the application.

``` c++
  recorder.updateMixModeSetting(width, height, isMixingEnabled ? !isAudioOnly:false);

  if (!recorder.createChannel(appId, channelKey, name, uid, config)) {
    cerr << "Failed to create agora channel: " << name << endl;
    return -1;
  }
```

Update the recording storage directory.

``` c++
  cout << "Recording directory is " << recorder.getRecorderProperties()->storageDir << endl;
  recorder.updateStorageDir(recorder.getRecorderProperties()->storageDir);
```

While the `recorder` is running, update the signal service using `recorder.startService()` and `recorder.stopService()`.

``` c++
  while (!recorder.stopped() && !g_bSignalStop) {
      if(g_bSignalStartService) {
          recorder.startService();
          g_bSignalStartService = false;
      }

      if(g_bSignalStopService) {
          recorder.stopService();
          g_bSignalStopService = false;
      }

      sleep(1);
  }
```

Once the signal stops, leave the channel using `recorder.leaveChannel()` and release the `recorder` object.

``` c++
  if (g_bSignalStop) {
    recorder.leaveChannel();
    recorder.release();
  }

  cerr << "Stopped \n";
  return 0;
```

## Resources
- See full API documentation in the [Document Center](https://docs.agora.io/en/)
- [File bugs about this sample](https://github.com/AgoraIO/Basic-Recording/issues)
- See [detailed Agora Linux Recording guides](https://docs.agora.io/en/2.3.1/addons/Recording/Quickstart%20Guide/recording_cpp?platform=C%2B%2B)

## License
This software is licensed under the MIT License (MIT). [View the license](LICENSE.md).
