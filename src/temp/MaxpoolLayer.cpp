
#include "convolution.cuh"
#include "MaxpoolLayer.h"
#include "Functionalities.h"

template<typename T>
Profiler MaxpoolLayer<T>::maxpool_profiler;

template<typename T>
MaxpoolLayer<T>::MaxpoolLayer(MaxpoolConfig* conf, int _layerNum) : Layer<T>(_layerNum),
 	conf(conf->imageHeight, conf->imageWidth, conf->features, 
	  		conf->poolSize, conf->stride, conf->batchSize),
 	activations(conf->batchSize * conf->features * 
			(((conf->imageWidth - conf->poolSize)/conf->stride) + 1) * 
 		    (((conf->imageHeight - conf->poolSize)/conf->stride) + 1)),
 	deltas(conf->batchSize * conf->features * 
	   		(((conf->imageWidth - conf->poolSize)/conf->stride) + 1) * 
	   		(((conf->imageHeight - conf->poolSize)/conf->stride) + 1)),
 	maxPrime((((conf->imageWidth - conf->poolSize)/conf->stride) + 1) * 
		 	(((conf->imageHeight - conf->poolSize)/conf->stride) + 1) * 
		 	conf->features * conf->batchSize * conf->poolSize * conf->poolSize) {
	// nothing
};

template<typename T>
void MaxpoolLayer<T>::printLayer()
{
	cout << "----------------------------------------------" << endl;  	
	cout << "(" << this->layerNum+1 << ") Maxpool Layer\t  " << conf.imageHeight << " x " << conf.imageWidth 
		 << " x " << conf.features << endl << "\t\t\t  " 
		 << conf.poolSize << "  \t\t(Pooling Size)" << endl << "\t\t\t  " 
		 << conf.stride << " \t\t(Stride)" << endl << "\t\t\t  " 
		 << conf.batchSize << "\t\t(Batch Size)" << endl;
}

template<typename T>
void MaxpoolLayer<T>::forward(RSSData<T>& input)
{
	log_print("Maxpool.forward");

    this->layer_profiler.start();
    maxpool_profiler.start();

    int nPools = ((conf.imageHeight - conf.poolSize)/conf.stride + 1) *
    			 ((conf.imageWidth - conf.poolSize)/conf.stride + 1);
   	RSSData<T> pools(nPools * conf.features * conf.batchSize * (conf.poolSize * conf.poolSize));
   	for(int share = 0; share <= 1; share++) {
	   	gpu::im2row(input[share], pools[share],
	   			conf.imageWidth, conf.imageHeight, conf.poolSize, conf.features * conf.batchSize,
	   			conf.stride, 0);
   	}
    
    NEW_funcMaxpool(pools, activations, maxPrime, conf.poolSize * conf.poolSize);

    this->layer_profiler.accumulate("maxpool-forward");
    maxpool_profiler.accumulate("maxpool-forward");
}

template<typename T>
void MaxpoolLayer<T>::backward(RSSData<T> &delta, RSSData<T> &forwardInput) {

	log_print("Maxpool.computeDelta");

    this->layer_profiler.start();
    maxpool_profiler.start();

    // (1) Compute backwards gradient for previous layer



    // (2) Compute gradients w.r.t. layer params and update
    // nothing for maxpool

    /*
	size_t B 	= conf.batchSize;
	size_t iw 	= conf.imageWidth;
	size_t ih 	= conf.imageHeight;
	size_t f 	= conf.poolSize;
	size_t Din 	= conf.features;
	size_t S 	= conf.stride;
	size_t ow 	= (((iw-f)/S)+1);
	size_t oh	= (((ih-f)/S)+1);

	RSSVectorSmallType temp1(iw*ih*Din*B);	//Contains maxPrime reordered
	RSSVectorMyType temp2(iw*ih*Din*B);		//Contains Delta reordered
	{
		size_t sizeY 	= iw;
		size_t sizeD 	= sizeY*ih;
		size_t sizeB 	= sizeD*Din;
		size_t counter1 = 0;
		size_t counter2 = 0;

		for (int b = 0; b < B; ++b)
			for (size_t r = 0; r < Din; ++r)
				for (int y = 0; y < oh; ++y)
					for (int x = 0; x < ow; ++x)
					{
						for (int q = 0; q < f; ++q)
						{
							for (int p = 0; p < f; ++p)
							{
								temp1[b*sizeB + r*sizeD + 
									(y*S + q)*sizeY + (x*S + p)] = 
								maxPrime[counter1++];

								temp2[b*sizeB + r*sizeD + 
									(y*S + q)*sizeY + (x*S + p)] = 
								deltas[counter2];
							}
						}
						counter2++;
					}
	}
    this->layer_profiler.accumulate("maxpool-delta-reorder");
    maxpool_profiler.accumulate("maxpool-delta-reorder");

    this->layer_profiler.start();
    maxpool_profiler.start();
	funcSelectShares(temp2, temp1, prevDelta, iw*ih*Din*B);
	*/

    maxpool_profiler.accumulate("maxpool-backward");
    this->layer_profiler.accumulate("maxpool-backward");
}

template class MaxpoolLayer<uint32_t>;