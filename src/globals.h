
#ifndef GLOBALS_H
#define GLOBALS_H

#pragma once
#include <vector>
#include <string>
#include <assert.h>
#include <limits.h>
#include <array>


/********************* Macros *********************/
#define _aligned_malloc(size,alignment) aligned_alloc(alignment,size)
#define _aligned_free free
#define getrandom(min, max) ((rand()%(int)(((max) + 1)-(min)))+ (min))
#define floatToMyType(a) ((myType)(a * (1 << FLOAT_PRECISION)))


/********************* AES and other globals *********************/
#define LOG_DEBUG false
#define LOG_DEBUG_NETWORK false
#define RANDOM_COMPUTE 256//Size of buffer for random elements
#define STRING_BUFFER_SIZE 256
#define MNIST false
#define PARALLEL true
#define NO_CORES 4


/********************* MPC globals *********************/
#define NUM_OF_PARTIES 3
#define PARTY_A 0
#define PARTY_B 1
#define PARTY_C 2
#define USING_EIGEN true
#define PRIME_NUMBER 67
#define FLOAT_PRECISION 13
#define PRECISE_DIVISION false


/********************* Neural Network globals *********************/
//Batch size has to be a power of two
#if MNIST
	#define TRAINING_DATA_SIZE 60000
	#define TEST_DATA_SIZE 10000
	#define LOG_MINI_BATCH 7
#else
	#define TRAINING_DATA_SIZE 8
	#define TEST_DATA_SIZE 8
	#define LOG_MINI_BATCH 0
#endif
#define MINI_BATCH_SIZE (1 << LOG_MINI_BATCH)
#define LOG_LEARNING_RATE 5
#define LEARNING_RATE (1 << (FLOAT_PRECISION - LOG_LEARNING_RATE))
#define NO_OF_EPOCHS 1.5
#define NUM_ITERATIONS 10
// #define NUM_ITERATIONS ((int) (NO_OF_EPOCHS * TRAINING_DATA_SIZE/MINI_BATCH_SIZE))



/********************* Typedefs and others *********************/
typedef uint32_t myType;
typedef uint8_t smallType;
typedef std::pair<myType, myType> RSSMyType;
typedef std::pair<smallType, smallType> RSSSmallType;
typedef std::vector<RSSMyType> RSSVectorMyType;
typedef std::vector<RSSSmallType> RSSVectorSmallType;

const int BIT_SIZE = (sizeof(myType) * CHAR_BIT);
const myType LARGEST_NEG = ((myType)1 << (BIT_SIZE - 1));
const myType MINUS_ONE = (myType)-1;
const smallType BOUNDARY = (256/PRIME_NUMBER) * PRIME_NUMBER;

#endif