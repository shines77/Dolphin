
/* matrix_mul_gpu.cl
 * Matrix multiplication: C = A * B.
 * Device code.
 */

// OpenCL Kernel
__kernel void
matrix_mul_float(__global float *C,
                 __global const float *A,
                 __global const float *B,
                 const unsigned int m,
                 const unsigned int p,
                 const unsigned int n)
{

   // 2D Thread ID
   // Old CUDA code
   // int tx = blockIdx.x * TILE_SIZE + threadIdx.x;
   // int ty = blockIdx.y * TILE_SIZE + threadIdx.y;
   int tx = get_global_id(0);       // tx = [0, m - 1]
   int ty = get_global_id(1);       // ty = [0, n - 1]

   // value stores the element that is
   // computed by the thread
   float value = 0.0;
   for (int k = 0; k < p; ++k) {
      float elementA = A[k * m + tx];
      float elementB = B[ty * p + k];
      value += elementA * elementB;
   }

   // Write the matrix to device memory each
   // thread writes one element
   C[ty * m + tx] = value;
}

// OpenCL Kernel
__kernel void
matrix_mul_double(__global double *C,
                  __global const double *A,
                  __global const double *B,
                  const unsigned int m,
                  const unsigned int p,
                  const unsigned int n)
{

   // 2D Thread ID
   // Old CUDA code
   // int tx = blockIdx.x * TILE_SIZE + threadIdx.x;
   // int ty = blockIdx.y * TILE_SIZE + threadIdx.y;
   int tx = get_global_id(0);       // tx = [0, m - 1]
   int ty = get_global_id(1);       // ty = [0, n - 1]

   // value stores the element that is
   // computed by the thread
   double value = 0.0;
   for (int k = 0; k < p; ++k) {
      double elementA = A[k * m + tx];
      double elementB = B[ty * p + k];
      value += elementA * elementB;
   }

   // Write the matrix to device memory each
   // thread writes one element
   C[ty * m + tx] = value;
}
