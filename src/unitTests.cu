
#include <gtest/gtest.h>
#include <iostream>
#include <thrust/device_vector.h>
#include <thrust/iterator/transform_iterator.h>
#include <vector>

//#include "bitwise.cuh"
//#include "CNNConfig.h"
//#include "CNNLayer.h"
//#include "convolution.cuh"
#include "DeviceData.h"
#include "DeviceBuffer.h"
#include "DeviceBufferView.h"
//#include "FCConfig.h"
//#include "FCLayer.h"
#include "Functionalities.h"
#include "globals.h"
//#include "matrix.cuh"
//#include "MaxpoolConfig.h"
//#include "MaxpoolLayer.h"
#include "Profiler.h"
//#include "ReLUConfig.h"
//#include "ReLULayer.h"
#include "RSS.h"
//#include "secondary.h"
#include "util.cuh"

extern int partyNum;
extern Profiler func_profiler;

namespace testing {

// Just for testing, not for anything serious.
// https://en.wikipedia.org/wiki/Permuted_congruential_generator
static uint64_t state = 0x4d595df4d0f33173;
static uint64_t const multiplier = 6364136223846793005u;
static uint64_t const increment = 1442695040888963407u;

static uint32_t rotr32(uint32_t x, unsigned r) {
    return x >> r | x << (-r & 31);
}

uint32_t pcg32() {
    uint64_t x = state;
    unsigned count = (unsigned)(x >> 59);

    state = x * multiplier + increment;
    x ^= x >> 18;
    return rotr32((uint32_t)(x >> 27), count);
}

void pcg32_init(uint64_t seed) {
    state = seed + increment;
    pcg32();
}

void leftShift(std::vector<uint32_t> &v, int bits) {
    for (int i = 0; i < v.size(); i++) {
        v[i] <<= bits;
    }
}

void rightShift(std::vector<uint32_t> &v, int bits) {
    for (int i = 0; i < v.size(); i++) {
        v[i] >>= bits;
    }
}

template<typename T, typename I, typename C>
void assertDeviceData(DeviceData<T, I, C> *result, std::vector<float> &expected, bool convertFixed=true) {

    ASSERT_EQ(result->size(), expected.size());
    
    std::vector<float> host_result(result->size());
    copyToHost(*result, host_result, convertFixed);

    for(int i = 0; i < host_result.size(); i++) {
        ASSERT_EQ(host_result[i], expected[i]);
    }
}

template<typename T, typename I, typename C>
void assertRSS(RSS<T, I, C> *result, std::vector<float> &expected, bool convertFixed=true) {

    ASSERT_EQ(result->size(), expected.size());

    std::vector<float> host_result(result->size());
    std::cout << "starting copy" << std::endl;
    copyToHost(*result, host_result, convertFixed);
    std::cout << "finished copy" << std::endl;

    for(int i = 0; i < host_result.size(); i++) {
        ASSERT_EQ(host_result[i], expected[i]);
    }
}

TEST(DataTest, DeviceBuffer) {

    DeviceBuffer<uint32_t> d1 = {1, 2, 3};
    DeviceBuffer<uint32_t> d2 = {1, 1, 1};

    d1 += d2;

    //thrust::device_vector<uint32_t> v(10, 1);
    //std::cout << abi::__cxa_demangle(typeid(v.begin()).name(), nullptr, nullptr, nullptr) << std::endl;

    //printDeviceData(d1, "test buffer", false);

    std::vector<float> expected = {2, 3, 4};
    assertDeviceData(&d1, expected, false);
}

template<typename T>
using VIterator = thrust::detail::normal_iterator<thrust::device_ptr<T> >;
template<typename T>
using VConstIterator = thrust::detail::normal_iterator<thrust::device_ptr<const T> >;

typedef thrust::transform_iterator<thrust::negate<uint32_t>, VIterator<uint32_t> > TIterator;
typedef thrust::transform_iterator<thrust::negate<uint32_t>, VConstIterator<uint32_t> > TConstIterator;

TEST(DataTest, DeviceBufferView) {

    DeviceBuffer<uint32_t> d1 = {1, 2, 3};
    DeviceBufferView<uint32_t, TIterator, TConstIterator> negated(
        thrust::make_transform_iterator(d1.first(), thrust::negate<uint32_t>()),
        thrust::make_transform_iterator(d1.last(), thrust::negate<uint32_t>())
    );

    d1 += negated;

    std::vector<float> expected = {0, 0, 0};
    assertDeviceData(&d1, expected, false);
}

/*
TEST(GPUTest, FixedPointMatMul) {
    DeviceBuffer<uint32_t> a = {1, 2, 1, 2, 1, 2};  // 2 x 3
    DeviceBuffer<uint32_t> b = {2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1}; // 3 x 4
    DeviceBuffer<uint32_t> c(8); // 2 x 4

    gpu::matrixMultiplication(a, b, c, false, false, 2, 3, 4);
    cudaDeviceSynchronize();

    std::vector<uint32_t> hostC(c.size());
    thrust::copy(c.getData().begin(), c.getData().end(), hostC.begin());
    rightShift(hostC, FLOAT_PRECISION);

    std::vector<float> result(c.size());
    fromFixed(hostC, result);

    std::vector<float> expected = {8, 4, 8, 4, 10, 5, 10, 5};
    for (int i = 0; i < result.size(); i++) {
        ASSERT_EQ(result[i], expected[i]);
    }
}

TEST(GPUTest, FixedPointMatMulTranspose) {
    DeviceBuffer<uint32_t> a = {1, 2, 1, 2, 1, 2}; // 2 x 3
    DeviceBuffer<uint32_t> c(4); // 2 x 2

    gpu::matrixMultiplication(a, a, c, false, true, 2, 3, 2);
    cudaDeviceSynchronize();

    std::vector<uint32_t> hostC(c.size());
    thrust::copy(c.getData().begin(), c.getData().end(), hostC.begin());
    rightShift(hostC, FLOAT_PRECISION);

    std::vector<float> result(c.size());
    fromFixed(hostC, result);

    std::vector<float> expected = {6, 6, 6, 9};
    for (int i = 0; i < result.size(); i++) {
        ASSERT_EQ(result[i], expected[i]);
    }
}

TEST(GPUTest, Transpose) {
    
    DeviceBuffer<uint32_t> a = {1, 2, 3, 4, 5, 6};
    DeviceBuffer<uint32_t> b(a.size());
    gpu::transpose(a, b, 2, 3);
    cudaDeviceSynchronize();

    std::vector<float> expected = {1, 4, 2, 5, 3, 6};
    assertDeviceBuffer(&b, expected);
}

TEST(GPUTest, ElementwiseVectorAdd) {
    
    DeviceBuffer<uint32_t> a = {1, 2, 3, 3, 2, 1}; // 2 x 3

    DeviceBuffer<uint32_t> row_b = {1, 1, 2};
    gpu::elementVectorAdd(a, row_b, true, 2, 3);
    cudaDeviceSynchronize();
    
    std::vector<float> expected = {2, 3, 5, 4, 3, 3};
    assertDeviceBuffer(&a, expected);

    DeviceBuffer<uint32_t> col_b = {2, 3};
    gpu::elementVectorAdd(a, col_b, false, 2, 3);
    cudaDeviceSynchronize();

    expected = {4, 5, 7, 7, 6, 6};
    assertDeviceBuffer(&a, expected);
}

TEST(GPUTest, BitExpand) {

    DeviceBuffer<uint32_t> a = {2, 3, 1};

    DeviceBuffer<uint8_t> abits(a.size() * 32);
    gpu::bitexpand(a, abits);
    cudaDeviceSynchronize();

    std::vector<float> expected = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    assertDeviceBuffer(&abits, expected, false);
}

TEST(GPUTest, Unzip) {
    
    DeviceBuffer<uint32_t> a = {1, 3, 2, 4};
    DeviceBuffer<uint32_t> b(a.size()/2), c(a.size()/2);

    gpu::unzip(a, b, c);
    cudaDeviceSynchronize();

    std::vector<float> expected = {1, 2};
    assertDeviceBuffer(&b, expected);

    expected = {3, 4};
    assertDeviceBuffer(&c, expected);
}

TEST(GPUTest, Zip) {
    
    DeviceBuffer<uint32_t> a = {1, 2};
    DeviceBuffer<uint32_t> b = {3, 4};
    DeviceBuffer<uint32_t> c(a.size() + b.size());

    gpu::zip(c, a, b);
    cudaDeviceSynchronize();

    std::vector<float> expected = {1, 3, 2, 4};
    assertDeviceBuffer(&c, expected);
}

TEST(GPUTest, Im2Row) {

    // 2x3, Din=2
    DeviceBuffer<uint32_t> im = {
        1, 2, 1,
        2, 1, 2,
        1, 2, 3,
        2, 0, 1
    };

    DeviceBuffer<uint32_t> out(12*9);
    gpu::im2row(im, out,
        3, 2, // width, height
        3, // filter size
        2, // Din
        1, 1 // stride, padding
    );
    cudaDeviceSynchronize();

    std::vector<float> expected = {
        // im 0  im 1 filter windows
        0, 0, 0, 0, 1, 2, 0, 2, 1,  0, 0, 0, 0, 1, 2, 0, 2, 0,
        0, 0, 0, 1, 2, 1, 2, 1, 2,  0, 0, 0, 1, 2, 3, 2, 0, 1,
        0, 0, 0, 2, 1, 0, 1, 2, 0,  0, 0, 0, 2, 3, 0, 0, 1, 0,
        0, 1, 2, 0, 2, 1, 0, 0, 0,  0, 1, 2, 0, 2, 0, 0, 0, 0,
        1, 2, 1, 2, 1, 2, 0, 0, 0,  1, 2, 3, 2, 0, 1, 0, 0, 0,
        2, 1, 0, 1, 2, 0, 0, 0, 0,  2, 3, 0, 0, 1, 0, 0, 0, 0
    };
    assertDeviceBuffer(&out, expected);
}

TEST(FuncTest, Reconstruct2of3) {
    
    RSSData<uint32_t> a = {1, 2, 3, 10, 5};

    DeviceBuffer<uint32_t> r(a.size());
    NEW_funcReconstruct(a, r);

    std::vector<float> expected = {1, 2, 3, 10, 5};
    assertDeviceBuffer(&r, expected);
}

TEST(FuncTest, Reconstruct3of3) {
    DeviceBuffer<uint32_t> data;
    switch (partyNum) {
        case PARTY_A:
            data = {1, 2, 3, 4};
            break;
        case PARTY_B:
            data = {1, 1, 2, 2};
            break;
        case PARTY_C:
            data = {1, 1, 0, 0};
            break;
    }

    DeviceBuffer<uint32_t> r(data.size());
    NEW_funcReconstruct3out3(data, r);

    std::vector<float> expected = {3, 4, 5, 6};
    assertDeviceBuffer(&r, expected);
}

TEST(FuncTest, MatMul) {

    RSSData<uint32_t> a = {1, 1, 1, 1, 1, 1};  // 2 x 3
    RSSData<uint32_t> b = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0}; // 3 x 4
    RSSData<uint32_t> c(8); // 2 x 4

    NEW_funcMatMul(a, b, c, 2, 3, 4, false, false, FLOAT_PRECISION);

    std::vector<float> expected = {1, 1, 1, 0, 1, 1, 1, 0};
    assertRSS(&c, expected);
}
*/

TEST(FuncTest, Reshare) {
    
    DeviceBuffer<uint32_t> *a;
    if (partyNum == PARTY_A) {
        a = new DeviceBuffer<uint32_t>({1, 2, 3, 4});
    } else {
        a = new DeviceBuffer<uint32_t>(4);
        a->zero();
    } 
       
    RSS<uint32_t, VIterator<uint32_t>, VConstIterator<uint32_t> > reshared(a->size());
    NEW_funcReshare(*a, reshared);

    std::vector<float> expected = {1, 2, 3, 4};
    std::cout << "starting assert" << std::endl;
    assertRSS(&reshared, expected);
    std::cout << "finished assert" << std::endl;
}

/*
TEST(FuncTest, SelectShare) {

    RSSData<uint32_t> x = {1, 2};
    RSSData<uint32_t> y = {4, 5};
    RSSData<uint32_t> b = {0, 1};
    b[0] >>= FLOAT_PRECISION;
    b[1] >>= FLOAT_PRECISION;

    RSSData<uint32_t> z(x.size());
    NEW_funcSelectShare(x, y, b, z);

    std::vector<float> expected = {1, 5};
    assertRSS(&z, expected);
}

TEST(FuncTest, Truncate) {

    RSSData<uint32_t> a = {1 << 3, 2 << 3, 3 << 3};
    NEW_funcTruncate(a, 3);

    std::vector<float> expected = {1, 2, 3};
    assertRSS(&a, expected);
}

TEST(FuncTest, Convolution) {

    // 2x3, Din=2
    RSSData<uint32_t> im = {
        1, 2, 1,
        2, 1, 2,
        1, 2, 3,
        2, 0, 1
    };

    // 2 3x3 filters, Dout=1
    RSSData<uint32_t> filters = {
        1, 1, 1,
        1, 1, 1,
        1, 1, 1,
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };

    // 1xDout, duplicated for each row in the convolved output
    RSSData<uint32_t> biases = {
        1
    };

    // imageW - filterSize + (2*padding) / stride + 1
    size_t wKernels = ((3 - 3 + (2 * 1))/1)+1;
    // imageH - filterSize + (2*padding) / stride + 1
    size_t hKernels = ((2 - 3 + (2 * 1))/1)+1;
    RSSData<uint32_t> out(wKernels * hKernels * 1); // Dout = 1

    NEW_funcConvolution(im, filters, biases, out,
            3, 2, 3, 2, 1, 1, 1, FLOAT_PRECISION);

    std::vector<float> expected = {
        8, 13, 10, 9, 11, 10
    };
    assertRSS(&out, expected);
}

TEST(FuncTest, DRELU) {
    
    RSSData<uint32_t> input = {-1, 2, -2, -3};

    RSSData<uint32_t> result(input.size());
    NEW_funcDRELU(input, result);

    std::vector<float> expected = {
        0, 1, 0, 0
    };
    assertRSS(&result, expected, false);
}

TEST(FuncTest, RELU) {

    RSSData<uint32_t> input = {
        -2, -3, 4, 3, 3.5, 1, -1.5, -1
    };

    RSSData<uint32_t> result(input.size());
    RSSData<uint32_t> dresult(input.size());
    NEW_funcRELU(input, result, dresult);

    std::vector<float> expected = {
        0, 0, 4, 3, 3.5, 1, 0, 0
    };
    assertRSS(&result, expected);

    std::vector<float> dexpected = {
        0, 0, 1, 1, 1, 1, 0, 0
    };
    assertRSS(&dresult, dexpected, false);
}

TEST(FuncTest, Maxpool) {

    RSSData<uint32_t> input = {1, 3, 4, 3, 7, 1, 2, 10};
    RSSData<uint32_t> result(input.size() / 4);
    RSSData<uint32_t> dresult(input.size());

    NEW_funcMaxpool(input, result, dresult, 4);

    std::vector<float> expected = {
        4, 10
    };
    assertRSS(&result, expected);

    std::vector<float> dexpected = {
        0, 0, 1, 0, 0, 0, 0, 1
    };
    assertRSS(&dresult, dexpected, false);
}

TEST(LayerTest, FCForward) {

    int inputDim = 4;
    int batchSize = 4;
    int outputDim = 3;

    RSSData<uint32_t> input {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    FCConfig *lconfig = new FCConfig(inputDim, batchSize, outputDim);
    FCLayer<uint32_t> layer(lconfig, 0); 

    layer.forward(input);

    std::vector<float> expected(4*3); // 12
    copyToHost(*(layer.getWeights()), expected);

    assertRSS(layer.getActivation(), expected);
}

TEST(LayerTest, CNNForward) {

    // 2x2, Din=3
    RSSData<uint32_t> im = {
        1, 0,
        0, 0,
        0, 1,
        0, 0,
        0, 0,
        0, 1,
    };

    // weights: 2 2x2 filters, Dout=2 -> 4 2x2 filters
    // biases: 1xDout, duplicated for each row in the convolved output
    //              -> 1x2 biases
    CNNConfig *lconfig = new CNNConfig(
        2, 2, // image width x image height
        3, // input features
        2, 2, // filters, filter size
        2, 1, // stride, padding
        1 // batch size
    );
    CNNLayer<uint32_t> layer(lconfig, 0); 
    layer.forward(im);

    // construct expected results based on randomized layer weights 
    std::vector<float> host_weights(2*2*3*2); // 24
    copyToHost(*layer.getWeights(), host_weights);

    std::vector<float> expected = {
        host_weights[3],
        host_weights[6],
        0,
        host_weights[8],
        host_weights[15],
        host_weights[18],
        0,
        host_weights[20]
    };

    assertRSS(layer.getActivation(), expected);
}

TEST(LayerTest, RELUForward) {

    RSSData<uint32_t> input = {
        -2, -3, 4, 3, 3.5, 1, -1.5, -1
    };

    ReLUConfig *lconfig = new ReLUConfig(
        input.size(),
        1 // batch size? 
    );
    ReLULayer<uint32_t> layer(lconfig, 0);
    layer.forward(input);

    std::vector<float> expected = {
        0, 0, 4, 3, 3.5, 1, 0, 0
    };
    assertRSS(layer.getActivation(), expected);
}

TEST(LayerTest, MaxpoolForward) {
    // imageWidth x imageHeight = 2 x 2
    // features = 3
    // batchSize = 2
    RSSData<uint32_t> inputImage = {
        // im 1
         1,  3,
        -3,  0,
        -1, -2,
         2,  1,
         3,  0,
         1, -1,
        // im 2
         2, -2,
         0, -1,
        -1, -3,
         0,  3,
        -2, -3,
         0, -2
    };

    MaxpoolConfig *lconfig = new MaxpoolConfig(
        2, 2, // imageWidth x imageHeight
        3, // features
        2, // poolSize
        1, // stride
        2 // batchSize
    );
    MaxpoolLayer<uint32_t> layer(lconfig, 0);
    layer.forward(inputImage);

    std::vector<float> expected = {
        3, 2, 3, 2, 3, 0    
    };
    assertRSS(layer.getActivation(), expected);
}

TEST(PerfTest, DISABLED_LargeMatMul) {

    int rows = 8;
    int shared = 784; // 786
    int cols = 128; // 128

    RSSData<uint32_t> a(rows * shared);
    RSSData<uint32_t> b(shared *  cols);
    RSSData<uint32_t> c(rows * cols);

    //std::cout << "generating inputs" << std::endl;

    / *
    std::default_random_engine generator;

    std::uniform_int_distribution<uint32_t> distribution(0,255);
    std::vector<uint32_t> randomInput(rows * shared);
    for (int i = 0; i < randomInput.size(); i++) {
        randomInput.push_back(distribution(generator));
    }
    if (partyNum == PARTY_A) {
        thrust::copy(randomInput.begin(), randomInput.end(), a[0].getData().begin());
    } else if (partyNum == PARTY_C) {
        thrust::copy(randomInput.begin(), randomInput.end(), a[1].getData().begin());
    }
    * /

    //std::cout << "generating weights" << std::endl;

    / *
    std::uniform_int_distribution<uint32_t> bit_distribution(0,1);
    std::vector<uint32_t> randomWeights(shared * cols);
    for (int i = 0; i < randomWeights.size(); i++) {
        randomInput.push_back(bit_distribution(generator));
    }
    if (partyNum == PARTY_A) {
        thrust::copy(randomWeights.begin(), randomWeights.end(), b[0].getData().begin());
    } else if (partyNum == PARTY_C) {
        thrust::copy(randomWeights.begin(), randomWeights.end(), b[1].getData().begin());
    }
    * /

    Profiler p;
    p.start();
    NEW_funcMatMul(a, b, c, rows, shared, cols, false, false, FLOAT_PRECISION);
    p.accumulate("matmul");

    p.dump_all();
}

TEST(PerfTest, DISABLED_FCLayer) {

    int inputDim = 784;
    int batchSize = 8;
    int outputDim = 128;

    / *
    std::default_random_engine generator;

    std::uniform_int_distribution<uint32_t> distribution(0,255);
    std::vector<uint32_t> randomVals(batchSize * inputDim);
    for (int i = 0; i < randomVals.size(); i++) {
        randomVals.push_back(distribution(generator));
    }* /

    RSSData<uint32_t> input(batchSize * inputDim);
    / *
    if (partyNum == PARTY_A) {
        thrust::copy(randomVals.begin(), randomVals.end(), input[0].getData().begin());
    } else if (partyNum == PARTY_C) {
        thrust::copy(randomVals.begin(), randomVals.end(), input[1].getData().begin());
    }
    * /

    FCConfig *lconfig = new FCConfig(inputDim, batchSize, outputDim);
    FCLayer<uint32_t> layer(lconfig, 0); 

    layer.forward(input);
    layer.layer_profiler.dump_all();
}

TEST(PerfTest, DISABLED_ReLULayer) {

    func_profiler.clear();

    int inputDim = 1000000;
    int batchSize = 1;

    RSSData<uint32_t> input(batchSize * inputDim);

    ReLUConfig *lconfig = new ReLUConfig(inputDim, batchSize);
    ReLULayer<uint32_t> layer(lconfig, 0); 

    start_m();

    layer.forward(input);

    end_m("relu-test");

	cout << "----------------------------------------------" << endl;  	
	cout << "Run details: " << NUM_OF_PARTIES << "PC (P" << partyNum 
		 << "), " << NUM_ITERATIONS << " iterations, batch size " << MINI_BATCH_SIZE << endl ;
	cout << "----------------------------------------------" << endl << endl;  

    layer.layer_profiler.dump_all();
    func_profiler.dump_all();
}
*/

} // namespace test

int runTests(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
