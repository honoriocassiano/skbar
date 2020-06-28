#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <string.h>

namespace skbar {

    namespace debug {
        static bool active = true;
    }

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

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

#define Log(args...) \
        if(debug::active) {\
        printf("[%s : %d] ", __FILENAME__, __LINE__); \
        printf(args); \
        printf("\n");}

#define Error(args...) \
        if(debug::active) {\
        fprintf(stderr, "[%s : %d] ", __FILENAME__, __LINE__); \
        fprintf(stderr, args); \
        fprintf(stderr, "\n");}

#endif

}

#endif // DEBUG_H
