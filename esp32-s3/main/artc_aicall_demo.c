#include "artc_aicall_demo.h"
#include "artc_helper.h"

#include "alirtc_iot_sdk.h"
#include "artc_aicall.h"
#include "audio_pipeline2.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include <string.h>
#include <time.h>
#include <unistd.h>


static const char *TAG = "AICall";

#define TOKEN_VALID_DURATION 60 * 60 * 24 // 24h

#define PCM_CHANNEL         1
#define PCM_SAMPLE_RATE     16000
#define PCM_NB              2
#define PCM_DURATION        20


typedef struct  {
    artc_aicall_engine_t engine;
    artc_aicall_call_config_t *config;
    volatile bool can_audio_task_running;
    audio_player_handle_t audio_player;
} aicall_t;
static aicall_t *g_curr_aicall = NULL;
static volatile int g_error_code = 0;


static void on_call_begin(artc_aicall_engine_t engine) {
    ESP_LOGI(TAG, "on_call_begin");
}

static void on_call_end(artc_aicall_engine_t engine) {
    ESP_LOGI(TAG, "on_call_end");
}

static void on_error_occurs(artc_aicall_engine_t engine, artc_aicall_error_code_e code, const char *msg) {
    ESP_LOGE(TAG, "on_error_occurs: %d, %s", code, msg);
    g_error_code = (int)code;
}

static void on_agent_state_changed(artc_aicall_engine_t engine, artc_aicall_agent_state_e state) {
    ESP_LOGI(TAG, "on_agent_state_changed: %d", state);
}

static void on_speaking_interrupted(artc_aicall_engine_t engine, int32_t reason) {
    ESP_LOGI(TAG, "on_speaking_interrupted: %d", (int)reason);
}

static void on_received_agent_audio_data(artc_aicall_engine_t engine, const void *p_data, size_t data_size, uint64_t sent_ts) {
    ESP_LOGD(TAG, "on_received_agent_audio_data");
    if (g_curr_aicall && g_curr_aicall->audio_player) {
        audio_player_write(g_curr_aicall->audio_player, p_data, data_size);
    }
}

