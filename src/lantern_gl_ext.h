#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifndef GL_ARRAY_BUFFER
#ifdef GL_ARRAY_BUFFER_ARB
#define GL_ARRAY_BUFFER GL_ARRAY_BUFFER_ARB
#else
#define GL_ARRAY_BUFFER 0x8892
#endif
#endif

#define LX_ARRAY_BUFFER 0x8892

#ifndef GL_STATIC_DRAW
#ifdef GL_STATIC_DRAW_ARB
#define GL_STATIC_DRAW GL_STATIC_DRAW_ARB
#else
#define GL_STATIC_DRAW 0x88E4
#endif
#endif

#define LX_STATIC_DRAW 0x88E4

#ifndef GL_FILL_RECTANGLE
#ifdef GL_FILL_RECTANGLE_EXT
#define GL_FILL_RECTANGLE GL_FILL_RECTANGLE_EXT
#elif defined GL_FILL_RECTANGLE_ARB
#define GL_FILL_RECTANGLE GL_FILL_RECTANGLE_ARB
#elif defined GL_FILL_RECTANGLE_NV
#define GL_FILL_RECTANGLE GL_FILL_RECTANGLE_NV
#else
#define GL_FILL_RECTANGLE_NV 0x933C
#define GL_FILL_RECTANGLE GL_FILL_RECTANGLE_NV
#endif
#endif

#define LX_FILL_RECTANGLE 0x933C

#if defined(_WIN32) || defined(LANTERN_GL_EXT) || defined __CYGWIN__

typedef void(*lxBufferDataOP)(GLenum target, GLsizei size, const GLvoid *data, GLenum usage);
extern lxBufferDataOP lxBufferData;

typedef void(*lxBufferOP)(GLsizei n, GLuint *buffer);
extern lxBufferOP lxGenBuffers;
extern lxBufferOP lxDeleteBuffers;

typedef void(*lxBindBufferOP)(GLenum n, GLuint buffer);
extern lxBindBufferOP lxBindBuffer;

#else

#define lxBufferData glBufferData
#define lxBindBuffer glBindBuffer
#define lxGenBuffers glGenBuffers
#define lxDeleteBuffers glDeleteBuffers

#endif

void Lantern_InitGLExt();
