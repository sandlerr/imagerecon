//#include <labviewdll.h>
#include <mkl.h>

#ifdef LABVIEWDLL_EXPORTS
#define DefMode _declspec (dllexport)
#else
#define DefMode _declspec (dllimport)
#endif

extern "C" {
  DefMode void transform2d_test(const float rotation, const float* translationMatrix, const float* object, float* newObject, const int xshape, const int yshape, const int* centre);
  DefMode void transform3dh(const float* transformationMatrix, float* object, const int xshape, const int yshape, const int zshape);
 // void transform2d_for_jacobi(MKL_INT* input_length, MKL_INT* output_length, float* input, float* output, void* data);
  DefMode int serialise_for_atlas(float* object, const int xshape, const int yshape, float* sampleProjection, MKL_INT n, float* transformationMatrix, float* lowerBound, float* upperBound, const int* centre, MKL_INT* iter, MKL_INT* st_cr, float* r1, float* r2);
  DefMode int         match_atlas(float* object, const int xshape, const int yshape, float* sampleProjection, MKL_INT n, float* transformationMatrix, float* lowerBound, float* upperBound, const int* centre, MKL_INT* iter, MKL_INT* st_cr, float* r1, float* r2);
}
