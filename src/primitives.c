#include "primitives.h"
#include "primitives_low.h"
#include "lantern_gl.h"

#include <assert.h>

const float rectangle_uv[16] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

LX_Buffer global_uv_buffer;

unsigned Lantern_PrimitiveSize(){
	return sizeof(struct Lantern_Primitive);
}

void Lantern_InitPrimitivesSubsystem(){
	LX_EnableVertexBuffer();
	LX_EnableTexCoordBuffer();
	
	LX_EnableTexture();
	LX_EnableBlending();
	LX_DisableDepthTest();
	
	global_uv_buffer = LX_CreateBuffer();
	LX_BufferData(global_uv_buffer, sizeof(rectangle_uv), rectangle_uv);
}

void Lantern_InitPrimitive(struct Lantern_Primitive *primitive){
	assert(primitive);

	primitive->vbo = LX_CreateBuffer();
	primitive->length = 0;
	primitive->type = eLX_Strip;
	primitive->texture.value = LX_INVALID_VALUE;
}

void Lantern_DestroyPrimitive(struct Lantern_Primitive *primitive){
	assert(primitive);
	LX_DestroyBuffer(primitive->vbo);
}

void Lantern_CreateRectangle(struct Lantern_Primitive *primitive, unsigned w, unsigned h, LX_Texture tex){

    RECTANGLE_COORDS(data, 0, 0, w, h);
	
	LX_UploadBuffer(primitive->vbo, data, sizeof(data));
	primitive->length = 4;
	primitive->texture.value = tex.value;
}

void Lantern_DrawPrimitiveLow(LX_Buffer vertex, LX_Buffer tex_coord, unsigned length, enum LX_DrawType type);
	LX_SetVertexBuffer(vertex);
	LX_SetTexCoordBuffer(tex_coord);
	LX_DrawArrays(type, length);
}

void Lantern_DrawPrimitive(const struct Lantern_Primitive *primitive){
    LX_SetTexture(primitive->texture);
    Lantern_DrawPrimitiveLow(primitive->vbo, global_uv_buffer, primitive->length, primitive->type);
}
