
#include <dolphin/cl_runner.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

#pragma comment(lib, "OpenCL.lib")
using namespace std;

#define DOL_TRACE(X)  std::cerr << X ;

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

size_t global_work_size[16];

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

    if (num_devices > 0)
        m_clDeviceId = devices[0];

    // Create the Command-queue
    m_clCmdQueue = clCreateCommandQueue(m_clContext, m_clDeviceId, 0, &err_num);
    if (err_num != CL_SUCCESS || m_clCmdQueue == NULL) {
        DOL_TRACE("Error creating command queue \n");
        return err_num;
    }

    m_bInitCL = true;
    return CL_SUCCESS;
}

#if 0
    // ��GPU�ϣ��߼��ͻ���һЩ��ͬ������ʹÿ���̼߳���һ��Ԫ�صķ���������cpu�����е�ѭ�����㡣ÿ���̵߳�index��Ҫ�����������index��ͬ��
    __kernel void vector_add_gpu(__global const float *fIn1,
                                 __global const float *fIn2,
                                 __global float *fOut,
                                 const int nNum)
    {
        /**
         * get_global_id(0) ��������ִ�е�����̵߳�ID��
         * ����̻߳���ͬһʱ�俪ʼִ��ͬһ��kernel��
         * ÿ���̶߳����յ�һ����ͬ��ID�����Ա�Ȼ��ִ��һ����ͬ�ļ���
         */
        const int idx = get_global_id(0);

        /**
         * ÿ��work-item�������Լ���id�Ƿ�����������������ڡ�
         * ����ڣ�work-item�ͻ�ִ����Ӧ�ļ���
         */
        if (idx < nNum) {
            fOut[idx] = fIn1[idx] + fIn2[idx];
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
    const char *source_content = &data[0];
    size_t src_size = length;

    // Create the program
    err_num = CL_SUCCESS;
    m_clProgram = clCreateProgramWithSource(m_clContext, 1, (const char **)&source_content, &src_size, &err_num);  // �����ļ�����
    _DOL_ASSERT(err_num == CL_SUCCESS);
    if (err_num != CL_SUCCESS || m_clProgram == NULL)
        return err_num;

    // Build the program
    err_num = clBuildProgram(m_clProgram, 1, &m_clDeviceId, NULL, NULL, NULL);   // ����cl����
    _DOL_ASSERT(err_num == CL_SUCCESS);
    if (err_num != CL_SUCCESS)
        return err_num;

    // Show the log
    char *build_log;
    size_t log_size;

    // First call to know the proper size
    err_num = clGetProgramBuildInfo(m_clProgram, m_clDeviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    if (err_num != CL_SUCCESS)
        return err_num;

    build_log = new char[log_size + 1]; // ����CL�ĳ����¼
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
    DOL_TRACE(strLog.c_str());      // ��Ϊcl������������ʱ����ģ������й��������������ʾ����CL�ļ��Ĵ����Ա��������
    if (build_log) {
        delete build_log;
        build_log = NULL;
    }

    const int DATA_SIZE = 1048576;
    std::vector<float> a(DATA_SIZE), b(DATA_SIZE), ret(DATA_SIZE);
    for (int i = 0; i < DATA_SIZE; ++i) {
        a[i] = std::rand() / (float)RAND_MAX;
        b[i] = std::rand() / (float)RAND_MAX;
        ret[i] = 0.0;
    }

    // Allocate the buffer memory objects
    cl_mem cl_a   = clCreateBuffer(m_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * DATA_SIZE, &a[0], NULL);
    cl_mem cl_b   = clCreateBuffer(m_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_float) * DATA_SIZE, &b[0], NULL);
    cl_mem cl_ret = clCreateBuffer(m_clContext, CL_MEM_WRITE_ONLY, sizeof(cl_float) * DATA_SIZE, NULL, NULL);
    cl_mem cl_num = clCreateBuffer(m_clContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int), (void *)&DATA_SIZE, NULL);

    // ��������Kernel��Ӧ��������

    // Extracting the kernel
    m_clKernel = clCreateKernel(m_clProgram, "vector_add_gpu", &err_num);   // ��������е��ַ���Ҫ��Ӧcl�ļ��е�kernel����
    _DOL_ASSERT(err_num == CL_SUCCESS);
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
        size_t work_size = DATA_SIZE;
        global_work_size[0] = DATA_SIZE;

        // Execute kernel
        //err_num = clEnqueueNDRangeKernel(m_clCmdQueue, m_clKernel, 1, NULL, &work_size, NULL, 0, NULL, NULL);
        err_num = clEnqueueNDRangeKernel(m_clCmdQueue, m_clKernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
        if (err_num != CL_SUCCESS) {
            if (err_num == CL_INVALID_KERNEL_ARGS)
                DOL_TRACE("Invalid kernel args \n");
            //return err_num;
        }

        // Read output array
        if (err_num == CL_SUCCESS) {
            err_num = clEnqueueReadBuffer(m_clCmdQueue, cl_ret, CL_TRUE, 0, sizeof(float) * DATA_SIZE, &ret[0], 0, NULL, NULL);
            if (err_num != CL_SUCCESS) {
                //return err_num;
            }

            if (err_num == CL_SUCCESS) {
                bool correct = true;
                for (int i = 0; i < DATA_SIZE; ++i) {
                    float diff = a[i] + b[i] - ret[i];
                    if (fabs(a[i] + b[i] - ret[i]) > 0.0001) {
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

}  // namespace dolphin
