
#include <dolphin/cl_runner.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#pragma comment(lib, "OpenCL.lib")
#if !defined(CL_VERSION_1_1) && !defined(CL_VERSION_1_2)
#pragma comment(lib, "aticalcl.lib")
#pragma comment(lib, "aticalrt.lib")
#endif

using namespace std;

//
// 可变参数及可变参数宏的使用
//
// 参考: http://www.cnblogs.com/caosiyang/archive/2012/08/21/2648870.html
// 参考: http://hi.baidu.com/dnboy/item/db70e731d79192352e20c4d8
//
#ifndef DOL_TRACE
#if defined(__GNUC__)
#ifndef __cplusplus
#define DOL_TRACE(fmt, args...) printf(fmt, args)
#else
#define DOL_TRACE(fmt, ...)     printf(fmt, ##__VA_ARGS__)
#endif
#define DOL_TRACE1              DOL_TRACE
#elif defined(_MSC_VER)
#define DOL_TRACE(fmt, ...)     printf(fmt, __VA_ARGS__)
#define DOL_TRACE1              DOL_TRACE
#else
#define DOL_TRACE(fmt)          std::cerr << fmt
#define DOL_TRACE1(fmt, args)   std::cerr << fmt << args
#endif
#endif

/* use double or float ? */
#define USE_CL_DOUBLE   0

#ifdef CL_FLOAT_T
#undef CL_FLOAT_T
#endif

#if defined(USE_CL_DOUBLE) && (USE_CL_DOUBLE != 0)
#define CL_FLOAT_T      cl_double
#else
#define CL_FLOAT_T      cl_float
#endif

//
// OpenCL 教學（一）
//
// 参考: http://www.kimicat.com/opencl-1/opencl-jiao-xue-yi
//
//       http://www.amd.com/cn/products/technologies/stream-technology/opencl/pages/opencl-intro.aspx
//

/******************************************************************************

  http://www.techpowerup.com/forums/threads/no-opencl-support-whyyyy.115926/

  Old thread but a high ranked page on Google so I'm going to reply.

  Open CL support was dropped by AMD for the Raedon HD 4xxx series with about the 12.8 drivers.
  (They baselined on OpenCL 1.1 which these cards don't support). Therefore if you want to use
  a 4xxx series card you need to use the version 12.4 drivers as they contain OpenCL 1.0 beta
  drivers (for bitcoin, folding at home etc).

******************************************************************************/

/******************************************************************************

  http://setiweb.ssl.berkeley.edu/beta/forum_thread.php?id=1964&sort_style=&start=0

  The app is fully OpenCL 1.0 compatible.
  I tested on HD 4770 (RV 770) HD 4850 with driver 12.1.
  Work group size 256 required.
  I suggest driver version 12.1. and newer.
  Running Windows XP driver 12.1 is the last driver with OpenCL support.

  Mike

******************************************************************************/

namespace dolphin {

cl_runner::cl_runner()
: m_bInitCL(false)
, m_clPlatformId(NULL)
, m_clDeviceId(NULL)
, m_clContext(NULL)
, m_clCmdQueue(NULL)
, m_clProgram(NULL)
, m_clKernel(NULL)
, m_clKernel1(NULL)
{
    /* do nothing! */
}

bool cl_runner::release(bool bForce /* = false */)
{
    if (!m_bInitCL && !bForce)
        return false;

    if (m_clKernel) {
        clReleaseKernel(m_clKernel);
        m_clKernel = NULL;
    }
    if (m_clKernel1) {
        clReleaseKernel(m_clKernel1);
        m_clKernel1 = NULL;
    }
    if (m_clProgram) {
        clReleaseProgram(m_clProgram);
        m_clProgram = NULL;
    }

    if (m_clCmdQueue) {
        clReleaseCommandQueue(m_clCmdQueue);
        m_clCmdQueue = NULL;
    }
    if (m_clContext) {
        clReleaseContext(m_clContext);
        m_clContext = NULL;
    }

    if (m_clDeviceId) {
#if defined(CL_VERSION_1_1) || defined(CL_VERSION_1_2)
        clReleaseDevice(m_clDeviceId);
#endif
        m_clDeviceId = NULL;
    }

    m_bInitCL = false;
    return true;
}

#define MAX_OPENCL_PLATFORMS    8
#define MAX_COPROC_INSTANCES    8

cl_int cl_runner::init_cl()
{
    if (m_bInitCL)
        return CL_SUCCESS;

    // Error code
    cl_int err_num = CL_SUCCESS;
    cl_uint num_platforms = 0;
    size_t i;

    // Get the number of PlatformIDs
    // /* Additional Error Codes */
    // CL_PLATFORM_NOT_FOUND_KHR    -1001
    err_num = clGetPlatformIDs(0, 0, &num_platforms);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE("cl_runner: Unable to get platforms \n");
        //std::cerr << "Unable to get platforms" << endl;
        return err_num;
    }

