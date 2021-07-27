// SHOULD BE COMPIL ED WITH NVCC SO NO INCLUDES
#include "matmul.h"
#include <stdio.h>
// include <cublas_v2.h>
#include <iostream>

#define checkCudaErrors(call)                                      \
    do {                                                           \
        cudaError_t err = call;                                    \
        if (err != cudaSuccess) {                                  \
            printf("CUDA error at %s %d: %s\n", __FILE__, __LINE__,\
                cudaGetErrorString(err));                          \
            exit(EXIT_FAILURE);                                    \
        }                                                          \
    } while (0)

__global__ void elemwise_tanh(float* activations, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    const int stride = blockDim.x * gridDim.x;
    for (; i < n; i += stride) {
        activations[i] = tanh(activations[i]);
    }
}

void matmul(
    const float* weights,  const float* biases, 
    float* inputs, float* outputs, 
    int num_layers, const int* l_sizes,
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
    float* d_outputs;

    checkCudaErrors(cudaMalloc(&d_weights, weight_bytes));
    checkCudaErrors(cudaMalloc(&d_biases, bias_bytes));
    checkCudaErrors(cudaMalloc(&d_inputs, input_bytes));

    checkCudaErrors(cudaMemcpy(d_weights, weights, weight_bytes, cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(d_biases, biases, bias_bytes, cudaMemcpyHostToDevice));
    checkCudaErrors(cudaMemcpy(d_inputs, inputs, input_bytes, cudaMemcpyHostToDevice));

    cublasStatus_t status;
    cublasHandle_t handle;
    status = cublasCreate(&handle);
    if (status != CUBLAS_STATUS_SUCCESS) {
        printf("Failed.");
        return;
    }

    int weights_so_far = 0;
    for (int layer = 0; layer < num_layers - 1; layer++) {
        int curr_output_bytes = l_sizes[layer + 1] * batches * sizeof(float);
        checkCudaErrors(cudaMalloc(&d_outputs, curr_output_bytes));
        
        
        const float m = 1.f;
        const float n = l_sizes[layer+1];
        const float k = l_sizes[layer];

        const float alpha = 1.f;
        const long long int strideA = l_sizes[layer];

        const long long int strideB = weight_bytes / (sizeof(float) * batches);
        const long long int startB = weights_so_far;

        const float beta = 0.f;

        const long long int strideC = l_sizes[layer + 1];

        // first do all matrix multiplications
        status = cublasSgemmStridedBatched(handle, CUBLAS_OP_T, CUBLAS_OP_T, 
                            m, n, k,
                            &alpha, d_inputs, k, strideA, 
                            d_weights + startB, n, strideB,
                            &beta, d_outputs, m, strideC,
                            batches);
        cudaDeviceSynchronize();


        // appply activation func
        const int activations_len = l_sizes[layer + 1] * batches;
        const int threads_tanh = 256;
        const int blocks_tanh = (activations_len + threads_tanh - 1) / threads_tanh;
        elemwise_tanh<<<blocks_tanh, threads_tanh>>>(d_outputs, l_sizes[layer + 1] * batches);
        cudaDeviceSynchronize();

        if (status != CUBLAS_STATUS_SUCCESS) {
            printf("Failed.");
            return;
        }

        if (layer < num_layers - 2) {
            checkCudaErrors(cudaFree(d_inputs));
            checkCudaErrors(cudaMalloc(&d_inputs, curr_output_bytes));
            checkCudaErrors(cudaMemcpy(d_inputs, d_outputs, curr_output_bytes, cudaMemcpyDeviceToDevice));
            checkCudaErrors(cudaFree(d_outputs));
        }
        weights_so_far += l_sizes[layer+1] * l_sizes[layer];
    }
    checkCudaErrors(cudaMemcpy(outputs, d_outputs, output_bytes, cudaMemcpyDeviceToHost));

    checkCudaErrors(cudaFree(d_weights));
    checkCudaErrors(cudaFree(d_biases));
    checkCudaErrors(cudaFree(d_inputs));
    checkCudaErrors(cudaFree(d_outputs));
    status = cublasDestroy(handle);
    if (status != CUBLAS_STATUS_SUCCESS) {
        printf("Failed.");
        return;
    }
}