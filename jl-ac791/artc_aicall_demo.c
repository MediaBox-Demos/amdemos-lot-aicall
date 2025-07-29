//
// Created by Bingo on 2025/06/25.
//

#include "app_config.h"
#include "artc_aicall_demo.h"

#include "artc_device_helper.h"
#include "artc_logger.h"

#include <time.h>
#include "audio_input2.h"

#include "alirtc_iot_sdk.h"
#include "artc_aicall.h"

// 智能体通话的License信息
#define LICENSE_PRODUCT_ID "xxx"  // License产品Id
#define LICENSE_AUTH_CODE "xxx"   // License授权码
#define LICENSE_DEVICE_ID "xxx"   // 设备唯一Id

// 当前用户参与智能体通话的用户Id
#define USER_ID "xxxx"               
// 智能体Id
#define VOICE_AGENT_ID "xxxx"
// 智能体所在的区域
#define AGENT_REGION "xxxx"
// 智能体关联的RTC应用Id
#define RTC_APP_ID "xxxx"
// 智能体关联的RTC应用Key
// 注意：RTC_APP_KEY此次在本地生成签名Token，仅用于开发阶段的本地调试。对于线上版本，请勿内置RTC_APP_KEY，请使用服务端生成签名并下发方式
#define RTC_APP_KEY "xxxx"




#define PCM_CHANNEL 1
#define PCM_SAMPLE_RATE 16000
#define PCM_NB 2
#define PCM_DURATION 10

#define ENABLE_READ_AGENT_AUDIO_DATA 1

typedef struct  {
    artc_aicall_engine_t engine;
    artc_aicall_call_config_t *config;

    int record_task_pid;
    bool can_audio_task_running;
    bool can_get_voice_data;
    bool can_write_voice_data;

} aicall_t;
static aicall_t *g_curr_aicall = NULL;
static volatile int g_error_code = 0;


static void generate_simple_uuid(char *out) {
    srand((unsigned int)time(NULL));

    for (int i = 0; i < 32; i++) {
        out[i] = "0123456789abcdef"[rand() % 16];
    }
    out[32] = '\0';
}

static void generate_token(const char *appId, const char *appKey, const char *channelId, const char *userId, int timestamp, char *token) {
    if (!appId || !appKey || !channelId || !userId) {
        ARTC_LOG_ERROR("generate token error: invalid params");
        return;
    }
    size_t contentSize = strlen(appId) + strlen(appKey) + strlen(channelId) + strlen(userId);
    int tmp = timestamp;
    do {
        ++contentSize;
        tmp /= 10;
    } while (tmp > 0);
    ++contentSize;

    char *buffer = (char *)malloc(contentSize);
    int len = snprintf(buffer, contentSize, "%s%s%s%s%d", appId, appKey, channelId, userId, timestamp);

    static const int Sha256Len = 32;
    char sha256Out[Sha256Len] = {0};
    jl_sha256(buffer, len, sha256Out, 0);
    free(buffer);

    // ToHex
    const char *HexChars = "0123456789abcdef";
    for (int i = 0; i < Sha256Len; ++i) {
        token[i * 2] = HexChars[(sha256Out[i] >> 4) & 0xf];
        token[i * 2 + 1] = HexChars[sha256Out[i] & 0xf];
    }
    token[Sha256Len * 2] = 0;
}


static void on_audio_running() {
    ARTC_LOG_INFO("on_audio_running");
    audio_stream_init2(PCM_SAMPLE_RATE, PCM_NB * 8, PCM_CHANNEL);
    start_audio_stream2();
    const size_t write_size = PCM_DURATION * PCM_SAMPLE_RATE * PCM_NB / 1000;
    void *write_audio_buffer = malloc(write_size);

    unsigned int nextTick = 0;
    const unsigned int StepTick = PCM_DURATION / 10;

    while (g_error_code == 0 && g_curr_aicall->can_audio_task_running && g_curr_aicall->engine ) {    // 收发音频数据，根据需要设置打断循环条件

        unsigned int curTick = os_time_get();
        if (nextTick == 0) {
            nextTick = curTick + StepTick;
        } else {
            if (curTick < nextTick) {
                os_time_dly(nextTick - curTick);
            }
            nextTick += StepTick;
        }

        if (g_curr_aicall->can_write_voice_data) {
            uint8_t *read_data = NULL;
            size_t read_size = 0;
            int64_t sent_ts = 0;
            artc_aicall_read_agent_audio_data(g_curr_aicall->engine, &read_data, &read_size, &sent_ts);
            if (read_size > 0 && read_data) {
                _device_write_voice_data2(read_data, read_size);
            }
            if (read_data) {
                free(read_data);
                read_data = NULL;
            }
            // ARTC_LOG_DEBUG("====read audio data: %d====", read_size);
        }

        if (g_curr_aicall->can_get_voice_data) {
            int ret = _device_get_voice_data2(write_audio_buffer, write_size);
            if (ret == write_size && g_curr_aicall->can_get_voice_data && g_curr_aicall->engine) {
                // 发送音频数据
                // ARTC_LOG_DEBUG("====send audio data: %d====", write_size);
                int ret = artc_aicall_send_audio_data(g_curr_aicall->engine, write_audio_buffer, write_size);
                if (ret != 0) {
                    ARTC_LOG_WARN("send mic data fail: %d, len(%d)", ret, write_size);
                }
            }
        }
        
        
    }
    free(write_audio_buffer);
    stop_audio_stream2();
}

