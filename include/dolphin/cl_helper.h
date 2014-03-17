
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

//using namespace cl;
using namespace gmtl;

namespace dolphin {

class cl_helper
{
public:
    cl_helper();
    ~cl_helper() { release(); };

public:
    cl_int  init_cl();
    bool    release(bool bForce = false);

    cl_int  run_native_matrix_mul(int m, int n);
    cl_int  run_cl_cpu_matrix_mul(const char *file_name,
        const char *func_name, int m, int n);
    cl_int  run_cl_gpu_matrix_mul(const char *file_name,
        const char *func_name, int m, int n);

    double  getSeconds();
    double  getMillisec();
    double  getTotalMillisec();

    double  getMillisec_Native();
    double  getMillisec_Native_CopyData();
    double  getMillisec_Kernel_ReadBuffer();

protected:
    cl_int clLoadProgramSource(const char *file_name, const char **source, size_t *length);
    
private:
    bool        m_bInitCL;

    stop_watch  sw_kernel, sw_kernel_readBuffer;
    stop_watch  sw_native, sw_native_copyData;
};

}  // namespace dolphin

#endif  /* _DOL_OPENCL_HELPER_H_ */
