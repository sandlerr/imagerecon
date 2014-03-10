// FFT runner.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "mkl_dfti.h"
#include <iostream>
void fourier_analysis();

int _tmain(int argc, _TCHAR* argv[])
{
  printf("Hello world!\n");
  fourier_analysis();
  std::cout <<"Goodbye world\n";
	return 0;
}

void fourier_analysis()
{
  float y[34];
  DFTI_DESCRIPTOR_HANDLE my_desc_handle;
  MKL_LONG status;
  for (int i = 0; i < 32; i++)
  {
    y[i] = (float)rand();
    printf("Y Value: %f\n", y[i]);
  }
  //...put input data into x[0],...,x[31]; y[0],...,y[31]
  status = DftiCreateDescriptor(&my_desc_handle, DFTI_SINGLE,
    DFTI_REAL, 1, 32);
  status = DftiCommitDescriptor(my_desc_handle);
  status = DftiComputeForward(my_desc_handle, y);
  status = DftiFreeDescriptor(&my_desc_handle);
  /* result is given in CCS format*/
  for (float &i : y) {
    printf("FFT Value: %f\n", i);
  }
}