#pragma once
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER) || defined(__WATCOMC__)
    #ifdef TURBOJSON_EXPORTS
        #define TURBOJSON_EXPORT __declspec(dllexport)
    #else
        #define TURBOJSON_EXPORT __declspec(dllimport)
    #endif /* TURBOJSON_EXPORTS */
#endif /* MSVC or Watcom */

enum Turbo_Type {TJ_Error, TJ_String, TJ_Number, TJ_Object, TJ_Array, TJ_Boolean, TJ_Null};

struct Turbo_Value{
    enum Turbo_Type type;
    unsigned length;
    union{
        double number;
        int boolean;
        const char *string;
        struct Turbo_Value *array;
        struct Turbo_Property *object;
        struct Turbo_Error *error;
    } value;
};

TURBOJSON_EXPORT
const char *Turbo_Value(struct Turbo_Value *to, const char *in, const char *const end);
TURBOJSON_EXPORT
void Turbo_WriteValue(struct Turbo_Value *that, FILE *out, int level);

#ifdef __cplusplus
}
#endif
