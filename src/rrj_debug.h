#ifndef RRJ_DEBUG_H
#define RRJ_DEBUG_H

#include <stdio.h>
#include <stdlib.h>

#ifndef NDEBUG
    #define RRJ_DEBUG_PRINTF printf
    #define RRJ_DEBUG_FPRINTF fprintf
    #define RRJ_DEBUG_PUTS puts
    #define RRJ_DEBUG_FPUTS fputs
    #define RRJ_DEBUG_FFLUSH fflush
    #define RRJ_DEBUG_GETENV getenv
#else
    #define RRJ_DEBUG_PRINTF(...) ((void)0, 0)
    #define RRJ_DEBUG_FPRINTF(...) ((void)0, 0)
    #define RRJ_DEBUG_PUTS(...) ((void)0, 0)
    #define RRJ_DEBUG_FPUTS(...) ((void)0, 0)
    #define RRJ_DEBUG_FFLUSH(...) ((void)0, 0)
    #define RRJ_DEBUG_GETENV(...) ((char *)0)
#endif

#endif
