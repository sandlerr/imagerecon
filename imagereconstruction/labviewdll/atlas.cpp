#include "stdafx.h"
#include <atlas_imp.h>
#include "mkl.h"
#include <malloc.h>
#include <math.h>
#include <fstream>
#include <iostream>

struct Vars4Jac
{
  float* m_pObject;
  int m_xshape;
  int m_yshape;
  int m_centre[3];
  float* m_psampleProjection;
  float* m_pnewProjection;
};

/*struct transform2d_params
{
};*/


void transform2d_test(const float rotation, const float* translationMatrix, const float* object, float* newProjection, const int xshape, const int yshape, const int* centre)
{
  for (int i = 0; i < xshape; ++i)
  {
    newProjection[i] = 0;
  }
  float* newObject = (float*) malloc(xshape*yshape*sizeof(float));
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

  //forward project
  for (int i = 0; i < xshape; ++i)
  {
    for (int j = 0; j < yshape; j++)
    {
      newProjection[i] += newObject[i+j*xshape];
    }
    // normalise
    newProjection[i] = newProjection[i] / yshape;
  }
  free(newObject);
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

void transform2d_for_jacobi(MKL_INT* output_length, MKL_INT* input_length, float* input, float* output, void* data)
{
  Vars4Jac* pVars4Jac = (Vars4Jac*) data;
  const float* transformationMatrix = input;
  const float* object = pVars4Jac->m_pObject;
  const int xshape = pVars4Jac->m_xshape;
  const int yshape = pVars4Jac->m_yshape;
  const int* centre = pVars4Jac->m_centre;
  const int numPoints = xshape * yshape;
  const float* sampleProjection = pVars4Jac->m_psampleProjection;
  float* newProjection = pVars4Jac->m_pnewProjection;
  float rotation = transformationMatrix[0];
  transform2d_test(rotation, transformationMatrix + 1, object, newProjection, xshape, yshape, centre);
  for (int pixel = 0; pixel < *output_length; pixel++)
  {
    output[pixel] = newProjection[pixel] - sampleProjection[pixel];
  }
}

// n is the number of inputs - the centre and transformation matrix in the 2d case, the homog transformation matrix in the other case. Matrices should be populated with inital guess
int match_atlas(float* object, const int xshape, const int yshape, float* sampleProjection, MKL_INT n, float* transformationMatrix, float* lowerBound, float* upperBound, const int* centre, MKL_INT* iter, MKL_INT* st_cr, float* r1, float* r2)
{
  
  //extern void transform2d_for_jacobi(MKL_INT*, MKL_INT*, float*, float*, void*);
  int numPoints = xshape * yshape;
  int vector_size = yshape;
  _TRNSPBC_HANDLE_t handle;
  MKL_INT iter1 = 1E9;
  MKL_INT iter2 = 1E9;
  float rs = 100.;
  /* reverse communication interface parameter */
  MKL_INT RCI_Request;      // reverse communication interface variable
  /* controls of rci cycle */
  MKL_INT successful;
  /* function (f(x)) value vector */
  float *fvec = NULL;
  /* jacobi matrix */
  float *fjac = NULL;
  /* number of iterations */
  //MKL_INT iter;
  /* number of stop-criterion */
  //MKL_INT st_cr;
  /* initial and final residuals */
  //float r1, r2;
  /* cycle’s counter */
  MKL_INT i;
  /* results of input parameter checking */
  MKL_INT info[6];
  /* memory allocation flag */
  MKL_INT mem_error, error;


  error = 0;
  /* memory allocation */
  mem_error = 1;
  fvec = (float *) malloc(sizeof (float) * vector_size);
  if (fvec == NULL)
  {
    return mem_error;
  }
  fjac = (float *) malloc(sizeof (float) * vector_size * n);
  if (fjac == NULL)
  {
    free(fvec);
    return mem_error;
  }
  /* memory allocated correctly */
  mem_error = 0;
  /* set precisions for stop-criteria */
//  float eps[6] = {.0001f, 200.f, 0.01f, 0.0001f, 0.01f, 0.01f};
  float eps[6] = { 1E-20, 100.0, .000001, 1E-20, 1E-30, 1.0 };
  float jacobi_eps = 1.0;
  /* for (i = 0; i < 6; i++)
  {
    eps[i] = 200.f;
  }*/
  
  /* set initial values */
  for (i = 0; i < vector_size; i++)
    fvec[i] = 0.0;
  for (i = 0; i < vector_size * n; i++)
    fjac[i] = 0.0;
  
  MKL_INT result = strnlspbc_init(&handle, &n, &vector_size, transformationMatrix, lowerBound, upperBound, eps, &iter1, &iter2, &rs);

  if (result != TR_SUCCESS)
  {
    free(fvec);
    free(fjac);
    MKL_Free_Buffers();
    return result;
  }

  result = strnlspbc_check(&handle, &n, &vector_size, fjac, fvec, lowerBound, upperBound, eps, info);
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
      info[3] != 0 ||   // lowerbound is invalid
      info[4] != 0 || // upperbound is invalid
      info[5] != 0 // eps is invalid
      )
    {
      free(fvec);
      free(fjac);
      MKL_Free_Buffers();
      return result;
    }
  }

  float* newProjection = (float*) malloc((sizeof(float) * yshape));
  Vars4Jac myVars4Jac;
  myVars4Jac.m_pObject = object;
  myVars4Jac.m_centre[0] = centre[0];
  myVars4Jac.m_centre[1] = centre[1];
  myVars4Jac.m_centre[2] = NULL;
  myVars4Jac.m_xshape = xshape;
  myVars4Jac.m_yshape = yshape;
  myVars4Jac.m_psampleProjection = sampleProjection;
  myVars4Jac.m_pnewProjection = newProjection;

  /* set initial rci cycle variables */
  RCI_Request = 0;
  successful = 0;
  /* rci cycle */
  while (successful == 0)
  {
    result = strnlspbc_solve(&handle, fvec, fjac,  &RCI_Request);
    if (result != TR_SUCCESS)
    {
      free(fvec);
      free(fjac);
      free(newProjection);
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
      float rotation = *transformationMatrix;
      transform2d_test(rotation, transformationMatrix + 1, object, newProjection, xshape, yshape, centre);
      float sum_least_squares = 0;
      for (int pixel = 0; pixel < vector_size; pixel++)
      {
        fvec[pixel] = newProjection[pixel] - sampleProjection[pixel];
        if (fvec[pixel] < 10000)
          sum_least_squares += fvec[pixel] * fvec[pixel];
        else
          int fail = 1;
      }
      sum_least_squares = sqrt(sum_least_squares);
    }
    else if (RCI_Request == 2)
    {
     
      if (sjacobix(transform2d_for_jacobi, &n, &vector_size, fjac, transformationMatrix, &jacobi_eps, &myVars4Jac) != TR_SUCCESS)
      {
        free(fvec);
        free(fjac);
        free(newProjection);
        MKL_Free_Buffers();
        return result;
      }
    }
  }
  if (strnlspbc_get(&handle, iter, st_cr, r1, r2) != TR_SUCCESS)
  {
    free(fvec);
    free(fjac);
    free(newProjection);
    MKL_Free_Buffers();
    return result;
  }
  if (strnlspbc_delete(&handle) != TR_SUCCESS)
  {
    free(fvec);
    free(fjac);
    free(newProjection);
    MKL_Free_Buffers();
    return result;
  }
  return result;
}

