#include "lantern_gl.h"
#include "lantern_gl_ext.h"
#include "lantern_attributes.h"

#include <GL/gl.h>

#include <assert.h>

void LX_UploadTexture(LX_Texture texture, const void *pixels, unsigned w, unsigned h){
	glBindTexture(GL_TEXTURE_2D, texture.value);
	assert(glGetError() == GL_NO_ERROR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	assert(glGetError() == GL_NO_ERROR);
    glFinish();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

LX_Texture LX_CreateTexture(){
	GLuint tex;
	LX_Texture ret;
	glGenTextures(1, &tex);
	assert(glGetError() == GL_NO_ERROR);
	ret.value = tex;
	return ret;
}

void LX_DestroyTexture(LX_Texture b){
	GLuint buf = b.value;
	glDeleteTextures(1, &buf);
	assert(glGetError() == GL_NO_ERROR);
}

LX_Buffer LX_CreateBuffer(){
	GLuint buf;
	LX_Buffer ret;
	lxGenBuffers(1, &buf);
	assert(glGetError() == GL_NO_ERROR);
	ret.value = buf;
	return ret;
}

void LX_DestroyBuffer(LX_Buffer b){
	GLuint buf = b.value;
	lxDeleteBuffers(1, &buf);
	assert(glGetError() == GL_NO_ERROR);
}

void LX_UploadBuffer(LX_Buffer buffer, const void *data, unsigned size){
	lxBindBuffer(GL_ARRAY_BUFFER, buffer.value);
	assert(glGetError() == GL_NO_ERROR);
	lxBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	assert(glGetError() == GL_NO_ERROR);
}

void LX_EnableTexture(){
	glEnable(GL_TEXTURE_2D);
	assert(glGetError() == GL_NO_ERROR);
}

void LX_DisableTexture(){
	glDisable(GL_TEXTURE_2D);
	assert(glGetError() == GL_NO_ERROR);
}

void LX_BindTexture(LX_Texture x){
	glBindTexture(GL_TEXTURE_2D, x.value);
	assert(glGetError() == GL_NO_ERROR);
}

void LX_SetTexture(LX_Texture x){
	LX_BindTexture(x);
}

#define CLIENT_STATE(X, Z, N, T)\
void LX_Enable ## X ## Buffer(){ glEnableClientState(GL_ ## Z ## _ARRAY); assert(glGetError() == GL_NO_ERROR); }\
void LX_Disable ## X ## Buffer(){ glDisableClientState(GL_ ## Z ## _ARRAY); assert(glGetError() == GL_NO_ERROR); }\
void LX_Set ## X ## Buffer(LX_Buffer b){ lxBindBuffer(GL_ARRAY_BUFFER, b.value); assert(glGetError() == GL_NO_ERROR); gl ## X ## Pointer(N, GL_ ## T, 0, NULL); assert(glGetError() == GL_NO_ERROR); }

CLIENT_STATE(Vertex, VERTEX, 2, FLOAT)
CLIENT_STATE(TexCoord, TEXTURE_COORD, 2, FLOAT)
CLIENT_STATE(Color, COLOR, 4, UNSIGNED_BYTE)

void LX_EnableDepthTest(){
	glEnable(GL_DEPTH_TEST);
	assert(glGetError() == GL_NO_ERROR);
}

void LX_DisableDepthTest(){
	glDisable(GL_DEPTH_TEST);
	assert(glGetError() == GL_NO_ERROR);
}

void LX_EnableBlending(){
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	assert(glGetError() == GL_NO_ERROR);
}

void LX_DisableBlending(){
	glDisable(GL_BLEND);
	assert(glGetError() == GL_NO_ERROR);
}

static GLenum LANTERN_CONST(lx_get_gl_draw_type(enum LX_DrawType type));
static GLenum lx_get_gl_draw_type(enum LX_DrawType type){
	switch(type){
		case eLX_Quads:
		case eLX_Fan:    return GL_TRIANGLE_FAN;
		case eLX_Strip:  return GL_TRIANGLE_STRIP;
		case eLX_LineLoop:  return GL_LINE_LOOP;
		case eLX_Points:    return GL_POINTS;
		
	}
	return GL_TRIANGLE_STRIP;
}

/* enum LX_DrawType { eLX_Fan, eLX_Quads, eLX_Strip, eLineLoop, ePoints }; */
void LX_DrawArrays(enum LX_DrawType type, unsigned num_vertices){
	if(type == eLX_Quads){
		unsigned i;
		for(i = 0; i < num_vertices >> 2; i++){
			glDrawArrays(GL_TRIANGLE_FAN, i << 2, 4);
			assert(glGetError() == GL_NO_ERROR);
		}
	}
	else{
		glDrawArrays(lx_get_gl_draw_type(type), 0, num_vertices);
		assert(glGetError() == GL_NO_ERROR);
	}
}

void LX_Ortho(float width, float height) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1.0, 1.0);
}

void LX_PushMatrix() {
    glPushMatrix();
}

void LX_Translate(float x, float y) {
    glTranslatef(x, y, 0.0f);
}

void LX_PopMatrix(){
    glPopMatrix();
}
