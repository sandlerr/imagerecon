#include "labviewdll.h"

extern "C" {
  _declspec (dllexport) void getPixelsForProjection(const int w, const double angle, const int* centre, int32_t* normalisation, int32_t* pixels, int32_t* count);
  _declspec (dllexport) void sliceBP(const int32_t w, const uint8_t* line, float* results, int32_t* normalisation, int32_t* rayNormalisation, int32_t* pixels, int32_t* count);
  _declspec (dllexport) void sliceBPf(const int32_t w, const double angle, const float* line, float* results, int32_t* normalisation, const int* centre);
  _declspec (dllexport) void normaliseArrayf(const int length, const int* normalisation, float* normalisedArray);
  _declspec (dllexport) void sliceFP(const uint8_t* object, float* projection, int32_t* normalisation, const int w, int32_t* rayNormalisation, int32_t* pixels, int32_t* count);
  _declspec (dllexport) void sliceFPf(const float* object, float* projection, int32_t* normalisation, const double angle, const int w, const int* centre);
}