    // Get the PlatformIDs
    std::vector<cl_platform_id> platformIds(num_platforms + 1);
    for (i = 0; i < num_platforms; ++i)
        platformIds[i] = NULL;

    // err_num = clGetPlatformIDs(num_platforms, &m_clPlatformId, &num_platforms);
    err_num = clGetPlatformIDs(num_platforms, &platformIds[0], &num_platforms);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE("cl_runner: Error getting platform id \n");
        return err_num;
    }

    if (num_platforms > 0) {
        char pbuf[256];
        for (i = 0; i < num_platforms; ++i) {
            err_num = clGetPlatformInfo(platformIds[i],
                CL_PLATFORM_VENDOR, sizeof(pbuf), pbuf, NULL);
            if (err_num != CL_SUCCESS) {
                DOL_TRACE("Error getting platform vendor info \n");
                return err_num;
            }
            m_clPlatformId = platformIds[i];
            if (!strcmp(pbuf, "Advanced Micro Devices, Inc.")) {
                printf("cl_runner: Find platform: %s\n\n", pbuf);
                break;
            }
            else
                printf("cl_runner: Find unknown platform: %s\n\n", pbuf);
        }
    }
    else {
        printf("cl_runner: num_platforms  = %d\n\n", num_platforms);
    }
    //m_clPlatformId = platformIds[0];

#if 0
    cl_device_id deviceIDs[MAX_COPROC_INSTANCES] = { NULL };
    size_t num_devices = 0;

    // Get the DeviceIDs
    // #define CL_DEVICE_NOT_FOUND      -1

    //err_num = clGetDeviceIDs(m_clPlatformId, CL_DEVICE_TYPE_GPU, 1, &m_clDeviceId, NULL);
    err_num = clGetDeviceIDs(m_clPlatformId, CL_DEVICE_TYPE_DEFAULT, 1, &m_clDeviceId, NULL);
    //err_num = clGetDeviceIDs(m_clPlatformId, CL_DEVICE_TYPE_ACCELERATOR, 1, &m_clDeviceId, NULL);
    //err_num = clGetDeviceIDs(m_clPlatformId, CL_DEVICE_TYPE_DEFAULT, MAX_COPROC_INSTANCES, deviceIDs, &num_devices);
    //m_clDeviceId = deviceIDs[0];
    if (err_num != CL_SUCCESS) {
        DOL_TRACE("cl_runner: Error getting device ids \n");
        return err_num;
    }

    // Create the Context
    m_clContext = clCreateContext(0, 1, &m_clDeviceId, NULL, NULL, &err_num);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE("cl_runner: Error creating context \n");
        return err_num;
    }
#else
    /*
     * If we could find our platform, use it.
     * Otherwise use just available platform.
     */
    cl_context_properties cps[3] = {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)m_clPlatformId,
        0
    };

    // Create the Context
    m_clContext = clCreateContextFromType(cps,
                    CL_DEVICE_TYPE_DEFAULT,
                    //CL_DEVICE_TYPE_ALL,
                    //CL_DEVICE_TYPE_CPU,
                    //CL_DEVICE_TYPE_GPU,
                    NULL,
                    NULL,
                    &err_num);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE("cl_runner: Error creating context \n");
        return err_num;
    }
