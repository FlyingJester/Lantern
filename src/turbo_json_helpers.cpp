#include "turbo_json_helpers.hpp"
#include "TurboJSON/object.h"

void TurboJSON_StringAdapter::accept(const struct Turbo_Value &that){
	value.assign(that.value.string, that.length);
}

void TurboJSON_BooleanAdapter::accept(const struct Turbo_Value &that){
	value = that.value.boolean;
}

bool TurbJSON_SearchObject(const struct Turbo_Value &value, const turbo_json_map_t &values_in){
	if(value.type != TJ_Object)
		return false;
	
	turbo_json_map_t key_values = values_in;
	
	for(unsigned n = 0; n < value.length; n++){
		turbo_json_map_t::iterator iter = key_values.find(std::string(value.value.object[n].name, value.value.object[n].name_length));
		if(iter == key_values.end())
			continue;
		
		if(iter->second->getType() != value.value.object[n].value.type)
			continue;
		
		iter->second->accept(value.value.object[n].value);
		key_values.erase(iter);
	}
	
	return true;
}
