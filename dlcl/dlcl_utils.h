/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/ */
#pragma once
#include <stdbool.h>

#if defined(_MSC_VER) || defined(__WATCOMC__)
    #ifdef DLCL_EXPORTS
        #define DLCL_EXPORT __declspec(dllexport)
    #else
        #define DLCL_EXPORT __declspec(dllimport)
    #endif
#else
    #define DLCL_EXPORT
#endif

#ifdef __cplusplus
    #define DLCL_DEFAULT_ARG(X) = X
    extern "C" {
#else
    #define DLCL_DEFAULT_ARG(X)
#endif
void DLCL_Utils_NumberToString(char *to, int i,
    int len DLCL_DEFAULT_ARG(-1),
    bool null_term DLCL_DEFAULT_ARG(true));

#ifdef __cplusplus
}
#endif
