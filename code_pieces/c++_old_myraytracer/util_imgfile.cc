//
// 2004.10.8 jesssoft
//

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "util_imgfile.h"

#define BITMAP_ID 0x4D42		// the universal bitmap ID

const int BITMAP_PAL_SIZE = 768;

/*
 *	static function declarations
 */
static unsigned char	*LoadBitmapFile(const char *filename,
			    BITMAPINFOHEADER *bitmapInfoHeader);
static size_t		WriteBitmapFile(const char *filename, int width,
			    int height, const unsigned char *imageData);
static unsigned char 	*LoadTGAFile(const char *filename, TGAFILE *tgaFile);
static size_t		WriteTGAFile(const char *filename, short int width,
			    short int height, const unsigned char *imageData);
static unsigned char	*LoadPCXFile(const char *filename,
			    PCXHEADER *pcxHeader);

/*
 *	wrapping function definitions
 */
unsigned char *
imgfile_load_bmpfile(const char *filename, BITMAPINFOHEADER *header)
{
	return LoadBitmapFile(filename, header);
}

size_t 
imgfile_write_bmpfile(const char *filename, int width, int height,
    const unsigned char *bits)
{
	return WriteBitmapFile(filename, width, height, bits);
}

unsigned char *
imgfile_load_tgafile(const char *filename, TGAFILE *header)
{
	return LoadTGAFile(filename, header);
}

size_t 
imgfile_write_tgafile(const char *filename, short int width,
    short int height, const unsigned char *bits) 
{
	return WriteTGAFile(filename, width, height, bits);
}

unsigned char *
imgfile_load_pcxfile(const char *filename, PCXHEADER *header)
{
	unsigned char *data = LoadPCXFile(filename, header);
	if (data) {
		//
		// Convert 256-color to RGB-color.
		//
		unsigned char *buf;
		unsigned char *pal;
		int x, y;

		assert(header->palette);

		int buf_size = header->width * header->height * 3;
		buf = (unsigned char *)malloc(buf_size + BITMAP_PAL_SIZE);

		assert(buf);		

		for (y = 0; y < header->height; y++)
			for (x = 0; x < header->width; x++) {
				int pos = y*header->width + x;
				int index = 3 * pos;
				unsigned char *pal = header->palette;

				buf[index + 0] = pal[(int)data[pos]*3 + 0];
				buf[index + 1] = pal[(int)data[pos]*3 + 1];
				buf[index + 2] = pal[(int)data[pos]*3 + 2];
			}

		// Copy palette data to a new allocated memory block
		pal = header->palette;
		header->palette = buf + buf_size;
		memcpy(header->palette, pal, BITMAP_PAL_SIZE);

		// Replace with rgb-color
		free(data);
		data = buf;
	}

	return data;
}

