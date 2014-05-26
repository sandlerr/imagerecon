#include "labviewdll.h"

extern "C"
{
  _declspec (dllexport) int correlation(const int arraySize, float* x, float* y, float* z);
}