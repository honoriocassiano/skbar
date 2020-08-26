#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <string.h>

namespace skbar {

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

template<typename ...T>
void CLog(const char *file, int line, const char *format, T... params) {
    printf("[%s : %d] ", file, line);
    printf(format, params...);
    printf("\n");
}

template<typename ...T>
void CError(const char *file, int line, const char *format, T... params) {
    fprintf(stderr, "[%s : %d] ", file, line);
    fprintf(stderr, format, params...);
    fprintf(stderr, "\n");
}

#ifdef USE_DEBUG_COLORS

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define Log(args...) \
        if(debug::active) {\
        printf(ANSI_COLOR_CYAN "[%s : %d] ", __FILENAME__, __LINE__); \
        printf(ANSI_COLOR_RESET args); \
        printf("\n");}

#define Error(args...) \
        if(debug::active) {\
        fprintf(stderr, ANSI_COLOR_RED "[%s : %d] ", __FILENAME__, __LINE__); \
        fprintf(stderr, ANSI_COLOR_RESET args); \
        fprintf(stderr, "\n");}

#else

#define Log(format, ...) CLog(__FILENAME__, __LINE__, format, __VA_ARGS__)
#define Error(format, ...) CError(__FILENAME__, __LINE__, format, __VA_ARGS__)

#endif

}

#endif // DEBUG_H
