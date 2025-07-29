//
//  alirtc_iot_interface.h
//  alirtc_iot_interface
//
//  Created by alibaba on 2021/2/26.
//  Copyright © 2021 mt. All rights reserved.
//
#ifndef alirtc_iot_interface_h
#define alirtc_iot_interface_h
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define ALIRTC_MAX_COMMON_ID_LEN 65

#define ALIRTC_MAX_URL_LEN 1024
#define ALIRTC_LICENSE_AUTH_CODE_LEN 64

#define ALIRTC_MAX_CHANNELID_LEN ALIRTC_MAX_COMMON_ID_LEN
#define ALIRTC_MAX_APPID_LEN ALIRTC_MAX_COMMON_ID_LEN
#define ALIRTC_MAX_USRID_LEN ALIRTC_MAX_COMMON_ID_LEN
#define ALIRTC_MAX_NONCE_LEN ALIRTC_MAX_COMMON_ID_LEN
#define ALIRTC_MAX_TIMESTAMP_LEN ALIRTC_MAX_COMMON_ID_LEN
#define ALIRTC_MAX_TOKEN_LEN 512
#define ALIRTC_MAX_GSLB_LOCATION_LEN ALIRTC_MAX_COMMON_ID_LEN
#define ALIRTC_MAX_DISPLAYNAME_LEN ALIRTC_MAX_COMMON_ID_LEN

#define ALIRTC_MAX_EXTRA_PARAM_LEN 2048

/**
 * The definition of log level enum
 */
typedef enum {
    ALIRTC_LOG_INVALD = 0,
    ALIRTC_LOG_DEBUG = 1,
    ALIRTC_LOG_INFO  = 2,
    ALIRTC_LOG_WARN = 3, // default log level is notice
    ALIRTC_LOG_ERROR = 4,
} alirtc_log_level_e;
/**
 * The definition of error event type
 */
typedef enum {
    /**
     * no event
     */
    ALIRTC_EVENT_NONE,
    /**
     * failed to join channel
     */
    ALIRTC_EVENT_JOINCHANNEL_ERR,
    /**
     * failed to leave channel
     */
    ALIRTC_EVENT_LEAVECHANNEL_ERR,
    /**
     * server send bye by some reason
     */
    ALIRTC_EVENT_SERVER_BYE,
    /**
     * receive server bad message
     */
    ALIRTC_EVENT_SERVER_ERR,
    /**
     * device inner error
     */
    ALIRTC_EVENT_DEV_ERR,
    /**
     * device network error
     */
    ALIRTC_EVENT_NETWORK_ERR,
} alirtc_event_type_e;

/**
 * The definition of license err reason
 */
typedef enum {
    /**
     * license invalid
     */
    ALIRTC_LICENSE_INVALID,
    
    /**
     * license expired
     */
    ALIRTC_LICENSE_EXPIRE,
    
    /**
     * network error can not verify license
     */
    ALIRTC_LICENSE_NETWORK_ERROR,
    
    /**
     * internal error
     */
    ALIRTC_LICENSE_INTERNAL_ERROR,
} alirtc_license_err_reason_e;

/**
 * The definition of the alirtc_init_config_t struct.
 */
typedef struct {
    /**
     * The auth code of license
     */
    char auth_code[ALIRTC_LICENSE_AUTH_CODE_LEN + 1];
} alirtc_init_config_t;

/**
 * The definition of audio codec enum
 */
typedef enum {
    /**
     * 1: PCM (audio codec should be enabled)
     */
    ALIRTC_AUDIO_CODEC_PCM = 1,
    /**
     * 2: G722 (audio codec should be disabled)
     */
    ALIRTC_AUDIO_CODEC_G722 = 2,
    /**
     * 3: AAC (audio codec should be disabled)
     */
    ALIRTC_AUDIO_CODEC_AAC = 3,
    /**
     * 4: AMR (audio codec should be disabled)
     */
    ALIRTC_AUDIO_CODEC_AMR = 4,
    /**
     * 5: OPUS (audio codec should be disabled)
     */
    ALIRTC_AUDIO_CODEC_OPUS = 5,
    /**
     * 6: G711.A (audio codec should be disabled)
     */
    ALIRTC_AUDIO_CODEC_PCMA = 6,
    /**
     * 7: G711.U (audio codec should be disabled) not suppport now
     */
    ALIRTC_AUDIO_CODEC_PCMU = 7,
    /**
     * 8: Gerenic (audio codec should be disabled) not suppport now
     */
    ALIRTC_AUDIO_CODEC_GERENIC = 8,
} alirtc_audio_codec_e;
/**
 * The definition of the alirtc_audio_frame_info_t struct.
 */
