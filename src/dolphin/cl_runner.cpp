
#include <dolphin/cl_runner.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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
// OpenCL �̌W��һ��
//
// �ο�: http://www.kimicat.com/opencl-1/opencl-jiao-xue-yi
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
        clReleaseDevice(m_clDeviceId);
        m_clDeviceId = NULL;
    }
    return true;
}

#define MAX_OPENCL_PLATFORMS    8
#define MAX_COPROC_INSTANCES    8

cl_int cl_runner::init_cl()
{
    if (m_bInitCL)
        return false;

    // Error code
    cl_int err_num = CL_SUCCESS;
    cl_uint num_platforms = 0;
    size_t i;

    // Get the number of PlatformIDs
    // /* Additional Error Codes */
    // CL_PLATFORM_NOT_FOUND_KHR    -1001
    err_num = clGetPlatformIDs(0, 0, &num_platforms);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE("Unable to get platforms \n");
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
        DOL_TRACE("Error getting platform id \n");
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
                printf("Find platform: %s\n\n", pbuf);
                break;
            }
        }
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
        DOL_TRACE("Error getting device ids \n");
        return err_num;
    }

    // Create the Context
    m_clContext = clCreateContext(0, 1, &m_clDeviceId, NULL, NULL, &err_num);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE("Error creating context \n");
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
                    //CL_DEVICE_TYPE_GPU,
                    NULL,
                    NULL,
                    &err_num);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE("Error creating context \n");
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
        DOL_TRACE("Error getting context info \n");
        return err_num;
    }

    num_devices = cb_devices / sizeof(cl_device_id);

    std::vector<cl_device_id> devices(num_devices + 1);
    for (i = 0; i <= num_devices; ++i)
        devices[i] = NULL;

    // Get number of contect devices - second step
    err_num = clGetContextInfo(m_clContext, CL_CONTEXT_DEVICES, cb_devices, &devices[0], 0);
    if (err_num != CL_SUCCESS) {
        DOL_TRACE("Error getting context info \n");
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
                std::cout << "Device Name: " << dev_name.c_str() << endl;
            else
                std::cout << "Device Name: unknown device name." << endl;
        }
    }

    if (num_devices > 0) {
        m_clDeviceId = devices[0];
        for (i = 1; i < num_devices; ++i) {
            cl_device_id deviceId = devices[i];
            if (deviceId)
                clReleaseDevice(deviceId);
        }
    }

    // Create the Command-queue
    m_clCmdQueue = clCreateCommandQueue(m_clContext, m_clDeviceId, 0, &err_num);
    if (err_num != CL_SUCCESS || m_clCmdQueue == NULL) {
        DOL_TRACE("Error creating command queue \n");
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

#if 0
    // ��GPU�ϣ��߼��ͻ���һЩ��ͬ������ʹÿ���̼߳���һ��Ԫ�صķ���������cpu�����е�ѭ�����㡣ÿ���̵߳�index��Ҫ�����������index��ͬ��
    __kernel void vector_add_gpu(__global const float *src_a,
                                 __global const float *src_b,
                                 __global float *out,
                                 const int nNum)
    {
        /**
         * get_global_id(0) ��������ִ�е�����̵߳�ID��
         * �����̻߳���ͬһʱ�俪ʼִ��ͬһ��kernel��
         * ÿ���̶߳����յ�һ����ͬ��ID�����Ա�Ȼ��ִ��һ����ͬ�ļ���
         */
        const int idx = get_global_id(0);

        /**
         * ÿ��work-item�������Լ���id�Ƿ�����������������ڡ�
         * ����ڣ�work-item�ͻ�ִ����Ӧ�ļ���
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
    if (err_num != CL_SUCCESS || src_length == 0 || source_content == NULL)
        return err_num;
#else
    std::ifstream ifs(filename, std::ios_base::binary);
    if (!ifs.good())
        return -1;

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
    m_clProgram = clCreateProgramWithSource(m_clContext, 1, (const char **)&source_content, &src_length, &err_num);  // �����ļ�����
    if (source_need_free) {
        if (source_content != NULL) {
            free(source_content);
            source_content = NULL;
        }
    }
    ifs.close();
    DOL_ASSERT(err_num == CL_SUCCESS);
    if (err_num != CL_SUCCESS || m_clProgram == NULL)
        return err_num;

    // Build the program
    err_num = clBuildProgram(m_clProgram, 1, &m_clDeviceId, NULL, NULL, NULL);   // ����cl����
    DOL_ASSERT(err_num == CL_SUCCESS);
    if (err_num != CL_SUCCESS)
        return err_num;

    // Show the log
    char *build_log;
    size_t log_size;

    // First call to know the proper size
    err_num = clGetProgramBuildInfo(m_clProgram, m_clDeviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    if (err_num != CL_SUCCESS)
        return err_num;

    build_log = new char[log_size + 1]; // ����CL�ĳ�����¼
    if (build_log == NULL)
        return (cl_int)-1;

    // Second call to get the log
    err_num = clGetProgramBuildInfo(m_clProgram, m_clDeviceId, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
    if (err_num != CL_SUCCESS) {
        if (build_log)
            delete build_log;
        return err_num;
    }

    build_log[log_size] = '\0';
    std::string strLog(build_log);
    strLog += "\n";
    DOL_TRACE(strLog.c_str());      // ��Ϊcl������������ʱ����ģ������й����������������ʾ����CL�ļ��Ĵ����Ա��������
    if (build_log) {
        delete build_log;
        build_log = NULL;
    }

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
    cl_mem cl_a   = clCreateBuffer(m_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_FLOAT_T) * DATA_SIZE, &a[0], NULL);
    cl_mem cl_b   = clCreateBuffer(m_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_FLOAT_T) * DATA_SIZE, &b[0], NULL);
    cl_mem cl_ret = clCreateBuffer(m_clContext, CL_MEM_READ_WRITE, sizeof(CL_FLOAT_T) * DATA_SIZE, NULL, NULL);
    cl_mem cl_num = clCreateBuffer(m_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), (void *)&DATA_SIZE, NULL);

    // ��������Kernel��Ӧ��������

    // Extracting the kernel
#if defined(USE_CL_FLOAT) && (USE_CL_FLOAT != 0)
    m_clKernel = clCreateKernel(m_clProgram, "vector_add_float", &err_num);   // ��������е��ַ���Ҫ��Ӧcl�ļ��е�kernel����
#else
    m_clKernel = clCreateKernel(m_clProgram, "vector_add_double", &err_num);   // ��������е��ַ���Ҫ��Ӧcl�ļ��е�kernel����
#endif
    DOL_ASSERT(err_num == CL_SUCCESS);
    if (err_num != CL_SUCCESS)
        return err_num;

    if (m_clKernel != NULL) {
        // Set the args values
        err_num  = clSetKernelArg(m_clKernel, 0, sizeof(cl_mem), &cl_a);
        err_num |= clSetKernelArg(m_clKernel, 1, sizeof(cl_mem), &cl_b);
        err_num |= clSetKernelArg(m_clKernel, 2, sizeof(cl_mem), &cl_ret);
        err_num |= clSetKernelArg(m_clKernel, 3, sizeof(cl_mem), &cl_num);
        if (err_num != CL_SUCCESS)
            return err_num;

        // Set work-item dimensions
        size_t globalWorkSize[2];
        size_t localWorkSize[2] = { 0 };
        globalWorkSize[0] = DATA_SIZE;
        globalWorkSize[1] = DATA_SIZE;

        sw1.start();

        // Execute kernel
        err_num = clEnqueueNDRangeKernel(m_clCmdQueue, m_clKernel, 1, NULL, globalWorkSize, NULL, 0, NULL, NULL);

        sw1.stop();
        if (err_num != CL_SUCCESS) {
            if (err_num == CL_INVALID_KERNEL_ARGS)
                DOL_TRACE("Invalid kernel args \n");
            //return err_num;
        }

        // Read output array
        if (err_num == CL_SUCCESS) {
            sw3.start();
            err_num = clEnqueueReadBuffer(m_clCmdQueue, cl_ret, CL_TRUE, 0, sizeof(CL_FLOAT_T) * DATA_SIZE, &ret[0], 0, NULL, NULL);
            sw3.stop();
            if (err_num != CL_SUCCESS) {
                //return err_num;
            }

            if (err_num == CL_SUCCESS) {
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

double cl_runner::test()
{
    // set seed for rand()
    srand(1314UL);

    const int DATA_SIZE = 1048576;
    std::vector<CL_FLOAT_T> a(DATA_SIZE), b(DATA_SIZE), ret(DATA_SIZE), ret2(DATA_SIZE);
    for (int i = 0; i < DATA_SIZE; ++i) {
        a[i]    = std::rand() / (CL_FLOAT_T)RAND_MAX;
        b[i]    = std::rand() / (CL_FLOAT_T)RAND_MAX;
        ret[i]  = 0.0;
    }

    sw2.start();
    for (int i = 0; i < DATA_SIZE; ++i)
        ret[i] = a[i] + b[i];
    sw2.stop();

    sw4.start();
    for (int i = 0; i < DATA_SIZE; ++i)
        ret2[i] = ret[i];
    sw4.stop();

    return sw2.getMillisec();
}

double cl_runner::getSeconds()
{
    return sw1.getSeconds();
}

double cl_runner::getMillisec()
{
    return sw1.getMillisec();
}

double cl_runner::getTotalMillisec()
{
    return sw1.getTotalMillisec();
}

double cl_runner::getTotalMillisec2()
{
    return sw4.getMillisec();
}

double cl_runner::getIORead()
{
    return sw3.getMillisec();
}

}  // namespace dolphin