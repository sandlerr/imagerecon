// labviewdll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include <string.h>
#include <ctype.h>
#include "labviewdll.h"
#include "tiffio.h"
#include <malloc.h>

FILE *errstream;
FILE *outstream;

/* Add two integers */
_declspec (dllexport) long add_num(long a, long b){
	return((long) (a + b));
}

/* This function finds the average of an array of single precision numbers */
_declspec (dllexport)  long  avg_num(float  *a, long size, float  *avg)
{
	float sum = 0;

	if (a != NULL)
	{
		for (int i = 0; i < size; i++)
			sum = sum + a[i];
	}
	else
		return (1);
	*avg = sum / size;
	return (0);
}

// Counts the number of integer numbers appearing in a string. */
// Note that this function does not check for sign, decimal, or exponent
_declspec (dllexport) unsigned int numIntegers(char *inputString) {

	int      lastDigit = 0;
	int      numberOfNumbers = 0;
	int      stringSize;

	stringSize = strlen(inputString);
	for (int i = 0; i < stringSize; i++)
	{
		if (!lastDigit && isdigit(inputString[i]))
			numberOfNumbers++;
		lastDigit = isdigit(inputString[i]);
	}
	return numberOfNumbers;
}

_declspec (dllexport) void tiffVersion(char x[]) {
	const char* p = TIFFGetVersion();
	memcpy(x, p, min(strlen(p),231)); // LabVIEW string size limit (unless pre-allocated)
}

_declspec (dllexport) TIFF* openTiffImage(const char * pathName) {
  TIFFSetErrorHandler(errorHandler);
	TIFF* tif = TIFFOpen(pathName, "r");
	return tif;
}

_declspec (dllexport) int countTiffPages(TIFF* tif) {
	if (tif) {
		int dircount = 0;
		do {
			dircount++;
		} while (TIFFReadDirectory(tif));
		return dircount;
	}
	else {
		return -1;
	}
}

_declspec (dllexport) void getTiffTags(TIFF* tif, int * compression, char * hostComputer, int32_t * imageWidth, int32_t * imageLength, char * software, int* bitness) {
  TIFFGetField(tif, TIFFTAG_COMPRESSION, compression);
  TIFFGetField(tif, TIFFTAG_HOSTCOMPUTER, hostComputer);
  TIFFGetField(tif, TIFFTAG_SOFTWARE, software);
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, imageWidth);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, imageLength);
  TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, bitness);
}

_declspec (dllexport) int readImage(TIFF* tif,              // TIFF handle - IN 
                                     const int directory,   // page ordinal number - IN
                                     uint8_t* redvals)      // OUT, caller allocates memory
{
  TIFFSetDirectory(tif, directory);
  int err = 0;
  uint32_t w, h;
  size_t npixels;

  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
  //TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, bits);
  npixels = w * h;
  uint32_t* raster = (uint32_t*) _TIFFmalloc(npixels * sizeof (uint32_t));
  if (raster != NULL) {
    err = TIFFReadRGBAImage(tif, w, h, raster, 1);
    // redvals = (uint8_t*) malloc(npixels);
    if (err != 1) {
      return err;
    }
    uint32_t* pFrom = raster;
    uint8_t* pTo = redvals;
    for (int i = 0; i < npixels; i++, ++pFrom) {
      *pTo++ = (uint8_t) TIFFGetR(*pFrom);
    }
  }
  _TIFFfree(raster);
  return err;
}

void errorHandler(const char *module, const char *fmt, va_list ap)
{
  char buffer[4096];
  vsnprintf_s(buffer, 4096, fmt, ap);
  Sleep(101);
  return;
}