typedef struct {
    /**
     * The ts of the audio frame
     */
    int64_t ts;
} alirtc_audio_frame_info_t;
/**
 * The definition of the alirtc_video_codec_e enum.
 */
typedef enum {
    /**
     * 1: H263：not support now
     */
    ALIRTC_VIDEO_CODEC_H263 = 1,
    /**
     * 2: H264
     */
    ALIRTC_VIDEO_CODEC_H264 = 2,
    /**
     * 3: Generic
     */
    ALIRTC_VIDEO_CODEC_GENERIC = 3,    
} alirtc_video_codec_e;
/**
 * The definition of the alirtc_video_frame_type_e enum.
 */
typedef enum {
    /**
     * 1: key frame
     */
    ALIRTC_VIDEO_FRAME_KEY = 1,
    /*
     * 2: delta frame, e.g: P-Frame
     */
    ALIRTC_VIDEO_FRAME_DELTA = 2,
} alirtc_video_frame_type_e;
/**
 * The definition of the alirtc_video_frame_rate_e enum.
 */
typedef enum {
    /**
     * 1: 1 fps.
     */
    ALIRTC_VIDEO_FRAME_RATE_FPS_1 = 1,
    /**
     * 7: 7 fps.
     */
    ALIRTC_VIDEO_FRAME_RATE_FPS_7 = 7,
    /**
     * 10: 10 fps.
     */
    ALIRTC_VIDEO_FRAME_RATE_FPS_10 = 10,
    /**
     * 15: 15 fps.
     */
    ALIRTC_VIDEO_FRAME_RATE_FPS_15 = 15,
    /**
     * 24: 24 fps.
     */
    ALIRTC_VIDEO_FRAME_RATE_FPS_24 = 24,
    /**
     * 30: 30 fps.
     */
    ALIRTC_VIDEO_FRAME_RATE_FPS_30 = 30,
    /**
     * 60: 60 fps
     */
    ALIRTC_VIDEO_FRAME_RATE_FPS_60 = 60,
} alirtc_video_frame_rate_e;
/**
 * The definition of the alirtc_video_frame_info_t struct.
 */
typedef struct {
    /**
     * The frame type of the encoded video frame: #alirtc_video_frame_type_e.
     */
    alirtc_video_frame_type_e type;
    /**
     * The pts of the encoded video frame
     */
    int64_t pts;
    /**
     * The dts of the encoded video frame
     */
    int64_t dts;
} alirtc_video_frame_info_t;

/**
 * The definition of the alirtc_codec_option_t struct.
 */
typedef struct {
    /**
     * Enable/disable sdk built-in audio codec
     * If audio codec is enabled, you are expected to send/receive pcm data
     * If audio codec is disabled, you are expected to send/receive encoded audio data (like aac, opus)
     */
    bool use_audio_codec;
    /**
     * Pcm sample rate. Ignored if audio coded is disabled
     */
    int pcm_sample_rate;
    /**
     * Pcm channel number. Ignored if audio coded is disabled
     */
    int pcm_channel_num;
    /**
     * Pcm number of samples. Ignored if audio coded is disabled
     */
    int pcm_nb_samples;
    /**
     * Pcm durantion of samples. Ignored if audio coded is disabled
     */
    int pcm_duration;
    
    /**
     * Audio codec type, reference #alirtc_audio_codec_e.
     */
    alirtc_audio_codec_e audio_codec;
    
    /**
     * The video codec: #alirtc_video_codec_e.
     */
    alirtc_video_codec_e video_codec;
    /**
     * The number of video frames per second.
     * -This value will be used for calculating timestamps of the encoded image.
     * - If frame_per_sec equals zero, then real timestamp will be used.
     * - Otherwise, timestamp will be adjusted to the value of frame_per_sec set.
     */
    alirtc_video_frame_rate_e frames_per_sec;
    
    int width;
    
    int height;
} alirtc_codec_option_t;