#endif

    //
    // Problems imstalling ATI Stream SDK on AT HD 4850
    //
    // FindNumDevices(), From: http://devgurus.amd.com/thread/131594
    //

    size_t num_devices, cb, cb_devices = 0;
    // Get number of contect devices - first step
    err_num = clGetContextInfo(m_clContext, CL_CONTEXT_DEVICES, 0, NULL, &cb_devices);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE("cl_runner: Error getting context info \n");
        return err_num;
    }

    num_devices = cb_devices / sizeof(cl_device_id);

    std::vector<cl_device_id> devices(num_devices + 1);
    for (i = 0; i <= num_devices; ++i)
        devices[i] = NULL;

    // Get number of contect devices - second step
    err_num = clGetContextInfo(m_clContext, CL_CONTEXT_DEVICES, cb_devices, &devices[0], 0);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE("cl_runner: Error getting context info \n");
        return err_num;
    }

    std::string dev_name;
    for (i = 0; i < num_devices; ++i) {
        cb = 0;
        // Get device name - first step
        err_num = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 0, NULL, &cb);
        if (err_num == CL_SUCCESS) {
            dev_name = "";
            dev_name.resize(cb);
            // Get device name - second step
            err_num = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, cb, &dev_name[0], 0);
            if (err_num == CL_SUCCESS)
                std::cout << "cl_runner: Device Name: " << dev_name.c_str() << endl;
            else
                std::cout << "cl_runner: Device Name: unknown device name." << endl;
        }
    }

    if (num_devices > 0) {
        m_clDeviceId = devices[0];
        for (i = 1; i < num_devices; ++i) {
            cl_device_id deviceId = devices[i];
#if defined(CL_VERSION_1_1) || defined(CL_VERSION_1_2)
            if (deviceId)
                clReleaseDevice(deviceId);
#endif
        }
    }
    else {
        DOL_TRACE1("cl_runner: num_devices = %d\n", num_devices);
    }

    // Create the command-queue
    m_clCmdQueue = clCreateCommandQueue(m_clContext, m_clDeviceId, 0, &err_num);
    if (err_num != CL_SUCCESS || m_clCmdQueue == NULL) {
        DOL_TRACE("cl_runner: Error creating command queue \n");
        return err_num;
    }

    m_bInitCL = true;
    return CL_SUCCESS;
}

cl_int cl_runner::clLoadProgramSource(const char *filename, const char **source, size_t *length)
{
    cl_int err_num = CL_SUCCESS;
    char *src_content = NULL;
    size_t src_length = 0;
    if (filename == NULL) {
        err_num = CL_INVALID_VALUE;
        if (length)
            *length = src_length;
        return err_num;
    }

    std::ifstream ifs(filename, std::ios_base::binary);
    if (!ifs.good()) {
        err_num = CL_OUT_OF_RESOURCES;
        goto CL_LOAD_PROGRAM_SOURCE_EXIT;
    }

    // get file length
    ifs.seekg(0, std::ios_base::end);
    src_length = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);

    if (source != NULL) {
        // read program source content
        src_content = (char *)malloc((src_length + 1) * sizeof(char));
        ifs.read(&src_content[0], src_length);
        src_content[src_length] = '\0';
    }

    ifs.close();

CL_LOAD_PROGRAM_SOURCE_EXIT:
    if (source)
        *source = src_content;
    if (length)
        *length = src_length;
    return err_num;
}

cl_int cl_runner::compile(const char *filename)
{
    return CL_SUCCESS;
}

#if 0
    // 在GPU上，逻辑就会有一些不同。我们使每个线程计算一个元素的方法来代替cpu程序中的循环计算。每个线程的index与要计算的向量的index相同。
    __kernel void vector_add_gpu(__global const float *src_a,
                                 __global const float *src_b,
                                 __global float *out,
                                 const int nNum)
    {
        /**
         * get_global_id(0) 返回正在执行的这个线程的ID。
         * 许多线程会在同一时间开始执行同一个kernel，
         * 每个线程都会收到一个不同的ID，所以必然会执行一个不同的计算
         */
        const int idx = get_global_id(0);

        /**
         * 每个work-item都会检查自己的id是否在向量数组的区间内。
         * 如果在，work-item就会执行相应的计算
         */
        if (idx < nNum) {
            out[idx] = src_a[idx] + src_b[idx];
        }
    }
