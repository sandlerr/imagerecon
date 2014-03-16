#include "labviewdll.h"

extern "C" {
  _declspec (dllexport) void sliceFBP(TIFF* tif, const uint32_t slice, const int32_t w, const int32_t l, const int32_t total_quantity, int32_t samples_per_pixel, uint32_t used_quantity, const double* angles, const char* resultPath);
}