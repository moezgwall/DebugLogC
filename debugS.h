#ifndef DEBUG_INFO_H
#define DEBUG_INFO_H

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define COMPILER_INFO \
    "Compiled on " __DATE__ " at " __TIME__ \
    " using " COMPILER_NAME

#if defined(__clang__)
    #define COMPILER_NAME "Clang " __clang_version__
#elif defined(__GNUC__)
    #define COMPILER_NAME "GCC " __VERSION__
#elif defined(_MSC_VER)
    #define COMPILER_NAME "MSVC"
#else
    #define COMPILER_NAME "Unknown Compiler"
#endif

typedef enum
{
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LGO_GL

} LogLevel;

typedef struct
{
    FILE *logfile;
    bool isConsoleEnabled;
    bool isFileEnabled;
    bool isTimeEnabled;
    LogLevel loglvl;
    void (*customOutput)(const char *msg); // a callback to make your customMsgs
} DebugS;

DebugS DebugInit(const char *pathLogfile, bool toConsole, bool toFile);
void EndDebugMode(DebugS *debug);
void DebugLog(DebugS *debug, LogLevel lvl, const char *fmt, ...);
void DebugPrintBuildInfo(DebugS *debug);
DebugS DebugInit(const char *pathLogfile, bool toConsole, bool toFile)
{
    DebugS debug = {0};
    debug.isConsoleEnabled = toConsole;
    debug.isFileEnabled = toFile;
    debug.isTimeEnabled = true;
    debug.loglvl = LOG_INFO;
    debug.customOuput = NULL;

    if (toFile == true)
    {
        debug.logfile = fopen(pathLogfile, "a");
        if (!debug.logfile)
        {
            fprintf(stderr, "Failed to open logfile [%s]", pathLogfile);
            debug.isFileEnabled = false;
        }
    }

    return debug;
}

void EndDebugMode(DebugS *debug)
{
    if (!debug)
    {
        return;
    }
    if (debug->logfile)
    {
        fclose(debug->logfile);
        debug->logfile = NULL;
        return;
    }
}

void DebugLog(DebugS *debug, LogLevel lvl, const char *fmt, ...)
{

    const char *debugLvl[] = {"INFO", "WARN", "ERROR", "GL"};
    const char *colorMode;

    switch (lvl)
    {
    case LOG_INFO:
        colorMode = "\x1b[32m";
        break;
    case LOG_ERROR:
        colorMode = "\x1b[31m";
        break;
    case LOG_WARN:
        colorMode = "\x1b[33m";
        break;
    default:
        colorMode = "\x1b[0m";
        break;
    }

    char buffer[1024];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    char currentTime[128] = "";
    if (debug->isTimeEnabled)
    {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        strftime(currentTime, sizeof(currentTime), "[%H:%M:%S] ", t);
    }

    char final[2048];
    snprintf(final, sizeof(final), "%s[%s]: %s \n", currentTime, debugLvl[lvl], buffer);

    if (debug->isConsoleEnabled)
    {
        fprintf(stderr, "%s%s\x1b[0m", colorMode, final);
    }
    if (debug->logfile)
    {
        if (debug->isFileEnabled)
        {
            fprintf(debug->logfile, "%s", final);
            fflush(debug->logfile);
        }
    }
    if (debug->customOutput)
    {
        debug->customOutput(finalMsg);
    }
}

void DebugPrintBuildInfo(DebugS *debug)
{
    DebugLog(debug, LOG_INFO, "=== Build Info ===");
    DebugLog(debug, LOG_INFO, "Compiler: %s", COMPILER_NAME);
    DebugLog(debug, LOG_INFO, "Date    : %s", __DATE__);
    DebugLog(debug, LOG_INFO, "Time    : %s", __TIME__);

    #ifdef DEBUG
        DebugLog(debug, LOG_INFO, "Build   : DEBUG");
    #else
        DebugLog(debug, LOG_INFO, "Build   : RELEASE");
    #endif
}

#endif