#endif

cl_int cl_runner::execute(const char *filename)
{
    if (filename == NULL || m_clContext == NULL)
        return -1;

    if (!m_bInitCL)
        init_cl();

    // Error code
    cl_int err_num = CL_SUCCESS;

    char *source_content = NULL;
    size_t src_length = 0;
    bool source_need_free = false;

#if 1
    std::ifstream ifs;
    source_need_free = true;
    err_num = clLoadProgramSource(filename, (const char **)&source_content, (size_t *)&src_length);
    if (err_num != CL_SUCCESS || src_length == 0 || source_content == NULL) {
        DOL_TRACE1("cl_runner: Error load program source. ErrCode = %d \n", err_num);
        return err_num;
    }
#else
    std::ifstream ifs(filename, std::ios_base::binary);
    if (!ifs.good()) {
        DOL_TRACE("cl_runner: Error load program source, open source file failed.\n");
        return -1;
    }

    // get file length
    ifs.seekg(0, std::ios_base::end);
    size_t length = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);

    // read program source
    std::vector<char> data(length + 1);
    ifs.read(&data[0], length);
    data[length] = 0;

    // create and build program
    source_content = &data[0];
    src_length = length;
#endif

    // Create the program
    err_num = CL_SUCCESS;
    m_clProgram = clCreateProgramWithSource(m_clContext, 1, (const char **)&source_content, &src_length, &err_num);  // 加载文件内容
    if (source_need_free) {
        if (source_content != NULL) {
            free(source_content);
            source_content = NULL;
        }
    }
    ifs.close();
    DOL_ASSERT(err_num == CL_SUCCESS);
    if (err_num != CL_SUCCESS || m_clProgram == NULL) {
        DOL_TRACE1("cl_runner: Error create program with source. ErrCode = %d \n", err_num);
        return err_num;
    }

    // Build the program
    err_num = clBuildProgram(m_clProgram, 1, &m_clDeviceId, NULL, NULL, NULL);   // 编译cl程序
    DOL_ASSERT(err_num == CL_SUCCESS);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE1("cl_runner: Error build program. ErrCode = %d \n", err_num);
        return err_num;
    }

    // Show the log
    char *build_log;
    size_t log_size;

    // First call to know the proper size
    err_num = clGetProgramBuildInfo(m_clProgram, m_clDeviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE1("cl_runner: Error get program build info step 1. ErrCode = %d \n", err_num);
        return err_num;
    }

    build_log = new char[log_size + 1]; // 编译CL的出错记录
    if (build_log == NULL)
        return (cl_int)-1;

    // Second call to get the log
    err_num = clGetProgramBuildInfo(m_clProgram, m_clDeviceId, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
    if (err_num != CL_SUCCESS) {
        if (build_log) {
            delete build_log;
            build_log = NULL;
        }
        DOL_TRACE1("cl_runner: Error get program build info step 2. ErrCode = %d \n", err_num);
        return err_num;
    }

    build_log[log_size] = '\0';
    std::string strLog(build_log);
    strLog += "\n";
    DOL_TRACE(strLog.c_str());      // 因为cl程序是在运行时编译的，在运行过程中如果出错，显示编译CL文件的错误，以便查找问题
    if (build_log) {
        delete build_log;
        build_log = NULL;
    }

    // set seed for rand()
    ::srand(FIXED_SRAND_SEED);

    const unsigned int DATA_SIZE = 1048576;
    std::vector<CL_FLOAT_T> a(DATA_SIZE), b(DATA_SIZE), ret(DATA_SIZE);
    for (unsigned int i = 0; i < DATA_SIZE; ++i) {
        a[i]    = std::rand() / (CL_FLOAT_T)RAND_MAX;
        b[i]    = std::rand() / (CL_FLOAT_T)RAND_MAX;
        ret[i]  = 0.0;
    }

    // Allocate the buffer memory objects
    cl_mem cl_a   = clCreateBuffer(m_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_FLOAT_T) * DATA_SIZE, (void *)&a[0], NULL);
    cl_mem cl_b   = clCreateBuffer(m_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_FLOAT_T) * DATA_SIZE, (void *)&b[0], NULL);
    cl_mem cl_ret = clCreateBuffer(m_clContext, CL_MEM_READ_WRITE,                       sizeof(CL_FLOAT_T) * DATA_SIZE, (void *)NULL, NULL);
    cl_mem cl_num = clCreateBuffer(m_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uint),                (void *)&DATA_SIZE, NULL);

    // 创建Kernel对应的函数

    // Extracting the kernel
