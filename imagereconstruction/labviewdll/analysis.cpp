#include "stdafx.h"
#include "mkl.h"
#include "analysis.h"

_declspec (dllexport) int correlation(const int arraySize, float* x, float* y, float* z)
{
  VSLCorrTaskPtr task;
  int status = vslsCorrNewTask1D(&task, VSL_CORR_MODE_AUTO, arraySize, arraySize, 1);
  if (status != VSL_STATUS_OK)
  {
    return status;
  }
  status = vslsCorrExec1D(task, x, 1, y, 1, z, 1);
  if (status != VSL_STATUS_OK)
  {
    return status;
  }
  int errcode = vslCorrDeleteTask(&task);
  if (errcode != 0)
  {
    return errcode;
  }
}