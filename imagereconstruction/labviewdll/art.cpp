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
  const int dims = 2;
  const int yshape[3] = { 3,3 };
  const float kernel[] = { 0, 1, 0, 1, -4, 1, 0, 1, 0 };
  status = vslsConvNewTask(&task, mode, dims, xshape, yshape, zshape);
  if (status != 0)
  {
    return status;
  }
  status2 = vslsConvExec(task, input, NULL, kernel, NULL, output, NULL);
  vslConvDeleteTask(&task);
  return status2;
}


_declspec (dllexport) void tvIteration(float* f, const int xshape, const int yshape, const float mu, const float beta, const float* observations, const int numProjections, const double* angles, const int* centre)
{
  int numVoxels = xshape * yshape;
  int V = numVoxels;

  float* g = (float*) malloc(sizeof(float) * numVoxels);
  float* P = (float*) malloc(sizeof(float) * xshape * numProjections);
  int* normalisation = (int*) calloc(xshape*yshape,sizeof(int));
  int* pixels = (int*) calloc(xshape*yshape,sizeof(int));
  int* count = (int*) calloc(xshape,sizeof(int));
  int* rayNormalisation = (int*) calloc(xshape,sizeof(int));
  float* residuals = (float*) malloc(xshape * sizeof(float));
  float* sqResiduals = (float*) malloc(xshape * sizeof(float));

  for (int i = 0; i < numProjections; i++)
  {
    getPixelsForProjection(xshape, angles[i], centre, normalisation, pixels, count);
    sliceFPf(f, P + i * xshape, normalisation, xshape, rayNormalisation, pixels, count);
    normaliseArrayf(xshape, rayNormalisation, P + i * xshape);
    memset(normalisation, 0, xshape*yshape*  sizeof(int))
  }

  free(normalisation);
  free(pixels);
  free(count);
  free(rayNormalisation);

  vsSub(xshape, P, observations, residuals);
  vsPowx(xshape, residuals,2,sqResiduals);

  for (int i = 0; i < numVoxels; i++)
  {

  }
  free(residuals);

  free(g);
  free(P);

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

  sliceFPf(object, forwardProjection, normalisation, w, rayNormalisation, pixels, count);
  normaliseArrayf(w, normalisation, forwardProjection);
  free(normalisation);
  for (int i = 0; i < w; i++)
  {
    if (forwardProjection[i] == 0)
    {
      forwardProjection[i] = 0.0039215686274509803921568627451f;
    }
  }
  //vmsMul(w, actualProjection, forwardProjection, newProjection, VML_ERRMODE_IGNORE);
  vmsLinearFrac(w, actualProjection, forwardProjection, 1, 0, 1, 0, newProjection, VML_ERRMODE_IGNORE);
  free(forwardProjection);


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
    }
  }
  free(newProjection);
}