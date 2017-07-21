#ifndef STATIC_LINK
#define IMPLEMENT_API
#endif

#if defined(HX_WINDOWS) || defined(HX_MACOS) || defined(HX_LINUX)
#define NEKO_COMPATIBLE
#endif

#include <hx/CFFI.h>
#include "Utils.h"

using namespace native_extension;

static value 
native_bytearray() 
{
	/*
	 * Assume the png variable has png image data
	 */
	unsigned char png[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	int png_size = sizeof(png);

	/*
	 * Allocate a buffer
	 */
	buffer buf = alloc_buffer_len(png_size);
	unsigned char *ptr = (unsigned char *)buffer_data(buf);

	/*
	 * Fill in the buf
	 */
	for (int i = 0; i < png_size; i++)
		ptr[i] = png[i];

	/*
	 * Get the bytes value
	 */
	value bytes;
	bytes = buffer_val(buf);

	/*
	 * Create a return object
	 */
	value obj = alloc_empty_object();
	alloc_field(obj, val_id("b"), bytes);
	alloc_field(obj, val_id("length"), alloc_int(png_size));

	return obj;
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

