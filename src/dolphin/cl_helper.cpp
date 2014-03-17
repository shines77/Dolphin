
#include <dolphin/cl_helper.h>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
//#include <string>
#include <vector>
#include <malloc.h>
#include <math.h>

#pragma comment(lib, "OpenCL.lib")
using namespace std;

#define DOL_TRACE(X)  std::cerr << X ;

/* use float or double ? */
#define USE_CL_FLOAT    1

#ifdef CL_FLOAT_T
#undef CL_FLOAT_T
#endif

#if defined(USE_CL_FLOAT) && (USE_CL_FLOAT != 0)
#define CL_FLOAT_T      cl_float
#else
#define CL_FLOAT_T      cl_double
#endif

//
// Matrix Multiplication 2 (OpenCL)
//
// http://gpgpu-computing4.blogspot.com/2009/09/matrix-multiplication-2-opencl.html
//

namespace dolphin {

cl_helper::cl_helper()
: m_bInitCL(false)
{
    /* do nothing! */
}

bool cl_helper::release(bool bForce /* = false */)
{
    if (!m_bInitCL && !bForce)
        return false;

    m_bInitCL = false;
    return true;
}

cl_int cl_helper::init_cl()
{
    if (m_bInitCL)
        return CL_SUCCESS;

    return CL_SUCCESS;
}

cl_int cl_helper::clLoadProgramSource(const char *file_name, const char **source, size_t *length)
{
    cl_int err = CL_SUCCESS;
    char *src_content = NULL;
    size_t src_length = 0;
    if (file_name == NULL) {
        err = CL_INVALID_VALUE;
        if (length)
            *length = src_length;
        return err;
    }

    std::ifstream ifs(file_name, std::ios_base::binary);
    if (!ifs.good()) {
        err = CL_OUT_OF_RESOURCES;
        goto CL_LOAD_PROGRAM_SOURCE_EXIT;
    }

    // get file length
    ifs.seekg(0, std::ios_base::end);
    src_length = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);

    if (source != NULL) {
        // read program source content
        src_content = new char[src_length + 1];
        if (src_content) {
            ifs.read(&src_content[0], src_length);
            src_content[src_length] = '\0';
        }
    }

    ifs.close();

CL_LOAD_PROGRAM_SOURCE_EXIT:
    if (source)
        *source = src_content;
    if (length)
        *length = src_length;
    return err;
}

cl_int cl_helper::run_native_matrix_mul(int m, int n)
{
    return CL_SUCCESS;
}

