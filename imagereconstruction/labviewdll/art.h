#include "labviewdll.h"

extern "C" {
  _declspec (dllexport) void artIteration(float* object, const float* actualProjection, const int w, int32_t* rayNormalisation, int32_t* pixels, int32_t* count);
  _declspec (dllexport) int laplacian(const float* input, float* output, const int* xshape, const int* zshape);
}
