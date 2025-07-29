//
//  alirtc_iot_sdk.h
//  alirtc_iot_sdk
//
//  Created by alibaba on 2021/2/26.
//  Copyright © 2021 mt. All rights reserved.
//
#ifndef alirtc_iot_sdk_h
#define alirtc_iot_sdk_h

#include "alirtc_iot_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
    #ifdef _BUILDING_ALIRTC_IOT_SDK_
        #define __alirtc_api__ __declspec(dllexport)
    #else
        #define __alirtc_api__ __declspec(dllimport)
    #endif
#elif defined(__GNUC__)
    #define __alirtc_api__ __attribute__((visibility("default")))
#else
    #define __alirtc_api__
#endif

/**
 * Get SDK version.
 *
 * @return A const static string describes the SDK version
 */
extern __alirtc_api__ const char* alirtc_get_version(void);
/**
 * Set the log level.
 *
 * @param[in] level Log level. Range reference to alirtc_log_level_e enum
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_rtc_set_log_level(alirtc_log_level_e level);
/**
 * Sets the log file configuration.
 *
 * @param[in] size_per_file The size (bytes) of each log file.
 *                             The value range is [0, 10*1024*1024], default 10*1024*1024(1M).
 *                             0 means set log off
 * @param[in] max_file_count      The maximum number of log file numbers.
 *                               The value range is [0, 100], default 10.
 *                               0 means set log off
 *
 * @return
 * - 0: Success.
 * - <0: Failure.
 */
