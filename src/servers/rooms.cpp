#include "rooms.hpp"
#include "src/turbo_json_helpers.hpp"
#include "bufferfile/bufferfile.h"

namespace Lantern {

Room::Room(TextureServer &texture_server, struct Turbo_Value &value){
    
    std::string uname, music, image; 
    
    TurboJSON_StringAdapter uname_adpt(uname),
        music_adpt(music),
        image_adpt(image);
        
	const struct Turbo_Value *items;
    
    TurboJSON_ArrayAdapter items_adpt(items);

#define NUM_ADAPTERS 4
    
#define PAIR_TYPES const char*, TurboJSON_Adapter *
#define MAKE_PAIR std::make_pair<PAIR_TYPES>
#define MAKE_PAIR2(X) MAKE_PAIR( (const char *) #X,  & X ## _adpt)

    std::pair<PAIR_TYPES> adapters[NUM_ADAPTERS] = {
        MAKE_PAIR2(uname),
        MAKE_PAIR2(music),
        MAKE_PAIR2(image),
        MAKE_PAIR2(items)
    };

#undef MAKE_PAIR2
#undef MAKE_PAIR
#undef PAIR_TYPES
    
    TurbJSON_SearchObject(value, adapters, adapters + NUM_ADAPTERS);
#undef NUM_ADAPTERS
    
    if(!image.empty())
        m_image = texture_server.load(image);
    
    
    m_num_items = 0;
}

Room *RoomServer::load(const std::string &name){
    if(Room *const found = find(name)){
        return found;
    }
    
    Room *room = NULL;
    
    if(Room *const found = findFirstUnused()){
        room = found;
        for(unsigned i = 0; i < LANTERN_NUM_CACHED_ROOMS; i++)
            if(found == m_rooms + i)
                m_in_use[i] = true;
    }
    else {
        const unsigned oldest = findOldest();
        room = m_rooms + oldest;
        room->~Room();
    }
    
    struct Lantern_ArchiveEntry entry;
    struct Lantern_Archive archive;
	struct Turbo_Value value;

    int i;
    const char *const src = (const char *)BufferFile((std::string("items/") + name).c_str(), &i),
        *const end = src + i;
    
    if(i == 0 || !src)
        return NULL;
    
    TurboJSON(&value, src, end);
    new (room) Room(m_texture_server, value);
    TurboFree(&value);
    
    FreeBufferFile((void*)src, i);

    return room;
}

} // namespace Lantern
