#include "archives.hpp"

#include <utility>

namespace Lantern {

struct match_callback_arg {
	
	match_callback_arg(ArchiveServer::simple_callback cb, void *arg, struct Lantern_ArchiveEntry &entry)
	  : is_simple(true)
	  , found(false)
	  , cb_arg(arg)
	  , match(entry){
		callback.simple_cb = cb;
	}
	
	match_callback_arg(lantern_enum_callback cb, void *arg, struct Lantern_ArchiveEntry &entry)
	  : is_simple(false)
	  , found(false)
	  , cb_arg(arg)
	  , match(entry){
		callback.cpio_cb = cb;
	}
	
	const bool is_simple;
	bool found;
	union {
		ArchiveServer::simple_callback simple_cb;
		lantern_enum_callback cpio_cb;
	} callback;
	void *const cb_arg;
	struct Lantern_ArchiveEntry &match;
};

bool ArchiveServer::FindFirstMatchCallback(void *LANTERN_RESTRICT arg,
	const struct Lantern_Archive *LANTERN_RESTRICT archive,
	const struct Lantern_ArchiveEntry *LANTERN_RESTRICT file){
	
	match_callback_arg *const cb_arg = static_cast<match_callback_arg*>(arg);
	
	if(cb_arg->is_simple){
		if(cb_arg->callback.simple_cb(static_cast<const char *>(archive->data) + file->start, file->length, cb_arg->cb_arg)){
			cb_arg->match = *file;
			cb_arg->found = true;
			return false;
		}
	}
	else{
		if(cb_arg->callback.cpio_cb(cb_arg->cb_arg, archive, file)){
			cb_arg->match = *file;
			cb_arg->found = true;
			return false;
		}
	}
	return true;
}

// The file name should be filled in on the entry when this is called. Returns false if no matching path is found.
bool ArchiveServer::findFile(Lantern_ArchiveEntry &in_out_entry, Lantern_Archive &archive) const {
	
	for(std::vector<Lantern_Archive>::const_iterator iter = m_archives.begin(); iter != m_archives.end(); iter++){
		const Lantern_Archive &arch = *iter;
		Lantern_ArchiveSearch(&arch, &in_out_entry, 1);
		
		if(in_out_entry.start != LANTERN_ARCHIVE_INVALID_SIZE &&
			in_out_entry.length != LANTERN_ARCHIVE_INVALID_SIZE){
			archive = arch;
			return true;
		}
	}
	
	return false;
}

// Template to implement the findFirstMatch methods.
template<typename CallbackType, typename MatchCallback, typename Iter>
static bool find_first_match(Iter start, Iter end, MatchCallback match_callback, Lantern_ArchiveEntry &entry, Lantern_Archive &archive, CallbackType cb, void *arg){
	match_callback_arg match_arg(cb, arg, entry);
	
	for(Iter i = start; i != end; i++){
		const Lantern_Archive &iter = *i;
		Lantern_ArchiveEnumerate(&iter, match_callback, &match_arg);
		
		if(match_arg.found){
			archive = iter;
			return true;
		}
	}
	
	return false;
}

// Fills out the entry for the first file which the callback returns true on. Returns false if no files match.
bool ArchiveServer::findFirstMatch(Lantern_ArchiveEntry &entry, Lantern_Archive &archive, lantern_enum_callback cb, void *arg) const {
	return find_first_match(m_archives.begin(), m_archives.end(), FindFirstMatchCallback, entry, archive, cb, arg);
}

bool ArchiveServer::findFirstMatch(Lantern_ArchiveEntry &entry, Lantern_Archive &archive, simple_callback cb, void *arg) const {
	return find_first_match(m_archives.begin(), m_archives.end(), FindFirstMatchCallback, entry, archive, cb, arg);
}

ArchiveServer *Lantern_CreateArchiveServer(){
	return new ArchiveServer();
}

void Lantern_DestroyArchiveServer(ArchiveServer *server){
	delete server;
}

void Lantern_AppendToArchiveServer(ArchiveServer *server, const void *data, unsigned long size){
	Lantern_Archive archive;
	archive.data = data;
	archive.size = size;
	server->append(archive);
}

} // namespace Lantern
