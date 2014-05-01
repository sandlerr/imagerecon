#include "stdafx.h"
#include "atlas.h"
#include "mkl.h"
#include <malloc.h>
#include <math.h>

_declspec (dllexport) void transform2d(const float rotation, const float* translationMatrix, const float* object, float* newObject, const int xshape, const int yshape, const int* centre)
{
  const float rotationMatrix[4] = { cos(rotation), -sin(rotation), sin(rotation), cos(rotation) };
  int c_x = centre[0];
  int c_y = centre[1];
  float addToX = c_x + translationMatrix[0];
  float addToY = c_y + translationMatrix[1];
  int numPoints = xshape * yshape;
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
    new_x = (int) (rotationMatrix[0] * (old_x - c_x) + rotationMatrix[2] * (old_y-c_y) + 0.5f + addToX); // add 0.5 because conversion to int rounds down
    if (new_x >= xshape || new_x < 0)
    {
      newObject[i] = 0;
      continue;
    }
    new_y = (int) (rotationMatrix[1] * (old_x - c_x) + rotationMatrix[3] * (old_y - c_y) + 0.5f + addToY);
    if (new_y >= yshape || new_y < 0)
    {
      newObject[i] = 0;
      continue;
    }
    coord = new_x + xshape * new_y;
    newObject[i] = object[coord];
  }
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


// n is the number of inputs - the centre and transformation matrix in the 2d case, the homog transformation matrix in the other case. Matrices should be populated with inital guess
_declspec (dllexport) int match_atlas(float* object, const int xshape, const int yshape, float* atlasObject, MKL_INT n, float* transformationMatrix, const int* centre)
{
  //extern void transform2d_for_jacobi(MKL_INT*, MKL_INT*, float*, float*, void*);
  int numPoints = xshape * yshape;
  float* newObject = (float *) malloc(numPoints * sizeof(float));
  _TRNSP_HANDLE_t handle;
  MKL_INT iter1 = 5;
  MKL_INT iter2 = 5;
  float rs = 0.0;
  /* reverse communication interface parameter */
  MKL_INT RCI_Request;      // reverse communication interface variable
  /* controls of rci cycle */
  MKL_INT successful;
  /* function (f(x)) value vector */
  float *fvec = NULL;
  /* jacobi matrix */
  float *fjac = NULL;
  /* number of iterations */
  MKL_INT iter;
  /* number of stop-criterion */
  MKL_INT st_cr;
  /* initial and final residuals */
  float r1, r2;
  /* cycle’s counter */
  MKL_INT i;
  /* results of input parameter checking */
  MKL_INT info[6];
  /* memory allocation flag */
  MKL_INT mem_error, error;


  error = 0;
  /* memory allocation */
  mem_error = 1;
  fvec = (float *) malloc(sizeof (float) * numPoints);
  if (fvec == NULL)
  {
    return mem_error;
  }
  fjac = (float *) malloc(sizeof (float) * numPoints * n);
  if (fjac == NULL)
  {
    free(fvec);
    return mem_error;
  }
  /* memory allocated correctly */
  mem_error = 0;
  /* set precisions for stop-criteria */
  float eps[6];
  for (i = 0; i < 6; i++)
  {
    eps[i] = 0.00001f;
  }

  /* set initial values */
  for (i = 0; i < numPoints; i++)
    fvec[i] = 0.0;
  for (i = 0; i < numPoints * n; i++)
    fjac[i] = 0.0;


  MKL_INT result = strnlsp_init(&handle, &n, &numPoints, transformationMatrix, eps, &iter1, &iter2, &rs);

  if (result != TR_SUCCESS)
  {
    free(fvec);
    free(fjac);
    MKL_Free_Buffers();
    return result;
  }

  result = strnlsp_check(&handle, &n, &numPoints, fjac, fvec, eps, info);
  if (result != TR_SUCCESS)
  {
    free(fvec);
    free(fjac);
    MKL_Free_Buffers();
    return result;
  }
  else
  {
    if (info[0] != 0 || // The handle is not valid.
      info[1] != 0 || // The fjac array is not valid.
      info[2] != 0 || // The fvec array is not valid.
      info[3] != 0    // The eps array is not valid.
      )
    {
      free(fvec);
      free(fjac);
      MKL_Free_Buffers();
      return result;
    }
  }

  /* set initial rci cycle variables */
  RCI_Request = 0;
  successful = 0;
  /* rci cycle */
  while (successful == 0)
  {
    result = strnlsp_solve(&handle, fvec, fjac,  &RCI_Request);
    if (result != TR_SUCCESS)
    {
      free(fvec);
      free(fjac);
      MKL_Free_Buffers();
      return result;
    }

    if (RCI_Request == -1 ||
      RCI_Request == -2 ||
      RCI_Request == -3 ||
      RCI_Request == -4 || RCI_Request == -5 || RCI_Request == -6)
    {
      /* exit rci cycle */
      successful = 1;
    }
    else if (RCI_Request == 1)
    {
      /* recalculate function value
      m            in:     dimension of function value
      n            in:     number of function variables
      x            in:     solution vector
      fvec    out:    function value f(x) */
      transform2d(transformationMatrix[0], transformationMatrix + 1, object, newObject, xshape, yshape, centre);
      for (int pixel = 0; pixel < numPoints; pixel++)
      {
        fvec[pixel] = newObject[pixel] - atlasObject[pixel];
      }
    }
    else if (RCI_Request == 2)
    {
      /* compute jacobi matrix
      extendet_powell      in:     external objective function
      n               in:     number of function variables
      m               in:     dimension of function value
      fjac            out:    jacobi matrix
      x               in:     solution vector
      jac_eps         in:     jacobi calculation precision */
      float* vars_for_jac[5];
      vars_for_jac[0] = object;
      vars_for_jac[1] = (float*) &xshape;
      vars_for_jac[2] = (float*) &yshape;
      vars_for_jac[3] = (float*) centre;
      vars_for_jac[4] = atlasObject;
      if (sjacobix(transform2d_for_jacobi, &n, &numPoints, fjac, transformationMatrix, eps, vars_for_jac) != TR_SUCCESS)
      {
        free(fvec);
        free(fjac);
        MKL_Free_Buffers();
        return result;
      }
    }
  }
  if (strnlsp_get(&handle, &iter, &st_cr, &r1, &r2) != TR_SUCCESS)
  {
    free(fvec);
    free(fjac);
    MKL_Free_Buffers();
    return result;
  }
  if (strnlsp_delete(&handle) != TR_SUCCESS)
  {
    free(fvec);
    free(fjac);
    MKL_Free_Buffers();
    return result;
  }
  return result;
}

void transform2d_for_jacobi(MKL_INT* output_length, MKL_INT* input_length, float* input, float* output, void* data)
{
  float** data_as_pointers = (float **) data;
  const float* transformationMatrix = input;
  const float* object = data_as_pointers[0];
  const int* pxshape = (int*) data_as_pointers[1];
  const int* pyshape = (int*) data_as_pointers[2];
  int xshape = *pxshape;
  int yshape = *pyshape;
  const float* centref = data_as_pointers[3];
  const int* centre = (int *) centref;
  const int numPoints = xshape * yshape;
  float* newObject = (float*) malloc((sizeof(float) *numPoints));
  const float* atlasObject = data_as_pointers[4];
  transform2d(transformationMatrix[0], transformationMatrix + 1, object, newObject, xshape, yshape, centre);
  for (int pixel = 0; pixel < *output_length; pixel++)
  {
    output[pixel] = newObject[pixel] - atlasObject[pixel];
  }
}