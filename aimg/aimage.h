#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined _MSC_VER || (defined __WATCOMC__)
	#ifdef AIMG_EXPORTS
		#define AIMG_EXPORT __declspec(dllexport)
	#else
		#define AIMG_EXPORT __declspec(dllimport)
	#endif
#else
	#define AIMG_EXPORT
#endif

#ifdef __WATCOMC__ 
#define CCALL _cdecl
#else
#define CCALL
#endif

struct AImg_Image{
    uint32_t *pixels;
    unsigned w, h;
};

AIMG_EXPORT
void CCALL AImg_CloneImage(struct AImg_Image *to, const struct AImg_Image *from);

AIMG_EXPORT
void CCALL AImg_SetPixel(struct AImg_Image *to, int x, int y, uint32_t color);
AIMG_EXPORT
uint32_t CCALL AImg_GetPixel(struct AImg_Image *to, int x, int y);

AIMG_EXPORT
void CCALL AImg_CreateImage(struct AImg_Image *that, unsigned w, unsigned h);
AIMG_EXPORT
void CCALL AImg_DestroyImage(struct AImg_Image *that);

AIMG_EXPORT
uint32_t CCALL AImg_RGBARawBlend(uint32_t src, uint32_t dst);

AIMG_EXPORT
void CCALL AImg_Blit(const struct AImg_Image *src, struct AImg_Image *dst, int x, int y);

/*
 * Semi-private, does no clipping.
 */
AIMG_EXPORT
uint32_t *CCALL AImg_Pixel(struct AImg_Image *to, int x, int y);
AIMG_EXPORT
const uint32_t *CCALL AImg_PixelConst(const struct AImg_Image *to, int x, int y);

AIMG_EXPORT
uint32_t CCALL AImg_RGBAToRaw(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
AIMG_EXPORT
void CCALL AImg_RawToRGBA(uint32_t rgba, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a);
AIMG_EXPORT
uint8_t CCALL AImg_RawToR(uint32_t rgba);
AIMG_EXPORT
uint8_t CCALL AImg_RawToG(uint32_t rgba);
AIMG_EXPORT
uint8_t CCALL AImg_RawToB(uint32_t rgba);
AIMG_EXPORT
uint8_t CCALL AImg_RawToA(uint32_t rgba);

/* From and to can be the same image safely. */
AIMG_EXPORT
void CCALL AImg_FlipImageVertically(const struct AImg_Image *from, struct AImg_Image *to);

#define AIMG_LOADPNG_SUCCESS 0u
#define AIMG_LOADPNG_NO_FILE 1u
#define AIMG_LOADPNG_BADFILE 2u
#define AIMG_LOADPNG_NFORMAT 4u
#define AIMG_LOADPNG_IS_NULL 8u
#define AIMG_LOADPNG_PNG_ERR 16u

/*
 * Returns 0 on success
 */
AIMG_EXPORT
unsigned CCALL AImg_LoadAuto(struct AImg_Image *to, const char *path);
AIMG_EXPORT
unsigned CCALL AImg_LoadPNG(struct AImg_Image *to, const char *path);
AIMG_EXPORT
unsigned CCALL AImg_LoadPNGMem(struct AImg_Image *to, const void *buffer, unsigned size);
AIMG_EXPORT
unsigned CCALL AImg_LoadTGA(struct AImg_Image *to, const char *path);
AIMG_EXPORT
unsigned CCALL AImg_LoadTGAMem(struct AImg_Image *to, const void *buffer, unsigned size);
unsigned AImg_LoadArctic(struct AImg_Image *to, const char *path);

AIMG_EXPORT
unsigned CCALL AImg_SaveAuto(const struct AImg_Image *from, const char *path);
AIMG_EXPORT
unsigned CCALL AImg_SavePNG(const struct AImg_Image *from, const char *path);
AIMG_EXPORT
unsigned CCALL AImg_SaveTGA(const struct AImg_Image *from, const char *path);
unsigned AImg_SaveArctic(const struct AImg_Image *from, const char *path);

#ifdef __cplusplus
}
#endif
