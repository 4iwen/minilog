#ifndef LOG_H
#define LOG_H

#include <cstdio>
#include <cstdarg>
#include <mutex>
#include <chrono>
#include <iostream>
#include <iomanip>

#define log_trace(...) log(TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log(DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log(INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log(WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log(ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log(FATAL, __FILE__, __LINE__, __VA_ARGS__)

enum LogLevel {
    TRACE, DEBUG, INFO, WARN, ERROR, FATAL
};

const char *logLevelStrings[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

#ifndef LOG_NO_COLOR
const char *logLevelColors[] = {"\x1b[90m", "\x1b[32m", "\x1b[36m", "\x1b[33m", "\x1b[31m", "\x1b[97;41m"};
const char *resetColor = "\x1b[0m";
#else
const char *logLevelColors[] = {"" /* gray */, "" /* green */, "" /* cyan */, "" /* yellow */, "" /* red */, "" /* white on red */};
const char *resetColor = "";
#endif

std::mutex mtx;

void log(LogLevel level, const char *file, int line, const char *fmt, ...) {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto epoch = now_ms.time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::lock_guard<std::mutex> guard(mtx);

    char buf[100];
    if (std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&now_c))) {
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%H:%M:%S");
        std::string formatted_time = ss.str();

        va_list args;
        va_start(args, fmt);

        fprintf(stdout, "%s.%03ld %s%-5s%s %s:%d: ", formatted_time.c_str(), value.count() % 1000,
                logLevelColors[level], logLevelStrings[level], resetColor, file, line);

        vfprintf(stdout, fmt, args);

        fprintf(stdout, "\n");

        va_end(args);
    } else {
        fprintf(stderr, "Error formatting time\n");
    }
}

#endif //LOG_H