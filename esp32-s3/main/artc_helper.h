//
// Created by Pi on 2025/06/11.
//

#ifndef artc_helper_h
#define artc_helper_h

#include <stdint.h>

int64_t get_time_ms();
void generate_simple_uuid(char *out);
char * ARTCGenerateToken(const char *appId, const char *appKey, const char *channelId, const char *userId, int timestamp);

#endif /* artc_helper_h */
