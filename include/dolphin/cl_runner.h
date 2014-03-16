
#ifndef _DOL_OPENCL_RUNNER_H_
#define _DOL_OPENCL_RUNNER_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif

#include <dolphin/dol_stddef.h>
#include <gmtl/stop_watch.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
//#include <CL/opencl.h>
#endif

using namespace gmtl;

//
// OpenCLÊ¹ÓÃGPUÂË²¨
//
// From: http://blog.csdn.net/iddialog/article/details/8090372
//

namespace dolphin {

class cl_runner
{
    //
public:
    cl_runner();
    ~cl_runner() { release(); };

public:
    cl_int  init_cl();
    bool    release(bool bForce = false);

    cl_int  compile(const char *filename);
    cl_int  execute(const char *filename);

    double  native_test();

    double  getSeconds();
    double  getMillisec();
    double  getTotalMillisec();

    double  getMillisec_Native();
    double  getMillisec_Native_CopyData();
    double  getMillisec_Kernel_ReadBuffer();

protected:
    cl_int  clLoadProgramSource(const char *filename,
        const char **source, size_t *length);
private:
    bool m_bInitCL;

    cl_platform_id      m_clPlatformId;
    cl_device_id        m_clDeviceId;
    cl_context          m_clContext;
    cl_command_queue    m_clCmdQueue;
    cl_program          m_clProgram;

    cl_kernel           m_clKernel, m_clKernel1;

    stop_watch          sw_kernel, sw_kernel_readBuffer;
    stop_watch          sw_native, sw_native_copyData;
};

}  // namespace dolphin

#endif  /* _DOL_OPENCL_RUNNER_H_ */
