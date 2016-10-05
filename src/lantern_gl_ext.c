#include "lantern_gl_ext.h"

#include <assert.h>

#if defined(_WIN32) || defined(LANTERN_GL_EXT) || defined __CYGWIN__
#include <Windows.h>
#else
#error Add glX or egl or whatever here!
#endif

lxBufferDataOP lxBufferData = NULL;
lxBufferOP lxGenBuffers = NULL, lxDeleteBuffers = NULL;
lxBindBufferOP lxBindBuffer = NULL;

void Lantern_InitGLExt(){
#if defined(_WIN32) || defined(LANTERN_GL_EXT) || defined __CYGWIN__

#if defined _WIN32 || defined __CYGWIN__
#define LX_LOAD(NAME, TYPE) lx ## NAME = (TYPE)wglGetProcAddress( "gl" #NAME); assert(lx ## NAME != NULL)
#else
#error Add glX or egl or whatever here!
#endif

LX_LOAD(GenBuffers, lxBufferOP);
LX_LOAD(DeleteBuffers, lxBufferOP);
LX_LOAD(BindBuffer, lxBindBufferOP);
LX_LOAD(BufferData, lxBufferDataOP);

#undef LX_LOAD

#endif
}
