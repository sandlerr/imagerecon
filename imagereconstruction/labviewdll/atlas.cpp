#include "stdafx.h"
#include "atlas.h"
#include "mkl.h"
#include <malloc.h>

_declspec (dllexport) void transform2d(const float* transformationMatrix, float* object, const int xshape, const int yshape, const int* centre)
{
  int numPoints = xshape * yshape;
  float* newObject = (float*) malloc(sizeof(float) * numPoints);
  int old_x = -1;
  int old_y = 0;
  for (int i = 0; i < numPoints; i++)
  {
    int new_x, new_y;
    int coord;
    if (++old_x == xshape)
    {
      ++old_y;
      old_x = 0;
    }
    new_x = (int) (transformationMatrix[0] * (old_x - centre[0]) + transformationMatrix[2] * (old_y-centre[1]) + 0.5f + centre[0]); // add 0.5 because conversion to int rounds down
    if (new_x >= xshape || new_x < 0)
    {
      newObject[i] = 0;
      continue;
    }
    new_y = (int) (transformationMatrix[1] * (old_x - centre[0]) + transformationMatrix[3] * (old_y - centre[1]) + 0.5f + centre[1]);
    if (new_y >= yshape || new_y < 0)
    {
      newObject[i] = 0;
      continue;
    }
    coord = new_x + xshape * new_y;
    newObject[i] = object[coord];
  }
  memcpy(object, newObject, numPoints * sizeof(float));
}

_declspec (dllexport) int match_atlas(float* object, const int xshape, const int yshape, const float* atlas_object, MKL_INT n, float* transformationMatrix)
{
  _TRNSP_HANDLE_t handle;
  MKL_INT iter1 = 1000;
  MKL_INT iter2 = 1000;
  float rs = 100;
  int numPoints = xshape * yshape;

  float eps[6] = {1E-3f, 1000.f, 1000.f, 1.f, 1000.f, 1E-6f};
  MKL_INT result = strnlsp_init(&handle, &n, &numPoints, transformationMatrix, eps, &iter1, &iter2, &rs);
  return result;
}