#include "aimage.h"
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <png.h>


AIMG_EXPORT unsigned AImg_SavePNG(const struct AImg_Image *from, const char *path){
    unsigned ret = AIMG_LOADPNG_SUCCESS;
    FILE * file;
    png_structp pngs = NULL;
    png_infop info = NULL;
    png_bytep *rowlist = NULL;

    if(!from || !path)
        return AIMG_LOADPNG_IS_NULL;

    file = fopen(path, "wb");
    if(!file)
      return AIMG_LOADPNG_NO_FILE;

    pngs = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info = png_create_info_struct(pngs);
    if((!pngs)||(!info)){
        ret = AIMG_LOADPNG_BADFILE;
        goto ending;
    }

    if(setjmp(png_jmpbuf(pngs))){
        ret = AIMG_LOADPNG_PNG_ERR;
        goto ending;
    }

    png_set_IHDR(pngs, info, from->w, from->h, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    rowlist = calloc(sizeof(png_bytep), from->h*sizeof(png_byte*));
    {
        unsigned y = 0;
start_save:
        rowlist[y] = (void *)AImg_PixelConst(from, 0, y);
        y++;
        if(y<from->h)
            goto start_save;

    }
    if(setjmp(png_jmpbuf(pngs))){
      ret = AIMG_LOADPNG_PNG_ERR;
        goto ending;
    }

    png_init_io(pngs, file);

    if(setjmp(png_jmpbuf(pngs))){
      ret = AIMG_LOADPNG_PNG_ERR;
        goto ending;
    }

    png_set_rows(pngs, info, rowlist);

    if(setjmp(png_jmpbuf(pngs))){
        ret = AIMG_LOADPNG_PNG_ERR;
        goto ending;
    }

    png_write_png(pngs, info, PNG_TRANSFORM_IDENTITY, NULL);

    png_destroy_write_struct(&pngs, &info);

    ret = AIMG_LOADPNG_SUCCESS;

ending:
    if(rowlist)
        free(rowlist);
    
    png_destroy_info_struct(pngs, &info);
    png_destroy_write_struct(&pngs, &info);

    fflush(file);
    fclose(file);

    return ret;
}