static void start_audio_task() {
    if (!g_curr_aicall || !g_curr_aicall->engine) {
        return;
    }
    ARTC_LOG_INFO("start_audio_task");
    int taskRet = thread_fork("aicall_audio_task", 10, 32 * 1024, 0, &g_curr_aicall->record_task_pid, on_audio_running, NULL);
    if (taskRet == 0) {
        ARTC_LOG_INFO("create audio task ok: %d", g_curr_aicall->record_task_pid);
    } 
    else {
        ARTC_LOG_ERROR("create audio task error: %d", taskRet);
    }
}


static void on_call_begin(artc_aicall_engine_t engine) {
    ARTC_LOG_INFO("on_call_begin");
}

static void on_call_end(artc_aicall_engine_t engine) {
    ARTC_LOG_INFO("on_call_end");
}

static void on_error_occurs(artc_aicall_engine_t engine, artc_aicall_error_code_e code, const char *msg) {
    ARTC_LOG_ERROR("on_error_occurs: %d, %s", code, msg);
    g_error_code = (int)code;
}

static void on_agent_state_changed(artc_aicall_engine_t engine, artc_aicall_agent_state_e state) {
    ARTC_LOG_INFO("on_agent_state_changed: %d", state);
}

static void on_speaking_interrupted(artc_aicall_engine_t engine, int32_t reason) {
    ARTC_LOG_INFO("on_speaking_interrupted: %d", reason);
}

static void on_received_agent_audio_data(artc_aicall_engine_t engine, const void *p_data, size_t data_size, uint64_t sent_ts) {
    ARTC_LOG_DEBUG("on_received_agent_audio_data");
    _device_write_voice_data2(p_data, data_size);
}


static void destroy_aicall(void) {
    if (!g_curr_aicall) {
        return;
    }
    
    if (g_curr_aicall->engine) {
        artc_aicall_destroy(g_curr_aicall->engine);
        g_curr_aicall->engine = NULL;
    }

    if (g_curr_aicall->config) {
        free(g_curr_aicall->config);
        g_curr_aicall->config = NULL;
    }

    free(g_curr_aicall);
    g_curr_aicall = NULL;
}

