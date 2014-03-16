#include <cstdint>

BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD, LPVOID);
typedef struct tiff TIFF;
extern "C"
{
  _declspec (dllexport) void  __cdecl tiffVersion(char[]);
  _declspec (dllexport) TIFF*  __cdecl openTiffImage(const char *);
  _declspec (dllexport) int  __cdecl countTiffPages(TIFF*);
  _declspec (dllexport) uint32_t  __cdecl getTiffTags(int * compression, char * hostComputer, int32_t * imageWidth, int32_t * imageLength, char * software, int* bitness, int* samples_per_pixel, TIFF* tif);
  _declspec (dllexport) int __cdecl readImage(TIFF* tif, const int directory, uint8_t* redvals);
  _declspec (dllexport) void sinograph(TIFF* tif, const uint32_t slice, const int32_t w, const int32_t l, const int32_t total_quantity, int32_t samples_per_pixel, uint32_t used_quantity, const char* resultPath);
}
TIFF* makeTiffImage(const char * pathName, int32_t w, int32_t l, int32_t samples_per_pixel);