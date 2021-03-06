
#include <dolphin/cl_helper.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
//#include <string>
#include <vector>
#include <malloc.h>
#include <math.h>

#pragma comment(lib, "OpenCL.lib")
#if !defined(CL_VERSION_1_1) && !defined(CL_VERSION_1_2)
#pragma comment(lib, "aticalcl.lib")
#pragma comment(lib, "aticalrt.lib")
#endif

using namespace std;

#define DOL_TRACE(X)  std::cerr << X;

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

bool cl_helper::use_double()
{
#if defined(CL_HELPER_USE_DOUBLE) && (CL_HELPER_USE_DOUBLE != 0)
    return true;
#else
    return false;
#endif
}

cl_int cl_helper::init_cl()
{
    if (m_bInitCL)
        return CL_SUCCESS;

    return CL_SUCCESS;
}

void cl_helper::reset_stopwatches()
{
    sw_kernel.reset();
    sw_kernel_readBuffer.reset();
    sw_native.reset();
    sw_native_copyData.reset();
}

cl_int cl_helper::clLoadProgramSource(const char *file_name, const char **content, size_t *length)
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

    std::ifstream in_file(file_name, std::ios::in | std::ios_base::binary);
    if (!in_file.good()) {
        err = CL_OUT_OF_RESOURCES;
        goto CL_LOAD_PROGRAM_SOURCE_EXIT;
    }

    // get file length
    in_file.seekg(0, std::ios_base::end);
    src_length = in_file.tellg();
    in_file.seekg(0, std::ios_base::beg);

    if (content != NULL) {
        // read program source content
        src_content = new char[src_length + 1];
        if (src_content) {
            in_file.read(&src_content[0], src_length);
            src_content[src_length] = '\0';
        }
    }

    in_file.close();

CL_LOAD_PROGRAM_SOURCE_EXIT:
    if (content)
        *content = src_content;
    if (length)
        *length = src_length;
    return err;
}

cl_int cl_helper::clLoadProgramSource(const char *file_name, std::string &content, size_t &length)
{
    cl_int err = CL_SUCCESS;
    size_t src_length = 0;
    if (file_name == NULL) {
        err = CL_INVALID_VALUE;
        length = src_length;
        return err;
    }

    std::ifstream in_file(file_name, std::ios::in | std::ios_base::binary);
    if (!in_file.good()) {
        err = CL_OUT_OF_RESOURCES;
        goto CL_LOAD_PROGRAM_SOURCE_EXIT;
    }

    // get file length
    in_file.seekg(0, std::ios_base::end);
    src_length = in_file.tellg();
    in_file.seekg(0, std::ios_base::beg);

    //
    // Read whole ASCII file into C++ std::string
    // From: http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
    //
#if 0
    stringstream str_stream;
    // read program source content
    str_stream << in_file.rdbuf();

    // source holds the content of the file
    content = str_stream.str();
#else
    content.resize(src_length);

    // read program source content
    in_file.read((char *)&content[0], content.size());;
#endif

    length = src_length;

    in_file.close();

CL_LOAD_PROGRAM_SOURCE_EXIT:
    return err;
}

cl_int cl_helper::run_vector_add(cl_runat_type device_type,
                                 const char *file_name,
                                 const char *func_name,
                                 const unsigned int data_size)
{
    if (device_type == CL_DEVICE_TYPE_NATIVE)
        return run_native_vector_add(data_size);
    else
        return run_cl_vector_add(device_type, file_name, func_name, data_size);
}

cl_int cl_helper::run_native_vector_add(const unsigned int data_size)
{
    return CL_SUCCESS;
}