// LoadBitmapFile
// desc: Returns a pointer to the bitmap image of the bitmap specified
// by filename. Also returns the bitmap header information.
// No support for 8-bit bitmaps.
unsigned char *
LoadBitmapFile(const char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE 			*filePtr;	  // the file pointer
	BITMAPFILEHEADER	bitmapFileHeader; // bitmap file header
	unsigned char		*bitmapImage; 	  // bitmap image data
	int			imageIdx = 0;	  // image index counter
	unsigned char		tempRGB;	  // swap variable

	// open filename in "read binary" mode
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	
	// verify that this is a bitmap by checking for the universal 
	// bitmap id
	if (bitmapFileHeader.bfType != BITMAP_ID) {
		fclose(filePtr);
		return NULL;
	}

	// read the bitmap information header
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	if (24 != bitmapInfoHeader->biBitCount) {
		fclose(filePtr);
		return NULL;
	}

	// move file pointer to beginning of bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// allocate enough memory for the bitmap image data
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// verify memory allocation
	if (!bitmapImage) {
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// read in the bitmap image data
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

	// make sure bitmap image data was read
	if (bitmapImage == NULL) {
		fclose(filePtr);
		return NULL;
	}

	// swap the R and B values to get RGB since the bitmap color 
	// format is in BGR
	int sizeImage = (int)bitmapInfoHeader->biSizeImage;
	for (imageIdx = 0; imageIdx < sizeImage; imageIdx+=3) {
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// close the file and return the bitmap image data
	fclose(filePtr);
	return bitmapImage;
}

// WriteBitmapFile()
// desc: takes image data and saves it into a 24-bit RGB .BMP file
// with width X height dimensions
size_t 
WriteBitmapFile(const char *filename, int width, int height,
    const unsigned char *imageData)
{
	FILE			*filePtr;	  // file pointer
	BITMAPFILEHEADER	bitmapFileHeader; // bitmap file header
	BITMAPINFOHEADER	bitmapInfoHeader; // bitmap info header
	int			imageIdx; // used for swapping RGB->BGR
	/*unsigned char		tempRGB;*/ // used for swapping
	size_t			writesize = 0;
	unsigned char		*buf;
	bool			flipped = false;

	if (!filename || !imageData)
		return 0;

	if (height < 0) {
		height = -height;
		flipped = true;
	}

	// open file for writing binary mode
	filePtr = fopen(filename, "wb");
	if (!filePtr)
		return 0;

	buf = (unsigned char*)malloc(width*height*4);
	if (!buf) {
		fclose(filePtr);		
		return 0;
	}

	// define the bitmap file header
	bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER);
	bitmapFileHeader.bfType = BITMAP_ID;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) 
	    + sizeof(BITMAPINFOHEADER);
	
	// define the bitmap information header
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 32; 	// 32-bit
	bitmapInfoHeader.biCompression = BI_RGB;// no compression
	bitmapInfoHeader.biSizeImage = width * abs(height) * 4;	
					// width * height * (BGRA color bytes)
	bitmapInfoHeader.biXPelsPerMeter = 0;
	bitmapInfoHeader.biYPelsPerMeter = 0;
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;
	bitmapInfoHeader.biWidth = width;	// bitmap width
	bitmapInfoHeader.biHeight = height;	// bitmap height

	// BGRA
	int sizeImage = (int)bitmapInfoHeader.biSizeImage;
	if (flipped) {
		int dstImageIdx = 0;
		for (imageIdx = sizeImage - 4; imageIdx >= 0; imageIdx -= 4) {
			buf[dstImageIdx + 0] = imageData[imageIdx + 0];
			buf[dstImageIdx + 1] = imageData[imageIdx + 1];
			buf[dstImageIdx + 2] = imageData[imageIdx + 2];
			buf[dstImageIdx + 3] = 0;
			dstImageIdx += 4;
		}
	} else {
		for (imageIdx = 0; imageIdx < sizeImage; imageIdx += 4) {
			buf[imageIdx + 0] = imageData[imageIdx + 0];
			buf[imageIdx + 1] = imageData[imageIdx + 1];
			buf[imageIdx + 2] = imageData[imageIdx + 2];
			buf[imageIdx + 3] = 0;
		}
	}

	// write the bitmap file header
	writesize += fwrite(&bitmapFileHeader, 1, sizeof(BITMAPFILEHEADER),
	    filePtr);	 

	// write the bitmap info header
	writesize += fwrite(&bitmapInfoHeader, 1, sizeof(BITMAPINFOHEADER),
	    filePtr);

	// write the image data
	writesize += fwrite(buf, 1, bitmapInfoHeader.biSizeImage, filePtr);

	// close our file
	fclose(filePtr);
	free(buf);

	return writesize;
}

