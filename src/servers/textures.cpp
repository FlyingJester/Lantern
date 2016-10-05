#include "textures.hpp"

#include "src/fileops.h"

#include "aimg/aimage.h"

#include <cstdio>

#ifdef _MSC_VER
#include <malloc.h>
#elif defined __GNUC__
#include <alloca.h>
#endif

namespace Lantern {

static const std::string image_dir = "images/";

LX_Texture TextureServer::doLoad(const std::string &key){
	Lantern_FileFinder *finder = (Lantern_FileFinder*)alloca(Lantern_FileFinderSize());
	
	AImg_Image image;
	LX_Texture texture;
	texture.value = 0;
	
	if(!Lantern_InitFileFinder(finder, image_dir.c_str())){
		
		fputs("Could not find images directory.\n", stderr);
		fflush(stderr);
		
	}
	else{
		
		do{
			if(Lantern_FileFinderFileSize(finder) < 64)
				continue;
			
			const char *const path = Lantern_FileFinderPath(finder);
			if(key == path + image_dir.length()){
				const unsigned err = AImg_LoadAuto(&image, path);
				if(err != AIMG_LOADPNG_SUCCESS){
					fprintf(stderr, "Error %i loading ", err);
					fputs(key.c_str(), stderr);
					fputc('\n', stderr);
				}
				else{
					texture = LX_CreateTexture();
					LX_UploadTexture(texture, image.pixels, image.w, image.h);
				}
				
				break;
			}
			
		}while(Lantern_FileFinderNext(finder));
		
	}
	
	Lantern_DestroyFileFinder(finder);
	return texture;
}

} // namespace Lantern
