#pragma once

#include "server.hpp"
#include "archives.hpp"

#include <string>
#include <utility>
#include <vector>

struct Turbo_Value;

namespace Lantern {

class Item{
    
    typedef char i_fxstr[16];
    typedef std::pair<i_fxstr, std::string> i_script;
    typedef std::pair<const char *, unsigned> i_str;
    
    std::vector<i_script> m_scripts;
    bool m_can_carry;
    i_fxstr m_uname, m_name, m_image;
    std::string m_description;
    
    Item(const i_str &name, const i_str &uname,
        const i_str &descrription, const i_str &image, bool can_carry);
public:
    
    static Item *Load(const struct Turbo_Value &value);
    
    ~Item(){ }
    
    inline const std::string &description() const { return m_description; }
    inline const char *name() const { return m_name; }
    inline const char *uname() const { return m_uname; }
    inline bool canCarry() const { return m_can_carry; }
    
    inline const char *image() const { return m_image; }
    const std::string *script(const char *other) const;
    const std::string *script(const char *other, unsigned len) const;
    const std::string *script(const std::string &other) const;
    
    std::string toString() const;
};

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
 