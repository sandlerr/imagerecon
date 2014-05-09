// atlas-runner.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <memory>
#include <stdexcept> 
#include <fstream>
#include <iostream>
#include <atlas_imp.h>

// typedef for nspTROptimise() routine
typedef void(*SolverObjectiveFunction)(int *nResidualsCount, int *nParamCount, double *x, double *f, void* pThis);

// Intel MKL Optimization Solvers Routines - nonlinear least square problem with optional boundary constraints - using Trust-Region (TR) algorithms.
// mklTROptimise() returns true if norm of final residuals is less than required precision, false otherwise.
// mklTROptimise throws std::runtime_error if MKL reported an error.
// Use mklSolver from icstat as a wrapper around this method
bool mklTROptimise(
  int nParamCount, double* pParams,				// number of parameters and parameters (solution) vector to optimise
  double* pLowBound,								// low bound constraints for each parameter, pass NULL if no constraints
  double* pUpperBound,								// upper bound constraints for each parameter, pass NULL if no constraints
  int nResidualsCount,								// number of residuals
  SolverObjectiveFunction pObjectiveFunction,		// objective function, calculates residuals as function of parameters
  void* pThis,										// opaque pointer to pass into pObjectiveFunction
  int iter1,										// maximum number of iterations
  int iter2,										// maximum number of iterations of calculation of trial-step
  double* eps,										// precisions for stop-criteria
  double rs,										// initial step bound
  int& iter,										// number of iterations - [out] parameter 
  int& st_cr,										// number of stop-criterion - [out] parameter
  double& r1, double& r2)							// initial and final residuals - [out] parameters
{
  //PRECONDITION((pLowBound && pUpperBound) || (!pLowBound && !pUpperBound));
  bool bConstraints = (pLowBound && pUpperBound);

  double* fvec = new double[nResidualsCount];					// function (f(pParams)) value vector, size of nResidualsCount
  std::auto_ptr<double> fvecAP(fvec);

  double* fjac = new double[nResidualsCount * nParamCount];	// jacobi matrix, nResidualsCount * nParamCount
  std::auto_ptr<double> fjacAP(fjac);

  std::fill(fvec, fvec + nResidualsCount, 0.);
  std::fill(fjac, fjac + nResidualsCount*nParamCount, 0.);

  _TRNSP_HANDLE_t handle; // TR solver handle
  MKL_INT nRes = bConstraints ? dtrnlspbc_init(&handle, &nParamCount, &nResidualsCount, pParams, pLowBound, pUpperBound, eps, &iter1, &iter2, &rs) :
    dtrnlsp_init(&handle, &nParamCount, &nResidualsCount, pParams, eps, &iter1, &iter2, &rs);
  if (nRes != TR_SUCCESS)
  {
    throw std::runtime_error("error in mkl - dtrnlsp_init()");
  }

  int RCI_Request = 0;	// reverse communication interface variable
  bool bLastRetCodeSuccess = false;
  while (true)	// rci cycle 
  {
    nRes = bConstraints ? dtrnlspbc_solve(&handle, fvec, fjac, &RCI_Request) : dtrnlsp_solve(&handle, fvec, fjac, &RCI_Request);
    if (nRes != TR_SUCCESS)
    {
      nRes = bConstraints ? dtrnlspbc_delete(&handle) : dtrnlsp_delete(&handle);
      throw std::runtime_error("error in mkl - dtrnlsp_solve()");
    }
    // according to rci_request value we do next step 
    if (RCI_Request == -1 ||		// the algorithm has exceeded the maximal number of iterations
      RCI_Request == -2 ||		// delta(k) < eps(1)
      RCI_Request == -3 ||		// ||F(pParams)|| < eps(2)
      RCI_Request == -4 ||		// ||A(pParams)ij|| < eps(3)
      RCI_Request == -5 ||		// ||s|| < eps(4)
      RCI_Request == -6)			// ||F(pParams)||- ||F(pParams) - A(pParams)s|| < eps(5)
    {
      break;		// exit rci cycle 
    }
    if (RCI_Request == 0)			// success
    {
      if (bLastRetCodeSuccess)	// exit on the second consequitive success
      {
        break;
      }
      else
      {
        bLastRetCodeSuccess = true;	// do not exit on the first success
        continue;
      }
    }
    bLastRetCodeSuccess = false;
    if (RCI_Request == 1)	// recalculate the function at vector X and put the result into fvec, fvec = f(pParams)
    {
      (*pObjectiveFunction)(&nResidualsCount, &nParamCount, pParams, fvec, pThis);
      continue;
    }
    if (RCI_Request == 2)	// calculate the Jacobian matrix and put the result into fjac
    {
      if (djacobix(pObjectiveFunction, &nParamCount, &nResidualsCount, fjac, pParams, eps, pThis) != TR_SUCCESS)
      {
        nRes = bConstraints ? dtrnlspbc_delete(&handle) : dtrnlsp_delete(&handle);
        throw std::runtime_error("error in mkl - djacobi()");
      }
      continue;
    }
   // PRECONDITION(false);		// must never happen
  }

  nRes = bConstraints ? dtrnlspbc_get(&handle, &iter, &st_cr, &r1, &r2) : dtrnlsp_get(&handle, &iter, &st_cr, &r1, &r2);
  if (nRes != TR_SUCCESS)		// get solution statuses
  {
    nRes = bConstraints ? dtrnlspbc_delete(&handle) : dtrnlsp_delete(&handle);
    throw std::runtime_error("error in mkl - dtrnlsp_get()");
  }

  nRes = bConstraints ? dtrnlspbc_delete(&handle) : dtrnlsp_delete(&handle);

  return (r2 < eps[1]);
}