// LoadTGAFile()
// desc: loads the TGA file "filename" into the tgaFile data structure
unsigned char *
LoadTGAFile(const char *filename, TGAFILE *tgaFile)
{
	FILE 		*filePtr;
	unsigned char	ucharBad;	// garbage unsigned char data
	short int	sintBad;	// garbage short int data
	long		imageSize;	// size of the TGA image
	int		colorMode;	// 4 for RGBA or 3 for RGB
	long		imageIdx;	// counter variable
	unsigned char 	colorSwap;	// swap variable
	unsigned char 	*buf;

	// open the TGA file
	filePtr = fopen(filename, "rb");
	if (!filePtr)
		return NULL;

	// read first two bytes of garbage
	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
	
	// read in the image type
	fread(&tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr);

	// for our purposes, the image type should be either a 2 (color) or
	// a 3 (greyscale)
	if ((tgaFile->imageTypeCode != 2) && (tgaFile->imageTypeCode != 3)) {
		fclose(filePtr);
		return NULL;
	}

	// read 13 bytes of garbage data
	fread(&sintBad, sizeof(short int), 1, filePtr);
	fread(&sintBad, sizeof(short int), 1, filePtr);
	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
	fread(&sintBad, sizeof(short int), 1, filePtr);
	fread(&sintBad, sizeof(short int), 1, filePtr);

	// read image dimensions
	fread(&tgaFile->imageWidth, sizeof(short int), 1, filePtr);
	fread(&tgaFile->imageHeight, sizeof(short int), 1, filePtr);

	// read image bit depth
	fread(&tgaFile->bitCount, sizeof(unsigned char), 1, filePtr);

	// read 1 byte of garbage data
	fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

	// colorMode -> 3 = BGR, 4 = BGRA
	colorMode = tgaFile->bitCount / 8;
	imageSize = tgaFile->imageWidth * tgaFile->imageHeight * colorMode;

	// allocate memory for image data
	//
	// tgaFile->imageData 
	//  = (unsigned char*)malloc(sizeof(unsigned char)*imageSize);
	//
	buf = (unsigned char*)malloc(sizeof(unsigned char)*imageSize);
	if (!buf)
		return NULL;

	// read in image data
	/* fread(tgaFile->imageData, sizeof(unsigned char), imageSize
	   , filePtr);*/
	fread(buf, sizeof(unsigned char), imageSize, filePtr);
	
	// change BGR to RGB so OpenGL can read the image data
	for (imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode) {
		colorSwap = buf[imageIdx];
		buf[imageIdx] = buf[imageIdx + 2];
		buf[imageIdx + 2] = colorSwap;
	}

	// close the file
	fclose(filePtr);

	return buf;
}

// saves an array of pixels as a TGA image
size_t 
WriteTGAFile(const char *filename, short int width, short int height,
    const unsigned char *imageData) 
{
	unsigned char	byteSkip; // used to fill in the data fields 
				  // that we don't care about
	short int	shortSkip;
	unsigned char	imageType; // type of image we're writing to file
	int		colorMode;
	/*unsigned char	colorSwap;*/
	int		imageIdx;
	unsigned char	bitDepth;
	long		imageSize;
	unsigned char	*buf;
	size_t		writesize = 0;
	FILE		*filePtr;
	bool		flipped = false;

	// create file for writing binary mode
	filePtr = fopen(filename, "wb");
	if (!filePtr) {
		fclose(filePtr);
		return 0;
	}

	if (height < 0) {
		height = -height;
		flipped = true;
	}

//	imageType = 2;		// RGB, uncompressed
//	bitDepth = 24;		// 24-bitdepth
//	colorMode = 3;		// RGB color mode

	imageType = 2;		// RGB, uncompressed
	bitDepth = 32;		// 32-bitdepth
	colorMode = 4;		// RGBA color mode

	byteSkip = 0;
	shortSkip = 0;

	// write 2 bytes of blank data
	writesize += fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);
	writesize += fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);

	// write imageType
	writesize += fwrite(&imageType, sizeof(unsigned char), 1, filePtr);

	writesize += fwrite(&shortSkip, sizeof(short int), 1, filePtr);
	writesize += fwrite(&shortSkip, sizeof(short int), 1, filePtr);
	writesize += fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);
	writesize += fwrite(&shortSkip, sizeof(short int), 1, filePtr);
	writesize += fwrite(&shortSkip, sizeof(short int), 1, filePtr);

	// write image dimensions
	writesize += fwrite(&width, sizeof(short int), 1, filePtr);
	writesize += fwrite(&height, sizeof(short int), 1, filePtr);
	writesize += fwrite(&bitDepth, sizeof(unsigned char), 1, filePtr);

	// write 1 byte of blank data
	writesize += fwrite(&byteSkip, sizeof(unsigned char), 1, filePtr);

	// calculate the image size
	imageSize = width * height * colorMode;

	buf = (unsigned char*)malloc(imageSize);
	if (!buf) {
		fclose(filePtr);
		return 0;
	}

	// BGRA
	if (flipped) {
		int dstImageIdx = 0;
		for (imageIdx = imageSize - colorMode; imageIdx >= 0; 
		    imageIdx -= colorMode) {
			buf[dstImageIdx + 0] = imageData[imageIdx + 0];
			buf[dstImageIdx + 1] = imageData[imageIdx + 1];
			buf[dstImageIdx + 2] = imageData[imageIdx + 2];
			buf[dstImageIdx + 3] = 255;
			dstImageIdx += colorMode;
		}
	} else {
		for (imageIdx = 0; imageIdx < imageSize; 
		    imageIdx += colorMode) {
			buf[imageIdx + 0] = imageData[imageIdx + 0];
			buf[imageIdx + 1] = imageData[imageIdx + 1];
			buf[imageIdx + 2] = imageData[imageIdx + 2];
			buf[imageIdx + 3] = 255;
		}
	}

	// write the image data
	writesize += fwrite(buf, sizeof(unsigned char), imageSize, filePtr);

	// close the file
	fclose(filePtr);
	free(buf);

	return writesize;
}

