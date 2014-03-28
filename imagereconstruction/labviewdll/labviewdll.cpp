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

_declspec (dllexport) uint32_t getTiffTags( int * compression, 
                                            char * hostComputer, 
                                            int32_t * imageWidth, 
                                            int32_t * imageLength, 
                                            char * software, 
                                            int* bitness, 
                                            int* samples_per_pixel, 
                                            int32_t* scanlineSize,
                                            int32_t* stripSize,
                                            TIFF* tif) {
  TIFFGetField(tif, TIFFTAG_COMPRESSION, compression);
  TIFFGetField(tif, TIFFTAG_HOSTCOMPUTER, hostComputer);
  TIFFGetField(tif, TIFFTAG_SOFTWARE, software);
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, imageWidth);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, imageLength);
  TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, bitness);
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, samples_per_pixel);
  uint32_t tifPages = countTiffPages(tif);
  *scanlineSize = TIFFScanlineSize(tif);
  *stripSize = TIFFStripSize(tif);
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

_declspec (dllexport) int setTiffDirectory(TIFF* tif, const int directory)
{
  return TIFFSetDirectory(tif, directory);
}

_declspec (dllexport) int32_t readStrip(TIFF* tif,              // TIFF handle - IN 
                                    const int slice,      // required slice - IN
                                    uint8_t* strip,      // OUT, caller allocates memory
                                    const int linesPerStrip,    // strip size / scanline size - IN
                                    const int stripSize)    // strip size - IN        
{
  int32_t err = 0;
  int stripOfInterest = slice / linesPerStrip; // keep integer division here
  err = TIFFReadEncodedStrip(tif, stripOfInterest, strip, stripSize);
  return err;
}

TIFF* makeTiffImage(const char * pathName)
{
  TIFF* resultTif = TIFFOpen(pathName, "w");

  return resultTif;
}

TIFF* writeTiff(TIFF* resultTif, const int w, const int l, const int scanlineSize, const int page, const int npages, uint8_t* data)
{
  TIFFSetField(resultTif, TIFFTAG_IMAGEWIDTH, w);
  TIFFSetField(resultTif, TIFFTAG_IMAGELENGTH, l);
  TIFFSetField(resultTif, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(resultTif, TIFFTAG_SAMPLESPERPIXEL, 1);
  TIFFSetField(resultTif, TIFFTAG_ROWSPERSTRIP, 1);
  TIFFSetField(resultTif, TIFFTAG_RESOLUTIONUNIT, 1);
  TIFFSetField(resultTif, TIFFTAG_XRESOLUTION, 1);
  TIFFSetField(resultTif, TIFFTAG_YRESOLUTION, 1);
  TIFFSetField(resultTif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(resultTif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

  TIFFSetField(resultTif, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
  TIFFSetField(resultTif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  /* We are writing single page of the multipage file */
  TIFFSetField(resultTif, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
  /* Set the page number */
  TIFFSetField(resultTif, TIFFTAG_PAGENUMBER, page, npages);
  // write out the result to a tif
  for (int i = 0; i < l; i++)
  {
    TIFFWriteScanline(resultTif, data, i, scanlineSize);
    data += w;
  }
  TIFFWriteDirectory(resultTif);
  return resultTif;
}

void closeTiff(TIFF* tiff)
{
  TIFFClose(tiff);
}

_declspec (dllexport) void sinograph(TIFF* tif, const uint32_t slice, const int32_t w, const int32_t l, const int32_t total_quantity, int32_t samples_per_pixel, uint32_t used_quantity, const char* resultPath)
{
  if (slice >= (uint32_t) l)
  {
    return;
  }
  char pathName[1024];
  strcpy_s(pathName, resultPath);
  strcat_s(pathName, "\\sinograph.tif");

  TIFF* resultTif = makeTiffImage(pathName);
  TIFFSetField(resultTif, TIFFTAG_IMAGEWIDTH, w);
  TIFFSetField(resultTif, TIFFTAG_IMAGELENGTH, used_quantity);
  TIFFSetField(resultTif, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(resultTif, TIFFTAG_SAMPLESPERPIXEL, samples_per_pixel);
  TIFFSetField(resultTif, TIFFTAG_ROWSPERSTRIP, 1);
  TIFFSetField(resultTif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(resultTif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

  TIFFSetField(resultTif, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
  TIFFSetField(resultTif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  int32_t scanlineSize = TIFFScanlineSize(tif);
  int32_t stripSize = TIFFStripSize(tif);
  int linesPerStrip = stripSize / scanlineSize;
  int stripOfInterest = slice / linesPerStrip; // keep integer division here
  int offsetPixels = slice % linesPerStrip * w;

  uint8_t* buffer = (uint8_t*) _TIFFmalloc(stripSize);

  int32_t selector = (total_quantity + used_quantity - 1) / used_quantity;
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