typedef struct {
    /**
      * create decoder handle
      *
      * @param[in] sample_rate audio sample rate
      * @param[in] channels    audio channel
      * @return[out] decoder handle
      */
    void *(*audio_create_cb)(int sample_rate, int channels);
    /**
      * decode audio callback
      *
      * @param[in] handle      audio decoder handle
      * @param[in] encoded_data    audio data need to be decoded
      * @encoded_length[in]      audio data need to be decoded length
      * @pcm_data[out]           audio data decoded to pcm
      * @encoded_length[out]      audio data decoded pcm length
      */
    int (*audio_decode_cb)(void * handle, unsigned char *encoded_data, int encoded_length, unsigned char * pcm_data, unsigned int *pcm_length);
    /**
      * close audio decoder
      *
      * @param[in] handle      audio decoder handle
      */
    void (*audio_close_cb)(void *handle);
} alirtc_audio_decoder_handler_t;

typedef struct {
    alirtc_log_level_e level;
    int32_t size_per_file;
    int32_t max_file_count;
} alirtc_log_config_t;

typedef struct {
  uint32_t min_bps;
  uint32_t max_bps;
  uint32_t start_bps;
} alirtc_bwe_param_t;

/**
 * The definition of the alirtc_channel_options_t struct.
 */
typedef struct {
    bool auto_subscribe_audio;
    bool auto_subscribe_video;
    bool auto_subscribe_data_channel;
    bool publish_video;
    bool publish_audio;
    bool publish_data_channel;
    alirtc_codec_option_t codec_opt;
    alirtc_audio_decoder_handler_t decoder_handler; // Only needed when use_audio_codec is set to false
    alirtc_log_config_t log_config;  // just for v2 api
    alirtc_bwe_param_t  bwe_param;   // just for v2 api
    bool read_audio;      // read audio data by user
    bool is_aiagent_communication;
} alirtc_channel_options_t;
/**
 * The definition of the alirtc_channel_config_t struct.
 */
typedef struct {
    char appid[ALIRTC_MAX_APPID_LEN];
    char userid[ALIRTC_MAX_USRID_LEN];
    char channelid[ALIRTC_MAX_CHANNELID_LEN];
    char nonce[ALIRTC_MAX_NONCE_LEN];
    char timestamp[ALIRTC_MAX_TIMESTAMP_LEN];
    char token[ALIRTC_MAX_TOKEN_LEN];
    char displayname[ALIRTC_MAX_DISPLAYNAME_LEN];
} alirtc_channel_config_t;

typedef struct {
    char agentid[ALIRTC_MAX_COMMON_ID_LEN];
    char region[ALIRTC_MAX_COMMON_ID_LEN];
    char action[ALIRTC_MAX_COMMON_ID_LEN];
    char params[ALIRTC_MAX_EXTRA_PARAM_LEN];
} alirtc_aiagent_param_t;

