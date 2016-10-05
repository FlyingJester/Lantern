#include "item.hpp"

#include <cstring>
#include <cassert>

#include "turbo_json_helpers.hpp"

#include "dlcl/dlcl_parser.hpp"
#include "dlcl/dlcl_lexer.hpp"

namespace Lantern {

static inline DarkLight::CL::Lexer *init_script(const char *script, unsigned script_len){
	assert(script != NULL || script_len == 0);
	
	DarkLight::CL::Lexer *lexer = NULL;
	
	if(script_len != 0){
		lexer = new DarkLight::CL::Lexer();
		lexer->lex(script, script_len);
		
		if(lexer->empty()){
			delete lexer;
			lexer = NULL;
		}
	}
	
	return lexer;
}

void Item::init(const char *name_str, unsigned name_str_len,
	const char *description_str, unsigned description_str_len,
	const char *get_script, unsigned get_script_len,
	const char *use_script, unsigned use_script_len){
	
	assert(init_canary == false);
	assert(init_canary = true);
	
	assert(name_str != NULL || name_str_len == 0);
	assert(description_str != NULL || description_str_len == 0);
	assert(get_script != NULL || get_script_len == 0);
	assert(use_script != NULL || use_script_len == 0);
	
	if(name_str_len)
		m_name.assign(name_str, name_str_len);
	if(description_str_len)
		m_name.assign(description_str, description_str_len);
	
	m_on_get_script = init_script(get_script, get_script_len);
	m_on_use_script = init_script(use_script, use_script_len);
}

Item::Item(const char *name_str, unsigned name_str_len,
	const char *description_str, unsigned description_str_len,
	const char *get_script, unsigned get_script_len,
	const char *use_script, unsigned use_script_len)
#ifndef NDEBUG
	: init_canary(false)
#endif
{
	
	assert(name_str != NULL || name_str_len == 0);
	assert(description_str != NULL || description_str_len == 0);
	assert(get_script != NULL || get_script_len == 0);
	assert(use_script != NULL || use_script_len == 0);
	
	init(name_str, name_str_len,
		description_str, description_str_len,
		get_script, get_script_len,
		use_script, use_script_len);
}
	
Item::Item(const char *name_str, const char *description_str,
	const char *get_script, const char *use_script)
#ifndef NDEBUG
	: init_canary(false)
#endif
{
	
	assert(name_str != NULL);
	assert(description_str != NULL);
	assert(get_script != NULL);
	assert(use_script != NULL);
	
	init(name_str, strlen(name_str),
		description_str, strlen(description_str),
		get_script, strlen(get_script),
		use_script, strlen(use_script));	
}
	
Item::Item(const std::string &name_str, const std::string &description_str,
	const std::string &get_script, const std::string &use_script)
#ifndef NDEBUG
	: init_canary(false)
#endif
{
	init(name_str.c_str(), name_str.length(),
		description_str.c_str(), description_str.length(),
		get_script.c_str(), get_script.length(),
		use_script.c_str(), use_script.length());
	
}

Item::~Item(){
	if(m_on_get_script)
		delete m_on_get_script;
	if(m_on_use_script)
		delete m_on_use_script;
}

bool Item::runUse(DarkLight::CL::Parser &parser) const {
	return m_on_use_script == NULL || parser.parse(*m_on_use_script);
}

bool Item::runGet(DarkLight::CL::Parser &parser) const {
	return m_on_get_script == NULL || parser.parse(*m_on_get_script);
}

const char *Item::getScriptError() const {
	return m_on_get_script ? m_on_get_script->getError() : "";
}

const char *Item::useScriptError() const {
	return m_on_use_script ? m_on_use_script->getError() : "";
}

void Item::move(Item &from_other){
	m_on_get_script = from_other.m_on_get_script;
	from_other.m_on_get_script = NULL;
	
	m_on_use_script = from_other.m_on_use_script;
	from_other.m_on_use_script = NULL;
	
	m_name = from_other.m_name;
	from_other.m_name.clear();

	m_description = from_other.m_description;
	from_other.m_description.clear();

	m_id = from_other.m_id;
	from_other.m_id = 0;

	m_damage_value = from_other.m_damage_value;
	from_other.m_damage_value = 0;
	
}

Item *LoadItem(const struct Turbo_Value &value){
	assert(value.type == TJ_Object);
	if(value.type != TJ_Object)
		return NULL;
	
	std::string on_use, on_get, image, name, description;
	TurboJSON_StringAdapter on_use_adapter(on_use),
		on_get_adapter(on_get),
		image_adapter(image),
		name_adapter(name),
		description_adapter(description);

	turbo_json_map_t adapters;
	
	adapters["onUse"] = &on_use_adapter;
	adapters["onGet"] = &on_get_adapter;
	adapters["image"] = &image_adapter;
	adapters["name"] = &name_adapter;
	adapters["description"] = &description_adapter;
	
	TurbJSON_SearchObject(value, adapters);
	
	return new Item(name, description, on_get, on_use);
}

Item *LoadItem(const std::string &json_src){
	struct Turbo_Value value;
	Turbo_Value(&value, json_src.c_str(), json_src.c_str() + json_src.length());
	Item *const item = LoadItem(value);
	TurboFree(&value);
	return item;
}

} // namespace Lantern
