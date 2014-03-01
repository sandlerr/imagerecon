// labviewdll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <windows.h>
#include <string.h>
#include <ctype.h>
#include "labviewdll.h"
#include "tiffio.h"


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
	memcpy(x, p, strlen(p));
}

_declspec (dllexport) TIFF* openTiffImage(char pathName[]) {
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

_declspec (dllexport) void getTiffTags(TIFF* tif, int compression, char hostComputer[]) {
  TIFFGetField(tif, 259, &compression);
  TIFFGetField(tif, 316, hostComputer);
}