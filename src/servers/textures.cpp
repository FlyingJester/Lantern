#include "textures.hpp"

#include "src/fileops.h"

#include "aimg/image.h"

#include <algorithm>
#include <cstdio>

#ifdef _MSC_VER
#include <malloc.h>
#elif defined __GNUC__
#include <alloca.h>
#endif

namespace Lantern {

void TextureServer::doTextureGenerate(const std::string &key,
	unsigned img_err, AImg_Image &image, Image &to){
	
	if(img_err != AIMG_LOADPNG_SUCCESS){
		fprintf(stderr, "Error %i loading ", img_err);
		fputs(key.c_str(), stderr);
		fputc('\n', stderr);
	}
	else{
        to.texture = LX_CreateTexture();
		LX_UploadTexture(to.texture, image.pixels, image.w, image.h);
        to.w = image.w;
        to.h = image.h;
        AImg_DestroyImage(&image);
	}
	
}

Image TextureServer::doLoad(const std::string &key){
	AImg_Image image;
    Image texture;
    texture.texture.value = 0;
	
	const std::string path = std::string("textures/") + key;
	if(AImg_LoadAuto(&image, path.c_str()) == AIMG_LOADPNG_SUCCESS){
        texture.texture = LX_CreateTexture();
		LX_UploadTexture(texture.texture, image.pixels, image.w, image.h);
        texture.w = image.w;
        texture.h = image.h;
		AImg_DestroyImage(&image);
	}
	else if(key.length() < 0x100 && texture.texture.value == 0){
		Lantern_ArchiveEntry entry;
		Lantern_Archive archive;
		std::copy(key.begin(), key.end(), entry.name);
		
		if(m_archive_server.findFile(entry, archive)){
			const char tga[] = "agt.", *const tga_end = tga + sizeof(tga),
				png[] = "gnp.", *const png_end = png + sizeof(png);
			
			if(std::mismatch(tga, tga_end, key.rbegin()).first == tga_end){
				const unsigned err =
					AImg_LoadTGAMem(&image, (uint8_t*)archive.data + entry.start, entry.length);
				doTextureGenerate(key, err, image, texture);
			}
			else if(std::mismatch(png, png_end, key.rbegin()).first == png_end){
				const unsigned err =
					AImg_LoadPNGMem(&image, (uint8_t*)archive.data + entry.start, entry.length);
				doTextureGenerate(key, err, image, texture);
			}
		}
	}
	
	return texture;
}

} // namespace Lantern
