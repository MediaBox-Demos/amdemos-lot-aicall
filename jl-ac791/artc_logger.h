//
// Created by Pi on 2025/06/09.
//

#ifndef artc_logger_h
#define artc_logger_h

#define ARTC_LOG_LEVEL_DEBUG 0
#define ARTC_LOG_LEVEL_INFO 1
#define ARTC_LOG_LEVEL_WARN 2
#define ARTC_LOG_LEVEL_ERROR 3
static char LogLevelTag[] = {'D', 'I', 'W', 'E'};
#define ARTC_CURRENT_LOG_LEVEL ARTC_LOG_LEVEL_DEBUG

#define ARTC_LOG_IMPL(_level, _fmt, ...) \ 
        if (_level >= ARTC_CURRENT_LOG_LEVEL) { printf("[%c] %s:%d " _fmt "\n", LogLevelTag[_level], __func__, __LINE__, ##__VA_ARGS__); } \

#define ARTC_LOG_GET_ARG_COUNT(...) ARTC_LOG_GET_ARG_COUNT_(__VA_ARGS__, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1)
#define ARTC_LOG_GET_ARG_COUNT_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, n, ...) n

#define ARTC_LOG_DISPATCH_2(_level, _fmt, ...) ARTC_LOG_IMPL(_level, _fmt, __VA_ARGS__)
#define ARTC_LOG_DISPATCH_1(_level, ...) ARTC_LOG_DISPATCH_2(_level, "%s", __VA_ARGS__)

#define ARTC_LOG_CONCAT(a, b) ARTC_LOG_CONCAT_(a, b)
#define ARTC_LOG_CONCAT_(a, b) a##b

#define ARTC_LOG_DISPATCH_(_level, _count, ...) ARTC_LOG_CONCAT(ARTC_LOG_DISPATCH_, _count)(_level, __VA_ARGS__)
#define ARTC_LOG(_level, ...) ARTC_LOG_DISPATCH_(_level, ARTC_LOG_GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define ARTC_LOG_DEBUG(...) ARTC_LOG(ARTC_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define ARTC_LOG_INFO(...) ARTC_LOG(ARTC_LOG_LEVEL_INFO, __VA_ARGS__)
#define ARTC_LOG_WARN(...) ARTC_LOG(ARTC_LOG_LEVEL_WARN, __VA_ARGS__)
#define ARTC_LOG_ERROR(...) ARTC_LOG(ARTC_LOG_LEVEL_ERROR, __VA_ARGS__)

#endif /* artc_logger_h */