int serialise_for_atlas(float* object, const int xshape, const int yshape, float* sampleProjection, MKL_INT n, float* transformationMatrix, float* lowerBound, float* upperBound, const int* centre, MKL_INT* iter, MKL_INT* st_cr, float* r1, float* r2)
{
  std::ofstream ofs;
  ofs.open("D:\\work\\uni\\FYP\\workingData\\data.bin", std::ios_base::binary | std::ios_base::trunc);
  if (!ofs.is_open())
  {
    std::cout << "error in open" << std::endl;
    return -1;
  }
  else
  {

    ofs.write((char*) (void*) &xshape, sizeof(xshape));
    ofs.write((char*) (void*) &yshape, sizeof(yshape));

    ofs.write((char*) (void*) object, sizeof(float) * xshape * yshape);


    ofs.write((char*) (void*) sampleProjection, sizeof(float) * yshape);

    ofs.write((char*) (void*) &n, sizeof(n));

    ofs.write((char*) (void*) transformationMatrix, sizeof(float) * n);
    ofs.write((char*) (void*) lowerBound, sizeof(float) * n);
    ofs.write((char*) (void*) upperBound, sizeof(float) * n);

    int centreLength = 2;
    ofs.write((char*) (void*) &centreLength, sizeof(centreLength));
    ofs.write((char*) (void*) centre, sizeof(int) * centreLength);

    ofs.write((char*) (void*) iter, sizeof(int));
    ofs.write((char*) (void*) st_cr, sizeof(int));
    ofs.write((char*) (void*) r1, sizeof(float));
    ofs.write((char*) (void*) r2, sizeof(float));

    ofs.close();
  }
  return 0;
}