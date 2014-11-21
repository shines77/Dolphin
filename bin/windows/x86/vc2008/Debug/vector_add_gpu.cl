
// ��GPU�ϣ��߼��ͻ���һЩ��ͬ������ʹÿ���̼߳���һ��Ԫ�صķ���������cpu�����е�ѭ�����㡣ÿ���̵߳�index��Ҫ�����������index��ͬ��
__kernel void vector_add_float(__global const float *src_a,
                               __global const float *src_b,
                               __global float *out,
                               const unsigned int nNum)
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
        out[idx] = src_a[idx] + src_b[idx];
    }
}

// ��GPU�ϣ��߼��ͻ���һЩ��ͬ������ʹÿ���̼߳���һ��Ԫ�صķ���������cpu�����е�ѭ�����㡣ÿ���̵߳�index��Ҫ�����������index��ͬ��
__kernel void vector_add_double(__global const double *src_a,
                                __global const double *src_b,
                                __global double *out,
                                const unsigned int nNum)
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
        out[idx] = src_a[idx] + src_b[idx];
    }
}
