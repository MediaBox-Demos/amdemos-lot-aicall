//
//  artc_aicall.h
//  demo
//
//  Created by Bingo on 2025/6/3.
//

#ifndef artc_aicall_h
#define artc_aicall_h

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
    #ifdef _BUILDING_ALIRTC_IOT_SDK_
        #define __artc_aicall_api__ __declspec(dllexport)
    #else
        #define __artc_aicall_api__ __declspec(dllimport)
    #endif
#elif defined(__GNUC__)
    #define __artc_aicall_api__ __attribute__((visibility("default")))
#else
    #define __artc_aicall_api__
#endif

/**
 * @brief 智能体类型枚举
 *
 * @enum artc_aicall_agent_type_e
 * @note 当前仅支持 VOICE_AGENT 类型
 */
typedef enum {
    VOICE_AGENT = 0,  /**< 语音智能体 */
} artc_aicall_agent_type_e;

/**
 * @brief 智能体状态枚举
 *
 * @enum artc_aicall_agent_state_e
 */
typedef enum {
    STATE_LISTENING = 1,  /**< 监听中 */
    STATE_THINKING = 2,   /**< 思考中 */
    STATE_SPEAKING = 3,   /**< 讲话中 */
} artc_aicall_agent_state_e;

/**
 * @brief 错误码枚举
 *
 * @enum artc_aicall_error_code_e
 * @note 负值表示错误类型
 */
typedef enum {
    UNKNOWN_ERROR = -1,        /**< 未知错误 */
    SERVER_ERROR = -2,         /**< 服务端错误 */
    NETWORK_ERROR = -3,        /**< 网络错误 */
    
    // 客户端错误
    LICENSE_VALIDATION_FAILURE = -10000,        /**< License校验失败 */
    JOIN_CHANNEL_FAILED = -10001,       /**< 加入频道失败 */
    CHANNEL_DISCONNECTED = -10002,      /**< 频道断开连接 */
    DEVICE_EXCEPTION = -10003,          /**< 设备异常 */
    KICKED_BY_SERVER = -10004,          /**< 被服务端踢出 */
    START_AGENT_FAILED = -10005,        /**< 启动智能体失败 */

    // 智能体错误
    AGENT_LEAVE_CHANNEL = -11001,       /**< 智能体离开频道 */
    AGENT_INNER_ERROR = -11002,         /**< 智能体内部错误 */
    
} artc_aicall_error_code_e;

#define ARTC_AICALL_COMMON_ID_LEN 65

/**
 * @brief License结构体
 *
 * @struct artc_aicall_license_info_t
 */
typedef struct {
    char product_id[ARTC_AICALL_COMMON_ID_LEN];  /**< 产品批次ID */
    char auth_code[ARTC_AICALL_COMMON_ID_LEN]; /**< 授权码 */
    char device_id[ARTC_AICALL_COMMON_ID_LEN];  /**< 设备唯一标识 */
} artc_aicall_license_info_t;

/**
 * @brief 智能体信息结构体
 *
 * @struct artc_aicall_agent_info_t
 */
typedef struct {
    char agent_id[ARTC_AICALL_COMMON_ID_LEN];  /**< 智能体ID */
    artc_aicall_agent_type_e type;             /**< 智能体类型 */
    char region[17];                           /**< 智能体所在区域 */
    char instance_id[ARTC_AICALL_COMMON_ID_LEN]; /**< 智能体通话时运行的实例ID */
    char channel_id[ARTC_AICALL_COMMON_ID_LEN];  /**< 智能体通话时所在的频道ID */
    char uid[ARTC_AICALL_COMMON_ID_LEN];        /**< 智能体通话时的唯一标识 */
} artc_aicall_agent_info_t;

/**
 * @brief 音频帧配置结构体
 *
 * @struct artc_aicall_audio_frame_t
 */
typedef struct {
    int pcm_sample_rate;     /**< PCM采样率 (Hz)，当前仅支持16000，请设置为16000 */
    int pcm_channel_num;     /**< 声道数 (1: 单声道, 2: 双声道)，当前仅支持单声道，请设置为1 */
    int pcm_nb_samples;      /**< 每帧采样点字节数 (1: 8位, 2: 16位)，当前仅支持16位整型，请设置为2 */
    int pcm_duration;        /**< 音频帧时长 (ms)，建议为10ms */
} artc_aicall_audio_frame_t;

/**
 * @brief 认证信息结构体
 *
 * @struct artc_aicall_auth_info_t
 */
typedef struct {
    char app_id[ARTC_AICALL_COMMON_ID_LEN];     /**< 应用ID */
    char user_id[ARTC_AICALL_COMMON_ID_LEN];    /**< 用户ID */
    char channel_id[ARTC_AICALL_COMMON_ID_LEN]; /**< 频道ID */
    char nonce[ARTC_AICALL_COMMON_ID_LEN];      /**< 随机字符串 */
    char timestamp[17];                         /**< 过期的是时间戳 */
    char token[256];                            /**< 访问令牌 */
} artc_aicall_auth_info_t;

