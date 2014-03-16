// labviewdll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include <string.h>
#include <ctype.h>
#include "labviewdll.h"
#include "tiffio.h"
#include <malloc.h>


_declspec (dllexport) void tiffVersion(char x[]) {
	const char* p = TIFFGetVersion();
	memcpy(x, p, min(strlen(p),231)); // LabVIEW string size limit (unless pre-allocated)
}

_declspec (dllexport) TIFF* openTiffImage(const char * pathName) {
	TIFF* tif = TIFFOpen(pathName, "r");
	return tif;
}

_declspec (dllexport) int countTiffPages(TIFF* tif) {
	if (tif) {
		uint32_t dircount = 0;
		do {
			dircount++;
		} while (TIFFReadDirectory(tif));
		return dircount;
	}
	else {
		return -1;
	}
}

_declspec (dllexport) uint32_t getTiffTags(int * compression, char * hostComputer, int32_t * imageWidth, int32_t * imageLength, char * software, int* bitness, int* samples_per_pixel, TIFF* tif) {
  TIFFGetField(tif, TIFFTAG_COMPRESSION, compression);
  TIFFGetField(tif, TIFFTAG_HOSTCOMPUTER, hostComputer);
  TIFFGetField(tif, TIFFTAG_SOFTWARE, software);
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, imageWidth);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, imageLength);
  TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, bitness);
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, samples_per_pixel);
  uint32_t tifPages = countTiffPages(tif);
  return tifPages;
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

_declspec (dllexport) void sinograph(TIFF* tif, const uint32_t slice, const int32_t w, const int32_t l, const int32_t total_quantity, int32_t samples_per_pixel, uint32_t used_quantity, const char* resultPath)
{
  if (slice >= l)
  {
    return;
  }
  char pathName[1024];
  strcpy_s(pathName, resultPath);
  strcat_s(pathName, "\\sinograph.tif");

  TIFF* resultTif     = makeTiffImage(pathName, w, used_quantity, samples_per_pixel);

  int scanlineSize    = TIFFScanlineSize(tif);
  int stripSize       = TIFFStripSize(tif);
  int linesPerStrip   = stripSize / scanlineSize;
  int stripOfInterest = slice / linesPerStrip; // keep integer division here
  int offsetPixels    = slice % linesPerStrip * w;

  uint8_t* buffer     = (uint8_t*) _TIFFmalloc(stripSize);

  int32_t selector    = (total_quantity + used_quantity - 1) / used_quantity;
  used_quantity = 0;

  for (int32_t i = 0; i < total_quantity; i++)
  {
    if (i % selector != 0)
    {
      continue;
    }
    TIFFSetDirectory(tif, i);
    TIFFReadEncodedStrip(tif, stripOfInterest, buffer, stripSize);
    TIFFWriteScanline(resultTif, buffer + offsetPixels, used_quantity, scanlineSize);
    ++used_quantity;
  }

  _TIFFfree(buffer);
  TIFFClose(resultTif);
}

TIFF* makeTiffImage(const char * pathName, int32_t w, int32_t l, int32_t samples_per_pixel)
{
  TIFF* resultTif = TIFFOpen(pathName, "w");
  TIFFSetField(resultTif, TIFFTAG_IMAGEWIDTH, w);
  TIFFSetField(resultTif, TIFFTAG_IMAGELENGTH, l);
  TIFFSetField(resultTif, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(resultTif, TIFFTAG_SAMPLESPERPIXEL, samples_per_pixel);
  TIFFSetField(resultTif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(resultTif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

  TIFFSetField(resultTif, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
  TIFFSetField(resultTif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  return resultTif;
}

