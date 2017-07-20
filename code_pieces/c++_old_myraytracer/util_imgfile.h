/*************************************************************************
 *
 *	jesssoft	2004/10/8
 *
 *************************************************************************/

#ifndef __UTIL_IMGFILE_H__
#define __UTIL_IMGFILE_H__

/*
 *	Defines structures with image files
 */

typedef struct tagfile {
     short int     imageWidth;
     short int     imageHeight;
     unsigned char imageTypeCode;
     unsigned char bitCount;
} TGAFILE, tgafile_t;

typedef struct pcxheader {
	short int	width;
	short int	height;
	unsigned char	manufacturer;
	unsigned char	version;
	unsigned char	encoding;
	unsigned char	xMin;
	unsigned char	yMin;
	unsigned char	xMax;
	unsigned char	yMax;
	unsigned char	*palette;	
	/* If not NULL, DON'T DELETE when don't need to use it any more. 
	   It will be deleted with image data(image data pointer) */
} PCXHEADER, pcxheader_t;

/*
 *	Declares functions
 */

#ifndef WINDOWS

#define BYTE 	unsigned char
#define DWORD  	unsigned int
#define LONG  	int
#define UINT  	unsigned int
#define WORD  	unsigned short int

#define LPSTR  	char *

#define BOOL  	int
#define FALSE  	0
#define TRUE  	1
#define BI_RGB 	0x00000000

// MS-Windows bitmap definition:

#pragma pack(push, 2)
typedef struct tagBITMAPFILEHEADER {
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
} BITMAPFILEHEADER;
#pragma pack(pop)

#pragma pack(push, 4)
typedef struct tagBITMAPINFOHEADER {
	DWORD biSize;
	LONG biWidth;
	LONG biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	LONG biXPelsPerMeter;
	LONG biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[1];
} BITMAPINFO;
#pragma pack(pop)

#define LPBITMAPFILEHEADER  BITMAPFILEHEADER*
#define LPBITMAPINFOHEADER  BITMAPINFOHEADER*
#define LPBITMAPINFO   BITMAPINFO*

#endif	// WINDOWS

/* bmp file */
unsigned char	*imgfile_load_bmpfile(const char *filename,
		    BITMAPINFOHEADER* header);
size_t		imgfile_write_bmpfile(const char *filename, int width,
		    int height, const unsigned char *bits);
/* targa file */
unsigned char	*imgfile_load_tgafile(const char *filename, TGAFILE *header);
size_t		imgfile_write_tgafile(const char *filename, short int width,
		    short int height, const unsigned char* bits);
/* pcx file */
unsigned char	*imgfile_load_pcxfile(const char *filename, PCXHEADER *header);

#endif

