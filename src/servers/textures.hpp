#pragma once

#include "server.hpp"
#include "archives.hpp"
#include "src/lantern_gl.h"

#include <string>

struct AImg_Image;

namespace Lantern {

class TextureServer : public ResourceServer<std::string, LX_Texture> {
	const ArchiveServer &m_archive_server;
	static void doTextureGenerate(const std::string &key, unsigned img_err, AImg_Image &image, LX_Texture &to); 
public:
	TextureServer(const ArchiveServer &archive_server) : m_archive_server(archive_server) { }
protected:
	virtual LX_Texture doLoad(const std::string&);
	virtual void finalize(std::map<std::string, LX_Texture>::iterator i) { LX_DestroyTexture(i->second); }
};

} // namespace Lantern
