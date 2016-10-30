#pragma once

#include "src/cpio.h"
#include "src/lantern_attributes.h"

#include <vector>

namespace Lantern {

class ArchiveServer {
	std::vector<Lantern_Archive> m_archives;
	
	static bool FindFirstMatchCallback(void *LANTERN_RESTRICT arg,
		const struct Lantern_Archive *LANTERN_RESTRICT archive,
		const struct Lantern_ArchiveEntry *LANTERN_RESTRICT file);
	
public:
	
	inline void append(const struct Lantern_Archive &archive) { m_archives.push_back(archive); }
	
	// The file name should be filled in on the entry when this is called. Returns false if no matching path is found.
	bool findFile(struct Lantern_ArchiveEntry &in_out_entry, Lantern_Archive &archive) const;
	
	// Fills out the entry for the first file which the callback returns true on. Returns false if no files match.
	// Notably, this means the callback's return value is the opposite of what would be used for Lantern_EnumerateArchive.
	bool findFirstMatch(Lantern_ArchiveEntry &entry, Lantern_Archive &archive, lantern_enum_callback, void *arg) const;
	
	// Alternative callback style for findFirstMatch.
	// T callback's return value is the opposite of what would be used for Lantern_EnumerateArchive.
	typedef bool (*simple_callback)(const void *data, unsigned length, void *arg);
	bool findFirstMatch(Lantern_ArchiveEntry &entry, Lantern_Archive &archive, simple_callback, void *arg) const;
	
};

extern "C" void *Lantern_CreateArchiveServer();
extern "C" void Lantern_DestroyArchiveServer(void *server);
extern "C" void Lantern_AppendToArchiveServer(void *server, const void *data, unsigned long size);

} // namespace Lantern
