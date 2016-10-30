#pragma once

#if defined(__GNUC__) || defined (__clang__)

#define LANTERN_NOT_NULL(X) X __attribute__((returns_nonnull))
#define LANTERN_PURE(X) X __attribute__((pure))
#define LANTERN_CONST(X) X __attribute__((const))
#define LANTERN_NO_THROW(X) X __attribute__((nothrow))
#define LANTERN_NO_RETURN(X) X __attribute__((noreturn))
#define LANTERN_WARN_UNUSED_RESULT(X) X __attribute__((warn_unused_result))
#define LANTERN_UNLIKELY_FUNCTION(X) X __attribute__((cold))
#define LANTERN_LIKELY_FUNCTION(X) X __attribute__((hot))
#define LANTERN_NEW_RETURN(X) X __attribute__((malloc))

#define LANTERN_LIKELY(X)   (__builtin_expect(!!(X), 1))
#define LANTERN_UNLIKELY(X) (__builtin_expect(!!(X), 0))

#define LANTERN_RESTRICT __restrict__

#elif defined _MSC_VER

#define LANTERN_NOT_NULL(X) X
#define LANTERN_PURE(X) X
#define LANTERN_CONST(X) X
#define LANTERN_NO_THROW(X) __declspec(nothrow) X
#define LANTERN_NO_RETURN(X) __declspec(noreturn) X
#define LANTERN_UNLIKELY_FUNCTION(X) X
#define LANTERN_LIKELY_FUNCTION(X) X
#define LANTERN_NEW_RETURN(X) __declspec(restrict) X

#define LANTERN_LIKELY(X)   (!!(X))
#define LANTERN_UNLIKELY(X) (!!(X))

#define LANTERN_RESTRICT __restrict

#else

#define LANTERN_NOT_NULL(X) X
#define LANTERN_PURE(X) X
#define LANTERN_CONST(X) X
#define LANTERN_NO_THROW(X) X
#define LANTERN_NO_RETURN(X) X
#define LANTERN_UNLIKELY_FUNCTION(X) X
#define LANTERN_LIKELY_FUNCTION(X) X
#define LANTERN_NEW_RETURN(X) X

#define LANTERN_LIKELY(X)   (!!(X))
#define LANTERN_UNLIKELY(X) (!!(X))

#define LANTERN_RESTRICT

#endif

#if __x86_64__ || ((defined (_WIN32)) && !defined(_WIN64))
    #ifdef __GNUC__
        #define LANTERN_FASTCALL
    #else
        #define LANTERN_FASTCALL __fastcall
    #endif
#else
#define LANTERN_FASTCALL
#endif