#if defined(USE_CL_DOUBLE) && (USE_CL_DOUBLE != 0)
    m_clKernel = clCreateKernel(m_clProgram, "vector_add_double", &err_num);    // 这个引号中的字符串要对应cl文件中的kernel函数
#else
    m_clKernel = clCreateKernel(m_clProgram, "vector_add_float", &err_num);     // 这个引号中的字符串要对应cl文件中的kernel函数
#endif
    DOL_ASSERT(err_num == CL_SUCCESS);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE1("cl_runner: Error create kernel. ErrCode = %d \n", err_num);
        return err_num;
    }

    if (m_clKernel != NULL) {
        // Set the args values
        err_num  = clSetKernelArg(m_clKernel, 0, sizeof(cl_mem), &cl_a);
        err_num |= clSetKernelArg(m_clKernel, 1, sizeof(cl_mem), &cl_b);
        err_num |= clSetKernelArg(m_clKernel, 2, sizeof(cl_mem), &cl_ret);
        //err_num |= clSetKernelArg(m_clKernel, 3, sizeof(cl_mem), &cl_num);
        err_num |= clSetKernelArg(m_clKernel, 3, sizeof(cl_uint), &DATA_SIZE);
        if (err_num != CL_SUCCESS)
            return err_num;

        // Set work-item dimensions
        size_t globalWorkSize[2];
        size_t localWorkSize[2] = { 0 };
        globalWorkSize[0] = DATA_SIZE;
        globalWorkSize[1] = DATA_SIZE;

        sw_kernel.start();

        // Execute kernel
        err_num = clEnqueueNDRangeKernel(m_clCmdQueue, m_clKernel, 1, NULL, globalWorkSize, NULL, 0, NULL, NULL);

        sw_kernel.stop();
        if (err_num != CL_SUCCESS) {
            if (err_num == CL_INVALID_KERNEL_ARGS)
                DOL_TRACE("Invalid kernel args \n");
            else
                DOL_TRACE1("cl_runner: Error enqueue NDRange kernel. ErrCode = %d \n", err_num);
            //return err_num;
        }

        // Read output array
        if (err_num == CL_SUCCESS) {
            sw_kernel_readBuffer.start();
            err_num = clEnqueueReadBuffer(m_clCmdQueue, cl_ret, CL_TRUE, 0, sizeof(CL_FLOAT_T) * DATA_SIZE, &ret[0], 0, NULL, NULL);
            sw_kernel_readBuffer.stop();
            if (err_num != CL_SUCCESS) {
                DOL_TRACE1("cl_runner: Error enqueue read buffer. ErrCode = %d \n", err_num);
                //return err_num;
            }

            if (err_num == CL_SUCCESS) {
                bool correct = true;
                CL_FLOAT_T diff;
                for (unsigned int i = 0; i < DATA_SIZE; ++i) {
                    diff = a[i] + b[i] - ret[i];
                    if (fabs(diff) > 0.0001) {
                        correct = false;
                        break;
                    }
                }

                if (correct)
                    std::cout << "cl_runner: Data is correct" << endl;
                else
                    std::cout << "cl_runner: Data is incorrect" << endl;
            }
            else {
                std::cerr << "cl_runner: Can't run kernel or read back data" << endl;
            }
        }
    }

    if (cl_a)
        clReleaseMemObject(cl_a);
    if (cl_b)
        clReleaseMemObject(cl_b);
    if (cl_ret)
        clReleaseMemObject(cl_ret);
    if (cl_num)
        clReleaseMemObject(cl_num);

    if (m_clKernel) {
        clReleaseKernel(m_clKernel);
        m_clKernel = NULL;
    }

    if (m_clProgram) {
        clReleaseProgram(m_clProgram);
        m_clProgram = NULL;
    }

    return err_num;
}

