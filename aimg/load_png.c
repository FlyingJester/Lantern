#include "aimage.h"
#include "../bufferfile/bufferfile.h"
#include <string.h>
#include <stdlib.h>
#include <png.h>

struct aimg_png_buffer { const void *data; int size; unsigned at; };

static void AImg_PNGReadCallback(png_struct *png_ctx, png_byte *data, png_size_t length){
    struct aimg_png_buffer *data_buffer = png_get_io_ptr(png_ctx);
    if(data_buffer->at + length > data_buffer->size){
        png_error(png_ctx, "Unexpected EOF");
        return;
    }
    memcpy(data, ((png_byte *)data_buffer->data) + data_buffer->at, length);
    data_buffer->at += length;
}

unsigned AImg_LoadPNG(struct AImg_Image *to, const char *path){
	void *data;
	int size;
	
    if(!to || !path)
        return AIMG_LOADPNG_IS_NULL;
	
	data = BufferFile(path, &size);
    if(data == NULL)
        return AIMG_LOADPNG_NO_FILE;
	
	{
		const int r = AImg_LoadPNGMem(to, data, size);
		FreeBufferFile(data, size);
		return r;
	}
}

AIMG_EXPORT unsigned AImg_LoadPNGMem(struct AImg_Image *to, const void *data, unsigned size){

    struct aimg_png_buffer data_buffer;
    unsigned char color_type, bit_depth;
    int interlaced, num_passes;
    png_struct *png_ctx;
    png_info *png_info;
	
	if(num_passes){}

    memset(&data_buffer, 0, sizeof(struct aimg_png_buffer));

	data_buffer.size = size;
	data_buffer.data = data;

    if(png_sig_cmp(data_buffer.data, 0, 8)!=0){}
        return AIMG_LOADPNG_BADFILE;

    if(!(png_ctx = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL))){
#ifndef AIMG_NO_STDIO
        fputs("Could not create PNG read struct", stderr);
#endif
        return AIMG_LOADPNG_PNG_ERR;
    }
    if(!(png_info = png_create_info_struct(png_ctx))){
#ifndef AIMG_NO_STDIO
        fputs("Could not create PNG info struct", stderr);
#endif
        return AIMG_LOADPNG_PNG_ERR;
    }

    if(setjmp(png_jmpbuf(png_ctx)))
        return AIMG_LOADPNG_PNG_ERR; 

    png_set_read_fn(png_ctx, &data_buffer, AImg_PNGReadCallback);
    png_set_sig_bytes(png_ctx, 0);
    
    if(setjmp(png_jmpbuf(png_ctx))){
#ifndef AIMG_NO_STDIO
        fputs("Error reading PNG info struct", stderr);
#endif
        return AIMG_LOADPNG_PNG_ERR;
    }
    png_read_info(png_ctx, png_info);
        
    if(setjmp(png_jmpbuf(png_ctx))){
#ifndef AIMG_NO_STDIO
        fputs("Error reading PNG attributes", stderr);
#endif
        return AIMG_LOADPNG_PNG_ERR;
    }
    
/* We do not use the AImg_CreateImage function here because there are a couple
 * error cases left, and this avoids having to deallocate the pixel buffer if they
 * occur.
 */
    to->w = png_get_image_width(png_ctx, png_info);
    to->h = png_get_image_height(png_ctx, png_info);
    
    if((interlaced = png_get_interlace_type(png_ctx, png_info))!=PNG_INTERLACE_NONE){
/*        num_passes = png_set_interlace_handling(png_ctx);*/
    }
    bit_depth = png_get_bit_depth(png_ctx, png_info);
    
    if((color_type = png_get_color_type(png_ctx, png_info)) == PNG_COLOR_TYPE_PALETTE){
        png_set_expand(png_ctx);
    }
    else if(bit_depth<8){
        png_set_expand(png_ctx);
    }

    if(png_get_valid(png_ctx, png_info, PNG_INFO_tRNS))
        png_set_expand(png_ctx);

    if(bit_depth == 16)
        png_set_strip_16(png_ctx);
    if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ctx);
    
    png_read_update_info(png_ctx, png_info);

    if((bit_depth = png_get_bit_depth(png_ctx, png_info))!=8){
        return AIMG_LOADPNG_NFORMAT;
    }

    color_type = png_get_color_type(png_ctx, png_info);

    to->pixels = malloc((to->w * to->h << 2));
    {
        unsigned char *row_buf = NULL;
        unsigned h = 0;
/* We kind of had to cheat to avoid a 'loop'. MSVC refused to allow tail-call recursion here, and
 * Solaris Studio refused on `start_pixel_mux'. I suspect this is because of memcpy builtins.
 * So I just used gotos. Which are loops, in a way. I still avoided using the `while' keyword. 
 * And this is kind of not in the game engine proper, so whatever.
 */
start_row:
        {
            unsigned char * row_p = (unsigned char *)(to->pixels + to->w * h);
            png_read_row(png_ctx, row_p, NULL);
            if(!(color_type & PNG_COLOR_MASK_ALPHA)){
                unsigned w = 0;
                unsigned char *row_mem;
                if(!row_buf)
                    row_buf = malloc(to->w*3);
                
                row_mem = row_buf;
                
                memcpy(row_mem, row_p, to->w * 3);

start_pixel_mux:
                memcpy(row_p, row_mem, 3);
                row_p += 3;
                row_mem += 3;
                    
                row_p[0] = 0xFF;
                row_p++;

                if(++w<to->w)
                    goto start_pixel_mux;
            }
        }
        if(++h < to->h)
            goto start_row;
        else if(row_buf)
            free(row_buf);
    }

    return AIMG_LOADPNG_SUCCESS;

}
