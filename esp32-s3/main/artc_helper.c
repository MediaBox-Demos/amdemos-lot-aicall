//
// Created by Pi on 2025/06/11.
//

#include "artc_helper.h"
#include "esp_log.h"
#include "mbedtls/sha256.h"

#include <string.h>
#include <stdio.h>
#include <sys/time.h>


#define Sha256Len 32

static const char *TAG = "UTILS";

int64_t get_time_ms() {
    int64_t t;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    t = (int64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return t;
}

void generate_simple_uuid(char *out) {
    srand((unsigned int)time(NULL));

    for (int i = 0; i < 32; i++) {
        out[i] = "0123456789abcdef"[rand() % 16];
    }
    out[32] = '\0';
}

char * ARTCGenerateToken(const char *appId, const char *appKey, const char *channelId, const char *userId, int timestamp)
{
    if (!appId || !appKey || !channelId || !userId) {
        ESP_LOGE(TAG, "generate token error: invalid params");
        return NULL;
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

    char sha256Out[Sha256Len] = {0};
    mbedtls_sha256((const unsigned char *)buffer, len, (uint8_t*)sha256Out, 0);
    free(buffer);

    // ToHex
    const char *HexChars = "0123456789abcdef";
    buffer = (char *)malloc(Sha256Len * 2 + 1);
    for (int i = 0; i < Sha256Len; ++i) {
        buffer[i * 2] = HexChars[(sha256Out[i] >> 4) & 0xf];
        buffer[i * 2 + 1] = HexChars[sha256Out[i] & 0xf];
    }
    buffer[Sha256Len * 2] = 0;
    return buffer;
}
