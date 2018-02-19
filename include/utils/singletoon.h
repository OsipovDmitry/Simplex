#ifndef SINGLETOON_H
#define SINGLETOON_H

#include <memory>

#define SINGLETON(Class, params...) \
	public: \
	static Class *instance() { \
		static std::unique_ptr<Class> p(new Class(params)); \
		return p.get(); \
	}


#endif