unsigned char *
LoadPCXFile(const char *filename, PCXHEADER *pcxHeader)
{
	int idx = 0;	// counter index
	int c;		// used to retrieve a char from the file
	int i;		// counter index
	int numRepeat;      
	FILE *filePtr;	// file handle
	int width;	// pcx width
	int height;	// pcx height
	unsigned char *pixelData;	// pcx image data
	unsigned char *paletteData;	// pcx palette data

	// open PCX file
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// retrieve first character; should be equal to 10
	c = getc(filePtr);
	if (c != 10) {
		fclose(filePtr);
		return NULL;
	}

	// retrieve next character; should be equal to 5
	c = getc(filePtr);
	if (c != 5) {
		fclose(filePtr);
		return NULL;
	}

	// reposition file pointer to beginning of file
	rewind(filePtr);

	// read 4 characters of data to skip
	fgetc(filePtr);
	fgetc(filePtr);
	fgetc(filePtr);
	fgetc(filePtr);

	// retrieve leftmost x value of PCX
	pcxHeader->xMin = (unsigned char)fgetc(filePtr);       // loword
	pcxHeader->xMin |= fgetc(filePtr) << 8; // hiword

	// retrieve bottom-most y value of PCX
	pcxHeader->yMin = (unsigned char)fgetc(filePtr);       // loword
	pcxHeader->yMin |= fgetc(filePtr) << 8; // hiword

	// retrieve rightmost x value of PCX
	pcxHeader->xMax = (unsigned char)fgetc(filePtr);       // loword
	pcxHeader->xMax |= fgetc(filePtr) << 8; // hiword

	// retrieve topmost y value of PCX
	pcxHeader->yMax = (unsigned char)fgetc(filePtr);       // loword
	pcxHeader->yMax |= fgetc(filePtr) << 8; // hiword

	// calculate the width and height of the PCX
	width = pcxHeader->xMax - pcxHeader->xMin + 1;
	height = pcxHeader->yMax - pcxHeader->yMin + 1;

	pcxHeader->width = (short)width;
	pcxHeader->height = (short)height;

	// allocate memory for PCX image data
	pixelData = (unsigned char*)malloc(width*height + BITMAP_PAL_SIZE);

	// set file pointer to 128th byte of file, 
	// where the PCX image data starts
	fseek(filePtr, 128, SEEK_SET);

	// decode the pixel data and store
	while (idx < (width*height)) {
		c = getc(filePtr);
		if (c > 0xbf) {
			numRepeat = 0x3f & c;
			c = getc(filePtr);

			for (i = 0; i < numRepeat; i++)
				pixelData[idx++] = (unsigned char)c;
		} else
			pixelData[idx++] = (unsigned char)c;

		fflush(stdout);
	}

	// allocate memory for the PCX image palette
	/*paletteData = (unsigned char*)malloc(768);*/
	paletteData = pixelData + width*height;

	// palette is the last 769 bytes of the PCX file
	fseek(filePtr, -769, SEEK_END);

	// verify palette; first character should be 12
	c = getc(filePtr);
	if (c != 12) {
		free(pixelData);
		fclose(filePtr);
		return NULL;
	}

	// read and store all of palette
	for (i = 0; i < BITMAP_PAL_SIZE; i++) {
		c = getc(filePtr);
		paletteData[i] = (unsigned char)c;
	}

	// close file and store palette in header
	fclose(filePtr);
	pcxHeader->palette = paletteData;

	// return the pixel image data
	return pixelData;
}

