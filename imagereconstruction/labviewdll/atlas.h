#include "labviewdll.h"

extern "C" {
  _declspec (dllexport) void transform2d(const float* transformationMatrix, float* object, const int xshape, const int yshape, const int* centre);
}