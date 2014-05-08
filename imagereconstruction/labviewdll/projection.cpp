#include "stdafx.h"
#include "projection.h"
#include "mkl.h"
#include <math.h>
#include <malloc.h>
#include <float.h>

#define PI 3.14159265


_declspec (dllexport) void getPixelsForProjection(const int w, const double angle, const int* centre, int32_t* normalisation, int32_t* pixels, int32_t* count)
{
  double c_angle = cos(angle);
  double s_angle = sin(angle);
  float centre_x = (float) centre[0];
  float centre_y = (float) centre[1];
  int32_t* thispix = pixels;
  int32_t* thisnorm = normalisation;
  // backproject  
  if (angle <= PI / 4 || angle >= 3 * PI / 4)
  {
    double tan_angle = s_angle / c_angle;
    double sec_angle = 1 / c_angle;
    double offset = (c_angle + s_angle*tan_angle);
    double x_f = tan_angle * (centre_y - 1) + (-1 - centre_x)*offset + centre_x;
    double x_r;
    for (int j = 0; j < w; j++)
    {
      x_f += offset;
      x_r = x_f;
      for (int y = 0; y < w; y++)
      {
        x_r -= tan_angle;

        int pixel_below = (int) floor(x_r);
        int pixel_above = pixel_below + 1;
        int current_pixel;
        if (pixel_above <= 0)
        {
          if (tan_angle > 0)
          {
            break;
          }
          else
          {
            continue;
          }
        }
        else if (pixel_below >= w - 1)
        {
          if (tan_angle < 0)
          {
            break;
          }
          else
          {
            continue;
          }
        }
        else if (pixel_above - x_r <= 0.5)
        {
          current_pixel = y*w + pixel_above;
        }
        else if (x_r - pixel_below < 0.5)
        {
          current_pixel = y*w + pixel_below;
        }
        else
        {
          continue;
        }
        *(thispix++) = current_pixel;
        count[j] += 1;
        *(thisnorm++) += 1;
      }
    }
  }
  else // angle between PI/4 and 3*PI/4
  {
    double cot_angle = c_angle / s_angle;
    double csc_angle = 1 / s_angle;
    double offset = (c_angle * cot_angle + s_angle);
    double y_f = cot_angle * (centre_x - 1) + (-1 - centre_x) * offset + centre_y;
    double y_r;
    for (int j = 0; j < w; j++)
    {
      y_f += offset;
      y_r = y_f;
      for (int x = 0; x < w; x++)
      {
        y_r -= cot_angle;

        int pixel_below = (int) floor(y_r);
        int pixel_above = pixel_below + 1;
        int current_pixel;
        if (pixel_above <= 0)
        {
          if (cot_angle > 0)
          {
            break;
          }
          else
          {
            continue;
          }
        }
        else if (pixel_below >= w - 1)
        {
          if (cot_angle < 0)
          {
            break;
          }
          else
          {
            continue;
          }
        }
        else if (pixel_above - y_r <= 0.5)
        {
          current_pixel = pixel_above*w + x;
        }
        else if (y_r - pixel_below < 0.5)
        {
          current_pixel = pixel_below*w + x;
        }
        else
        {
          continue;
        }
        *(thispix++) = current_pixel;
        count[j] += 1;
        *(thisnorm++) += 1;
      }
    }
  }
}

// ANGLE MUST BE IN RADIANS! CONVERT IT YOURSELF
_declspec (dllexport) void sliceBP(const int32_t w, const uint8_t* line, float* results, int32_t* normalisation, int32_t* rayNormalisation, int32_t* pixels, int32_t* count)
{

  float* backprojectBuffer = (float*) malloc(sizeof(float) *w); // make a buffer to cast the slice to float
  int current_pixel, current_norm;
  int32_t* this_pixel = pixels;
  int32_t* this_norm = rayNormalisation;
  for (int j = 0; j < w; j++)
  {
    // process backprojectBuffer[j] here, for example FFT / filter / IFFT
    //status = DftiComputeForward(dftiDescriptor, reconSlice);
    /* result is given in CCS format*/
    backprojectBuffer[j] = (float) line[j];
  }

  // backproject
  for (int i = 0; i < w; i++)
  {
    for (int j = 0; j < count[i]; j++)
    {
      current_pixel = *(this_pixel++);
      current_norm = *(this_norm++);
      normalisation[current_pixel] += current_norm; // change to float and use distance??
      results[current_pixel] += backprojectBuffer[i] * current_norm;
    }
  }
  free(backprojectBuffer);
}

