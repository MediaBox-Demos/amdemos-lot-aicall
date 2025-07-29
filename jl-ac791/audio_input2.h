#ifndef audio_input2_h
#define audio_input2_h

#include "system/includes.h"
#include "server/audio_server.h"

int _device_write_voice_data2(void *data, unsigned int len);
int _device_get_voice_data2(void *data, unsigned int max_len);
void audio_stream_init2(int sample_rate, int bit_dept, int channel_num);
void start_audio_stream2(void);
void stop_audio_stream2(void);

#endif /* audio_input2_h */