static void destroy_aicall(void) {
    if (!g_curr_aicall) {
        return;
    }
    
    ESP_LOGI(TAG, "destroy aicall");
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

void artc_aicall_demo_start_call_impl() {
    ESP_LOGI(TAG, "start call");
    // alirtc_rtc_set_log_level(ALIRTC_LOG_INFO);

    g_error_code = 0;
    g_curr_aicall = malloc(sizeof(aicall_t));
    memset(g_curr_aicall, 0, sizeof(aicall_t));
    g_curr_aicall->config = malloc(sizeof(artc_aicall_call_config_t));
    memset(g_curr_aicall->config, 0, sizeof(artc_aicall_call_config_t));

    int ts = (int)time(NULL);
    ts += TOKEN_VALID_DURATION;
    char ts_str[17] = { 0 };
    snprintf(ts_str, sizeof(ts_str), "%d", ts);
    
    char channel_id[33] = { 0 } ;
    generate_simple_uuid(channel_id);

    // ❗❗❗当前用户参与智能体通话的用户Id（建议每个设备使用唯一的Id） 
    char*user_id = CONFIG_RTC_USER_ID;
    
    // ❗❗❗注意：RTC_APP_KEY此次在本地生成签名Token，仅用于开发阶段的本地调试。对于线上版本，请勿内置RTC_APP_KEY，请使用服务端生成签名并下发方式。 
    char *token = ARTCGenerateToken(CONFIG_RTC_APP_ID, CONFIG_RTC_APP_KEY, channel_id, user_id, ts);

    artc_aicall_call_config_t *call_config = g_curr_aicall->config;
    memset(call_config, 0, sizeof(artc_aicall_call_config_t));
    call_config->enable_callback_agent_audio_data = true;
    strncpy(call_config->auth_info.app_id, CONFIG_RTC_APP_ID, sizeof(call_config->auth_info.app_id) - 1);
    strncpy(call_config->auth_info.channel_id, channel_id, sizeof(call_config->auth_info.channel_id) - 1);
    strncpy(call_config->auth_info.user_id, user_id, sizeof(call_config->auth_info.user_id) - 1);
    strncpy(call_config->auth_info.timestamp, ts_str, sizeof(call_config->auth_info.timestamp) - 1);
    strncpy(call_config->auth_info.token, token, sizeof(call_config->auth_info.token) - 1);
    ESP_LOGI(TAG, "app_id: %s, channel_id: %s, user_id: %s, timestamp: %s, token: %s", 
        call_config->auth_info.app_id, 
        call_config->auth_info.channel_id, 
        call_config->auth_info.user_id,
        call_config->auth_info.timestamp,
        call_config->auth_info.token
    );
    free(token);

    call_config->agent_info.type = VOICE_AGENT;
    strncpy(call_config->agent_info.region, CONFIG_VOICE_AGENT_REGION, sizeof(call_config->agent_info.region) - 1);
    strncpy(call_config->agent_info.agent_id, CONFIG_VOICE_AGENT_ID, sizeof(call_config->agent_info.agent_id) - 1);
    strncpy(call_config->agent_info.channel_id, channel_id, sizeof(call_config->agent_info.channel_id) - 1);
    ESP_LOGI(TAG, "agent_id: %s, region: %s, channel_id: %s", 
        call_config->agent_info.agent_id, 
        call_config->agent_info.region, 
        call_config->agent_info.channel_id
    );

    call_config->audio_frame.pcm_channel_num = PCM_CHANNEL;
    call_config->audio_frame.pcm_sample_rate = PCM_SAMPLE_RATE;
    call_config->audio_frame.pcm_nb_samples = PCM_NB;
    call_config->audio_frame.pcm_duration = PCM_DURATION;
    
    ESP_LOGI(TAG, "create aicall engine");
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
    strncpy(license_info.product_id, CONFIG_LICENSE_PRODUCT_ID, sizeof(license_info.product_id) - 1);
    strncpy(license_info.auth_code, CONFIG_LICENSE_AUTH_CODE, sizeof(license_info.auth_code) - 1);
    strncpy(license_info.device_id, CONFIG_LICENSE_DEVICE_ID, sizeof(license_info.device_id) - 1);
    artc_aicall_engine_t engine = artc_aicall_init(&license_info, &event_handler);
    if (!engine) {
        ESP_LOGE(TAG, "engine init failed");
        destroy_aicall();
        goto __err0;
    }
    g_curr_aicall->engine = engine;
    
    // 端侧启动通话
    ESP_LOGI(TAG, "start call");
    const char *start_agent_config = "{\"AsrConfig\": {\"VadLevel\": 11}, \"TurnDetectionConfig\" : {\"Mode\" : \"Semantic\", \"SemanticWaitDuration\" : 5000}, \"ExperimentalConfig\" : \"{\\\"AudioQualityMode\\\" : 2, \\\"AudioCodec\\\" : \\\"G722\\\"}\"}";
    int ret = artc_aicall_call(g_curr_aicall->engine, g_curr_aicall->config, start_agent_config, NULL);
    if (ret != 0) {
        ESP_LOGE(TAG, "raise artc_aicall_call fail");
        goto __err1;
    }

    ESP_LOGI(TAG, "raise artc_aicall_call success");
    g_curr_aicall->can_audio_task_running = true;
    
    const int DEFAULT_READ_SIZE = PCM_SAMPLE_RATE * PCM_NB * PCM_CHANNEL / 1000 * PCM_DURATION;
    uint8_t *audio_buffer = malloc(DEFAULT_READ_SIZE);
    if (!audio_buffer) {
        ESP_LOGE(TAG, "Failed to alloc audio buffer!");
        goto __err1;
    }

    audio_recorder_handle_t audio_recorder = audio_recorder_create();
    audio_player_handle_t audio_player = audio_player_create();
    audio_recorder_run(audio_recorder);
    audio_player_run(audio_player);
    g_curr_aicall->audio_player = audio_player;

    while (g_error_code == 0 && g_curr_aicall->can_audio_task_running && g_curr_aicall->engine) {
        int read_len = audio_recorder_read(audio_recorder, (char*) audio_buffer, DEFAULT_READ_SIZE);
        if (read_len == DEFAULT_READ_SIZE && g_curr_aicall && g_curr_aicall->can_audio_task_running) {
            int send_ret = artc_aicall_send_audio_data(g_curr_aicall->engine, audio_buffer, DEFAULT_READ_SIZE);
            if (send_ret != 0) {
                ESP_LOGW(TAG, "send mic data fail: %d, len(%d)", send_ret, DEFAULT_READ_SIZE);
            }
        }
    }

    g_curr_aicall->audio_player = NULL;
    audio_recorder_destroy(audio_recorder);
    audio_player_destroy(audio_player);

    if (audio_buffer) {
        free(audio_buffer);
    }

__err1:
    g_curr_aicall->can_audio_task_running = false;
    artc_aicall_handup(g_curr_aicall->engine);
    destroy_aicall();

__err0:
    vTaskDelete(NULL);
}

void artc_aicall_demo_handup(void) {
    if (!g_curr_aicall || !g_curr_aicall->engine) {
        ESP_LOGI(TAG, "current is not running");
        return;
    }

    ESP_LOGI(TAG, "handup");

    g_curr_aicall->can_audio_task_running = false;

    vTaskDelay(pdMS_TO_TICKS(1000));
}

void artc_aicall_demo_interrupt_speaking(void) {
    if (!g_curr_aicall || !g_curr_aicall->engine) {
        ESP_LOGI(TAG, "current is not running");
        return;
    }
    
    ESP_LOGI(TAG, "interrupt speaking");
    artc_aicall_interrupt_speaking(g_curr_aicall->engine);    
}

void artc_aicall_demo_mute_local_audio(void) {
    if (!g_curr_aicall || !g_curr_aicall->engine) {
        ESP_LOGI(TAG, "current is not running");
        return;
    }
    
    ESP_LOGI(TAG, "mute local audio");
    artc_aicall_mute_local_audio(g_curr_aicall->engine, true);
}

void artc_aicall_demo_unmute_local_audio(void) {
    if (!g_curr_aicall || !g_curr_aicall->engine) {
        ESP_LOGI(TAG, "current is not running");
        return;
    }
    
    ESP_LOGI(TAG, "unmute local audio");
    artc_aicall_mute_local_audio(g_curr_aicall->engine, false);
}

void artc_aicall_demo_start_call(void)
{
    ESP_LOGI(TAG, "artc aicall demo start call");
    artc_aicall_demo_handup();

    xTaskCreate(artc_aicall_demo_start_call_impl, "AICallDemo", 8 * 1024, NULL, 5, NULL);
}
