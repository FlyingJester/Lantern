#include "items.hpp"

#include "src/item.hpp"

#include "TurboJSON/parse.h"
#include "TurboJSON/object.h"

#include <cstring>

namespace Lantern {

void ItemServer::finalize(std::map<std::string, Item*>::iterator i) { delete i->second; }

bool ItemServer::ItemMatchesName(const void *data, unsigned length, void *arg){
	const std::string &name = *static_cast<const std::string*>(arg);
	
	// Unlikely, but still cheap to check for.
	if(length < name.length() + 12)
		return false;
	
	// Quick check to be sure this could even remotely be a JSON object:
	const char *first = static_cast<const char *>(data),
		*last = first + length;
	
	while(first != last){
		if(*first == '{')
			break;
		switch(*first){
			case ' ': case '\n': case '\r': case '\t': break;
			default: return false;
		}
		first++;
	}
	
	while(--last != first){
		if(*last == '}')
			break;
		switch(*first){
			case ' ': case '\n': case '\r': case '\t': case '\0': case ';': break;
			default: return false;
		}
	}
	
	struct Turbo_Value value;
	TurboJSON(&value, first, last+1);
	
	if(value.type != TJ_Object)
		return false;

#define END(VALUE) do { TurboFree(&value); return VALUE; } while(0)

	for(unsigned i = 0; i < value.length; i++){
		struct Turbo_Property &property = value.value.object[i];
		bool success = property.name_length == name.length() &&
			memcmp(property.name, property.name, name.length()) == 0;
		
		if(success)
			END(true);
	}
	
	END(false);
	
#undef END
}

Item *ItemServer::doLoad(const std::string &name){
	
	struct Lantern_ArchiveEntry entry;
	struct Lantern_Archive archive;
	if(!m_archive_server.findFirstMatch(entry, archive, ItemMatchesName, (void*)(&name)))
		return NULL;
	
	struct Turbo_Value value;
	const char *const src = static_cast<const char *>(archive.data) + entry.start, *const end = src + entry.length;
	TurboJSON(&value, src, end);

	Item *const item = LoadItem(value);	

	TurboFree(&value);
	return item;
}

} // namespace Lantern
