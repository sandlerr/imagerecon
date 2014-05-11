// atlas-runner.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <atlas_imp.h>


int _tmain(int argc, _TCHAR* argv[])
{
  int xshape = NULL;
  int yshape = NULL;
  float* object = NULL;
  int objectLength = NULL;
  float* sampleProjection = NULL;
  int sampleLength = NULL;
  float* transformationMatrix = NULL;
  float* lowerBound = NULL;
  float* upperBound = NULL;
  int n = NULL;
  int* centre = NULL;
  int centreLength = NULL;
  int iter_v;
  int st_cr_v;
  float r1_v;
  float r2_v;

  std::ifstream ifs;
  ifs.open("D:\\work\\uni\\FYP\\workingData\\data.bin", std::ios_base::binary);
  if (ifs.fail())
  {
    std::cout << "error in open" << std::endl;
    return -1;
  }
  else
  {
    ifs.read((char*) (void*) &xshape, sizeof(xshape));
    ifs.read((char*) (void*) &yshape, sizeof(yshape));

    objectLength = xshape * yshape;
    object = (float*) malloc(sizeof(float) * objectLength);
    if (object == NULL)
    {
      std::cout << "Memory allocation error" << std::endl;
      return -1;
    }
    ifs.read((char*) (void*) object, sizeof(float) * objectLength);

    sampleLength = yshape;
    sampleProjection = (float*) malloc(sizeof(float) * sampleLength);
    if (sampleProjection == NULL)
    {
      std::cout << "Memory allocation error" << std::endl;
      free(object);
      return -1;
    }
    ifs.read((char*) (void*) sampleProjection, sizeof(float) * sampleLength);

    ifs.read((char*) (void*) &n, sizeof(n));
    transformationMatrix = (float*) malloc(sizeof(float) * n);
    lowerBound = (float*) malloc(sizeof(float) * n);
    upperBound = (float*) malloc(sizeof(float) * n);
    if (transformationMatrix == NULL || lowerBound == NULL || upperBound == NULL)
    {
      std::cout << "Memory allocation error" << std::endl;
      free(object);
      free(sampleProjection);
      free(transformationMatrix);
      free(lowerBound);
      return -1;
    }
    ifs.read((char*) (void*) transformationMatrix, sizeof(float) * n);
    ifs.read((char*) (void*) lowerBound, sizeof(float) * n);
    ifs.read((char*) (void*) upperBound, sizeof(float) * n);

    ifs.read((char*) (void*) &centreLength, sizeof(centreLength));
    centre = (int*) malloc(sizeof(int) * centreLength);
    if (centre == NULL)
    {
      std::cout << "Memory allocation error" << std::endl;
      free(object);
      free(sampleProjection);
      free(transformationMatrix);
      free(lowerBound);
      free(upperBound);
      return -1;
    }
    ifs.read((char*) (void*) centre, sizeof(int) * centreLength);

    ifs.read((char*) (void*) &iter_v, sizeof(int));
    ifs.read((char*) (void*) &st_cr_v, sizeof(int));
    ifs.read((char*) (void*) &r1_v, sizeof(float));
    ifs.read((char*) (void*) &r2_v, sizeof(float));
    ifs.close();
  }

  match_atlas(object, xshape, yshape, sampleProjection, n, transformationMatrix, lowerBound, upperBound, centre, &iter_v, &st_cr_v, &r1_v, &r2_v);

  /*std::ofstream ofs;
  ofs.open("D:\\work\\uni\\FYP\\workingData\\results.bin", std::ios_base::binary | std::ios_base::trunc);
  if (!ofs.is_open())
  {
    std::cout << "error in open" << std::endl;
    return -1;
  }
  else
  {

    ofs.write((char*) (void*) &xshape, sizeof(xshape));
    ofs.write((char*) (void*) &yshape, sizeof(yshape));

  }*/

  std::cout << std::fixed << (transformationMatrix[0] * 180 / 3.14159) << std::endl;
  std::cout << transformationMatrix[1] << std::endl;
  std::cout << transformationMatrix[2] << std::endl;

  return 0;

}