typedef enum {
    /**
     * 0: Remote user leaves channel actively
     */
    ALIRTC_USER_LEAVE_QUIT = 0,
    /**
     * 1: Remote user is dropped due to timeout
     */
    ALIRTC_USER_LEAVE_DROPPED = 1,
} alirtc_user_leave_reason_e;
typedef struct {
    /**
      * Report error message during runtime.
      *
      * In most cases, it means SDK can't fix the issue and application should take action.
      *
     * @param[in] usr_data defined by user
     * @param[in] channel Channel ID
     * @param[in] e_eventtype    Error event, see #alirtc_event_type_e
     * @param[in] code    Error code
     */
    void (*on_error)(void *user_data, const char *channel, alirtc_event_type_e e_eventtype, uint32_t code);
    
    /**
     * Report license error during join channel
     *
     * @param[in] user_data defined by user
     * @param[in] channel Channel ID
     * @param[in] code license error code
     */
    void (*on_license_validation_failure)(void *user_data, const char *channel, alirtc_license_err_reason_e code);

    /**
      * Occurs when local user joins channel successfully.
      *
      * @param[in] user_data defined by user
      * @param[in] channel    Channel ID
      */
    void (*on_join_channel_success)(void *user_data, const char *channel);
    /**
      * Occurs when channel is disconnected from the server.
      *
      * @param[in] user_data defined by user
      * @param[in] channel Channel ID
      */
    void (*on_channel_disconnected)(void *user_data, const char *channel);
    /**
      * Occurs when local user rejoins channel successfully after disconnect
      *
      * When channel loses connection with server due to network problems,
      * SDK will retry to connect automatically. If success, it will be triggered.
      *
      * @param[in] user_data defined by user
      * @param[in] channel    Channel ID
      */
    void (*on_rejoin_channel_success)(void *user_data, const char *channel);
    /**
     * Occurs when a remote user joins channel successfully.
     *
     * @param[in] user_data defined by user
     * @param[in] channel    Channel ID
     * @param[in] uid        Remote user ID
     */
    void (*on_user_join_channel)(void *user_data, const char *channel, const char *uid);
    /**
     * Occurs when remote user leaves the channel.
     *
     * @param[in] user_data defined by user
     * @param[in] channel Channel ID
     * @param[in] uid     Remote user ID~
     * @param[in] reason  Reason, see #alirtc_user_leave_reason_e
     */
    void (*on_user_leave_channel)(void *user_data, const char *channel, const char *uid, alirtc_user_leave_reason_e reason);

    /**
     * Occurs when receiving the audio frame of a remote user in the channel.
     *
     * @param[in] user_data defined by user
     * @param[in] channel    Channel ID
     * @param[in] uid        Remote user ID to which data is sent
     * @param[in] sent_ts    Timestamp (ms) for sending data
     * @param[in] codec      Audio codec type
     * @param[in] data   Audio frame buffer
     * @param[in] data_size   Audio frame buffer length (bytes)
     */
    void (*on_audio_data)(void *user_data, const char *channel, const char *uid, const void *data, size_t data_size, alirtc_audio_codec_e codec, uint64_t sent_ts);
    /**
     * Occurs when receiving the video frame of a remote user in the channel.
     *
     * @param[in] user_data defined by user
     * @param[in] channel      Channel ID
     * @param[in] uid          Remote user ID to which data is sent
     * @param[in] sent_ts      Timestamp (ms) for sending data
     * @param[in] codec        Video codec type
     * @param[in] stream_id    Video stream ID. Range is [0, 15]
     * @param[in] is_key_frame Frame type:
     *                          0 : P frame
     *                          1 : I frame
     * @param[in] data     Video frame buffer
     * @param[in] data_size     Video frame buffer lenth (bytes)
     */
    void (*on_video_data)(void *user_data, const char *channel, const char *uid, const void *data, size_t data_size, alirtc_video_codec_e codec, uint64_t sent_ts, uint8_t stream_id, int is_key_frame);

    /**
     * Occurs when receiving the message of a remote user in the channel.
     *
     * @param[in] user_data defined by user
     * @param[in] uid          Remote user ID to which message is sent
     * @param[in] data         message frame buffer
     * @param[in] data_size    Message frame buffer length (bytes)
     */
    void (*on_datachannel_message)(void *user_data, const char *uid, const void* data, uint32_t data_size);

    /**
     * Occurs when remote user already subscribed my data channel.
     *
     * @param[in] user_data defined by user
     * @param[in] uid          Remote user ID to which subscribed my data channel
     */
    void (*on_remote_user_subscribe_data_channel)(void *user_data, const char *uid);
    
    /**
     * Occurs when receiving the result of ai agent.
     *
     * @param[in] user_data defined by user
     * @param[in] code      Result code of AI agent action. 0 indicates success.
     * @param[in] action    The action corresponding to the code 
     * @param[in] agent_result  The return result (usually a JSON string).
     */
    void (*on_ai_agent_result)(void *user_data, int32_t code, const char *action, const char *agent_result);
    
    /**
      * Occurs when a remote user requests a keyframe.
      *
      * This callback notifies the sender to generate a new keyframe.
      *
      * @param[in] user_data defined by user
      * @param[in] channel    Channel ID
      * @param[in] uid        Remote user ID
      * @param[in] stream_id  Stream ID for which a keyframe is requested
      */
    void (*on_key_frame_gen_req)(void *user_data, const char *channel, const char *uid, uint8_t stream_id);
    /**
      * Occurs when network bandwidth change is detected. User is expected to adjust encoder
      * bitrate to |target_bps|
      *
      * @param[in] user_data defined by user
      * @param[in] channel    Channel ID
      * @param[in] target_bps Target value (bps) by which the bitrate should update
      */
    void (*on_target_bitrate_changed)(void *user_data, const char *channel, uint32_t target_bps);

} alirtc_event_handler_t;


#endif
