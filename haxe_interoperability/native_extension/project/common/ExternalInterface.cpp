#ifndef STATIC_LINK
#define IMPLEMENT_API
#endif

#if defined(HX_WINDOWS) || defined(HX_MACOS) || defined(HX_LINUX)
#define NEKO_COMPATIBLE
#endif

#include <hx/CFFI.h>
#include <string.h>
#include "Utils.h"

using namespace native_extension;

static value 
native_bytearray() 
{
	const char *msg = "Hello Native!";

	return alloc_string_len(msg, strlen(msg));
}
DEFINE_PRIM(native_bytearray, 0);

extern "C" void 
native_extension_main() 
{
	val_int(0); // Fix Neko init
}
DEFINE_ENTRY_POINT (native_extension_main);

extern "C" int 
native_extension_register_prims() 
{ 
	return 0; 
}

