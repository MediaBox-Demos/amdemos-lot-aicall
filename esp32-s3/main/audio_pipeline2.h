#ifndef _AUDIO_PIPELINE_2_H_
#define _AUDIO_PIPELINE_2_H_

typedef struct audio_recorder *audio_recorder_handle_t;
typedef struct audio_player *audio_player_handle_t;

audio_recorder_handle_t audio_recorder_create();
void audio_recorder_run(audio_recorder_handle_t handle);
int audio_recorder_read(audio_recorder_handle_t handle, char *buffer, int size);
void audio_recorder_destroy(audio_recorder_handle_t handle);


audio_player_handle_t audio_player_create();
void audio_player_run(audio_player_handle_t handle);
int audio_player_write(audio_player_handle_t handle, const char *buffer, int size);
void audio_player_destroy(audio_player_handle_t handle);

#endif // _AUDIO_PIPELINE_2_H_