class  mklSolver
{
private:
  // Assignment operator, declared (to prevent compiler from generating it) but not implemented. 
  mklSolver& operator=(const mklSolver& other);
public:
  mklSolver();
  virtual ~mklSolver(){};
  // Methods to set optimisation parameters
  // According to MKL manual, there are 6 epsilons used for optimisation stop criteria,
  //	eps(1): delta(k) < eps(1)
  //  eps(2): ||F(x)|| < eps(2)
  //  eps(3): ||A(x)ij|| < eps(3)
  //  eps(4): ||s|| < eps(4)
  //  eps(5): ||F(x)||- ||F(x) - A(x)s|| < eps(5)
  //	eps(6): trial step precision. If eps(6) = 0, then eps(6) = 1.d-10,
  // where A is a Jacobi matrix.
  void SetEpsilon(int nOrdinal, double r);		// nOrdinal should be between 1 and 6
  double GetEpsilon(int nOrdinal) const;
  void SetMaxIter(int nIter) { m_nMaxIter = nIter; };
  int GetMaxIter() const { return m_nMaxIter; };
  void SetMaxTrialStepIter(int nIter) { m_nMaxIterTrialStep = nIter; };
  int GetMaxTrialStepIter() const { return m_nMaxIterTrialStep; };
  void SetInitialStepBound(double r);		// initial step bound must be between 0.1 and 100
  double GetInitialStepBound() const { return m_rs; };

  // Methods to get optimisation results
  int GetActualIter() const { return m_nActualIter; };
  double GetInitialResiduals() const { return m_rInitialResiduals; };// sqrt of initial residuals sum of squares
  double GetFinalResiduals() const { return m_rFinalResiduals; };// sqrt of final residuals sum of squares
  // According to MKL manual, the actual stop criterion is one of the following, 
  //  st_cr = 1 - the algorithm has exceeded the maximal number of iterations.
  //  st_cr = 2 - delta(k) < eps(1)
  //  st_cr = 3 - ||F(x)|| < eps(2)
  //  st_cr = 4 - ||A(x)ij|| < eps(3)
  //  st_cr = 5 - ||s|| < eps(4)
  //  st_cr = 6 - ||F(x)||- ||F(x) - A(x)s|| < eps(5),
  // where A is a Jacobi matrix.
  int GetStopCriterion() const { return m_nStopCriterion; };

  // Run optimisation method. Returns true if norm of final residuals is less than required precision eps(2), false otherwise
  bool Solve(int nParamCount,		// number of parameters and parameters
    double* pParams,		// parameters vector to optimise, in/out parameter
    double* pLowBound,	// low bound constraints for each parameter, pass NULL if no constraints
    double* pUpperBound,	// upper bound constraints for each parameter, pass NULL if no constraints
    int nResidualsCount,	// number of residuals
    void* pThis,			// will be passed unchanged to pObjectiveFunction callback
    SolverObjectiveFunction pObjectiveFunction); // objective function, calculates residuals as function of parameters

private:

  double m_vecEpsilon[6];
  int m_nMaxIter;
  int m_nMaxIterTrialStep;
  double m_rs;
  int m_nActualIter;
  double m_rInitialResiduals;
  double m_rFinalResiduals;
  int m_nStopCriterion;
};

mklSolver::mklSolver() : m_nMaxIter(1000), m_nMaxIterTrialStep(100), m_rs(10.),
m_nActualIter(0), m_rInitialResiduals(0.), m_rFinalResiduals(0.), m_nStopCriterion(0)
{
  std::fill(m_vecEpsilon, m_vecEpsilon + 6, 0.000001);
}

void mklSolver::SetEpsilon(int nOrdinal, double r)
{
  //PRECONDITION((nOrdinal >= 1) && (nOrdinal <= 6));
  m_vecEpsilon[nOrdinal - 1] = r;
}

double mklSolver::GetEpsilon(int nOrdinal) const
{
 // PRECONDITION((nOrdinal >= 1) && (nOrdinal <= 6));
  return m_vecEpsilon[nOrdinal - 1];
}

void mklSolver::SetInitialStepBound(double r)
{
 // PRECONDITION((r >= 0.1) && (r <= 100.));
  m_rs = r;
}

