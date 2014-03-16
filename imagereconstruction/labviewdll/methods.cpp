#include "stdafx.h"
#include "methods.h"
#include "mkl_dfti.h"
#include <math.h>
#include "tiffio.h"
#include <malloc.h>
#include <string.h>

#define PI 3.14159265


_declspec (dllexport) void sliceFBP(TIFF* tif, const uint32_t slice, const int32_t w, const int32_t l, const int32_t total_quantity, int32_t samples_per_pixel, uint32_t used_quantity, const double* angles, const char* resultPath)
{
  if (slice >= l)
  {
    return;
  }

  int scanlineSize    = TIFFScanlineSize(tif);
  int stripSize       = TIFFStripSize(tif);
  int linesPerStrip   = stripSize / scanlineSize;
  int stripOfInterest = slice / linesPerStrip; // keep integer division here
  int offsetPixels    = slice % linesPerStrip * w;

  uint8_t* tiffBuffer      = (uint8_t*) _TIFFmalloc(stripSize); // buffer for a tiff image
  uint8_t* tiffStrip       = tiffBuffer + offsetPixels; // buffer for one strip of the image
  float** reconSlice       = (float**) malloc(used_quantity * sizeof(float*)); // handle to floats where we reconstruct each layer
  float* reconFinalSlice   = (float*) malloc(sizeof(float) *w*w); // ptr to the final reconstruction of each layer
  uint8_t* reconFinalChar  = (uint8_t*) malloc(sizeof(uint8) *w*w); // ptr to the char-cast final reconstruction of each layer
  int* normalisation       = (int*) malloc(sizeof(int) *w*w); // need to divide the contributions to each pixel by the normalisation value of that pixel


  // zero out the normalisation array
  long* normBuff = (long*) normalisation;
  for (int i = 0; i < w*w/2;++i)
  {
    *normBuff++ = 0;
  }

  int32_t selector = (total_quantity + used_quantity - 1) / used_quantity; // used to decide if we want each image or not based on roughly even spacing
  int32_t this_selection = 0;

  DFTI_DESCRIPTOR_HANDLE dftiDescriptor;
  MKL_LONG status;
  status = DftiCreateDescriptor(&dftiDescriptor, DFTI_SINGLE,
    DFTI_REAL, 1, 32);
  status = DftiCommitDescriptor(dftiDescriptor);

  for (int32_t i = 0; i < total_quantity; i++)
  {
    if (i % selector != 0)
    {
      continue;
    }
    double angle   = angles[i] * PI/180;
    double c_angle = cos(angle);
    double s_angle = sin(angle);

    TIFFSetDirectory(tif, i);
    TIFFReadEncodedStrip(tif, stripOfInterest, tiffBuffer, stripSize);

    reconSlice[this_selection] = (float*) malloc(sizeof(float) *w); // make a buffer to cast the slice to float
    float* fourierbuffer = reconSlice[this_selection++];

    for (int j = 0; j < scanlineSize; j++)
    {
      // process fourierbuffer[j] here, for example FFT / filter / IFFT
      //status = DftiComputeForward(dftiDescriptor, reconSlice);
      /* result is given in CCS format*/
      fourierbuffer[j] = (float) tiffStrip[j];
    }

    // backproject
    for (int j = 0; j < w; j++)
    {
      if (angle <= PI/4 || angle >= 3*PI/4)
      {
        double tan_angle = s_angle / c_angle;
        double sec_angle = 1 / c_angle;
        double offset = c_angle + s_angle*tan_angle;
        for (int y = 0; y < w; y++)
        {
          float x_f = tan_angle*(w / 2 - y) + w / 2 + (j - w / 2)*offset;

          int pixel_above = floor(0.5 + x_f);
          int pixel_below = pixel_above - 1;
          int current_pixel;
          if (pixel_above <= 0 || pixel_below >= w-1)
          {
            continue;
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
          reconFinalSlice[current_pixel] += fourierbuffer[j] * 1;
        }
      }
      else // angle between PI/4 and 3*PI/4
      {
        double cot_angle = c_angle / s_angle;
        double csc_angle = 1 / s_angle;
        double offset = c_angle * cot_angle + s_angle;

        for (int x = 0; x < w; x++)
        {
          float y_f = -cot_angle*(x - w / 2) + (j - w / 2) * offset +w/2;

          int pixel_above = floor(0.5 + y_f);
          int pixel_below = pixel_above - 1;
          int current_pixel;
          if (pixel_above <= 0 || pixel_below >= w-1)
          {
            continue;
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
          reconFinalSlice[current_pixel] += fourierbuffer[j] *1;
        }
      }
    }
  }

  for (int i = 0; i < w*w; i++) // normalise backprojection
  {
    if (normalisation[i] > 0)
      reconFinalChar[i] = (uint8_t) (reconFinalSlice[i] / normalisation[i]);
    else
      reconFinalChar[i] = (uint8_t) 0;
  }

  // write out the result to a tif
  char pathName[1024];
  strcpy_s(pathName, resultPath);
  strcat_s(pathName, "\\fbp.tif");
  TIFF* resultTif = makeTiffImage(pathName, w, w, samples_per_pixel);
  for (int i = 0; i < w; i++)
  {
    TIFFWriteScanline(resultTif, reconFinalChar + w*i, i, scanlineSize);
  }

  //clean up
  for (int j = 0; j < used_quantity; j++)
  {
    free(reconSlice[j]);
  }
  free(normalisation);
  free(reconFinalSlice);
  free(reconFinalChar);
  status = DftiFreeDescriptor(&dftiDescriptor);
  _TIFFfree(tiffBuffer);
  free(reconSlice);
  TIFFClose(resultTif);
}