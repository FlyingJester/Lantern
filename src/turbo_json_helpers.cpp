#include "turbo_json_helpers.hpp"
#include "TurboJSON/object.h"

void TurboJSON_StringAdapter::accept(const struct Turbo_Value &that){
	value.assign(that.value.string, that.length);
}

void TurboJSON_BooleanAdapter::accept(const struct Turbo_Value &that){
	value = that.value.boolean;
}

void TurboJSON_ObjectAdapter::accept(const struct Turbo_Value &that){
    value = &that;
}

void TurboJSON_ArrayAdapter::accept(const struct Turbo_Value &that){
    value = &that;
}

bool StringCompare(const std::string &i, const char *str, unsigned n){
    return i.length() == n && i == str;
}

bool StringCompare(const char *str1, const char *str2, unsigned n){
    for(unsigned i = 0; i < n; i++){
        if(str1[i] != str2[i])
            return false;
    }
    return str1[n] == '\0';
}
