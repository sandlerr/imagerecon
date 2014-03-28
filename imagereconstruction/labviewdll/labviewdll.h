#include <cstdint>

BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD, LPVOID);
typedef struct tiff TIFF;
extern "C"
{
  _declspec (dllexport) void  __cdecl tiffVersion(char[]);
  _declspec (dllexport) TIFF*  __cdecl openTiffImage(const char *);
  _declspec (dllexport) int  __cdecl countTiffPages(TIFF*);
  _declspec (dllexport) uint32_t getTiffTags( int * compression,
                                              char * hostComputer,
                                              int32_t * imageWidth,
                                              int32_t * imageLength,
                                              char * software,
                                              int* bitness,
                                              int* samples_per_pixel,
                                              int32_t* scanlineSize,
                                              int32_t* stripSize,
                                              TIFF* tif);

  _declspec (dllexport) int __cdecl readImage(TIFF* tif, const int directory, uint8_t* redvals);
  _declspec (dllexport) int setTiffDirectory(TIFF* tif, const int directory);

  _declspec (dllexport) int32_t readStrip(TIFF* tif,              // TIFF handle - IN 
                                      const int slice,      // required slice - IN
                                      uint8_t* strip,      // OUT, caller allocates memory
                                      const int linesPerStrip,    // strip size / scanline size - IN
                                      const int stripSize);    // strip size - IN

  _declspec (dllexport) TIFF* writeTiff(TIFF* resultTif, const int w, const int l, const int scanlineSize, const int page, const int npages, uint8_t* data);
  _declspec (dllexport) TIFF* makeTiffImage(const char * pathName);
  _declspec (dllexport) void closeTiff(TIFF* tiff);

  _declspec (dllexport) void sinograph(TIFF* tif, const uint32_t slice, const int32_t w, const int32_t l, const int32_t total_quantity, int32_t samples_per_pixel, uint32_t used_quantity, const char* resultPath);


}