cl_int cl_helper::run_cl_vector_add(cl_device_type device_type,
                                    const char *file_name,
                                    const char *func_name,
                                    const unsigned int data_size)
{
    cl_int err = CL_SUCCESS;
    std::string src_content;
    size_t src_length = 0;
    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.size() == 0) {
            std::cout << "Platform size = 0" << std::endl;
            return -1;
        }

        cl_context_properties properties[] = {
            CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0
        };
        // device_type = CL_DEVICE_TYPE_DEFAULT;
        // device_type = CL_DEVICE_TYPE_CPU;
        //cl::Context clContext(device_type, &properties[0]);
        cl::Context context(device_type, &properties[0], NULL, NULL, &err);

        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

        err = clLoadProgramSource(file_name, src_content, src_length);
        if (err != CL_SUCCESS) {
            std::cerr << "cl_helper: clLoadProgramSource() failed. err_code = " << err << endl;
        }

        cl::Program::Sources source(1, std::make_pair(src_content.c_str(), src_length));
        cl::Program program = cl::Program(context, source);
        program.build(devices);

        cl::Kernel kernel(program, func_name, &err);

        // Set seed for rand()
        srand(FIXED_SRAND_SEED);

        std::vector<CL_FLOAT_T> a(data_size), b(data_size), result(data_size);
        for (unsigned int i = 0; i < data_size; ++i) {
            a[i]      = std::rand() / (CL_FLOAT_T)RAND_MAX;
            b[i]      = std::rand() / (CL_FLOAT_T)RAND_MAX;
            result[i] = 0.0;
        }

        // Allocate the buffer memory objects
        cl::Buffer cl_a     (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_FLOAT_T) * data_size, (void *)&a[0],         NULL);
        cl::Buffer cl_b     (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_FLOAT_T) * data_size, (void *)&b[0],         NULL);
        cl::Buffer cl_result(context, CL_MEM_WRITE_ONLY,                       sizeof(CL_FLOAT_T) * data_size, (void *)NULL,          NULL);
        cl::Buffer cl_num   (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uint),                (void *)&data_size,    NULL);

        // Set the args values
        kernel.setArg(0, sizeof(cl_mem),  (void *)&cl_a);
        kernel.setArg(1, sizeof(cl_mem),  (void *)&cl_b);
        kernel.setArg(2, sizeof(cl_mem),  (void *)&cl_result);
        //clKernel.setArg(3, sizeof(cl_mem), (void *)&cl_num);
        kernel.setArg(3, sizeof(cl_uint), (void *)&data_size);

        // Set work-item dimensions and execute kernel
        cl::Event event;
        cl::CommandQueue cmdQueue(context, devices[0], 0, &err);
        sw_kernel.start();
        err = cmdQueue.enqueueNDRangeKernel(
            kernel,                   // kernel
            cl::NullRange,              // global_work_offset
            cl::NDRange(data_size),     // global_work_size
            cl::NullRange,              // local_work_size
            NULL,                       // event_wait_list
            //&event                    // event
            NULL                        // event
           );

        //event.wait();
        sw_kernel.stop();

        if (err == CL_SUCCESS) {
            sw_kernel_readBuffer.start();
            err = cmdQueue.enqueueReadBuffer(cl_result, CL_TRUE, 0, sizeof(CL_FLOAT_T) * data_size, (void *)&result[0], NULL, NULL);
            sw_kernel_readBuffer.stop();

            // Verify the result
            cl_int correct = vector_add_verify(err, a, b, result, data_size);
        }
    }
    catch (cl::Error cl_err) {
        std::cerr
            << "cl_helper Error: "
            << cl_err.what()
            << "("
            << cl_err.err()
            << ")"
            << std::endl;
        err = cl_err.err();
    }

    return err;
}

cl_int cl_helper::vector_add_verify(cl_int err,
                                    std::vector<CL_FLOAT_T> &a,
                                    std::vector<CL_FLOAT_T> &b,
                                    std::vector<CL_FLOAT_T> &result,
                                    const unsigned int data_size)
{
    cl_int is_correct = -1;
    if (err == CL_SUCCESS) {
        bool correct = true;
        CL_FLOAT_T diff;
        for (unsigned int i = 0; i < data_size; ++i) {
            diff = a[i] + b[i] - result[i];
            if (fabs(diff) > 0.0001) {
                correct = false;
                break;
            }
        }

        if (correct)
            std::cout << "cl_helper: Data is correct" << endl;
        else
            std::cout << "cl_helper: Data is incorrect" << endl;
        is_correct = correct;
    }
    else {
        std::cerr << "cl_helper: Can't run kernel or read back data" << endl;
    }
    return is_correct;
}

cl_int cl_helper::run_matrix_mul(cl_runat_type device_type,
                                 const char *file_name,
                                 const char *func_name,
                                 const unsigned int m,
                                 const unsigned int p,
                                 const unsigned int n)
{
    if (device_type == CL_DEVICE_TYPE_NATIVE)
        return run_native_matrix_mul(m, p, n);
    else
        return run_cl_matrix_mul(device_type, file_name, func_name, m, p, n);
}

cl_int cl_helper::run_native_matrix_mul(const int m, const int p, const int n)
{
    return CL_SUCCESS;
}