extern __alirtc_api__ int32_t alirtc_config_log(int32_t size_per_file, int32_t max_file_count);
/**
 * Initialize the SDK service.
 *
 * @note Each process can only be initialized once.
 *
 * @param[in] event_handler   see #alirtc_event_handler_t
 * @param[in] sdk_log_dir     Directory for storing SDK log
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_init(alirtc_init_config_t *config, alirtc_event_handler_t* event_handler, const char *sdk_log_dir);
/**
 * Release all resource allocated by SDK
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_fini(void);
/**
 * Local user joins channel.
 *
 * @note Users in the same channel with the same App ID can send data to each other.
 *       You can join more than one channel at the same time. All channels that
 *       you join will receive the audio/video data stream that you send unless
 *       you stop sending the audio/video data stream in a specific channel.
 *
 * @param[in] config   see #alirtc_channel_config_t
 *
 * @param[in] options see #alirtc_channel_options_t
 *
 * @param[in] user_data defined by user
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_join_channel(alirtc_channel_config_t *config, alirtc_channel_options_t *options, void *user_data);
/**
 * Local user joins channel to communicate with the AI agent.
 *
 * @note Users in the same channel with the same App ID can send data to each other.
 *       You can join more than one channel at the same time. All channels that
 *       you join will receive the audio/video data stream that you send unless
 *       you stop sending the audio/video data stream in a specific channel.
 *
 * @param[in] config   see #alirtc_channel_config_t
 *
 * @param[in] options see #alirtc_channel_options_t
 * 
 * @param[in] params see #alirtc_aiagent_param_t
 *
 * @param[in] user_data defined by user
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_join_ai_channel(alirtc_channel_config_t *config, alirtc_channel_options_t *options, alirtc_aiagent_param_t *params, void *user_data);
/**
 * Allow Local user leaves channel.
 *
 * @note Local user should leave channel when data transmission is stopped
 *
 * @param[in] channel   Channel ID : NULL is not allow
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_leave_channel(const char *channel);
/**
 * Send an audio frame to all channels OR specific channel.
 *
 * All remote users in this channel will receive the audio frame.
 *
 * @note All channels that you joined will receive the audio frame that you send
 *       unless you stop sending the local audio to a specific channel.
 *
 * @param[in] channel   Channel ID : NULL is not allow
 * @param[in] data  Audio frame buffer
 * @param[in] data_size  Audio frame buffer length (bytes)
 * @param[in] audio_info  Audio frame info, see #alirtc_audio_frame_info_t
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_send_audio_data(const char *channel, const void *data, size_t data_size, alirtc_audio_frame_info_t *audio_info);
/**
 * Send a video frame to all channels OR specific channel.
 *
 * All remote users in the channel will receive the video frame.
 *
 * @note All channels that you join will receive the video frame that you send
 *       unless you stop sending the local video to a specific channel.
 *
 * @param[in] channel   Channel ID : NULL is not allow
 * @param[in] stream_id Stream ID now noly one streamid 0
 * @param[in] data  Video frame buffer
 * @param[in] data_size  Video frame buffer length (bytes)
 * @param[in] video_info  Video frame info
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_send_video_data(const char *channel, const void *data, size_t data_size, alirtc_video_frame_info_t *video_info, uint8_t stream_id);

/**
 * Send a message to data channel.
 *
 * All remote users subscribes the data channel will receive the message.
 *
 * @param[in] channel_id    Channel ID : NULL is not allow
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_send_data_channel_message(const char* channel_id, const void* data, uint32_t data_size);

/**
 * Request remote user to generate a keyframe for all video streams OR specific video stream.
 *
 * @param[in] channel      Channel ID : NULL is not allow
 * @param[in] remote_uid   Remote user ID: NULL is not allow
 * @param[in] stream_id    Stream ID: NULL is not allow
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_request_video_key_frame(const char *channel, const char *remote_uid, uint8_t stream_id);
/**
 * Decide whether to enable/disable sending local audio data to specific channel OR all channels.
 *
 * @param[in] channel   Channel ID : NULL is not allow
 * @param[in] mute      Toggle sending local audio
 *                      - 0:        enable (default)
 *                      - non-ZERO: disable
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_mute_local_audio(const char *channel, int32_t mute);
/**
 * Decide whether to enable/disable sending local video data to specific channel OR all channels.
 *
 * @param[in] channel   Channel ID : NULL is not allow
 * @param[in] mute      Toggle sending local video
 *                      - 0:        enable (default)
 *                      - non-ZERO: disable
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_mute_local_video(const char *channel, int32_t mute);
/**
 * Decide whether to enable/disable receiving remote audio data from specific channel OR all channels.
 *
 * @param[in] channel       Channel ID : NULL is not allow
 * @param[in] remote_uid    Remote user ID
 * @param[in] mute          Toggle receiving remote audio
 *                          - 0:        enable (default)
 *                          - non-ZERO: disable
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_mute_remote_audio(const char *channel, const char *remote_uid, int32_t mute);
/**
 * Decide whether to enable/disable receiving remote video data from specific channel OR all channels.
 *
 * @param[in] channel       Channel ID : NULL is not allow
 * @param[in] remote_uid    Remote user ID
 *                          - if `remote_uid` is set 0, it's for all users
 *                          - else it's for specific user
 * @param[in] mute          Toggle receiving remote audio
 *                          - 0:        enable (default)
 *                          - non-ZERO: disable
 *
 * @return
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_mute_remote_video(const char *channel, const char *remote_uid, int32_t mute);
/**
 * Set network bandwdith estimation (bwe) param
 *
 * @param[in] min_bps   : bwe min bps
 * @param[in] max_bps   : bwe max bps
 * @param[in] start_bps : bwe start bps
 *
 * @return:
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_set_bwe_param(uint32_t min_bps, uint32_t max_bps, uint32_t start_bps);
/**
 * Set config params
 *
 * @param [in] params : config params described by json
 * @note  supported sets are shown below, they can be together in params json string
 *  - {"wss_ca_path": "/home/dir/ca.pem"}
 * @return:
 *  - = 0: Success
 *  - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_set_params(const char *params);

/**
 * Get audio data
 *
 * @param[in] channel       Channel ID : NULL is not allow
* @param[in] uid        Remote user ID to which data is sent
* @param[in] sent_ts    Timestamp (ms) for sending data
* @param[in] codec      Audio codec type
* @param[in] data   Audio frame buffer
* @param[in] length   Audio frame buffer length (bytes)
 *
 * @return:
 * - = 0: Success
 * - < 0: Failure
 */
extern __alirtc_api__ int32_t alirtc_read_audio_data(const char *channel, char **uid, uint8_t **data, int32_t *length, int64_t *sent_ts, alirtc_audio_codec_e *codec); //音频数据输入

#ifdef __cplusplus
}
#endif


#endif
