/**
 * Created by cq on 12/02/2019.
 */

package io.agora.cloud_recording.client;

import io.agora.cloud_recording.CloudRecorder;
import io.agora.cloud_recording.ICloudRecordingEventHandler;
import io.agora.cloud_recording.RecordingErrorCode;
import io.agora.cloud_recording.DecryptionMode;
import io.agora.cloud_recording.RecordingStreamType;
import io.agora.cloud_recording.VideoStreamType;
import io.agora.cloud_recording.ChannelType;
import io.agora.cloud_recording.AudioProfile;
import io.agora.cloud_recording.MixedVideoLayoutType;
import io.agora.cloud_recording.CloudStorageVendor;
import io.agora.cloud_recording.CloudStorageConfig;
import io.agora.cloud_recording.RecordingConfig;


import java.util.logging.Logger;
import java.util.Scanner;

import org.apache.commons.cli.CommandLineParser;  
import org.apache.commons.cli.DefaultParser;  
import org.apache.commons.cli.Options; 
import org.apache.commons.cli.ParseException;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.HelpFormatter;


public class Client {
    private CloudRecorder cloud_recorder;
    private volatile boolean recording_started = false;
    private volatile boolean stopped_ = false;

    private Options options = new Options();
    private HelpFormatter help = new HelpFormatter();
    private static Logger logger = Logger.getLogger("Client");

    private void printHelp() {
        help.printHelp("Cloud Recording Java Demo", options);
    }

    private void printTypeHelp() {
        System.out.println("Type \"stop\" to stop recording!");
        System.out.println("Type \"quit\" to release and exit recording!");
    }

    private int getIntOptionValue(CommandLine line, String opt, int default_value) {
        String arg = line.getOptionValue(opt);
        if (arg == null) {
            return default_value;
        }
        try {
            int value = Integer.parseInt(arg);
            if (value < 0) {
                value = default_value;
            }
            return value;
        } catch(NumberFormatException e) {
            printHelp();
            System.exit(0);
        }
        return -1;
    }

    private boolean checkChannelParam(String app_id, String channel_name, int uid) {
        if (app_id.length() == 0) {
            System.out.println("Invalid app id.");
            return false;
        }
        if (channel_name.length() == 0) {
            System.out.println("Invalid channel name");
            return false;
        }
        if (uid == 0) {
            System.out.println("uid can't be 0.");
            return false;
        }
        return true;
    }

    private boolean checkConfigParam(int stream_type, int channel_type, int audio_profile,
      int mixed_video_layout, int decryption_mode, int video_stream_type) {
        if (stream_type > RecordingStreamType.values().length) {
            System.out.println(String.format("Invalid stream type, it must be less or equal to %d.",
              RecordingStreamType.values().length - 1));
            return false;
        }
        if (channel_type > ChannelType.values().length) {
            System.out.println(String.format("Invalid channel type, it must be less or equal to %d.", 
              ChannelType.values().length - 1));
            return false;
        }
        if (audio_profile > AudioProfile.values().length) {
            System.out.println(String.format("Invalid audio profile, it must be less or equal to %d.", 
              AudioProfile.values().length - 1));
            return false;
        }
        if (mixed_video_layout > MixedVideoLayoutType.values().length) {
            System.out.println(String.format("Invalid mixed video layout. it must be less or equal to %d.",
              MixedVideoLayoutType.values().length - 1));
            return false;
        }
        if (decryption_mode > DecryptionMode.values().length) {
            System.out.println(String.format("Invalid decryption mode. it must be less or equal to %d.",
              DecryptionMode.values().length - 1));
            return false;
        }
        if (video_stream_type > VideoStreamType.values().length) {
            System.out.println(String.format("Invalid video stream type. it must be less or equal to %d.",
              VideoStreamType.values().length - 1));
            return false;
        }
        return true;
    }

    private boolean checkStorageParam(int vendor, int region, String bucket, String secret_key, String access_key) {
        if (vendor > CloudStorageVendor.values().length) {
            System.out.println(String.format("Invalid vendor id. it must be less or equal to %d.",
              CloudStorageVendor.values().length - 1));
            return false;
        }
        if (bucket.length() == 0) {
            System.out.println("Invalid bucket.");
            return false;
        }
        if (secret_key.length() == 0) {
            System.out.println("Invalid secret_key.");
            return false;
        }
        if (access_key.length() == 0) {
            System.out.println("Invalid access_key.");
            return false;
        }
        return true;
    }

