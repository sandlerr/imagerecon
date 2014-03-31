#include "labviewdll.h"

extern "C" {
  _declspec (dllexport) int laplacian(const float* input, float* output, const int* xshape, const int* zshape);
}
