#include "labviewdll.h"

extern "C" {
  _declspec (dllexport) void transform2d(const float* transformationMatrix, float* object, const int xshape, const int yshape, const int* centre);
  _declspec (dllexport) void transform3dh(const float* transformationMatrix, float* object, const int xshape, const int yshape, const int zshape);
}