cl_int cl_helper::run_cl_cpu_matrix_mul(const char *file_name,
                                        const char *func_name,
                                        int m, int n)
{
    cl_int err = CL_SUCCESS;
    char *src_content = NULL;
    size_t src_length = 0;
    try {
        std::vector<cl::Platform> clPlatforms;
        cl::Platform::get(&clPlatforms);
        if (clPlatforms.size() == 0) {
            std::cout << "Platform size = 0" << std::endl;
            return -1;
        }

        cl_context_properties properties[] = {
            CL_CONTEXT_PLATFORM, (cl_context_properties)(clPlatforms[0])(), 0
        };
        cl_device_type device_type;
        device_type = CL_DEVICE_TYPE_DEFAULT;
        //device_type = CL_DEVICE_TYPE_CPU;
        cl::Context clContext(device_type, properties);

        std::vector<cl::Device> clDevices = clContext.getInfo<CL_CONTEXT_DEVICES>();

        err = clLoadProgramSource(file_name, (const char **)&src_content, &src_length);

        cl::Program::Sources clSource(1,
            std::make_pair(src_content, src_length));
        cl::Program clProgram = cl::Program(clContext, clSource);
        clProgram.build(clDevices);

        cl::Kernel clKernel(clProgram, func_name, &err);

        // set seed for rand()
        srand(1314UL);

        const int DATA_SIZE = 1048576;
        std::vector<CL_FLOAT_T> a(DATA_SIZE), b(DATA_SIZE), ret(DATA_SIZE);
        for (int i = 0; i < DATA_SIZE; ++i) {
            a[i]    = std::rand() / (CL_FLOAT_T)RAND_MAX;
            b[i]    = std::rand() / (CL_FLOAT_T)RAND_MAX;
            ret[i]  = 0.0;
        }

        // Allocate the buffer memory objects
        cl::Buffer cl_a  (clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_FLOAT_T) * DATA_SIZE, &a[0],                NULL);
        cl::Buffer cl_b  (clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_FLOAT_T) * DATA_SIZE, &b[0],                NULL);
        cl::Buffer cl_ret(clContext, CL_MEM_READ_WRITE,                       sizeof(CL_FLOAT_T) * DATA_SIZE, NULL,                 NULL);
        cl::Buffer cl_num(clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int),                 (void *)&DATA_SIZE,   NULL);

        // Set the args values
        clKernel.setArg(0, sizeof(cl_mem), &cl_a);
        clKernel.setArg(1, sizeof(cl_mem), &cl_b);
        clKernel.setArg(2, sizeof(cl_mem), &cl_ret);
        clKernel.setArg(3, sizeof(cl_mem), &cl_num);

        // Set work-item dimensions and execute kernel
        cl::Event clEvent;
        cl::CommandQueue clCmdQueue(clContext, clDevices[0], 0, &err);
        sw_kernel.start();
        err = clCmdQueue.enqueueNDRangeKernel(
            clKernel,                   // kernel
            cl::NullRange,              // global_work_offset
            cl::NDRange(DATA_SIZE),     // global_work_size
            cl::NullRange,              // local_work_size
            NULL,                       // event_wait_list
            //&clEvent                    // event
            NULL                        // event
            );

        //clEvent.wait();
        sw_kernel.stop();

        if (err == CL_SUCCESS) {
            sw_kernel_readBuffer.start();
            err = clCmdQueue.enqueueReadBuffer(cl_ret, CL_TRUE, 0, sizeof(CL_FLOAT_T) * DATA_SIZE, &ret[0], NULL, NULL);
            sw_kernel_readBuffer.stop();
            if (err == CL_SUCCESS) {
                bool correct = true;
                CL_FLOAT_T diff;
                for (int i = 0; i < DATA_SIZE; ++i) {
                    diff = a[i] + b[i] - ret[i];
                    if (fabs(diff) > 0.0001) {
                        correct = false;
                        break;
                    }
                }

                if (correct)
                    std::cout << "Data is correct" << endl;
                else
                    std::cout << "Data is incorrect" << endl;
            }
            else {
                std::cerr << "Can't run kernel or read back data" << endl;
            }
        }
    }
    catch (cl::Error clErr) {
        std::cerr
            << "ERROR: "
            << clErr.what()
            << "("
            << clErr.err()
            << ")"
            << std::endl;
        err = clErr.err();
    }

    if (src_content) {
        delete[] src_content;
        src_content = NULL;
    }

    return err;
}

cl_int cl_helper::run_cl_gpu_matrix_mul(const char *file_name,
                                        const char *func_name,
                                        int m, int n)
{
    return CL_SUCCESS;
}

double cl_helper::getSeconds()
{
    return sw_kernel.getSeconds();
}

double cl_helper::getMillisec()
{
    return sw_kernel.getMillisec();
}

double cl_helper::getTotalMillisec()
{
    return sw_kernel.getTotalMillisec();
}

double cl_helper::getMillisec_Native()
{
    return sw_native.getMillisec();
}

double cl_helper::getMillisec_Native_CopyData()
{
    return sw_native_copyData.getMillisec();
}

double cl_helper::getMillisec_Kernel_ReadBuffer()
{
    return sw_kernel_readBuffer.getMillisec();
}

}  // namespace dolphin
