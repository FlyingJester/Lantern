#pragma once
#include "value.h"

#if defined(_MSC_VER) || defined(__WATCOMC__)
    #ifdef TURBOJSON_EXPORTS
        #define TURBOJSON_EXPORT __declspec(dllexport)
    #else
        #define TURBOJSON_EXPORT __declspec(dllimport)
    #endif /* TURBOJSON_EXPORTS */
#endif /* MSVC or Watcom */

#ifdef __cplusplus
extern "C" {
#endif

TURBOJSON_EXPORT
int TurboJSON(struct Turbo_Value *to, const char *in, const char *end);
#define Turbo_Parse TurboJSON
TURBOJSON_EXPORT
int TurboFree(struct Turbo_Value *that);
#define Turbo_FreeParse TurboFree

#ifdef __cplusplus
}
#endif