    private void run(String[] args) throws ParseException {
        CommandLineParser parser = new DefaultParser();
        CommandLine line = null;

        System.out.println(cloud_recorder.getRecordingId());
        options.addOption("h", "help", false, "help");
        options.addOption("a", "appId", true, "app id");
        options.addOption("n", "channelName", true, "channel name");
        options.addOption("u", "uid", true, "User id");
        options.addOption("t", "token", true, "channel token/Optional");
        options.addOption("r", "recordingStreamType", true, "stream types (Optional:" + 
          " 0 for audio only,  1 for video only, 2 for audio and video, default 2)");
        options.addOption("v", "videoStreamType", true, "video stream type(Optional:" +
          " 0 for high video, 1 for low video, default 0. Only works when recordingStreamType is not 0.)");
        options.addOption("d", "decryption_mode", true, "decryption mode(Optional:" + 
          ", 0 for none, 1 for aes-128-xts, 2 for aes-128-ecb, 3 for aes-256-xts, no decryption by default.");
        options.addOption("e", "secret", true, "secret(Optional, emptry by default");
        options.addOption("c", "channelType", true, "channel type (Optional 0 for communication, 1 for live, default 0");
        options.addOption("A", "audioProfile", true, "audio profile, (Optional:" + 
          " 0 for single channel mono, 1 for single channel music, 2 for multi channel music. default 0)");
        options.addOption("W", "mixWidth",true, "transcoding mixing width (Optional, defualt 360)");
        options.addOption("H", "mixHeight", true, "transcoding mixing height (Optional, default 640)");
        options.addOption("f", "fps", true, "transcoding fps(Optional, default 15)");
        options.addOption("b", "bitrate", true, "transcoding bitrate(Optional, 500 by default");
        options.addOption("U", "maxResolutionUid", true, "transcoding max resolution uid(Optional)");
        options.addOption("l", "mixedVideoLayoutType", true, "mixed video layout mode(Optional:" + 
          ", 0 for float, 1 for BestFit, 2 for vertical, 0 by defualt)");
        options.addOption("i", "maxIdleTime", true, "max idle time(Optional, 30 by default)");
        options.addOption("V", "vendor", true, "cloud storage vendor(Optional, 0 for Qiniu, 1 for AWS, 2 for Aliyun");
        options.addOption("R", "region", true, "cloud storage region");
        options.addOption("B", "bucket", true, "cloud storage bucket");
        options.addOption("AK", "accessKey", true, "cloud storage access key");
        options.addOption("SK", "secretKey", true, "cloud storage secret key");

        line = parser.parse(options, args);
        if (args == null || args.length == 0 || line.hasOption("h")) {
            printHelp();
            System.exit(0);
        }

        String app_id = line.getOptionValue("appId", "");
        String channel_name = line.getOptionValue("channelName", "");
        String token = line.getOptionValue("token", "");
        String secret = line.getOptionValue("secret", "");
        String bucket = line.getOptionValue("bucket", "");
        String access_key = line.getOptionValue("accessKey", "");
        String secret_key = line.getOptionValue("secretKey", "");

        int uid = getIntOptionValue(line, "uid", 0);
        int max_resolution_uid = getIntOptionValue(line, "maxResolutionUid", 0);
        int region = getIntOptionValue(line, "region", 0);;
        int width = getIntOptionValue(line, "mixWidth", 360);
        int height = getIntOptionValue(line, "mixHeight", 640);
        int fps = getIntOptionValue(line, "fps", 15);
        int bitrate = getIntOptionValue(line, "bitrate", 500);
        int max_idle_time = getIntOptionValue(line, "maxIdleTime", 30);
        int decryption_mode = getIntOptionValue(
          line, "decryption_mode", DecryptionMode.DecryptionModeNone.ordinal());
        int recording_stream_types = getIntOptionValue(
          line, "recordingStreamType", RecordingStreamType.StreamTypeAudioVideo.ordinal());
        int video_stream_type = getIntOptionValue(
          line, "videoStreamType", VideoStreamType.VideoStreamTypeHigh.ordinal());
        int channel_type = getIntOptionValue(
          line, "channelType", ChannelType.ChannelTypeCommunication.ordinal());
        int audio_profile = getIntOptionValue(
          line, "audioProfile", AudioProfile.AudioProfileMusicStandard.ordinal());
        int mixed_video_layout = getIntOptionValue(
          line, "mixedVideoLayoutType", MixedVideoLayoutType.MixedVideoLayoutTypeFloat.ordinal());
        int vendor = getIntOptionValue(
          line, "vendor", CloudStorageVendor.CloudStorageVendorQiniu.ordinal());

        if (checkChannelParam(app_id, channel_name, uid) == false ||
          checkConfigParam(recording_stream_types, channel_type, audio_profile,
                           mixed_video_layout, decryption_mode, video_stream_type) == false ||
          checkStorageParam(vendor, region, bucket, secret_key, access_key) == false) {
            printHelp();
            System.exit(0);
        }

        CloudStorageConfig storage_config = new CloudStorageConfig();
        storage_config.vendor = CloudStorageVendor.values()[vendor];
        storage_config.region = region;
        storage_config.bucket = bucket;
        storage_config.accessKey = access_key;
        storage_config.secretKey = secret_key;

        RecordingConfig recording_config = new RecordingConfig();
        recording_config.recordingStreamType = RecordingStreamType.values()[recording_stream_types];
        recording_config.decryptionMode = DecryptionMode.values()[decryption_mode];
        recording_config.secret = secret;
        recording_config.channelType = ChannelType.values()[channel_type];
        recording_config.audioProfile = AudioProfile.values()[audio_profile];
        recording_config.videoStreamType = VideoStreamType.values()[video_stream_type];
        recording_config.maxIdleTime = max_idle_time;
        recording_config.setTranscodingWidth(width);
        recording_config.setTranscodingHeight(height);
        recording_config.setTranscodingFps(fps);
        recording_config.setTrancodingBitrate(bitrate);
        recording_config.setTranscodingMixedVideoLayout(MixedVideoLayoutType.values()[mixed_video_layout], max_resolution_uid);

        if (cloud_recorder.startCloudRecording(app_id, channel_name, token, 
              uid, recording_config, storage_config) != RecordingErrorCode.RecordingErrorOk) {
            stopped_ = true;
        }

        Scanner scn = new Scanner(System.in);
        while (stopped_ == false) {
            System.out.println("please input...");
            String input = scn.nextLine();
            if (input.equals("quit")) {
                stopped_ = true;
            } else if (input.equals("stop")) {
                if (cloud_recorder.stopCloudRecording() != RecordingErrorCode.RecordingErrorOk) {
                    System.out.println("Stop recording failed...");
                }       
            } else if(input.equals("help")) {
                printTypeHelp(); 
            } else{ 
                System.out.println("Undefined command:" + input + "  Try \"help\"");
            }
            try {
                Thread.currentThread().sleep(5);//sleep 5ms
            } catch (InterruptedException ie) {
                System.out.println("exception throwed!");
            }
        }
        if (cloud_recorder.release(false) != RecordingErrorCode.RecordingErrorOk) {
            System.out.println("Release failed...");
        }
        System.out.println("Stopped...");
    }

