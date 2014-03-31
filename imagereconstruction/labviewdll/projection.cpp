#include "stdafx.h"
#include "projection.h"
#include <math.h>
#include <malloc.h>

#define PI 3.14159265


_declspec (dllexport) int getPixelsForProjection(const int w, const double angle, const int* centre, int32_t** normalisation, int32_t** pixels, int32_t* count)
{

  double c_angle = cos(angle);
  double s_angle = sin(angle);
  float centre_x = (float) centre[0];
  float centre_y = (float) centre[1];

  if (angle <= PI / 4 || angle >= 3 * PI / 4)
  {
    double tan_angle = s_angle / c_angle;
    double sec_angle = 1 / c_angle;
    double increment = (c_angle + s_angle*tan_angle);
    double x_f = tan_angle * (centre_y - 1) + (-1 - centre_x)*increment + centre_x;
    for (int j = 0; j < w; j++)
    {
      int32_t* thispix = pixels[j];
      int32_t* thisnorm = normalisation[j];
      x_f += increment;
      for (int y = 0; y < w; y++)
      {
        x_f -= tan_angle;
        int pixel_below = (int) floor(x_f);
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
        else if (pixel_above - x_f <= 0.5)
        {
          current_pixel = y*w + pixel_above;
        }
        else if (x_f - pixel_below < 0.5)
        {
          current_pixel = y*w + pixel_below;
        }
        else
        {
          continue;
        }
        *(thispix++) = current_pixel;
        count[j] += 1;
        *(thisnorm++) = 1;
      }
    }
  }
    else // angle between PI/4 and 3*PI/4
    {
      double cot_angle = c_angle / s_angle;
      double csc_angle = 1 / s_angle;
      double increment = (c_angle * cot_angle + s_angle);
      double y_f = cot_angle * (centre_x - 1) + (-1 - centre_x) * increment + centre_y;
      for (int j = 0; j < w; j++)
      {
        int32_t* thispix = pixels[j];
        int32_t* thisnorm = normalisation[j];
        y_f += increment;
        for (int x = 0; x < w; x++)
        {
          y_f -= cot_angle;

          int pixel_below = (int) floor(y_f);
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
          else if (pixel_above - y_f <= 0.5)
          {
            current_pixel = pixel_above*w + x;
          }
          else if (y_f - pixel_below < 0.5)
          {
            current_pixel = pixel_below*w + x;
          }
          else
          {
            continue;
          }
          *(thispix++) = current_pixel;
          count[j] += 1;
          *(thisnorm++) = 1;;
        }
      }
  }
}

// ANGLE MUST BE IN RADIANS! CONVERT IT YOURSELF
_declspec (dllexport) void sliceBP(const int32_t w, const double angle, const uint8_t* line, float* results, int32_t* normalisation, const int* centre)
{

  double c_angle = cos(angle);
  double s_angle = sin(angle);
  float centre_x = (float) centre[0];
  float centre_y = (float) centre[1];

  float* backprojectBuffer = (float*) malloc(sizeof(float) *w); // make a buffer to cast the slice to float

  for (int j = 0; j < w; j++)
  {
    // process backprojectBuffer[j] here, for example FFT / filter / IFFT
    //status = DftiComputeForward(dftiDescriptor, reconSlice);
    /* result is given in CCS format*/
    backprojectBuffer[j] = (float) line[j];
  }

  // backproject
  for (int j = 0; j < w; j++)
  {
    if (angle <= PI / 4 || angle >= 3 * PI / 4)
    {
      double tan_angle = s_angle / c_angle;
      double sec_angle = 1 / c_angle;
      double x_f = tan_angle * (centre_y - 1) + (j - centre_x)*(c_angle + s_angle*tan_angle) + centre_x;
      for (int y = 0; y < w; y++)
      {
        x_f -= tan_angle;

        int pixel_below = (int) floor(x_f);
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
        else if (pixel_above - x_f <= 0.5)
        {
          current_pixel = y*w + pixel_above;
        }
        else if (x_f - pixel_below < 0.5)
        {
          current_pixel = y*w + pixel_below;
        }
        else
        {
          continue;
        }
        normalisation[current_pixel] += 1; // change to float and use distance??
        results[current_pixel] += backprojectBuffer[j] * 1;
      }
    }
    else // angle between PI/4 and 3*PI/4
    {
      double cot_angle = c_angle / s_angle;
      double csc_angle = 1 / s_angle;
      double y_f = cot_angle * (centre_x - 1) + (j - centre_x) * (c_angle * cot_angle + s_angle) + centre_y;

      for (int x = 0; x < w; x++)
      {
        y_f -= cot_angle;

        int pixel_below = (int) floor(y_f);
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
        else if (pixel_above - y_f <= 0.5)
        {
          current_pixel = pixel_above*w + x;
        }
        else if (y_f - pixel_below < 0.5)
        {
          current_pixel = pixel_below*w + x;
        }
        else
        {
          continue;
        }
        normalisation[current_pixel] += 1; // change to float and use distance??
        results[current_pixel] += backprojectBuffer[j] * 1;
      }
    }
  }
  free(backprojectBuffer);
}

