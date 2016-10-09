#pragma once

#include "server.hpp"
#include "archives.hpp"
#include "src/lantern_gl.h"

#include <string>

struct AImg_Image;

namespace Lantern {

struct Image {
    LX_Texture texture;
    unsigned w, h;
};

class TextureServer : public ResourceServer<std::string, Image> {
	const ArchiveServer &m_archive_server;
	static void doTextureGenerate(const std::string &key, unsigned img_err, AImg_Image &image, Image &to);
public:
	TextureServer(const ArchiveServer &archive_server) : m_archive_server(archive_server) { }
protected:
	virtual Image doLoad(const std::string&);
	virtual void finalize(std::map<std::string, Image>::iterator i) { LX_DestroyTexture(i->second.texture); }

};

} // namespace Lantern
