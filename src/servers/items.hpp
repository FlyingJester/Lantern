#pragma once

#include "server.hpp"
#include "archives.hpp"

#include <string>

namespace Lantern {

class Item;

class ItemServer : public ResourceServer<std::string, Item*> {
	const ArchiveServer &m_archive_server;
	// arg should be a pointer to a std::string
	static bool ItemMatchesName(const void *data, unsigned length, void *arg);
public:
	ItemServer(const ArchiveServer &archive_server) : m_archive_server(archive_server) { }
protected:
	virtual Item *doLoad(const std::string&);
	virtual void finalize(std::map<std::string, Item*>::iterator i);
};

} // namespace Lantern
