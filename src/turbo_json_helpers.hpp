#pragma once

#include "TurboJSON/parse.h"
#include "TurboJSON/object.h"
#include "TurboJSON/value.h"

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

class TurboJSON_ValueAdapter : public TurboJSON_Adapter {
	const struct Turbo_Value *&value;
public:
	TurboJSON_ValueAdapter(const struct Turbo_Value *&v) : value(v) {}
	
	virtual void accept(const struct Turbo_Value &that);
};

class TurboJSON_ObjectAdapter : public TurboJSON_ValueAdapter {
public:
	TurboJSON_ObjectAdapter(const struct Turbo_Value *&v) : TurboJSON_ValueAdapter(v) {}
	virtual Turbo_Type getType() const;
};

class TurboJSON_ArrayAdapter : public TurboJSON_ValueAdapter {
public:
	TurboJSON_ArrayAdapter(const struct Turbo_Value *&v) : TurboJSON_ValueAdapter(v) {}
	virtual Turbo_Type getType() const;
};

bool StringCompare(const std::string &i, const char *, unsigned n);
bool StringCompare(const char *, const char *, unsigned n);

template<typename IterI>
inline bool TurbJSON_SearchObject(const struct Turbo_Value &value,
    IterI adapterBegin, IterI adapterEnd){
	if(value.type != TJ_Object)
		return false;
	
	for(unsigned n = 0; n < value.length; n++){
        
        IterI iter = adapterEnd;
        for(IterI i = adapterBegin; i != adapterEnd; i++){
            if(StringCompare(i->first, value.value.object[n].name, value.value.object[n].name_length)){
                iter = i;
                break; 
            }
        }
        
		if(iter == adapterEnd)
			continue;
		
		if(iter->second->getType() != value.value.object[n].value.type)
			continue;
		
		iter->second->accept(value.value.object[n].value);
        
        if(iter == adapterBegin)
            adapterBegin++;
        
	}
	
	return true;
}
