#include "stdafx.h"
#include "tifOp.h"

tifImage::tifImage()
{
}

tifImage::tifImage(TIFF* tifFile)
{
	tif = tifFile;
}


int tifImage::getProjectionNum()
{
	int dircount = 0;
	if (tif)
	{
		do {
			dircount++;
		} while (TIFFReadDirectory(tif));
	}
	return dircount;
}

tifImage::~tifImage()
{
	TIFFClose(tif);
}