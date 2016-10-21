#pragma once
#include "textures.hpp"
#include "archives.hpp"
#include "TurboJSON/parse.h"

#include <string>

namespace Lantern {

struct ItemHandle {
    char uname[16];
    unsigned x, y;
};

class Room {
    Image m_image;
    ItemHandle m_items[64];
    unsigned m_num_items;
public:
    Room(){}
    Room(TextureServer &texture_server, struct Turbo_Value &value);
    const Image& image() const { return m_image; }
    unsigned numItems() const { return m_num_items; }
    const ItemHandle *getItem(unsigned i) const {
        if(i < m_num_items)
            return m_items + i;
        else
            return NULL;
    }
};

#define LANTERN_NUM_CACHED_ROOMS 8

class RoomServer {
    
    const ArchiveServer &m_archive_server;
    TextureServer &m_texture_server;
    Room m_rooms[LANTERN_NUM_CACHED_ROOMS];
    bool m_in_use[LANTERN_NUM_CACHED_ROOMS];
    std::string m_names[LANTERN_NUM_CACHED_ROOMS];
    unsigned m_ages[LANTERN_NUM_CACHED_ROOMS];
    
    inline Room *find(const std::string &str){
        for(unsigned i = 0; i < LANTERN_NUM_CACHED_ROOMS; i++){
            if(m_in_use[i] && m_names[i] == str)
                return m_rooms + i;
        }
        return NULL;
    }
    
    inline Room *findFirstUnused() {
        for(unsigned i = 0; i < LANTERN_NUM_CACHED_ROOMS; i++){
            if(!m_in_use[i])
                return m_rooms + i;
        }
        return NULL;
    }
    
    inline unsigned findOldest() const {
        unsigned age = 0xFFFFFFFF, oldest = 0;
        for(unsigned i = 0; i < LANTERN_NUM_CACHED_ROOMS; i++){
            if(m_ages[i] < age){
                oldest = i;
                age = m_ages[i];
            }
        }
        
        return oldest;
    }
    
public:
    RoomServer(const ArchiveServer & arch, TextureServer &tex)
      : m_archive_server(arch)
      , m_texture_server(tex){
        for(unsigned i = 0; i < LANTERN_NUM_CACHED_ROOMS; i++){
            m_in_use[i] = false;
            m_rooms[i].~Room();
            m_ages[i] = 0xFFFFFFFF - i;
        }
    }
    
	~RoomServer(){ clear(); }
    
	Room *load(const std::string &name);
	
	void clear() {
        for(unsigned i = 0; i < LANTERN_NUM_CACHED_ROOMS; i++){
            if(m_in_use[i])
                m_rooms[i].~Room();
            m_in_use[i] = false;
        }
    }
	
};

} // namespace Lantern
