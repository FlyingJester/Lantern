#pragma once

#include <string>

struct Turbo_Value;

namespace DarkLight { namespace CL { class Parser; class Lexer; } }

namespace Lantern {

class Item {
	DarkLight::CL::Lexer *m_on_get_script, *m_on_use_script;
	std::string m_name, m_description;
	unsigned m_id, m_damage_value;

#ifndef NDEBUG
	bool init_canary;
#endif

	void init(const char *name_str, unsigned name_str_len,
		const char *description_str, unsigned description_str_len,
		const char *get_script, unsigned get_script_len,
		const char *use_script, unsigned use_script_len);
	
public:
	Item() : m_on_get_script(NULL), m_on_use_script(NULL), m_id(0u), m_damage_value(0u) {}

	Item(const char *name_str, unsigned name_str_len,
		const char *description_str, unsigned description_str_len,
		const char *get_script, unsigned get_script_len,
		const char *use_script, unsigned use_script_len);
		
	Item(const char *name_str, const char *description_str,
		const char *get_script, const char *use_script);
		
	explicit Item(const std::string &name_str, const std::string &description_str,
		const std::string &get_script, const std::string &use_script);
	
	~Item();
	
	bool runUse(DarkLight::CL::Parser &parser) const;
	bool runGet(DarkLight::CL::Parser &parser) const;
	const std::string &name() const { return m_name; }
	const std::string &description() const { return m_description; }
	
	const char *getScriptError() const;
	const char *useScriptError() const;
	
	void move(Item &from_other);
	
};

Item *LoadItem(const struct Turbo_Value &value);
Item *LoadItem(const std::string &json_src);

} // namespace Lantern
