#ifndef __UTIL__ATTRIBUTE_H__
#define __UTIL__ATTRIBUTE_H__

#define ATTRIBUTE(...) __attribute__ (( __VA_ARGS__ ))
#define NOINIT __attribute__ (( section(".noinit")))

#endif