/**
 * @brief 通话配置结构体
 *
 * @struct artc_aicall_call_config_t
 */
typedef struct {
    artc_aicall_auth_info_t auth_info;              /**< 认证信息 */
    artc_aicall_audio_frame_t audio_frame;          /**< 音频帧配置 */
    artc_aicall_agent_info_t agent_info;            /**< 智能体信息 */
    bool enable_callback_agent_audio_data;          /**< 是否启用智能体音频数据回调 */
} artc_aicall_call_config_t;

typedef void * artc_aicall_engine_t;

/**
 * @brief 引擎事件回调函数集合
 *
 * @struct artc_aicall_engine_event_handler_t
 */
typedef struct {
    void (*on_call_begin)(artc_aicall_engine_t engine);  /**< 通话开始回调 */
    void (*on_call_end)(artc_aicall_engine_t engine);    /**< 通话结束回调 */
    void (*on_error_occurs)(artc_aicall_engine_t engine, artc_aicall_error_code_e code, const char *msg);  /**< 错误发生回调 */
    void (*on_agent_state_changed)(artc_aicall_engine_t engine, artc_aicall_agent_state_e state);  /**< 智能体状态变化回调 */
    void (*on_speaking_interrupted)(artc_aicall_engine_t engine, int32_t reason);  /**< 智能体讲话被打断回调 */
    void (*on_received_agent_audio_data)(artc_aicall_engine_t engine, const void *p_data, size_t data_size, uint64_t sent_ts);  /**< 收到智能体音频数据回调 */
} artc_aicall_engine_event_handler_t;

/**
 * @brief 创建语音智能体引擎实例
 *
 * @param license_info License信息
 * @param event_handler 事件回调处理器
 * @return artc_aicall_engine_t 引擎实例句柄 (NULL表示失败)
 */
extern __artc_aicall_api__ artc_aicall_engine_t artc_aicall_init(artc_aicall_license_info_t *license_info, artc_aicall_engine_event_handler_t *event_handler);

/**
 * @brief 释放语音智能体引擎资源
 *
 * @param engine 引擎实例句柄
 * @return int32_t 0 表示成功，非0表示接口调用失败
 */
extern __artc_aicall_api__ int32_t artc_aicall_destroy(artc_aicall_engine_t engine);

/**
 * @brief 客户端加入频道（需先通过OpenAPI启动智能体）
 *
 * @param engine 引擎实例句柄
 * @param config 通话配置参数
 * @return int32_t 0 表示成功，非0表示接口调用失败
 */
extern __artc_aicall_api__ int32_t artc_aicall_join(artc_aicall_engine_t engine, const artc_aicall_call_config_t *config);

/**
 * @brief 启动智能体并加入频道开始通话
 *
 * @param engine 引擎实例句柄
 * @param config 通话配置参数
 * @param startup_agent_config 智能体启动配置 (JSON字符串)
 * @param startup_user_data 用户自定义数据
 * @return int32_t 0 表示成功，，非0表示接口调用失败
 */
extern __artc_aicall_api__ int32_t artc_aicall_call(artc_aicall_engine_t engine, const artc_aicall_call_config_t *config, const char *startup_agent_config, const char  *startup_user_data);

/**
 * @brief 挂断通话并离开频道
 *
 * @param engine 引擎实例句柄
 * @return int32_t 0 表示成功，，非0表示接口调用失败
 */
extern __artc_aicall_api__ int32_t artc_aicall_handup(artc_aicall_engine_t engine);

/**
 * @brief 静音/取消静音本地音频输入
 *
 * @param engine 引擎实例句柄
 * @param mute true: 静音, false: 取消静音
 * @return int32_t 0 表示成功，，非0表示接口调用失败
 */
extern __artc_aicall_api__ int32_t artc_aicall_mute_local_audio(artc_aicall_engine_t engine, bool mute);

/**
 * @brief 打断智能体的讲话
 *
 * @param engine 引擎实例句柄
 * @return int32_t 0 表示成功，，非0表示接口调用失败
 */
extern __artc_aicall_api__ int32_t artc_aicall_interrupt_speaking(artc_aicall_engine_t engine);

/**
 * @brief 发送本地音频帧数据
 *
 * @param engine 引擎实例句柄
 * @param data 音频数据指针
 * @param data_size 音频数据大小 (字节)
 * @return int32_t 0 表示成功，，非0表示接口调用失败
 */
extern __artc_aicall_api__ int32_t artc_aicall_send_audio_data(artc_aicall_engine_t engine, const void *data, size_t data_size);

/**
 * @brief 读取智能体端音频帧数据
 *
 * @param engine 引擎实例句柄
 * @param data 输出音频数据指针
 * @param data_size 输出音频数据大小 (字节)
 * @param sent_ts 输出时间戳 (毫秒)
 * @return int32_t 0 表示成功，，非0表示接口调用失败
 */
extern __artc_aicall_api__ int32_t artc_aicall_read_agent_audio_data(artc_aicall_engine_t engine, uint8_t **data, size_t *data_size, int64_t *sent_ts);

#endif /* artc_aicall_h */
