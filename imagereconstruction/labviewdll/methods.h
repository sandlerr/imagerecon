#include "labviewdll.h"

extern "C" {
  _declspec (dllexport) void sliceBP(const int32_t w, const double angle, uint8_t* line, float* results, int32_t* normalisation, const int* centre);
  _declspec (dllexport) void normaliseArray(const int length, const int* normalisation, uint8_t* normalisedArray);
  _declspec (dllexport) void sliceFP(const char* object, float* projection, int32_t* normalisation, const double angle, const int w, const int* centre);
}