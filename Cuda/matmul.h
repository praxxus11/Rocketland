#define checkCudaErrors(call)                                      \
    do {                                                           \
        cudaError_t err = call;                                    \
        if (err != cudaSuccess) {                                  \
            printf("CUDA error at %s %d: %s\n", __FILE__, __LINE__,\
                cudaGetErrorString(err));                          \
            exit(EXIT_FAILURE);                                    \
        }                                                          \
    } while (0)


void matmul(
    float* weights, 
    float* biases, 
    float* input, 
    float* output, 
    int num_layers,
    int* l_sizes,
    int batches);