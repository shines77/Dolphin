
#ifndef _DOL_OPENCL_HELPER_H_
#define _DOL_OPENCL_HELPER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#define __CL_ENABLE_EXCEPTIONS
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
//#define __NO_STD_STRING

#ifdef __APPLE__
//#include <OpenCL/opencl.h>
#include <OpenCL/opencl.hpp>
#else
//#include <CL/cl.h>
#include <CL/cl.hpp>
#endif

#include <dolphin/dol_stddef.h>
#include <gmtl/stop_watch.h>
#include <string>

//using namespace cl;
using namespace std;
using namespace gmtl;

#define CL_DEVICE_TYPE_NATIVE       (1 << 15)

/* use float or double ? */
#define CL_HELPER_USE_FLOAT     1

#ifdef CL_FLOAT_T
#undef CL_FLOAT_T
#endif

#if defined(CL_HELPER_USE_FLOAT) && (CL_HELPER_USE_FLOAT != 0)
#define CL_FLOAT_T      cl_float
#else
#define CL_FLOAT_T      cl_double
#endif

namespace dolphin {

typedef enum cl_runat_type
{
    CL_RUNAT_DEFAULT        = CL_DEVICE_TYPE_DEFAULT,
    CL_RUNAT_CPU            = CL_DEVICE_TYPE_CPU, 
    CL_RUNAT_GPU            = CL_DEVICE_TYPE_GPU,
    CL_RUNAT_ACCELERATOR    = CL_DEVICE_TYPE_ACCELERATOR,
    CL_RUNAT_CUSTOM         = CL_DEVICE_TYPE_CUSTOM,
    CL_RUNAT_NATIVE         = CL_DEVICE_TYPE_NATIVE
} cl_runat_type;

class cl_helper
{
public:
    cl_helper();
    ~cl_helper() { release(); };

public:
    cl_int  init_cl();
    bool    release(bool bForce = false);

    bool    use_double();
    cl_int  run_cl_matrix_mul(cl_runat_type device_type,
                const char *file_name,
                const char *func_name,
                const unsigned int m,
                const unsigned int p,
                const unsigned int n);
    cl_int  run_native_matrix_mul(const int m, const int p, const int n);
    cl_int  run_cl_gpu_matrix_mul(const cl_device_type device_type,
                const char *file_name,
                const char *func_name,
                const unsigned int m,
                const unsigned int p,
                const unsigned int n);

    cl_int  matrix_mul_verify(const cl_int err,
                std::vector<CL_FLOAT_T> &A,
                std::vector<CL_FLOAT_T> &B,
                std::vector<CL_FLOAT_T> &C,
                const unsigned int m,
                const unsigned int p,
                const unsigned int n);

    cl_int  run_cl_vector_add(cl_runat_type device_type,
                const char *file_name,
                const char *func_name,
                const unsigned int data_size);
    cl_int  run_native_vector_add(const unsigned int data_size);
    cl_int  run_cl_gpu_vector_add(cl_device_type device_type,
                const char *file_name,
                const char *func_name,
                const unsigned int data_size);

    cl_int  vector_add_verify(const cl_int err,
                std::vector<CL_FLOAT_T> &a,
                std::vector<CL_FLOAT_T> &b,
                std::vector<CL_FLOAT_T> &result,
                const unsigned int data_size);

    void    reset_stopwatches();

    double  getSeconds();
    double  getMillisec();
    double  getTotalMillisec();

    double  getMillisec_Native();
    double  getMillisec_Native_CopyData();
    double  getMillisec_Kernel_ReadBuffer();

protected:
    cl_int clLoadProgramSource(const char *file_name, const char **content, size_t *length);
    cl_int clLoadProgramSource(const char *file_name, std::string &content, size_t &length);

private:
    bool        m_bInitCL;

    stop_watch  sw_kernel, sw_kernel_readBuffer;
    stop_watch  sw_native, sw_native_copyData;
};

}  // namespace dolphin

#endif  /* _DOL_OPENCL_HELPER_H_ */
