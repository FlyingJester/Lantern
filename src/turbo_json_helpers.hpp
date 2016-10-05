#pragma once

#include "TurboJSON/parse.h"

#include <map>
#include <string>
#include <vector>

class TurboJSON_Adapter {
public:
	virtual Turbo_Type getType() const = 0;
	virtual void accept(const struct Turbo_Value &that) = 0;
};

template<typename T>
class TurboJSON_NumberAdapter : public TurboJSON_Adapter {
	T &value;
public:
	TurboJSON_NumberAdapter(T &v) : value(v) {}
	
	virtual Turbo_Type getType() const { return TJ_Number; }
	virtual void accept(const struct Turbo_Value &that){
		value = reinterpret_cast<T>(that.value.number);
	}
};

class TurboJSON_StringAdapter : public TurboJSON_Adapter {
	std::string &value;
public:
	TurboJSON_StringAdapter(std::string &v) : value(v) {}
	
	virtual Turbo_Type getType() const { return TJ_String; }
	virtual void accept(const struct Turbo_Value &that);
};

class TurboJSON_BooleanAdapter : public TurboJSON_Adapter {
	bool &value;
public:
	TurboJSON_BooleanAdapter(bool &v) : value(v) {}
	
	virtual Turbo_Type getType() const { return TJ_Boolean; }
	virtual void accept(const struct Turbo_Value &that);
};

typedef std::map<std::string, TurboJSON_Adapter *> turbo_json_map_t;
bool TurbJSON_SearchObject(const struct Turbo_Value &value, const turbo_json_map_t &values);
