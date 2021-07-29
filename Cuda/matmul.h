void matmul(
    const float* weights, 
    const float* biases, 
    float* input, 
    float* output, 
    int num_layers,
    const int* l_sizes,
    int batches);