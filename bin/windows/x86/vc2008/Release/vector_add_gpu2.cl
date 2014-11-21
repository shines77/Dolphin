
__kernel void vector_add_float(__global const float *src_a,
                               __global const float *src_b,
                               __global float *out,
                               const int nNum)
{
    const int idx = get_global_ad(0);

    if (idx < nNum) {
        out[idx] = src_a[idx] + src_b[idx];
    }
}

__kernel void vector_add_double(__global const double *src_a,
                                __global const double *src_b,
                                __global double *out,
                                const int nNum)
{
    const int idx = get_global_ad(0);

    if (idx < nNum) {
        out[idx] = src_a[idx] + src_b[idx];
    }
}
