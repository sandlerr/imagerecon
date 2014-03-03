BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD, LPVOID);
typedef struct tiff TIFF;
extern "C"
{
  _declspec (dllexport) long  __cdecl add_num(long, long);
  _declspec (dllexport) long  __cdecl avg_num(float *, long, float *);
  _declspec (dllexport) unsigned int  __cdecl numIntegers(unsigned char *);
  _declspec (dllexport) void  __cdecl tiffVersion(char[]);
  _declspec (dllexport) TIFF*  __cdecl openTiffImage(const char *);
  _declspec (dllexport) int  __cdecl countTiffPages(TIFF*);
  _declspec (dllexport) void  __cdecl getTiffTags(TIFF*, int*, char*, int*, int*);
}