double cl_runner::native_vector_add_test()
{
    // set seed for rand()
    ::srand(FIXED_SRAND_SEED);

    const int DATA_SIZE = 1048576;
    std::vector<CL_FLOAT_T> a(DATA_SIZE), b(DATA_SIZE), ret(DATA_SIZE), ret2(DATA_SIZE);
    for (int i = 0; i < DATA_SIZE; ++i) {
        a[i]    = (std::rand() / (CL_FLOAT_T)RAND_MAX) * (CL_FLOAT_T)2.0 - (CL_FLOAT_T)1.0;
        b[i]    = (std::rand() / (CL_FLOAT_T)RAND_MAX) * (CL_FLOAT_T)2.0 - (CL_FLOAT_T)1.0;
        ret[i]  = 0.0;
    }

    sw_native.start();
    for (int i = 0; i < DATA_SIZE; ++i) {
        ret[i] = a[i] + b[i];
    }
    sw_native.stop();

    sw_native_copyData.start();
    for (int i = 0; i < DATA_SIZE; ++i) {
        ret2[i] = ret[i];
    }
    sw_native_copyData.stop();

    return sw_native.getMillisec();
}

double cl_runner::native_vector_mult_test()
{
    // set seed for rand()
    ::srand(FIXED_SRAND_SEED);

    const int DATA_SIZE = 1048576;
    CL_FLOAT_T *a, *b, *ret, *ret2;
    a    = (CL_FLOAT_T *)malloc(sizeof(CL_FLOAT_T) * DATA_SIZE);
    b    = (CL_FLOAT_T *)malloc(sizeof(CL_FLOAT_T) * DATA_SIZE);
    ret  = (CL_FLOAT_T *)malloc(sizeof(CL_FLOAT_T) * DATA_SIZE);
    ret2 = (CL_FLOAT_T *)malloc(sizeof(CL_FLOAT_T) * DATA_SIZE);
    for (int i = 0; i < DATA_SIZE; ++i) {
        a[i]    = (std::rand() / (CL_FLOAT_T)RAND_MAX) * (CL_FLOAT_T)2.0 - (CL_FLOAT_T)1.0;
        b[i]    = (std::rand() / (CL_FLOAT_T)RAND_MAX) * (CL_FLOAT_T)2.0 - (CL_FLOAT_T)1.0;
        ret[i]  = 0.0;
    }

    sw_native.start();
    for (int i = 0; i < DATA_SIZE; ++i) {
        if (fabs(a[i]) > 0.05)
            ret[i] += a[i] * b[i];
        else
            i += 1024;
    }
    sw_native.stop();

    sw_native_copyData.start();
    for (int i = 0; i < DATA_SIZE; ++i) {
        ret2[i] = ret[i];
    }
    sw_native_copyData.stop();

    if (a)    free(a);
    if (b)    free(b);
    if (ret)  free(ret);
    if (ret2) free(ret2);

    return sw_native.getMillisec();
}

double cl_runner::getSeconds()
{
    return sw_kernel.getSeconds();
}

double cl_runner::getMillisec()
{
    return sw_kernel.getMillisec();
}

double cl_runner::getTotalMillisec()
{
    return sw_kernel.getTotalMillisec();
}

double cl_runner::getMillisec_Native()
{
    return sw_native.getMillisec();
}

double cl_runner::getMillisec_Native_CopyData()
{
    return sw_native_copyData.getMillisec();
}

double cl_runner::getMillisec_Kernel_ReadBuffer()
{
    return sw_kernel_readBuffer.getMillisec();
}

}  // namespace dolphin
