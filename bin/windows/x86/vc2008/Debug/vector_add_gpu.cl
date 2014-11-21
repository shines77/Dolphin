
// 在GPU上，逻辑就会有一些不同。我们使每个线程计算一个元素的方法来代替cpu程序中的循环计算。每个线程的index与要计算的向量的index相同。
__kernel void vector_add_float(__global const float *src_a,
                               __global const float *src_b,
                               __global float *out,
                               const unsigned int nNum)
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

// 在GPU上，逻辑就会有一些不同。我们使每个线程计算一个元素的方法来代替cpu程序中的循环计算。每个线程的index与要计算的向量的index相同。
__kernel void vector_add_double(__global const double *src_a,
                                __global const double *src_b,
                                __global double *out,
                                const unsigned int nNum)
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
