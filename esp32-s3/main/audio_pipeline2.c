#include "audio_pipeline2.h"

#include "board.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "audio_mem.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "algorithm_stream.h"
#include "i2s_stream.h"
#include "raw_stream.h"


static const char *TAG = "AUDIO";


#define I2S_SAMPLE_RATE         16000
#define I2S_CHANNELS            I2S_CHANNEL_TYPE_ONLY_LEFT
#if defined(CONFIG_AUDIO_RECORDER_AEC_ENABLE)
#define I2S_BITS                CODEC_ADC_BITS_PER_SAMPLE
#define ALGORITHM_INPUT_FORMAT  "RM"
#else
#define I2S_BITS                I2S_DATA_BIT_WIDTH_16BIT
#endif // defined(CONFIG_AUDIO_RECORDER_AEC_ENABLE)

#define ESP_RING_BUFFER_SIZE    256


// reader
struct audio_recorder {
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t i2s_stream;
    audio_element_handle_t algo_stream;
    audio_element_handle_t raw_stream;
};

// writer
struct audio_player {
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t raw_stream;
    audio_element_handle_t i2s_stream;
};


audio_recorder_handle_t audio_recorder_create() {
    audio_recorder_handle_t recorder = audio_calloc(1, sizeof(struct audio_recorder));

    ESP_LOGI(TAG, "[ 1 ] Create audio pipeline for recording");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    recorder->pipeline = audio_pipeline_init(&pipeline_cfg);

    ESP_LOGI(TAG, "[1.1] Create i2s stream to read audio data from codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT_WITH_PARA(CODEC_ADC_I2S_PORT, I2S_SAMPLE_RATE, I2S_BITS, AUDIO_STREAM_READER);
    i2s_stream_set_channel_type(&i2s_cfg, I2S_CHANNELS);
    recorder->i2s_stream = i2s_stream_init(&i2s_cfg);
    audio_pipeline_register(recorder->pipeline, recorder->i2s_stream, "i2s");

#if defined(CONFIG_AUDIO_RECORDER_AEC_ENABLE)
    ESP_LOGI(TAG, "[1.3] Create algorithm stream for aec");
    algorithm_stream_cfg_t algo_config = ALGORITHM_STREAM_CFG_DEFAULT();
    algo_config.sample_rate = I2S_SAMPLE_RATE;
    algo_config.out_rb_size = ESP_RING_BUFFER_SIZE;
    algo_config.algo_mask = ALGORITHM_STREAM_USE_AEC | ALGORITHM_STREAM_USE_AGC;
    algo_config.input_format = ALGORITHM_INPUT_FORMAT;
    algo_config.mode = AFE_MODE_HIGH_PERF;
    recorder->algo_stream = algo_stream_init(&algo_config);
    audio_element_set_music_info(recorder->algo_stream, I2S_SAMPLE_RATE, 1, 16);
    audio_pipeline_register(recorder->pipeline, recorder->algo_stream, "algo");
#endif // defined(CONFIG_AUDIO_RECORDER_AEC_ENABLE)

    ESP_LOGI(TAG, "[1.2] Create raw stream to receive data");
    raw_stream_cfg_t raw_cfg = RAW_STREAM_CFG_DEFAULT();
    raw_cfg.type = AUDIO_STREAM_READER;
    raw_cfg.out_rb_size = 320 * 6;
    recorder->raw_stream = raw_stream_init(&raw_cfg);
    audio_pipeline_register(recorder->pipeline, recorder->raw_stream, "raw");

#if defined(CONFIG_AUDIO_RECORDER_AEC_ENABLE)
    ESP_LOGI(TAG, "[1.4] Link record elements together [codec_chip]-->i2s_stream-->algo-->raw");
    const char *link_tag[3] = {"i2s", "algo", "raw"};
#else
    ESP_LOGI(TAG, "[1.3] Link record elements together [codec_chip]-->i2s_stream-->raw");
    const char *link_tag[2] = {"i2s", "raw"};
#endif // defined(CONFIG_AUDIO_RECORDER_AEC_ENABLE)
    audio_pipeline_link(recorder->pipeline, &link_tag[0], sizeof(link_tag) / sizeof(link_tag[0]));

    return recorder;
}

void audio_recorder_run(audio_recorder_handle_t recorder) {
    audio_pipeline_run(recorder->pipeline);
}

int audio_recorder_read(audio_recorder_handle_t recorder, char *buffer, int size) {
    return raw_stream_read(recorder->raw_stream, buffer, size);
}

void audio_recorder_destroy(audio_recorder_handle_t recorder) {
    ESP_LOGI(TAG, "[ 3 ] Stop audio_pipeline for recording");

    audio_pipeline_stop(recorder->pipeline);
    audio_pipeline_wait_for_stop(recorder->pipeline);
    audio_pipeline_terminate(recorder->pipeline);

    audio_pipeline_unregister(recorder->pipeline, recorder->i2s_stream);
    audio_pipeline_unregister(recorder->pipeline, recorder->raw_stream);

    if (recorder->algo_stream) {
        audio_pipeline_unregister(recorder->pipeline, recorder->algo_stream);
        audio_element_deinit(recorder->algo_stream);
    }

    audio_element_deinit(recorder->i2s_stream);
    audio_element_deinit(recorder->raw_stream);
    audio_pipeline_deinit(recorder->pipeline);

    audio_free(recorder);
}


audio_player_handle_t audio_player_create() {
    audio_player_handle_t player = audio_calloc(1, sizeof(struct audio_player));

    ESP_LOGI(TAG, "[ 2 ] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    player->pipeline = audio_pipeline_init(&pipeline_cfg);

    ESP_LOGI(TAG, "[2.1] Create raw stream to write data");
    raw_stream_cfg_t raw_cfg = RAW_STREAM_CFG_DEFAULT();
    raw_cfg.type = AUDIO_STREAM_WRITER;
    raw_cfg.out_rb_size = 320 * 10;
    player->raw_stream = raw_stream_init(&raw_cfg);
    audio_pipeline_register(player->pipeline, player->raw_stream, "raw");

    ESP_LOGI(TAG, "[2.2] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT_WITH_PARA(I2S_NUM_0, I2S_SAMPLE_RATE, I2S_BITS, AUDIO_STREAM_WRITER);
    i2s_cfg.need_expand = (16 != I2S_BITS);
    i2s_cfg.out_rb_size = 320 * 10;
    i2s_stream_set_channel_type(&i2s_cfg, I2S_CHANNELS);
    player->i2s_stream = i2s_stream_init(&i2s_cfg);
    audio_pipeline_register(player->pipeline, player->i2s_stream, "i2s");
    
    ESP_LOGI(TAG, "[2.3] Link playback elements together raw-->i2s_stream-->[codec_chip]");
    const char *link_tag[] = {"raw", "i2s"};
    audio_pipeline_link(player->pipeline, &link_tag[0], sizeof(link_tag) / sizeof(link_tag[0]));

    return player;
}

void audio_player_run(audio_player_handle_t player) {
    audio_pipeline_run(player->pipeline);
}

int audio_player_write(audio_player_handle_t player, const char *buffer, int size) {
    return raw_stream_write(player->raw_stream, buffer, size);
}

void audio_player_destroy(audio_player_handle_t player) {
    ESP_LOGI(TAG, "[ 4 ] Stop audio_pipeline for playback");

    audio_pipeline_stop(player->pipeline);
    audio_pipeline_wait_for_stop(player->pipeline);
    audio_pipeline_terminate(player->pipeline);

    audio_pipeline_unregister(player->pipeline, player->raw_stream);
    audio_pipeline_unregister(player->pipeline, player->i2s_stream);

    audio_element_deinit(player->raw_stream);
    audio_element_deinit(player->i2s_stream); 
    audio_pipeline_deinit(player->pipeline);

    audio_free(player);
}
