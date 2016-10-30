#include "lantern_gl_ext.h"

#include <assert.h>

#if defined(_WIN32) || defined(LANTERN_GL_EXT) || defined __CYGWIN__
    #if defined(_WIN32) || defined __CYGWIN__
        #include <Windows.h>
    #elif defined __APPLE__
        #include <OpenGL/gl.h>
        #include <OpenGL/glx.h>
    #else
        #include <GL/gl.h>
        #include <GL/glx.h>
    #endif
#else

#endif

lxBufferDataOP lxBufferData = NULL;
lxBufferOP lxGenBuffers = NULL, lxDeleteBuffers = NULL;
lxBindBufferOP lxBindBuffer = NULL;

void Lantern_InitGLExt(){
#if defined(_WIN32) || defined(LANTERN_GL_EXT) || defined __CYGWIN__

#if defined _WIN32 || defined __CYGWIN__
#define LX_LOAD(NAME, TYPE) lx ## NAME = (TYPE)wglGetProcAddress( "gl" #NAME); assert(lx ## NAME != NULL)
#else
#define LX_LOAD(NAME, TYPE) lx ## NAME = (TYPE)glXGetProcAddressARB( (unsigned char*) "gl" #NAME); assert(lx ## NAME != NULL)
#endif

LX_LOAD(GenBuffers, lxBufferOP);
LX_LOAD(DeleteBuffers, lxBufferOP);
LX_LOAD(BindBuffer, lxBindBufferOP);
LX_LOAD(BufferData, lxBufferDataOP);

#undef LX_LOAD

#endif
}