_declspec (dllexport) void sliceFP(const uint8_t* object, float* projection, int32_t* normalisation, const double angle, const int w, const int* centre)
{
  double c_angle = cos(angle);
  double s_angle = sin(angle);
  float centre_x = (float) centre[0];
  float centre_y = (float) centre[1];

  float* objectBuffer = (float*) malloc(sizeof(float) *w*w); // make a buffer to cast the slice to float

  for (int j = 0; j < w*w; j++)
  {
    // process backprojectBuffer[j] here, for example FFT / filter / IFFT
    //status = DftiComputeForward(dftiDescriptor, reconSlice);
    /* result is given in CCS format*/
    objectBuffer[j] = (float) object[j];
  }


  // forward project
  for (int j = 0; j < w; j++)
  {
    if (angle <= PI / 4 || angle >= 3 * PI / 4)
    {
      double tan_angle = s_angle / c_angle;
      double sec_angle = 1 / c_angle;

      double x_f = tan_angle * (centre_y - 1) + (j - centre_x)*(c_angle + s_angle*tan_angle) + centre_x;
      for (int y = 0; y < w; y++)
      {
        x_f -= tan_angle;

        int pixel_below = (int) floor(x_f);
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
        else if (pixel_above - x_f <= 0.5)
        {
          current_pixel = y*w + pixel_above;
        }
        else if (x_f - pixel_below < 0.5)
        {
          current_pixel = y*w + pixel_below;
        }
        else
        {
          continue;
        }
        normalisation[j] += 1; // change to float and use distance??
        projection[j] += objectBuffer[current_pixel] * 1;
      }
    }
    else // angle between PI/4 and 3*PI/4
    {
      double cot_angle = c_angle / s_angle;
      double csc_angle = 1 / s_angle;
      double y_f = cot_angle * (centre_x - 1) + (j - centre_x) * (c_angle * cot_angle + s_angle) + centre_y;

      for (int x = 0; x < w; x++)
      {
        y_f -= cot_angle;

        int pixel_below = (int) floor(y_f);
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
        else if (pixel_above - y_f <= 0.5)
        {
          current_pixel = pixel_above*w + x;
        }
        else if (y_f - pixel_below < 0.5)
        {
          current_pixel = pixel_below*w + x;
        }
        else
        {
          continue;
        }
        normalisation[j] += 1; // change to float and use distance??
        projection[j] += objectBuffer[current_pixel] * 1;
      }
    }
  }
  free(objectBuffer);
}