bool mklSolver::Solve(int nParamCount, double* pParams,
  double* pLowBound, double* pUpperBound,
  int nResidualsCount,
  void* pThis,
  SolverObjectiveFunction pObjectiveFunction)
{
  return mklTROptimise(nParamCount, pParams, pLowBound, pUpperBound, nResidualsCount, pObjectiveFunction, pThis,
    m_nMaxIter, m_nMaxIterTrialStep, m_vecEpsilon, m_rs,
    m_nActualIter, m_nStopCriterion, m_rInitialResiduals, m_rFinalResiduals);
}

class lrtSelRecImpl
{
public:
  lrtSelRecImpl();
  void Calculate3ParSmoothingRes(int* numOfResiduals, int* numOfParam, double* params, double* residuals);
  bool lrtSelRecImpl::Optimise3Par();
private:
  int m_m = 5;	// number of residuals
  double m_obs[5];
};

lrtSelRecImpl::lrtSelRecImpl()
{
  m_m = 5;
  m_obs[0] = 10;  m_obs[1] = 11; m_obs[2] = 21; m_obs[3] = 33; m_obs[4] = 40;
}

void Optimise3ParObjectiveFunction(int *m, int *n, double *x, double *f, void* pThis)
{
  //if (typeid(pThis) != typeid(lrtSelRecImpl*))
  //{
  //	throw std::runtime_error("wrong pThis");
  //}
  lrtSelRecImpl* pLrtSelRecImpl = reinterpret_cast<lrtSelRecImpl*>(pThis);
  pLrtSelRecImpl->Calculate3ParSmoothingRes(m, n, x, f);
}

void lrtSelRecImpl::Calculate3ParSmoothingRes(int* numOfResiduals, int* numOfParam, double* params, double* residuals)
{
  //PRECONDITION (*numOfParam == 3);
  for (int i = 0; i < m_m; i++)
  {
    residuals[i] = m_obs[i] - (params[0] + params[1] * i + params[2] * (i*i));
  }
}

bool lrtSelRecImpl::Optimise3Par()
{
  double x[3];		// solution vector. contains values x for f(x), 3 values
  x[0] = 1;
  x[1] = 1;
  x[2] = 1;

  mklSolver* pSolver = new mklSolver;
  std::auto_ptr<mklSolver> pSolverAP(pSolver);

  pSolver->SetMaxIter(1000);
  pSolver->SetMaxTrialStepIter(100);
  pSolver->SetInitialStepBound(1.);	// Do not set it above 10.
  for (int i = 1; i <= 6; i++)
  {
    pSolver->SetEpsilon(i, 1E-12);	// note, if mkl solver with constraints is used, too small eps will prevent convergence 
  }									// do not set it below 1e-12

  double LW[3] = { -std::numeric_limits<double>::max(), std::numeric_limits<double>::min(), -std::numeric_limits<double>::max() };
  double UP[3] = { std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max() };

  bool b = pSolver->Solve(3, x, LW, UP, m_m, this, Optimise3ParObjectiveFunction);

  // x contains the optimal parameters

  std::ofstream ofs;
  int n = 3;
  ofs.open("D:\\work\\uni\\FYP\\reconrepo\\imagereconstruction\\x64\\Debug\\data.bin", std::ios_base::binary | std::ios_base::trunc);
  if (!ofs.is_open())
  {
    std::cout << "error in open" << std::endl;
    return -1;
  }
  else
  {
    ofs.write((char*) (void*) &n, sizeof(n));
    //ofs.write((char*)(void*)x, sizeof(x));  // if x is a pointer then use ofs.write((char*)(void*)x, sizeof(float)*n);
    ofs.write((char*) (void*) x, sizeof(double) * n);
    ofs.close();
  }

  std::ifstream ifs;
  ifs.open("D:\\work\\uni\\FYP\\reconrepo\\imagereconstruction\\x64\\Debug\\data.bin", std::ios_base::binary);
  if (ifs.fail())
  {
    std::cout << "error in open" << std::endl;
    return -1;
  }
  else
  {
    x[0] = 1;
    x[1] = 1;
    x[2] = 1;
    n = 0;
    ifs.read((char*) (void*) &n, sizeof(n));
    //ifs.read((char*) (void*) x, sizeof(x)); // if x is a pointer then use ifs.read((char*)(void*)x, sizeof(float)*n);
    ifs.read((char*) (void*) x, sizeof(double) *n);
    ifs.close();
  }


  return b;
}

int _tmain(int argc, _TCHAR* argv[])
{
  lrtSelRecImpl mySelRec;
  mySelRec.Optimise3Par();



 /* float* object = NULL;
  const int xshape = 1;
  const int yshape = 1;
  float* sampleProjection = NULL;
  int n = 1;
  float* transformationMatrix = NULL;
  float* lowerBound = NULL;
  float* upperBound = NULL;
  const int* centre = NULL;
  int* iter = NULL;
  int* st_cr = NULL;
  float* r1 = NULL;
  float* r2 = NULL;
  match_atlas(object, xshape, yshape, sampleProjection, n, transformationMatrix, lowerBound, upperBound, centre, iter, st_cr, r1, r2);*/
  return 0;

}

