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

_declspec (dllexport) void transform3dh(const float* transformationMatrix, float* object, const int xshape, const int yshape, const int zshape)
{
  int numPoints = xshape * yshape * zshape;
  float* newObject = (float*) malloc(sizeof(float) * numPoints);
  float old_xyz[4]; // homogeneous vector
  float new_xyz[4];
  int old_xyzi[3] = { -1, 0, 0 };
  int new_xyzi[3];
  int coord;

  for (int i = 0; i < numPoints; i++)
  {
    if (++(old_xyzi[0]) == xshape)
    {
      old_xyzi[0] = 0;
      if (++(old_xyzi[1]) == yshape)
      {
        old_xyzi[1] = 0;
        ++(old_xyzi[2]);
      }
    }
    for (int j = 0; j < 3; j++)
    {
      old_xyz[j] = (float) old_xyzi[j];
    }
    old_xyz[3] = 1;
    cblas_sgemv(CblasColMajor, CblasNoTrans, 4, 4, 1, transformationMatrix, 4, old_xyz, 1, 0, new_xyz, 1); // transformation matrix times pixel vector
    float scale_factor = 1 / new_xyz[3];
    cblas_sscal(4, scale_factor, new_xyz, 1); // divide by the fourth element of the homogeneous vector
    for (int j = 0; j < 3; j++)
    {
      new_xyzi[j] = (int) (new_xyz[j]+0.5f);
    }
    
    if (new_xyzi[0] >= xshape || new_xyzi[0] < 0)
    {
      newObject[i] = 0;
      continue;
    }
    if (new_xyzi[1] >= yshape || new_xyzi[1] < 0)
    {
      newObject[i] = 0;
      continue;
    }
    if (new_xyzi[1] >= zshape || new_xyzi[1] < 0)
    {
      newObject[i] = 0;
      continue;
    }

    coord = new_xyzi[0] + xshape * new_xyzi[1] + xshape*yshape*new_xyzi[2];
    newObject[i] = object[coord];
  }
  memcpy(object, newObject, numPoints * sizeof(float));
}
// n is the number of inputs - the centre and transformation matrix in the 2d case, the homog transformation matrix in the other case
_declspec (dllexport) int match_atlas(float* object, const int xshape, const int yshape, const float* atlas_object, MKL_INT n, float* transformationMatrix)
{
  _TRNSP_HANDLE_t handle;
  MKL_INT iter1 = 1000;
  MKL_INT iter2 = 100;
  float rs = 0.0;
  int numPoints = xshape * yshape;

  float eps[6] = {1E-3f, 1000.f, 1000.f, 1.f, 1000.f, 1E-6f};
  MKL_INT result = strnlsp_init(&handle, &n, &numPoints, transformationMatrix, eps, &iter1, &iter2, &rs);

  if (result != TR_SUCCESS)
  {
    return result;
  }

  // cruft
  MKL_INT info[6] = {0, 0, 0, 0, 0, 0};
  result = strnlsp_check(&handle, &n, &numPoints, transformationMatrix, object, eps, info);
  if (result != TR_SUCCESS)
  {
    return result;
  }
  for (int i = 0; i < 4; i++)
  {
    if (info[i] != 0)
    {
      return i;
    }
  }
//  result = strnlsp_solve(&handle, float* fvec, float* fjac, MKL_INT* &RCI_Request);

  return result;
}