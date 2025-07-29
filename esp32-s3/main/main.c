#include "artc_aicall_demo.h"

#include "audio_event_iface.h"
#include "board.h"
#include "es7210.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_sntp.h"
#include "esp_peripherals.h"
#include "nvs_flash.h"
#include "periph_button.h"
#include "periph_wifi.h"
#include "sdkconfig.h"


static const char *TAG = "Main";

typedef struct {
    const char *caseName;
    void (*on_play_key_pressed_event)(void);
    void (*on_record_key_pressed_event)(void);
    void (*on_record_key_release_event)(void);
} TestCase;


static TestCase sTestCase = {
    .caseName = "AICallDemo",
    .on_play_key_pressed_event = artc_aicall_demo_start_call,
    .on_record_key_pressed_event = NULL,
    .on_record_key_release_event = NULL,
};


void print_run_time_stats() {
    while (true) {
        static char buf[1024 * 2];

#ifdef CONFIG_FREERTOS_USE_TRACE_FACILITY
        vTaskList(buf);
        ESP_LOGI(TAG, "Task List:\nTask Name    Status   Prio      HWM    Task Number\n%s", buf); // HWM: StackHighWaterMark, 最小剩余堆栈空间
#endif

        vTaskGetRunTimeStats(buf);
        ESP_LOGI(TAG, "Run Time Stats:\nTask Name       Time        Percent\n%s", buf);

        uint32_t heap_size = esp_get_free_heap_size();
        ESP_LOGI(TAG, "Free Heap Size: %" PRIu32 " bytes (%" PRIu32 " Kb)", heap_size, heap_size / 1024);

        vTaskDelay(pdMS_TO_TICKS(10 * 1000));
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set("AUDIO", ESP_LOG_INFO);
    esp_log_level_set("ARTC", ESP_LOG_INFO);
    esp_log_level_set("AICall", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_INFO);

    ESP_LOGI(TAG, "[ 1 ] Initialize start");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());


    ESP_LOGI(TAG, "[1.1] Initialize peripherals");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);


    ESP_LOGI(TAG, "[ 2 ] Start and wait for Wi-Fi network");
    periph_wifi_cfg_t wifi_cfg = {
        .wifi_config.sta.ssid = CONFIG_WIFI_SSID,
        .wifi_config.sta.password = CONFIG_WIFI_PASSWORD,
    };
    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
    esp_periph_start(set, wifi_handle);
    periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY);


    ESP_LOGI(TAG, "[2.1] Initializing SNTP");
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
    esp_netif_sntp_init(&config);

    // wait for time to be set
    int retry = 0;
    const int retry_count = 10;
    while (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(2000)) != ESP_OK && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    }
    

    ESP_LOGI(TAG, "[ 3 ] Start codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_BOTH, AUDIO_HAL_CTRL_START);
    audio_hal_set_volume(board_handle->audio_hal, CONFIG_AUDIO_PLAY_VOLUME);
#if defined (CONFIG_ESP32_S3_BOX_3_BOARD)
    es7210_adc_set_gain(ES7210_INPUT_MIC1 | ES7210_INPUT_MIC2, GAIN_37_5DB);
    es7210_adc_set_gain(ES7210_INPUT_MIC3, GAIN_6DB);
#endif


    ESP_LOGI(TAG, "[ 4 ] Set up event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);
    audio_board_key_init(set);
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);


#if defined(CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS)
    ESP_LOGI(TAG, "[ 5 ] Print run time stats frequently");
    xTaskCreate(print_run_time_stats, "stats", 4096, NULL, 5, NULL);
#endif


    ESP_LOGI(TAG, "[ 6 ] Initialize finish, listen to events");
    while (true) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            continue;
        }

        if (msg.source_type == PERIPH_ID_BUTTON) {
            if (msg.cmd == PERIPH_BUTTON_PRESSED && (int) msg.data == get_input_play_id()) {
                ESP_LOGI(TAG, "[ * ] play button pressed event");
                if (sTestCase.on_play_key_pressed_event) {
                    sTestCase.on_play_key_pressed_event();
                }
            } else if (msg.cmd == PERIPH_BUTTON_PRESSED && (int) msg.data == get_input_rec_id()) {
                ESP_LOGI(TAG, "[ * ] record button pressed event");
                if (sTestCase.on_record_key_pressed_event) {
                    sTestCase.on_record_key_pressed_event();
                }
            } else if ((msg.cmd == PERIPH_BUTTON_LONG_RELEASE || msg.cmd == PERIPH_BUTTON_RELEASE) && (int) msg.data == get_input_rec_id()) {
                ESP_LOGI(TAG, "[ * ] record button release event");
                if (sTestCase.on_record_key_release_event) {
                    sTestCase.on_record_key_release_event();
                }
            }
        }
    }
}