void artc_aicall_demo_start_call(void) {

    if (g_error_code != 0) {
        artc_aicall_demo_handup();
        g_error_code = 0;
    }
    
    if (g_curr_aicall) {
        ARTC_LOG_ERROR("current is running");
        return;
    }

    // 检查网络状态
    ARTC_LOG_INFO("check network");
    if (!ARTCIsWifiConnected()) {
        ARTC_LOG_ERROR("Network is not connected!");
        return;
    }

    // alirtc_rtc_set_log_level(ALIRTC_LOG_DEBUG);

    ARTC_LOG_INFO("start call");

    g_curr_aicall = malloc(sizeof(aicall_t));
    memset(g_curr_aicall, 0, sizeof(aicall_t));
    g_curr_aicall->config = malloc(sizeof(artc_aicall_call_config_t));
    memset(g_curr_aicall->config, 0, sizeof(artc_aicall_call_config_t));
    g_curr_aicall->record_task_pid = 0;
    g_curr_aicall->can_audio_task_running = false;
    g_curr_aicall->can_get_voice_data = false;
    g_curr_aicall->can_write_voice_data = false;

    int ts = (int)time(NULL); // 获取当前时间戳
    ts += 60 * 60 * 24;
    char ts_str[17] = { 0 };
    snprintf(ts_str, sizeof(ts_str), "%d", ts);
    
    char channel_id[33] = { 0 } ;
    generate_simple_uuid(channel_id);
    
    char token[65] = { 0 };
    generate_token(RTC_APP_ID, RTC_APP_KEY, channel_id, USER_ID, ts, token);
    
    
    artc_aicall_call_config_t *call_config = g_curr_aicall->config;
    call_config->enable_callback_agent_audio_data = ENABLE_READ_AGENT_AUDIO_DATA != 1;
    strncpy(call_config->auth_info.app_id, RTC_APP_ID, sizeof(call_config->auth_info.app_id) - 1);
    strncpy(call_config->auth_info.channel_id, channel_id, sizeof(call_config->auth_info.channel_id) - 1);
    strncpy(call_config->auth_info.user_id, USER_ID, sizeof(call_config->auth_info.user_id) - 1);
    strncpy(call_config->auth_info.timestamp, ts_str, sizeof(call_config->auth_info.timestamp) - 1);
    strncpy(call_config->auth_info.token, token, sizeof(call_config->auth_info.token) - 1);
    ARTC_LOG_DEBUG("app_id: %s, channel_id: %s, user_id: %s, timestamp: %s, token: %s", 
        call_config->auth_info.app_id, 
        call_config->auth_info.channel_id, 
        call_config->auth_info.user_id,
        call_config->auth_info.timestamp,
        call_config->auth_info.token
    );

    call_config->agent_info.type = VOICE_AGENT;
    strncpy(call_config->agent_info.region, AGENT_REGION, sizeof(call_config->agent_info.region) - 1);
    strncpy(call_config->agent_info.agent_id, VOICE_AGENT_ID, sizeof(call_config->agent_info.agent_id) - 1);
    strncpy(call_config->agent_info.channel_id, channel_id, sizeof(call_config->agent_info.channel_id) - 1);
    ARTC_LOG_DEBUG("agent_id: %s, region: %s, channel_id: %s", 
        call_config->agent_info.agent_id, 
        call_config->agent_info.region, 
        call_config->agent_info.channel_id
    );

    call_config->audio_frame.pcm_channel_num = PCM_CHANNEL;
    call_config->audio_frame.pcm_sample_rate = PCM_SAMPLE_RATE;
    call_config->audio_frame.pcm_nb_samples = PCM_NB;
    call_config->audio_frame.pcm_duration = PCM_DURATION;
    
    ARTC_LOG_DEBUG("create aicall engine");
    // 端侧初始化引擎
    artc_aicall_engine_event_handler_t event_handler = {
        .on_call_begin                = on_call_begin,
        .on_call_end                  = on_call_end,
        .on_error_occurs              = on_error_occurs,
        .on_agent_state_changed       = on_agent_state_changed,
        .on_speaking_interrupted      = on_speaking_interrupted,
        .on_received_agent_audio_data = on_received_agent_audio_data,
    };
    artc_aicall_license_info_t license_info = { 0 };
    strncpy(license_info.product_id, LICENSE_PRODUCT_ID, sizeof(license_info.product_id) - 1);
    strncpy(license_info.auth_code, LICENSE_AUTH_CODE, sizeof(license_info.auth_code) - 1);
    strncpy(license_info.device_id, LICENSE_DEVICE_ID, sizeof(license_info.device_id) - 1);
    artc_aicall_engine_t engine = artc_aicall_init(&license_info, &event_handler);
    if (!engine) {
        ARTC_LOG_ERROR("engine init failed");
        destroy_aicall();
        return;
    }
    g_curr_aicall->engine = engine;
    
    // 端侧启动通话
    ARTC_LOG_INFO("start call");
    if (artc_aicall_call(g_curr_aicall->engine, g_curr_aicall->config, "{\"AsrConfig\": {\"VadLevel\": 11}, \"TurnDetectionConfig\" : {\"Mode\" : \"Semantic\", \"SemanticWaitDuration\" : 5000}, \"ExperimentalConfig\" : \"{\\\"AudioQualityMode\\\" : 2, \\\"AudioCodec\\\" : \\\"G722\\\"}\"}", NULL) == 0) {
        ARTC_LOG_INFO("raise artc_aicall_call success");
        g_curr_aicall->can_audio_task_running = true;
        g_curr_aicall->can_get_voice_data = true;
        g_curr_aicall->can_write_voice_data = ENABLE_READ_AGENT_AUDIO_DATA == 1;
        start_audio_task();
    }
    else {
        ARTC_LOG_ERROR("raise artc_aicall_call fail");
        artc_aicall_demo_handup();
    }
}

void artc_aicall_demo_handup(void) {
    if (!g_curr_aicall || !g_curr_aicall->engine) {
        ARTC_LOG_ERROR("current is not running");
        return;
    }

    ARTC_LOG_INFO("handup");

    g_curr_aicall->can_get_voice_data = false;
    g_curr_aicall->can_write_voice_data = false;
    g_curr_aicall->can_audio_task_running = false;
    if (g_curr_aicall->record_task_pid > 0) {
        thread_kill(&g_curr_aicall->record_task_pid, KILL_WAIT);
        g_curr_aicall->record_task_pid = 0;
    }
    
    artc_aicall_handup(g_curr_aicall->engine);

    destroy_aicall();
}

void artc_aicall_demo_interrupt_speaking(void) {
    if (!g_curr_aicall || !g_curr_aicall->engine) {
        ARTC_LOG_ERROR("current is not running");
        return;
    }
    
    ARTC_LOG_INFO("interrupt speaking");
    artc_aicall_interrupt_speaking(g_curr_aicall->engine);
    
}

void artc_aicall_demo_mute_local_audio(void) {
    if (!g_curr_aicall || !g_curr_aicall->engine) {
        ARTC_LOG_ERROR("current is not running");
        return;
    }
    
    ARTC_LOG_INFO("mute local audio");
    artc_aicall_mute_local_audio(g_curr_aicall->engine, true);
    
}

void artc_aicall_demo_unmute_local_audio(void) {
    if (!g_curr_aicall || !g_curr_aicall->engine) {
        ARTC_LOG_ERROR("current is not running");
        return;
    }
    
    ARTC_LOG_INFO("unmute local audio");
    artc_aicall_mute_local_audio(g_curr_aicall->engine, false);
    
}