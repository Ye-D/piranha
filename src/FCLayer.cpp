
#include "FCLayer.h"
#include "Functionalities.h"
#include "matrix.cuh"

Profiler matmul_profiler;

template<typename T>
FCLayer<T>::FCLayer(FCConfig* conf, int _layerNum) :
        Layer<T>(_layerNum),
        conf(conf->inputDim, conf->batchSize, conf->outputDim),
        activations(conf->batchSize * conf->outputDim), 
        deltas(conf->batchSize * conf->outputDim),
        weights(conf->inputDim * conf->outputDim),
        biases(conf->outputDim) {
	initialize();
}

template<typename T>
void FCLayer<T>::initialize()
{
	//Initialize weights and biases here.
	//Ensure that initialization is correctly done.
	size_t lower = 30;
	size_t higher = 50;
	size_t decimation = 10000;

    std::vector<float> weight_vals(weights.size());
    for (int i = 0; i < weight_vals.size(); i++) {
        weight_vals[i] = ((float)(rand() % (higher - lower) + lower)) / decimation;
    }

    weights.setKnown(weight_vals);
    biases.zero();	
}

template<typename T>
void FCLayer<T>::printLayer()
{
	cout << "----------------------------------------------" << endl;  	
	cout << "(" << this->layerNum+1 << ") FC Layer\t\t  " << conf.inputDim << " x " << conf.outputDim << endl << "\t\t\t  "
		 << conf.batchSize << "\t\t (Batch Size)" << endl;
}

template<typename T>
void FCLayer<T>::forward(RSSData<T> &input)
{
	log_print("FC.forward");

    this->layer_profiler.start();
    
	size_t rows = conf.batchSize;
	size_t columns = conf.outputDim;
	size_t common_dim = conf.inputDim;
	size_t size = rows*columns;

    matmul_profiler.start();
	NEW_funcMatMul(input, weights, activations,
            rows, common_dim, columns, false, false, FLOAT_PRECISION);
    matmul_profiler.accumulate("fc-matmul");

    // add biases to each column
    for (int share = 0; share <= 1; share++) {
        gpu::elementVectorAdd(activations[share], biases[share], false, rows, columns);
    }

    this->layer_profiler.accumulate("fc-forward");
}

template<typename T>
void FCLayer<T>::backward(RSSData<T> &delta, RSSData<T> &forwardInput) {
    
	log_print("FC.backward");
    this->layer_profiler.start();

    // (1) Compute backwards gradient for previous layer
    // deltas = incomingDelta * W.T
    NEW_funcMatMul(delta, weights, this->deltas,
            conf.batchSize, conf.outputDim, conf.inputDim, false, true, FLOAT_PRECISION);

    // (2) Compute gradients w.r.t. weights and biases and update
    RSSData<T> db(conf.outputDim);
    for (int share = 0; share <= 1; share++) {
        gpu::reduceSum(delta[share], db[share], true, conf.batchSize, conf.outputDim);
    }
    NEW_funcTruncate(db, LOG_MINI_BATCH + LOG_LEARNING_RATE);
    biases -= db;

    RSSData<T> dW(conf.outputDim * conf.inputDim);
    NEW_funcMatMul(this->deltas, forwardInput, dW,
            conf.outputDim, conf.batchSize, conf.inputDim, true, false,
            FLOAT_PRECISION + LOG_LEARNING_RATE + LOG_MINI_BATCH);
    weights -= dW;

    this->layer_profiler.accumulate("fc-backward");
}

template class FCLayer<uint32_t>;
