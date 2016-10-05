#pragma once
#include <map>
#include <utility>

namespace Lantern {

template<typename Key, typename Data>
class ResourceServer {
protected:
	typedef std::map<Key, Data> res_map;
protected:
	virtual Data doLoad(const Key &key) = 0;
	virtual void finalize(typename res_map::iterator value) const {(void)value;}
	virtual void finalize(typename res_map::iterator first, const typename res_map::iterator &last) const {
		while(first != last)
			finalize(first++);
	}
public:

	virtual ~ResourceServer(){ clear(); }

	Data &load(const Key &key){
		typename res_map::iterator i = m_values.find(key);
		if(i != m_values.end())
			return i->second;
		m_values[key] = doLoad(key);
		return m_values[key];
	}
	
	void clear(){
		finalize(m_values.begin(), m_values.end());
		m_values.clear();
	}
	
private:
	res_map m_values;
};

} // namespace Lantern
