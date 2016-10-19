#include "items.hpp"

#include "TurboJSON/parse.h"
#include "TurboJSON/object.h"

#include "src/turbo_json_helpers.hpp"

#include "bufferfile/bufferfile.h"

#include <cstring>
#include <algorithm>

namespace Lantern {

Item::Item(const i_str &name, const i_str &uname,
    const i_str &descrription, const i_str &image, bool can_carry)
  : m_can_carry(can_carry)
  , m_description(descrription.first, descrription.second){

#define ISTR_SET(X) memcpy(m_ ## X, X.first, std::min<unsigned>(X.second, 16)); m_ ## X [15] = '\0'
    
    ISTR_SET(name);
    ISTR_SET(uname);
    ISTR_SET(image);
    
}

Item *Item::Load(const struct Turbo_Value &value){
	const struct Turbo_Value *scripts;
    TurboJSON_ObjectAdapter scripts_adapter(scripts);
    
    std::string name, uname, description, image; 
    
    TurboJSON_StringAdapter name_adpt(name),
        uname_adpt(uname),
        description_adpt(description),
        image_adpt(image);
    
    bool can_carry;
    
    TurboJSON_BooleanAdapter canCarry_adpt(can_carry);
    
#define NUM_ADAPTERS 6
    
#define PAIR_TYPES const char*, TurboJSON_Adapter *
#define MAKE_PAIR std::make_pair<PAIR_TYPES>
#define MAKE_PAIR2(X) MAKE_PAIR( (const char *) #X,  & X ## _adpt)

    std::pair<PAIR_TYPES> adapters[NUM_ADAPTERS] = {
        MAKE_PAIR((const char *)"onUse", &scripts_adapter),
        MAKE_PAIR2(name),
        MAKE_PAIR2(uname),
        MAKE_PAIR2(description),
        MAKE_PAIR2(image),
        MAKE_PAIR2(canCarry)
    };

#undef MAKE_PAIR2
#undef PAIR_TYPES
    
    TurbJSON_SearchObject(value, adapters, adapters + NUM_ADAPTERS);
    
    if(name.empty() || uname.empty() || description.empty() || image.empty())
        return NULL;
    
#define PAIR_TYPES const char *, unsigned
#define MAKE_ISTR(X) MAKE_PAIR(X.c_str(), X.length())
    Item *const item = new Item(MAKE_ISTR(name), MAKE_ISTR(uname),
        MAKE_ISTR(description), MAKE_ISTR(image), can_carry);
        
#undef MAKE_ISTR
#undef MAKE_PAIR
#undef PAIR_TYPES
    
    if(scripts == NULL)
        return item;
    
    item->m_scripts.reserve(scripts->length);
    for(unsigned n = 0; n < scripts->length; n++){
        
        if(scripts->value.object[n].value.type != TJ_String)
            continue;
        
        item->m_scripts.resize(item->m_scripts.size() + 1);
        const unsigned len = std::min<unsigned>(scripts->value.object[n].name_length, 16);
        memcpy(item->m_scripts.back().first, scripts->value.object[n].name, len);
        item->m_scripts.back().first[15] = 0;
        item->m_scripts.back().second.assign(scripts->value.object[n].value.value.string,
            scripts->value.object[n].value.length);
    }
    
    return item;
    
}
    
const std::string *Item::script(const char *other) const {
    return script(other, strlen(other));
}

const std::string *Item::script(const std::string &other) const {
    return script(other.c_str(), other.length());
}

const std::string *Item::script(const char *other, unsigned len) const {
    for(std::vector<i_script>::const_iterator i = m_scripts.begin();
        i != m_scripts.end(); i++){
        if(strncmp(other, i->first, std::min<unsigned>(len, 16)) == 0)
            return &(i->second);
    }
    return NULL;
}

/*
    std::vector<std::pair<i_fxstr, std::string> > m_scripts;
    bool m_can_carry;
    char m_uname[16], m_name[16], m_image[16];
    std::string m_description;
*/

std::string Item::toString() const{
    std::string str("{\n\t", 3);

#define I_PUSH_I_STR(ISTR)\
    for(unsigned I_Z_ = 0; I_Z_ < 16; I_Z_++)\
        if(ISTR[I_Z_]) str.push_back(ISTR[I_Z_]); else break

#define I_STR_PROPERTY(X)\
    str.reserve(str.length() + 32);\
    str.append("\"" #X "\":\"");\
    I_PUSH_I_STR(m_ ## X);\
    str.append("\",\n\t")
    
    I_STR_PROPERTY(uname);
    I_STR_PROPERTY(name);
    
    str.append("\"description\":\"");
    str.append(m_description);
    str.append("\",\n\t");
    
    I_STR_PROPERTY(image);
    
    str.append("\"canCarry\":");
    str.append(m_can_carry ? "true" : "false");
    str.append("\n\t\"onUse\":{\n");
    
    for(std::vector<i_script>::const_iterator i = m_scripts.begin();
        i != m_scripts.end(); i++){
        
        str.append(2, '\t');
        str.push_back('"');
        I_PUSH_I_STR(i->first);
        
        str.append("\":\"");
        
        str.append(i->second + "\",\n");
    }
    
    str.append("\t}\n\t");
    
    str.append("}\n");
    
    return str;
}

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

static Item *do_load(const char *src, const char *end ){
	struct Turbo_Value value;

	TurboJSON(&value, src, end);

	Item *const item = Item::Load(value);	

	TurboFree(&value);
	return item;
}

Item *ItemServer::doLoad(const std::string &name){
	
	struct Lantern_ArchiveEntry entry;
	struct Lantern_Archive archive;
    if (!m_archive_server.findFirstMatch(entry, archive, ItemMatchesName, (void*)(&name))) {
        int i;
        const char *const src = (const char *)BufferFile((std::string("items/") + name).c_str(), &i),
            *const end = src + i;
        
        if(i == 0 || !src)
            return NULL;
        
        Item *const item = do_load(src, end);
        FreeBufferFile((void*)src, i);
        
        return item;
    }
    else{
        const char *const src = static_cast<const char *>(archive.data) + entry.start,
            *const end = src + entry.length;
        return do_load(src, end);
    }
}

} // namespace Lantern
