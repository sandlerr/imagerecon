#include "stdafx.h"
#include "art.h"
#include "mkl.h"
#include <math.h>
#include <malloc.h>
#include "projection.h"

_declspec (dllexport) int laplacian(const float* input, float* output, const int* xshape, const int* zshape)
{
  int status, status2;
  VSLConvTaskPtr task;
  const int mode = VSL_CONV_MODE_AUTO;
  const int dims = 3;
  const int yshape[3] = { 3, 3, 3 };
  const float kernel[] = { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, -6, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 };
  status = vslsConvNewTask(&task, mode, dims, xshape, yshape, zshape);
  if (status != 0)
  {
    return status;
  }
  status2 = vslsConvExec(task, input, NULL, kernel, NULL, output, NULL);
  return status2;
}

_declspec (dllexport) void tvIteration(float* f, int* shape, float mu, float beta)
{
  int V = shape[0] * shape[1] * shape[2];
  float* g = (float*) malloc(sizeof(float) * V);
}

_declspec (dllexport) void artIteration(const float angle, float* object, const float* projection, const int w, const int* centre)
{
  float* newProjectionRatio = (float*) malloc(sizeof(float) * w);
  int* normalisation = (int*) malloc(sizeof(int) * w);
  sliceFPf(object, newProjectionRatio,normalisation, (double) angle,w,centre);
  normaliseArrayf(w, normalisation, newProjectionRatio);

  float* invert = newProjectionRatio;
  for (int i = 0; i < w; i++)
  {
    *invert = 1 / *(invert++);
  }
  vsMul(w,projection,newProjectionRatio, newProjectionRatio);

}