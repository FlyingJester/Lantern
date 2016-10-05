#pragma once

/* This is kept in its own file to make sure we don't actually include Windows.h or gl.h in other
 * code. Both of them spit out a BUNCH of annoying warnings and weird defines. It also holds the
 * extension components.
 * Our GL wrapper lives in the LX namespace. We also add just a little bit of strong typing.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define LX_INVALID_VALUE 0

#define LX_TYPE(X) typedef struct { unsigned value; } LX_ ## X; LX_ ## X LX_Create ## X(); void LX_Destroy ## X (LX_ ## X ); void LX_Bind ## X(LX_ ## X)
#define LX_V(X) ((X).value)

LX_TYPE(Texture);
LX_TYPE(Buffer);
LX_TYPE(VertexArray);
LX_TYPE(PixelBuffer);
LX_TYPE(FrameBuffer);

#undef LX_TYPE

/* Invalidates the current bindings */
void LX_UploadTexture(LX_Texture texture, const void *pixels, unsigned w, unsigned h);
#define LX_TextureData(TEX, W, H, PIX) LX_UploadTexture(TEX, PIX, W, H)
/* Invalidates the current bindings */
void LX_UploadBuffer(LX_Buffer buffer, const void *data, unsigned size);
#define LX_BufferData(BUFFER, SIZE, DATA) LX_UploadBuffer(BUFFER, DATA, SIZE)

#define LX_ENABLEABLE(X) void LX_Enable ## X(); void LX_Disable ## X()
#define LX_SETTABLE(X, Z) LX_ENABLEABLE(X); void LX_Set ## X(LX_ ## Z)

LX_SETTABLE(Texture, Texture);
LX_SETTABLE(VertexBuffer, Buffer);
LX_SETTABLE(TexCoordBuffer, Buffer);
LX_SETTABLE(ColorBuffer, Buffer);

LX_ENABLEABLE(DepthTest);
LX_ENABLEABLE(Blending);

#undef LX_ENABLEABLE
#undef LX_SETTABLE

enum LX_DrawType { eLX_Fan, eLX_Strip, eLX_Quads, eLX_LineLoop, eLX_Points };
void LX_DrawArrays(enum LX_DrawType type, unsigned num_vertices);

#ifdef __cplusplus
}
#endif