cl_int cl_helper::run_cl_matrix_mul(cl_device_type device_type,
                                    const char *file_name,
                                    const char *func_name,
                                    const unsigned int m,
                                    const unsigned int p,
                                    const unsigned int n)
{
    cl_int err = CL_SUCCESS;
    std::string src_content;
    size_t src_length = 0;
    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.size() == 0) {
            std::cout << "Platform size = 0" << std::endl;
            return -1;
        }

        cl_context_properties properties[] = {
            CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0
        };
        // cl_device_type device_type;
        // device_type = CL_DEVICE_TYPE_DEFAULT;
        // device_type = CL_DEVICE_TYPE_CPU;
        cl::Context context(device_type, properties);

        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();

        err = clLoadProgramSource(file_name, src_content, src_length);


        cl::Program::Sources source(1, std::make_pair(src_content.c_str(), src_length));
        cl::Program program = cl::Program(context, source);
        program.build(devices);

        cl::Kernel kernel(program, func_name, &err);

        // Set seed for rand()
        ::srand(FIXED_SRAND_SEED);

        std::vector<CL_FLOAT_T> C(m * n), A(m * p), B(p * n);
        unsigned int i;
        for (i = 0; i < m * n; ++i)
            C[i] = 0.0;

        for (i = 0; i < m * p; ++i)
            A[i] = std::rand() / (CL_FLOAT_T)RAND_MAX;

        for (i = 0; i < p * n; ++i)
            B[i] = std::rand() / (CL_FLOAT_T)RAND_MAX;

        // Allocate the buffer memory objects
        cl::Buffer cl_C (context, CL_MEM_WRITE_ONLY,                       sizeof(CL_FLOAT_T) * m * n,    (void *)NULL,      NULL);
        cl::Buffer cl_A (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_FLOAT_T) * m * p,    (void *)&A[0],     NULL);
        cl::Buffer cl_B (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(CL_FLOAT_T) * p * n,    (void *)&B[0],     NULL);
        cl::Buffer cl_m (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uint),               (void *)&m,        NULL);
        cl::Buffer cl_p (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uint),               (void *)&p,        NULL);
        cl::Buffer cl_n (context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_uint),               (void *)&n,        NULL);

        // Set the args values
        kernel.setArg(0, sizeof(cl_mem), (void *)&cl_C);
        kernel.setArg(1, sizeof(cl_mem), (void *)&cl_A);
        kernel.setArg(2, sizeof(cl_mem), (void *)&cl_B);
#if 0
        clKernel.setArg(3, sizeof(cl_mem), &cl_m);
        clKernel.setArg(4, sizeof(cl_mem), &cl_p);
        clKernel.setArg(5, sizeof(cl_mem), &cl_n);
#else
        kernel.setArg(3, sizeof(cl_uint), (void *)&m);
        kernel.setArg(4, sizeof(cl_uint), (void *)&p);
        kernel.setArg(5, sizeof(cl_uint), (void *)&n);
#endif

        // Set work-item dimensions and execute kernel
        cl::Event event;
        cl::CommandQueue cmdQueue(context, devices[0], 0, &err);
        sw_kernel.start();
        err = cmdQueue.enqueueNDRangeKernel(
            kernel,                     // kernel
            cl::NullRange,              // global_work_offset
            cl::NDRange(m, n),          // global_work_size
            cl::NDRange(16, 16),        // local_work_size
            NULL,                       // event_wait_list
            &event                      // event
            //NULL                        // event
           );

        event.wait();
        sw_kernel.stop();

        if (err == CL_SUCCESS) {
            sw_kernel_readBuffer.start();
            err = cmdQueue.enqueueReadBuffer(cl_C, CL_TRUE, 0, sizeof(CL_FLOAT_T) * m * n, (void *)&C[0], NULL, NULL);
            sw_kernel_readBuffer.stop();
            
            // Verify the result
            cl_int correct = matrix_mul_verify(err, A, B, C, m, p, n);
        }
    }
    catch (cl::Error cl_err) {
        std::cerr
            << "cl_helper Error: "
            << cl_err.what()
            << "("
            << cl_err.err()
            << ")"
            << std::endl;
        err = cl_err.err();
    }
    catch (...) {
        std::cerr << "Unknown exception." << std::endl;
    }

    return err;
}

cl_int cl_helper::matrix_mul_verify(const cl_int err,
                                    std::vector<CL_FLOAT_T> &A,
                                    std::vector<CL_FLOAT_T> &B,
                                    std::vector<CL_FLOAT_T> &C,
                                    const unsigned int m,
                                    const unsigned int p,
                                    const unsigned int n)
{
    cl_int is_correct = -1;
    if (err == CL_SUCCESS) {
        bool correct = true;
        CL_FLOAT_T diff;
        std::vector<CL_FLOAT_T> _C(m * n);
        unsigned int i;
        for (i = 0; i < m * n; ++i)
            _C[i] = 0.0;
        for (i = 0; i < m * n; ++i) {
            diff = _C[i] - C[i];
            if (fabs(diff) > 0.0001) {
                correct = false;
                break;
            }
        }

        if (correct)
            std::cout << "Data is correct" << endl;
        else
            std::cout << "Data is incorrect" << endl;
        is_correct = correct;
    }
    else {
        std::cerr << "Can't run kernel or read back data" << endl;
    }
    return is_correct;
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