    public Client() {
        ICloudRecordingEventHandler handler = new ICloudRecordingEventHandler() {
            public void onRecordingConnecting(String recording_id) {
                logger.info("onRecordingConnecting " + recording_id);
            }
            public void onRecordingStarted(String recording_id) {
                recording_started = true;
                logger.info("onRecordingStarted " + recording_id);
            }
            public void onRecordingStopped(String recording_id) {
                logger.info("onRecordingStopped " + recording_id);
            }
            public void onRecordingUploaded(String recording_id, String file_name) {
                stopped_ = true;
                logger.info("onRecordingUploaded " + recording_id + " " + file_name);
            }
            public void onRecordingBackedUp(String recording_id, String file_name) {
                stopped_ = true;
                logger.info("onRecordingBackedUp " + recording_id + " " + file_name);
            }
            public void onRecordingUploadingProgress(String recording_id,
              int progress, String recording_playlist_filename) {
                logger.info("onRecordingUploadingProgress " + recording_id + " " + progress
                  + " " + recording_playlist_filename);
            }
            public void onRecorderFailure(String recording_id,
              RecordingErrorCode code, String msg) {
                logger.info("onRecorderFailure " + recording_id + " " + code + " " + msg);
            }
            public void onUploaderFailure(String recording_id,
              RecordingErrorCode code, String msg) {
                logger.info("onUploaderFailure " + recording_id + " " + code + " " + msg);
            }
            public void onRecordingFatalError(String recording_id,
              RecordingErrorCode code) {
                stopped_ = true;
                logger.info("onRecordingFatalError " + recording_id + " " + code);
            }
        };

        cloud_recorder = CloudRecorder.createCloudRecorderInstance(handler);
    }

    public static void main(String[] args) {
        logger.info("Client Started!");
        Client client_demo = new Client();
        try {
            client_demo.run(args);
        } catch(ParseException e) {
            e.printStackTrace();
            client_demo.printHelp();
        }
    }
}