// ANGLE MUST BE IN RADIANS! CONVERT IT YOURSELF
_declspec (dllexport) void sliceBPf(const int32_t w, const double angle, const float* line, float* results, int32_t* normalisation, const int* centre)
{

  double c_angle = cos(angle);
  double s_angle = sin(angle);
  float centre_x = (float) centre[0];
  float centre_y = (float) centre[1];

  // backproject
  for (int j = 0; j < w; j++)
  {
    if (angle <= PI / 4 || angle >= 3 * PI / 4)
    {
      double tan_angle = s_angle / c_angle;
      double sec_angle = 1 / c_angle;
      double x_f = tan_angle * (centre_y - 1) + (j - centre_x)*(c_angle + s_angle*tan_angle) + centre_x;
      for (int y = 0; y < w; y++)
      {
        x_f -= tan_angle;

        int pixel_below = (int) floor(x_f);
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
        else if (pixel_above - x_f <= 0.5)
        {
          current_pixel = y*w + pixel_above;
        }
        else if (x_f - pixel_below < 0.5)
        {
          current_pixel = y*w + pixel_below;
        }
        else
        {
          continue;
        }
        normalisation[current_pixel] += 1; // change to float and use distance??
        results[current_pixel] += line[j] * 1;
      }
    }
    else // angle between PI/4 and 3*PI/4
    {
      double cot_angle = c_angle / s_angle;
      double csc_angle = 1 / s_angle;
      double y_f = cot_angle * (centre_x - 1) + (j - centre_x) * (c_angle * cot_angle + s_angle) + centre_y;

      for (int x = 0; x < w; x++)
      {
        y_f -= cot_angle;

        int pixel_below = (int) floor(y_f);
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
        else if (pixel_above - y_f <= 0.5)
        {
          current_pixel = pixel_above*w + x;
        }
        else if (y_f - pixel_below < 0.5)
        {
          current_pixel = pixel_below*w + x;
        }
        else
        {
          continue;
        }
        normalisation[current_pixel] += 1; // change to float and use distance??
        results[current_pixel] += line[j] * 1;
      }
    }
  }
}


_declspec (dllexport) void  normaliseArrayf(const int length, const int32_t* normalisation, float* normalisedArray)
{
  for (int i = 0; i < length; i++) // normalise backprojection or forward projection
  {
    if (normalisation[i] > 0)
      normalisedArray[i] =  normalisedArray[i] / (float)normalisation[i];
    else
      normalisedArray[i] = 0;
  }
}

_declspec (dllexport) void sliceFPf(const float* object, float* projection, int32_t* normalisation, const double angle, const int w, const int* centre)
{
  double c_angle = cos(angle);
  double s_angle = sin(angle);
  float centre_x = (float) centre[0];
  float centre_y = (float) centre[1];


  // forward project
  for (int j = 0; j < w; j++)
  {
    if (angle <= PI / 4 || angle >= 3 * PI / 4)
    {
      double tan_angle = s_angle / c_angle;
      double sec_angle = 1 / c_angle;

      double x_f = tan_angle * (centre_y - 1) + (j - centre_x)*(c_angle + s_angle*tan_angle) + centre_x;
      for (int y = 0; y < w; y++)
      {
        x_f -= tan_angle;

        int pixel_below = (int) floor(x_f);
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
        else if (pixel_above - x_f <= 0.5)
        {
          current_pixel = y*w + pixel_above;
        }
        else if (x_f - pixel_below < 0.5)
        {
          current_pixel = y*w + pixel_below;
        }
        else
        {
          continue;
        }
        normalisation[j] += 1; // change to float and use distance??
        projection[j] += object[current_pixel] * 1;
      }
    }
    else // angle between PI/4 and 3*PI/4
    {
      double cot_angle = c_angle / s_angle;
      double csc_angle = 1 / s_angle;
      double y_f = cot_angle * (centre_x - 1) + (j - centre_x) * (c_angle * cot_angle + s_angle) + centre_y;

      for (int x = 0; x < w; x++)
      {
        y_f -= cot_angle;

        int pixel_below = (int) floor(y_f);
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
        else if (pixel_above - y_f <= 0.5)
        {
          current_pixel = pixel_above*w + x;
        }
        else if (y_f - pixel_below < 0.5)
        {
          current_pixel = pixel_below*w + x;
        }
        else
        {
          continue;
        }
        normalisation[j] += 1; // change to float and use distance??
        projection[j] += object[current_pixel] * 1;
      }
    }
  }
}
