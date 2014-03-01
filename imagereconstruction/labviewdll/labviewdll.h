BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD, LPVOID);
typedef struct tiff TIFF;
extern "C"
{
	_declspec (dllexport) long add_num(long, long);
	_declspec (dllexport) long avg_num(float *, long, float *);
	_declspec (dllexport) unsigned int  numIntegers(unsigned char *);
	_declspec (dllexport) void tiffVersion(char []);
	_declspec (dllexport) TIFF* openTiffImage(char []);
	_declspec (dllexport) int countTiffPages(TIFF*);
	_declspec (dllexport) void getTiffTags(TIFF*, int, char []);
}