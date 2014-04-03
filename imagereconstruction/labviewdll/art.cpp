#include "stdafx.h"
#include "art.h"
#include "mkl.h"
#include <math.h>
#include <malloc.h>
#include "projection.h"
#include <float.h>

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
  vslConvDeleteTask(&task);
  return status2;
}

_declspec (dllexport) void tvIteration(float* f, int* shape, float mu, float beta)
{
  int V = shape[0] * shape[1] * shape[2];
  float* g = (float*) malloc(sizeof(float) * V);
}

_declspec (dllexport) void artIteration(float* object, const float* actualProjection, const int w, int32_t* rayNormalisation, int32_t* pixels, int32_t* count)
{
  int* normalisation = (int*) malloc(sizeof(int) * w);
  float* forwardProjection = (float*) malloc(sizeof(float) * w);
  float* newProjection = (float*) malloc(sizeof(float) * w);
  for (int i = 0; i < w; i++)
  {
    normalisation[i] = 0;
    forwardProjection[i] = 0;
    newProjection[i] = 0;
  }

  for (int i = 0; i < w; i++)
  {
    normalisation[i] = 0;
  }
  sliceFPf(object, forwardProjection, normalisation, w, rayNormalisation, pixels, count);
  normaliseArrayf(w, normalisation, forwardProjection);

  float* invert = forwardProjection;
  for (int i = 0; i < w; i++)
  {
    int fpclass = _fpclassf(*invert);
    if (fpclass != _FPCLASS_NZ && fpclass != _FPCLASS_PZ)
    {
      *invert = 1 / *(invert++);
    }
    else
    {
      *(invert++) = 255;
    }
  }
  vmsMul(w, actualProjection, forwardProjection, newProjection, VML_ERRMODE_IGNORE);

  int current_pixel;
  int32_t* this_pixel = pixels;
  int32_t* this_norm = rayNormalisation;

  // backproject
  for (int i = 0; i < w; i++)
  {
    for (int j = 0; j < count[i]; j++)
    {
      current_pixel = *(this_pixel++);
      object[current_pixel] *= newProjection[i];
      int fpclass = _fpclassf(object[current_pixel]);
      if (fpclass != _FPCLASS_PN && fpclass != _FPCLASS_PZ)
      {
        int U = 0;
      }
    }
  }
}