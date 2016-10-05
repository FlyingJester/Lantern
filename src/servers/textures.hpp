#pragma once

#include "server.hpp"
#include "src/lantern_gl.h"

#include <string>

namespace Lantern {

class TextureServer : public ResourceServer<std::string, LX_Texture> {
protected:
	virtual LX_Texture doLoad(const std::string&);
	virtual void finalize(std::map<std::string, LX_Texture>::iterator i) { LX_DestroyTexture(i->second); }
};

} // namespace Lantern
