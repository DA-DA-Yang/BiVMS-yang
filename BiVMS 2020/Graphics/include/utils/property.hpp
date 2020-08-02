#pragma once

#ifndef _PROPERTIES_H
#define _PROPERTIES_H

#define PROPERTY(_T, name) __declspec(property(put = _set_##name, get = _get_##name)) _T name; \
	typedef _T _type_##name

#define READONLY_PROPERTY(_T, name) __declspec(property(get = _get_##name)) _T name;\
	typedef _T _type_##name

#define WRITEONLY_PROPERTY(_T, name) __declspec(property(put = _set_##name)) _T name;\
	typedef _T _type_##name

#define __get__(name) _type_##name _get_##name()
#define __set__(name) void _set_##name(const _type_##name name)

#endif /* _PROPERTIES_H */ 