_declspec (dllexport) void sliceFP(const uint8_t* object, float* projection, int32_t* normalisation, const int w, int32_t* rayNormalisation, int32_t* pixels, int32_t* count)
{

  float* objectBuffer = (float*) malloc(sizeof(float) *w*w); // make a buffer to cast the slice to float
  int current_pixel, current_norm;
  int32_t* this_pixel = pixels;
  int32_t* this_norm = rayNormalisation;
  for (int j = 0; j < w*w; j++)
  {
    // process backprojectBuffer[j] here, for example FFT / filter / IFFT
    //status = DftiComputeForward(dftiDescriptor, reconSlice);
    /* result is given in CCS format*/
    objectBuffer[j] = (float) object[j];
  }


  // forward project
  for (int i = 0; i < w; i++)
  {
    for (int j = 0; j < count[i]; j++)
    {
      current_pixel = *(this_pixel++);
      current_norm = *(this_norm++);
      projection[i] += objectBuffer[current_pixel] * current_norm;
      normalisation[i] += current_norm;
    }
  }
  free(objectBuffer);
}

// ANGLE MUST BE IN RADIANS! CONVERT IT YOURSELF
_declspec (dllexport) void sliceBPf(const int32_t w, const float* line, float* results, int32_t* normalisation, int32_t* rayNormalisation, int32_t* pixels, int32_t* count)
{

  int current_pixel, current_norm;
  int32_t* this_pixel = pixels;
  int32_t* this_norm = rayNormalisation;

  // backproject
  for (int i = 0; i < w; i++)
  {
    for (int j = 0; j < count[i]; j++)
    {
      current_pixel = *(this_pixel++);
      current_norm = *(this_norm++);
      normalisation[current_pixel] += current_norm; // change to float and use distance??
      results[current_pixel] += line[i] * current_norm;
    }
  }
}

// ANGLE MUST BE IN RADIANS! CONVERT IT YOURSELF
_declspec (dllexport) void sliceFBPf(const int32_t w, const float* line, float* results, int32_t* normalisation, int32_t* rayNormalisation, int32_t* pixels, int32_t* count)
{
  DFTI_DESCRIPTOR_HANDLE fft_handle;
  int status;
  int current_pixel, current_norm;
  int32_t* this_pixel = pixels;
  int32_t* this_norm = rayNormalisation;

  float* fourierLine = (float *) malloc(sizeof(float) * w);
  memcpy(fourierLine, line, w*sizeof(float));
  status = DftiCreateDescriptor(&fft_handle, DFTI_SINGLE, DFTI_REAL, 1, w);
  if (status != 0)
  {
    free(fourierLine);
    return;
  }
  status = DftiCommitDescriptor(fft_handle);
  if (status != 0)
  {
    free(fourierLine);
    return;
  }
  status = DftiComputeForward(fft_handle, fourierLine);
  if (status != 0)
  {
    free(fourierLine);
    return;
  }
  for (int i = 0; i < w; i++)
  {
    fourierLine[i] *= i * 2.f / w;
  }
  status = DftiComputeBackward(fft_handle, fourierLine);
  if (status != 0)
  {
    free(fourierLine);
    return;
  }
  DftiFreeDescriptor(&fft_handle);
  // backproject
  for (int i = 0; i < w; i++)
  {
    for (int j = 0; j < count[i]; j++)
    {
      current_pixel = *(this_pixel++);
      current_norm = *(this_norm++);
      normalisation[current_pixel] += current_norm; // change to float and use distance??
      results[current_pixel] += fourierLine[i] * current_norm;
    }
  }
  free(fourierLine);
}


_declspec (dllexport) void  normaliseArrayf(const int length, const int32_t* normalisation, float* normalisedArray)
{
  for (int i = 0; i < length; i++) // normalise backprojection or forward projection
  {
    if (normalisation[i] > 0)
    {
      normalisedArray[i] = normalisedArray[i] / (float) normalisation[i];
    }
    else
    {
      normalisedArray[i] = 0;
    }
  }
}

_declspec (dllexport) void sliceFPf(const float* object, float* projection, int32_t* normalisation, const int w, int32_t* rayNormalisation, int32_t* pixels, int32_t* count)
{
  int current_pixel, current_norm;
  int32_t* this_pixel = pixels;
  int32_t* this_norm = rayNormalisation;


  // forward project
  for (int i = 0; i < w; i++)
  {
    for (int j = 0; j < count[i]; j++)
    {
      current_pixel = *(this_pixel++);
      current_norm = *(this_norm++);
      projection[i] += object[current_pixel] * current_norm;
      normalisation[i] += current_norm;
    }
  }
}
