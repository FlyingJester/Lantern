#include "primitives.h"
#include "lantern_gl.h"

#include <assert.h>

const float rectangle_uv[16] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f
};

LX_Buffer global_uv_buffer;

struct Lantern_Primitive{
	LX_Buffer vbo;
	unsigned length;
	enum LX_DrawType type;
	LX_Texture texture;
};

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

	float data[8] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};
	
	data[2] = data[4] = w;
	data[5] = data[7] = h;
	
	LX_UploadBuffer(primitive->vbo, data, sizeof(data));
	primitive->length = 4;
	primitive->texture.value = tex.value;
}

void Lantern_DrawPrimitive(const struct Lantern_Primitive *primitive){
	LX_SetVertexBuffer(primitive->vbo);
	LX_SetTexCoordBuffer(global_uv_buffer);
	LX_DrawArrays(primitive->type, primitive->length);
}
