#include "labviewdll.h"
#include "mkl.h"

extern "C" {
  _declspec (dllexport) void transform2d(const float rotation, const float* translationMatrix, const float* object, float* newObject, const int xshape, const int yshape, const int* centre);
  _declspec (dllexport) void transform3dh(const float* transformationMatrix, float* object, const int xshape, const int yshape, const int zshape);
  void transform2d_for_jacobi(MKL_INT* input_length, MKL_INT* output_length, float* input, float* output, void* data);
  _declspec (dllexport) int match_atlas(float* object, const int xshape, const int yshape, float* atlasObject, MKL_INT n, float* transformationMatrix, const int* centre);
}