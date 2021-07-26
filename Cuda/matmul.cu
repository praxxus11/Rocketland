// SHOULD BE COMPIL ED WITH NVCC SO NO INCLUDES
#include "matmul.h"

void matmul(
    float* weights, float* biases, 
    float* inputs, float* outputs, 
    int num_layers, int* l_sizes,
    int batches) {

    int weight_bytes = 0;
    int bias_bytes = 0;
    int input_bytes = l_sizes[0];
    int output_bytes = l_sizes[num_layers-1];

    for (int i=0; i<num_layers-1; i++) {
        weight_bytes += l_sizes[i] * l_sizes[i+1];
        bias_bytes += l_sizes[i+1];
    }
    weight_bytes *= batches * sizeof(float);
    bias_bytes *= batches * sizeof(float);
    input_bytes *= batches * sizeof(float);
    output_bytes *= batches* sizeof(float);

    float* d_weights;
    float* d_biases;
    float* d_inputs;

    checkCudaErrors(cudaMalloc(&d_weights, weight_bytes));
    checkCudaErrors(cudaMalloc(&d_biases, bias_bytes));
    checkCudaErrors(cudaMalloc(&d_inputs, input_bytes));

    checkCudaErrors(cudaMemcpy(d_weights, weights, weight_bytes, cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(d_biases, biases, bias_bytes, cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(d_inputs, inputs, input_bytes, cudaMemcpyHostToDevice));

    cublasHandle_t handle;
    checkCudaErrors(cublasCreate(&handle));


    int weights_so_far = 0;
    for (int layer = 0; layer < num_layers; layer++) {
        float* intermediate;
        int intermediate_bytes = l_sizes[layer + 1] * batches * sizeof(float);
        checkCudaErrors(cudaMalloc(&intermediate, intermediate_bytes));
        
        const float m = 1.f;
        const float n = l_sizes[layer + 1];
        const float k = l_sizes[layer];

        const float alpha = 1.f;
        const float lda = m;
        const long long int strideA = l_sizes[layer];

        const float ldb = k;
        const long long int strideB = weight_bytes / (sizeof(float) * batches);
        const long long int startB = weights_so_far;

        const float beta = 0.f;

        const float ldc = m;
        const long long int strideC = l_sizes[layer + 1];

        cublasSgemmStridedBatched(handle, CUBLAS_OP_N, CUBLAS_OP_N, 
                            m, n, k,
                            alpha, inputs, lda, strideA, 
                            weights + startB, ldb, strideB,
                            beta, intermediate, ldc, strideC,
                            batches);
        weights_so_far += l_sizes[layer+1] * l_sizes[layer];
    }


    cudaFree(d_weights);
    cudaFree(d_biases);
    cudaFree(d_inputs);
    cublasDestroy(handle);
}