
#pragma once
#include "Functionalities.h"
#include "Precompute.h"
#include <algorithm>    // std::rotate
#include <thread>

// extern inline smallType subModPrime(smallType a, smallType b);

using namespace std;
extern Precompute PrecomputeObject;

/******************************** Functionalities 2PC ********************************/
// Share Truncation, truncate shares of a by power (in place) (power is logarithmic)
void funcTruncate2PC(RSSVectorMyType &a, size_t power, size_t size, size_t party_1, size_t party_2)
{
/******************************** TODO ****************************************/
	// assert((partyNum == party_1 or partyNum == party_2) && "Truncate called by spurious parties");

	// if (partyNum == party_1)
	// 	for (size_t i = 0; i < size; ++i)
	// 		a[i] = static_cast<uint64_t>(static_cast<int64_t>(a[i]) >> power);

	// if (partyNum == party_2)
	// 	for (size_t i = 0; i < size; ++i)
	// 		a[i] = - static_cast<uint64_t>(static_cast<int64_t>(- a[i]) >> power);
/******************************** TODO ****************************************/
}


// XOR shares with a public bit into output.
void funcXORModuloOdd2PC(RSSVectorSmallType &bit, RSSVectorMyType &shares, RSSVectorMyType &output, size_t size)
{
/******************************** TODO ****************************************/	
	// if (partyNum == PARTY_A)
	// {
	// 	for (size_t i = 0; i < size; ++i)
	// 	{
	// 		if (bit[i] == 1)
	// 			output[i] = subtractModuloOdd<smallType, myType>(1, shares[i]);
	// 		else
	// 			output[i] = shares[i];
	// 	}
	// }

	// if (partyNum == PARTY_B)
	// {
	// 	for (size_t i = 0; i < size; ++i)
	// 	{
	// 		if (bit[i] == 1)
	// 			output[i] = subtractModuloOdd<smallType, myType>(0, shares[i]);
	// 		else
	// 			output[i] = shares[i];
	// 	}
	// }
/******************************** TODO ****************************************/
}

//Add public vector b to RSS vector a into c.
void funcAddMyTypeAndRSS(RSSVectorMyType &a, vector<myType> &b, RSSVectorMyType &c, size_t size)
{
	if (partyNum == PARTY_A)
	{
		for (int i = 0; i < size; ++i)
		{
			c[i].first = a[i].first + b[i];
			c[i].second = a[i].second;
		}
	}
	else if (partyNum == PARTY_B)
	{
		for (int i = 0; i < size; ++i)
		{
			c[i].first = a[i].first;
			c[i].second = a[i].second;
		}
	}
	else if (partyNum == PARTY_C)
	{
		for (int i = 0; i < size; ++i)
		{
			c[i].first = a[i].first;
			c[i].second = a[i].second + b[i];
		}
	}
}

//Fixed-point data has to be processed outside this function.
void funcGetShares(RSSVectorMyType &a, const vector<myType> &data)
{
	size_t size = a.size();

	if (partyNum == PARTY_A)
	{
		for (int i = 0; i < size; ++i)
		{
			a[i].first = data[i];
			a[i].second = 0;
		}
	}
	else if (partyNum == PARTY_B)
	{
		for (int i = 0; i < size; ++i)
		{
			a[i].first = 0;
			a[i].second = 0;
		}
	}
	else if (partyNum == PARTY_C)
	{
		for (int i = 0; i < size; ++i)
		{
			a[i].first = 0;
			a[i].second = data[i];
		}
	}
}


void funcGetShares(RSSVectorSmallType &a, const vector<smallType> &data)
{
	size_t size = a.size();
	
	if (partyNum == PARTY_A)
	{
		for (int i = 0; i < size; ++i)
		{
			a[i].first = data[i];
			a[i].second = 0;
		}
	}
	else if (partyNum == PARTY_B)
	{
		for (int i = 0; i < size; ++i)
		{
			a[i].first = 0;
			a[i].second = 0;
		}
	}
	else if (partyNum == PARTY_C)
	{
		for (int i = 0; i < size; ++i)
		{
			a[i].first = 0;
			a[i].second = data[i];
		}
	}
}

void funcReconstruct(const RSSVectorSmallType &a, vector<smallType> &b, size_t size, string str, bool print)
{
	log_print("Reconst: RSSSmallType, smallType");

	vector<smallType> a_next(size), a_prev(size);
	for (int i = 0; i < size; ++i)
	{
		// cout << "a.first " << (int)a[i].first << " " << (int)a[i].second << endl;
		a_prev[i] = 0;
		a_next[i] = a[i].first;
		b[i] = a[i].first;
		b[i] = additionModPrime[b[i]][a[i].second];
	}

	thread *threads = new thread[2];

	threads[0] = thread(sendVector<smallType>, ref(a_next), nextParty(partyNum), size);
	threads[1] = thread(receiveVector<smallType>, ref(a_prev), prevParty(partyNum), size);

	for (int i = 0; i < 2; i++)
		threads[i].join();

	delete[] threads;

	for (int i = 0; i < size; ++i)
		b[i] = additionModPrime[b[i]][a_prev[i]];

	if (print)
	{
		std::cout << str << ": \t";
		for (int i = 0; i < size; ++i)
			cout << (int)(b[i]) << " "; 
		std::cout << std::endl;
	}
	// if (print)
	// {
	// 	std::cout << str << ": ";
	// 	for (int i = 0; i < size/64; ++i)
	// 	{
	// 		for (int j = 0; j < 64; ++j)
	// 			cout << (int)(b[i*64+j]) << " "; 
	// 		cout << endl;
	// 	}
	// 	std::cout << std::endl;
	// }
}


void funcReconstructBit(const RSSVectorSmallType &a, vector<smallType> &b, size_t size)
{
	log_print("Reconst: RSSSmallType, smallType");

	vector<smallType> a_next(size), a_prev(size);
	for (int i = 0; i < size; ++i)
	{
		// cout << "a.first " << (int)a[i].first << " " << (int)a[i].second << endl;
		a_prev[i] = 0;
		a_next[i] = a[i].first;
		b[i] = a[i].first;
		b[i] = b[i] ^ a[i].second;
	}

	thread *threads = new thread[2];

	threads[0] = thread(sendVector<smallType>, ref(a_next), nextParty(partyNum), size);
	threads[1] = thread(receiveVector<smallType>, ref(a_prev), prevParty(partyNum), size);

	for (int i = 0; i < 2; i++)
		threads[i].join();

	delete[] threads;

	for (int i = 0; i < size; ++i)
		b[i] = b[i] ^ a_prev[i];

	// if (print)
	// {
	// 	std::cout << "B: \t";
	// 	for (int i = 0; i < size; ++i)
	// 		cout << (int)(b[i]) << " "; 
	// 	std::cout << std::endl;
	// }
}



void funcReconstruct(const RSSVectorMyType &a, vector<myType> &b, size_t size, string str, bool print)
{
	log_print("Reconst: RSSMyType, myType");
	assert(a.size() == size && "a.size mismatch for reconstruct function");

	vector<myType> a_next(size), a_prev(size);
	for (int i = 0; i < size; ++i)
	{
		a_prev[i] = 0;
		a_next[i] = a[i].first;
		b[i] = a[i].first;
		b[i] = b[i] + a[i].second;
	}

	thread *threads = new thread[2];

	threads[0] = thread(sendVector<myType>, ref(a_next), nextParty(partyNum), size);
	threads[1] = thread(receiveVector<myType>, ref(a_prev), prevParty(partyNum), size);

	for (int i = 0; i < 2; i++)
		threads[i].join();

	delete[] threads;

	for (int i = 0; i < size; ++i)
		b[i] = b[i] + a_prev[i];

	if (print)
	{
		std::cout << str << ": ";
		for (int i = 0; i < size; ++i)
			print_linear(b[i], "SIGNED");
		std::cout << std::endl;
	}
}


//Asymmetric protocol for semi-honest setting.
void funcReconstruct(const vector<myType> &a, vector<myType> &b, size_t size, string str, bool print)
{
	log_print("Reconst: myType, myType");
	assert(a.size() == size && "a.size mismatch for reconstruct function");

	vector<myType> temp_A(size,0), temp_B(size, 0);

	if (partyNum == PARTY_A or partyNum == PARTY_B)
		sendVector<myType>(a, PARTY_C, size);

	if (partyNum == PARTY_C)
	{
		receiveVector<myType>(temp_A, PARTY_A, size);
		receiveVector<myType>(temp_B, PARTY_B, size);
		addVectors<myType>(temp_A, a, temp_A, size);
		addVectors<myType>(temp_B, temp_A, b, size);
		sendVector<myType>(b, PARTY_A, size);
		sendVector<myType>(b, PARTY_B, size);
	}

	if (partyNum == PARTY_A or partyNum == PARTY_B)
		receiveVector<myType>(b, PARTY_C, size);

	if (print)
	{
		std::cout << str << ": ";
		for (int i = 0; i < size; ++i)
			print_linear(b[i], "SIGNED");
		std::cout << std::endl;
	}
}

//Symmetric variant of the reconstruct protocol.
// void funcReconstruct(const vector<myType> &a, vector<myType> &b, size_t size, string str, bool print)
// {
// 	assert(a.size() == size && "a.size mismatch for reconstruct function");

// 	vector<myType> a_next(size), a_prev(size);
// 	for (int i = 0; i < size; ++i)
// 	{
// 		a_prev[i] = 0;
// 		a_next[i] = 0;
// 	}

// 	thread *threads = new thread[4];

// 	threads[0] = thread(sendVector<myType>, ref(a), nextParty(partyNum), size);
// 	threads[1] = thread(sendVector<myType>, ref(a), prevParty(partyNum), size);
// 	threads[2] = thread(receiveVector<myType>, ref(a_next), nextParty(partyNum), size);
// 	threads[3] = thread(receiveVector<myType>, ref(a_prev), prevParty(partyNum), size);

// 	for (int i = 0; i < 4; i++)
// 		threads[i].join();

// 	delete[] threads;

// 	for (int i = 0; i < size; ++i)
// 		b[i] = a[i] + a_prev[i] + a_next[i];

// #if (LOG_DEBUG)
// 	if (print)
// 	{
// 		std::cout << str << ": ";
// 		for (int i = 0; i < size; ++i)
// 			print_linear(b[i], "SIGNED");
// 		std::cout << std::endl;
// 	}
// #endif
// }


void funcReconstructBit2PC(const RSSVectorSmallType &a, size_t size, string str)
{
/******************************** TODO ****************************************/	
	// assert((partyNum == PARTY_A or partyNum == PARTY_B) && "Reconstruct called by spurious parties");

	// RSSVectorSmallType temp(size);
	// if (partyNum == PARTY_B)
	// 	sendVector<RSSSmallType>(a, PARTY_A, size);

	// if (partyNum == PARTY_A)
	// {
	// 	receiveVector<RSSSmallType>(temp, PARTY_B, size);
	// 	XORVectors(temp, a, temp, size);
	
	// 	cout << str << ": ";
	// 	for (size_t i = 0; i < size; ++i)
	// 		cout << (int)temp[i] << " ";
	// 	cout << endl;
	// }
/******************************** TODO ****************************************/	
}


void funcConditionalSet2PC(const RSSVectorMyType &a, const RSSVectorMyType &b, RSSVectorSmallType &c, 
					RSSVectorMyType &u, RSSVectorMyType &v, size_t size)
{
/******************************** TODO ****************************************/	
	// assert((partyNum == PARTY_C or partyNum == PARTY_D) && "ConditionalSet called by spurious parties");

	// for (size_t i = 0; i < size; ++i)
	// {
	// 	if (c[i] == 0)
	// 	{
	// 		u[i] = a[i];
	// 		v[i] = b[i];
	// 	}
	// 	else
	// 	{
	// 		u[i] = b[i];
	// 		v[i] = a[i];
	// 	}
	// }
/******************************** TODO ****************************************/	
}

/******************************** Functionalities MPC ********************************/
// Matrix Multiplication of a*b = c with transpose flags for a,b.
// Output is a share between PARTY_A and PARTY_B.
// a^transpose_a is rows*common_dim and b^transpose_b is common_dim*columns
void funcMatMul(const RSSVectorMyType &a, const RSSVectorMyType &b, RSSVectorMyType &c, 
					size_t rows, size_t common_dim, size_t columns,
				 	size_t transpose_a, size_t transpose_b)
{
	log_print("funcMatMul");
	assert(a.size() == rows*common_dim && "Matrix a incorrect for Mat-Mul");
	assert(b.size() == common_dim*columns && "Matrix b incorrect for Mat-Mul");
	assert(c.size() == rows*columns && "Matrix c incorrect for Mat-Mul");
	assert(transpose_a == 0 && "Currently transpose_a off");
	assert(transpose_b == 0 && "Currently transpose_b off");

#if (LOG_DEBUG)
	cout << "Rows, Common_dim, Columns: " << rows << "x" << common_dim << "x" << columns << endl;
#endif

	// size_t first_size = rows*common_dim;
	// size_t second_size = common_dim*columns;
	size_t final_size = rows*columns;
	vector<myType> temp3(final_size, 0), diffReconst(final_size, 0);


	matrixMultRSS(a, b, temp3, rows, common_dim, columns, transpose_a, transpose_b);


	RSSVectorMyType r(final_size), rPrime(final_size);
	PrecomputeObject.getDividedShares(r, rPrime, FLOAT_PRECISION, final_size);
	for (int i = 0; i < final_size; ++i)
		temp3[i] = temp3[i] - rPrime[i].first;
	
	funcReconstruct(temp3, diffReconst, final_size, "Mat-Mul diff reconst", false);
	dividePlainSA(diffReconst, (1 << FLOAT_PRECISION));
	if (partyNum == PARTY_A)
	{
		for (int i = 0; i < final_size; ++i)
		{
			c[i].first = r[i].first + diffReconst[i];
			c[i].second = r[i].second;
		}
	}

	if (partyNum == PARTY_B)
	{
		for (int i = 0; i < final_size; ++i)
		{
			c[i].first = r[i].first;
			c[i].second = r[i].second;
		}
	}

	if (partyNum == PARTY_C)
	{
		for (int i = 0; i < final_size; ++i)
		{
			c[i].first = r[i].first;
			c[i].second = r[i].second + diffReconst[i];
		}
	}	
}


// Term by term multiplication of 64-bit vectors 
void funcDotProduct(const RSSVectorMyType &a, const RSSVectorMyType &b, 
						   RSSVectorMyType &c, size_t size) 
{
	log_print("funcDotProduct");
	assert(a.size() == size && "Matrix a incorrect for Mat-Mul");
	assert(b.size() == size && "Matrix b incorrect for Mat-Mul");
	assert(c.size() == size && "Matrix c incorrect for Mat-Mul");


	vector<myType> temp3(size, 0), diffReconst(size, 0);
	for (int i = 0; i < size; ++i)
	{
		temp3[i] += a[i].first * b[i].first +
				    a[i].first * b[i].second +
				    a[i].second * b[i].first;
	}

	// funcReconstruct(temp3, diffReconst, size, "temp", true);

	RSSVectorMyType r(size), rPrime(size);
	PrecomputeObject.getDividedShares(r, rPrime, FLOAT_PRECISION, size);
	for (int i = 0; i < size; ++i)
		temp3[i] = temp3[i] - rPrime[i].first;
	
	funcReconstruct(temp3, diffReconst, size, "Dot-product diff reconst", false);
	dividePlainSA(diffReconst, (1 << FLOAT_PRECISION));
	if (partyNum == PARTY_A)
	{
		for (int i = 0; i < size; ++i)
		{
			c[i].first = r[i].first + diffReconst[i];
			c[i].second = r[i].second;
		}
	}

	if (partyNum == PARTY_B)
	{
		for (int i = 0; i < size; ++i)
		{
			c[i].first = r[i].first;
			c[i].second = r[i].second;
		}
	}

	if (partyNum == PARTY_C)
	{
		for (int i = 0; i < size; ++i)
		{
			c[i].first = r[i].first;
			c[i].second = r[i].second + diffReconst[i];
		}
	}
}


// Term by term multiplication of 64-bit vectors overriding precision
void funcDotProduct(const RSSVectorMyType &a, const RSSVectorMyType &b, 
						   RSSVectorMyType &c, size_t size, size_t precision) 
{
	log_print("funcDotProduct");
	assert(a.size() == size && "Matrix a incorrect for Mat-Mul");
	assert(b.size() == size && "Matrix b incorrect for Mat-Mul");
	assert(c.size() == size && "Matrix c incorrect for Mat-Mul");


	vector<myType> temp3(size, 0), diffReconst(size, 0);
	for (int i = 0; i < size; ++i)
	{
		temp3[i] += a[i].first * b[i].first +
				    a[i].first * b[i].second +
				    a[i].second * b[i].first;
	}

	RSSVectorMyType r(size), rPrime(size);
	PrecomputeObject.getDividedShares(r, rPrime, precision, size);
	for (int i = 0; i < size; ++i)
		temp3[i] = temp3[i] - rPrime[i].first;
	
	funcReconstruct(temp3, diffReconst, size, "Dot-product diff reconst", false);
	dividePlainSA(diffReconst, (1 << precision));
	if (partyNum == PARTY_A)
	{
		for (int i = 0; i < size; ++i)
		{
			c[i].first = r[i].first + diffReconst[i];
			c[i].second = r[i].second;
		}
	}

	if (partyNum == PARTY_B)
	{
		for (int i = 0; i < size; ++i)
		{
			c[i].first = r[i].first;
			c[i].second = r[i].second;
		}
	}

	if (partyNum == PARTY_C)
	{
		for (int i = 0; i < size; ++i)
		{
			c[i].first = r[i].first;
			c[i].second = r[i].second + diffReconst[i];
		}
	}
}


// Term by term multiplication of mod 67 vectors 
void funcDotProduct(const RSSVectorSmallType &a, const RSSVectorSmallType &b, 
							 RSSVectorSmallType &c, size_t size) 
{
	log_print("funcDotProduct");
	assert(a.size() == size && "Matrix a incorrect for Mat-Mul");
	assert(b.size() == size && "Matrix b incorrect for Mat-Mul");
	assert(c.size() == size && "Matrix c incorrect for Mat-Mul");


	vector<smallType> temp3(size, 0), recv(size, 0);
	for (int i = 0; i < size; ++i)
	{
		// cout << "(" << i << ") \t a[i].first " << (int)a[i].first << " b[i].first " << (int)b[i].first << " a[i].second " 
		// 		<< (int)a[i].second << " b[i].second " << (int)b[i].second << endl;
		temp3[i] = multiplicationModPrime[a[i].first][b[i].first];
		temp3[i] = additionModPrime[temp3[i]][multiplicationModPrime[a[i].first][b[i].second]];
		temp3[i] = additionModPrime[temp3[i]][multiplicationModPrime[a[i].second][b[i].first]];
	}

	//Add random shares of 0 locally
	thread *threads = new thread[2];

	threads[0] = thread(sendVector<smallType>, ref(temp3), prevParty(partyNum), size);
	threads[1] = thread(receiveVector<smallType>, ref(recv), nextParty(partyNum), size);

	for (int i = 0; i < 2; i++)
		threads[i].join();

	delete[] threads; 

	for (int i = 0; i < size; ++i)
	{
		c[i].first = temp3[i];
		c[i].second = recv[i];
	}
}


//Thread function for parallel private compare
void parallelPC(smallType* c, size_t start, size_t end, int t, 
				const smallType* share_m, const myType* r, 
				const smallType* beta, const smallType* betaPrime, size_t dim)
{
/******************************** TODO ****************************************/	
	// size_t index3, index2;
	// size_t PARTY;

	// smallType bit_r, a, tempM;
	// myType valueX;

	// thread_local int shuffle_counter = 0;
	// thread_local int nonZero_counter = 0;

	// //Check the security of the first if condition
	// for (size_t index2 = start; index2 < end; ++index2)
	// {
	// 	if (beta[index2] == 1 and r[index2] != MINUS_ONE)
	// 		valueX = r[index2] + 1;
	// 	else
	// 		valueX = r[index2];

	// 	if (beta[index2] == 1 and r[index2] == MINUS_ONE)
	// 	{
	// 		//One share of zero and other shares of 1
	// 		//Then multiply and shuffle
	// 		for (size_t k = 0; k < dim; ++k)
	// 		{
	// 			index3 = index2*dim + k;
	// 			c[index3] = aes_common->randModPrime();
	// 			if (partyNum == PARTY_A)
	// 				c[index3] = subtractModPrime((k!=0), c[index3]);

	// 			c[index3] = multiplyModPrime(c[index3], aes_parallel->randNonZeroModPrime(t, nonZero_counter));
	// 		}
	// 	}
	// 	else
	// 	{
	// 		//Single for loop
	// 		a = 0;
	// 		for (size_t k = 0; k < dim; ++k)
	// 		{
	// 			index3 = index2*dim + k;
	// 			c[index3] = a;
	// 			tempM = share_m[index3];

	// 			bit_r = (smallType)((valueX >> (63-k)) & 1);

	// 			if (bit_r == 0)
	// 				a = addModPrime(a, tempM);
	// 			else
	// 				a = addModPrime(a, subtractModPrime((partyNum == PARTY_A), tempM));

	// 			if (!beta[index2])
	// 			{
	// 				if (partyNum == PARTY_A)
	// 					c[index3] = addModPrime(c[index3], 1+bit_r);
	// 				c[index3] = subtractModPrime(c[index3], tempM);
	// 			}
	// 			else
	// 			{
	// 				if (partyNum == PARTY_A)
	// 					c[index3] = addModPrime(c[index3], 1-bit_r);
	// 				c[index3] = addModPrime(c[index3], tempM);
	// 			}

	// 			c[index3] = multiplyModPrime(c[index3], aes_parallel->randNonZeroModPrime(t, nonZero_counter));
	// 		}
	// 	}
	// 	aes_parallel->AES_random_shuffle(c, index2*dim, (index2+1)*dim, t, shuffle_counter);
	// }
	// aes_parallel->counterIncrement();
/******************************** TODO ****************************************/	
}


// Private Compare functionality
void funcPrivateCompare(const RSSVectorSmallType &share_m, const vector<myType> &r, 
							const RSSVectorSmallType &beta, vector<smallType> &betaPrime, 
							size_t size, size_t dim)
{
	log_print("funcPrivateCompare");
	assert(dim == BIT_SIZE && "Private Compare assert issue");
	size_t sizeLong = size*dim;
	size_t index3, index2;
	RSSVectorSmallType c(sizeLong), diff(sizeLong), twoBetaMinusOne(sizeLong), xMinusR(sizeLong);
	RSSSmallType a, tempM, tempN;
	smallType bit_r;

	//Computing x[i] - r[i]
	for (int index2 = 0; index2 < size; ++index2)
	{
		//Computing 2Beta-1
		twoBetaMinusOne[index2*dim] = subConstModPrime(beta[index2], 1);
		twoBetaMinusOne[index2*dim] = addModPrime(twoBetaMinusOne[index2*dim], beta[index2]);

		for (size_t k = 0; k < dim; ++k)
		{
			index3 = index2*dim + k;
			twoBetaMinusOne[index3] = twoBetaMinusOne[index2*dim];

			bit_r = (smallType)((r[index2] >> (63-k)) & 1);
			diff[index3] = share_m[index3];
					
			if (bit_r == 1)
				diff[index3] = subConstModPrime(diff[index3], 1);
		}
	}

	//(-1)^beta * x[i] - r[i]
	funcDotProduct(diff, twoBetaMinusOne, xMinusR, sizeLong);


	for (int index2 = 0; index2 < size; ++index2)
	{
		a = make_pair(0, 0);
		for (size_t k = 0; k < dim; ++k)
		{
			index3 = index2*dim + k;
			c[index3] = a;
			tempM = share_m[index3];

			bit_r = (smallType)((r[index2] >> (63-k)) & 1);

			tempN = XORPublicModPrime(tempM, bit_r);
			a = addModPrime(a, tempN);

			if (partyNum == PARTY_A)
			{
				c[index3].first = additionModPrime[c[index3].first][xMinusR[index3].first];
				c[index3].first = additionModPrime[c[index3].first][1];
				c[index3].second = additionModPrime[c[index3].second][xMinusR[index3].second];
			}
			else if (partyNum == PARTY_B)
			{
				c[index3].first = additionModPrime[c[index3].first][xMinusR[index3].first];
				c[index3].second = additionModPrime[c[index3].second][xMinusR[index3].second];
			}
			else if (partyNum == PARTY_C)
			{
				c[index3].first = additionModPrime[c[index3].first][xMinusR[index3].first];
				c[index3].second = additionModPrime[c[index3].second][xMinusR[index3].second];
				c[index3].second = additionModPrime[c[index3].second][1];
			}			
		}
	}

	//TODO 7 rounds of multiplication
	funcCrunchMultiply(c, betaPrime, size, dim);	
}


//Multiply each group of 64 with a random number in Z_p* and reconstruct output in betaPrime.
void funcCrunchMultiply(const RSSVectorSmallType &c, vector<smallType> &betaPrime, size_t size, size_t dim)
{
	size_t sizeLong = size*dim;
	RSSVectorSmallType c_0(sizeLong/2, make_pair(0,0)), c_1(sizeLong/4, make_pair(0,0)), 
					   c_2(sizeLong/8, make_pair(0,0)), c_3(sizeLong/16, make_pair(0,0)), 
					   c_4(sizeLong/32, make_pair(0,0)), c_5(sizeLong/64, make_pair(0,0));
	vector<smallType> reconst(size, 0);

	funcMultiplyNeighbours(c, c_0, sizeLong);
	funcMultiplyNeighbours(c_0, c_1, sizeLong/2);
	funcMultiplyNeighbours(c_1, c_2, sizeLong/4);
	funcMultiplyNeighbours(c_2, c_3, sizeLong/8);
	funcMultiplyNeighbours(c_3, c_4, sizeLong/16);
	funcMultiplyNeighbours(c_4, c_5, sizeLong/32);

	vector<smallType> a_next(size), a_prev(size);
	for (int i = 0; i < size; ++i)
	{
		a_prev[i] = 0;
		a_next[i] = c_5[i].first;
		reconst[i] = c_5[i].first;
		reconst[i] = additionModPrime[reconst[i]][c_5[i].second];
	}

	thread *threads = new thread[2];

	threads[0] = thread(sendVector<smallType>, ref(a_next), nextParty(partyNum), size);
	threads[1] = thread(receiveVector<smallType>, ref(a_prev), prevParty(partyNum), size);

	for (int i = 0; i < 2; i++)
		threads[i].join();

	delete[] threads;

	for (int i = 0; i < size; ++i)
		reconst[i] = additionModPrime[reconst[i]][a_prev[i]];

	for (int i = 0; i < size; ++i)
	{
		if (reconst[i] == 0)
			betaPrime[i] = 1;
	}
}


void funcMultiplyNeighbours(const RSSVectorSmallType &c_1, RSSVectorSmallType &c_2, size_t size)
{
	vector<smallType> temp3(size/2, 0), recv(size/2, 0);
	for (int i = 0; i < size/2; ++i)
	{
		temp3[i] = additionModPrime[temp3[i]][multiplicationModPrime[c_1[2*i].first][c_1[2*i+1].first]];
		temp3[i] = additionModPrime[temp3[i]][multiplicationModPrime[c_1[2*i].first][c_1[2*i+1].second]];
		temp3[i] = additionModPrime[temp3[i]][multiplicationModPrime[c_1[2*i].second][c_1[2*i+1].first]];
	}

	//Add random shares of 0 locally
	thread *threads = new thread[2];

	threads[0] = thread(sendVector<smallType>, ref(temp3), nextParty(partyNum), size/2);
	threads[1] = thread(receiveVector<smallType>, ref(recv), prevParty(partyNum), size/2);

	for (int i = 0; i < 2; i++)
		threads[i].join();

	delete[] threads;

	for (int i = 0; i < size/2; ++i)
	{
		c_2[i].first = temp3[i];
		c_2[i].second = recv[i];
	}
}


//Wrap functionality.
void funcWrap(const RSSVectorMyType &a, RSSVectorSmallType &theta, size_t size)
{
	log_print("funcWrap");
	
	size_t sizeLong = size*BIT_SIZE;
	RSSVectorMyType x(size), r(size); 
	RSSVectorSmallType shares_r(sizeLong), alpha(size), beta(size), eta(size); 
	vector<smallType> delta(size), etaPrime(size); 
	vector<myType> reconst_x(size);

	PrecomputeObject.getShareConvertObjects(r, shares_r, alpha, size);
	addVectors<RSSMyType>(a, r, x, size);
	for (int i = 0; i < size; ++i)
	{
		beta[i].first = wrapAround(a[i].first, r[i].first);
		x[i].first = a[i].first + r[i].first;
		beta[i].second = wrapAround(a[i].second, r[i].second);
		x[i].second = a[i].second + r[i].second;
	}

	vector<myType> x_next(size), x_prev(size);
	for (int i = 0; i < size; ++i)
	{
		x_prev[i] = 0;
		x_next[i] = x[i].first;
		reconst_x[i] = x[i].first;
		reconst_x[i] = reconst_x[i] + x[i].second;
	}

	thread *threads = new thread[2];
	threads[0] = thread(sendVector<myType>, ref(x_next), nextParty(partyNum), size);
	threads[1] = thread(receiveVector<myType>, ref(x_prev), prevParty(partyNum), size);
	for (int i = 0; i < 2; i++)
		threads[i].join();
	delete[] threads;

	for (int i = 0; i < size; ++i)
		reconst_x[i] = reconst_x[i] + x_prev[i];

	wrap3(x, x_prev, delta, size); // All parties have delta
	PrecomputeObject.getRandomBitShares(eta, size);
	funcPrivateCompare(shares_r, reconst_x, eta, etaPrime, size, BIT_SIZE);

	if (partyNum == PARTY_A)
	{
		for (int i = 0; i < size; ++i)
		{
			theta[i].first = beta[i].first ^ delta[i] ^ alpha[i].first ^ eta[i].first ^ etaPrime[i];
			theta[i].second = beta[i].second ^ alpha[i].second ^ eta[i].second;
		}
	}
	else if (partyNum == PARTY_B)
	{
		for (int i = 0; i < size; ++i)
		{
			theta[i].first = beta[i].first ^ delta[i] ^ alpha[i].first ^ eta[i].first;
			theta[i].second = beta[i].second ^ alpha[i].second ^ eta[i].second;
		}
	}
	else if (partyNum == PARTY_C)
	{
		for (int i = 0; i < size; ++i)
		{
			theta[i].first = beta[i].first ^ alpha[i].first ^ eta[i].first;
			theta[i].second = beta[i].second ^ delta[i] ^ alpha[i].second ^ eta[i].second ^ etaPrime[i];
		}
	}
}


// 3PC SelectShares: c contains shares of selector bit (encoded in myType). 
// a,b,c are shared across PARTY_A, PARTY_B
void funcSelectShares3PC(const RSSVectorMyType &a, const RSSVectorMyType &b, 
								RSSVectorMyType &c, size_t size)
{
	log_print("funcSelectShares3PC");

/******************************** TODO ****************************************/
	// funcDotProduct(a, b, c, size);
/******************************** TODO ****************************************/	
}


// b holds bits of ReLU' of a
void funcRELUPrime(const RSSVectorMyType &a, RSSVectorSmallType &b, size_t size)
{
	log_print("funcRELUPrime");

	RSSVectorMyType twoA(size);
	RSSVectorSmallType theta(size);
	for (int i = 0; i < size; ++i)
		twoA[i] = a[i] << 1;

	funcWrap(twoA, theta, size);

	for (int i = 0; i < size; ++i)
	{
		b[i].first = theta[i].first ^ (getMSB(a[i].first));
		b[i].second = theta[i].second ^ (getMSB(a[i].second));
	}
}

//PARTY_A, PARTY_B hold shares in a, want shares of RELU in b.
void funcRELU(const RSSVectorMyType &a, RSSVectorMyType &b, size_t size)
{
	log_print("funcRELU");

	RSSVectorSmallType temp(size), c(size), bXORc(size);
	RSSVectorMyType m_c(size);
	vector<smallType> reconst_b(size);

	funcRELUPrime(a, temp, size);
	PrecomputeObject.getSelectorBitShares(c, m_c, size);

	for (int i = 0; i < size; ++i)
	{
		bXORc[i].first  = c[i].first ^ temp[i].first;
		bXORc[i].second = c[i].second ^ temp[i].second;
	}

	funcReconstructBit(bXORc, reconst_b, size);
	for (int i = 0; i < size; ++i)
	{
		if (reconst_b[i] == 0)
		{
			if (partyNum == PARTY_A)
			{
				m_c[i].first = (myType)1 - m_c[i].first;
				m_c[i].second = - m_c[i].second;
			}
			else if (partyNum == PARTY_B)
			{
				m_c[i].first = - m_c[i].first;
				m_c[i].second = - m_c[i].second;
			}
			else if (partyNum == PARTY_C)
			{
				m_c[i].first = - m_c[i].first;
				m_c[i].second = (myType)1 - m_c[i].second;
			}
		}
	}

	// vector<myType> reconst_m_c(size);
	// funcReconstruct(m_c, reconst_m_c, size, "m_c", true);
	funcDotProduct(a, m_c, b, size);
}


//All parties start with shares of a number in a and b and the quotient is in quotient.
//alpha is the order of divisiors, 2^alpha < b < 2^{alpha+1}.
void funcDivisionMPC(const RSSVectorMyType &a, const RSSVectorMyType &b, RSSVectorMyType &quotient, 
							size_t size)
{
	log_print("funcDivisionMPC");

	size_t alpha = 3;
	size_t precision = alpha + FLOAT_PRECISION + 1;
	const myType constTwoPointNine = ((myType)(2.9142 * (1 << precision)));
	const myType constOne = ((myType)(1 * (1 << precision)));

	vector<myType> data_twoPointNine(size, constTwoPointNine), data_one(size, constOne), reconst(size);
	RSSVectorMyType ones(size), twoPointNine(size), twoX(size), w0(size), xw0(size), 
					epsilon0(size), epsilon1(size), termOne(size), termTwo(size), answer(size);
	funcGetShares(twoPointNine, data_twoPointNine);
	funcGetShares(ones, data_one);

	multiplyByScalar(b, 2, twoX);
	subtractVectors<RSSMyType>(twoPointNine, twoX, w0, size);
	funcDotProduct(b, w0, xw0, size, precision); 
	subtractVectors<RSSMyType>(ones, xw0, epsilon0, size);
	if (PRECISE_DIVISION)
		funcDotProduct(epsilon0, epsilon0, epsilon1, size, precision);
	addVectors(ones, epsilon0, termOne, size);
	if (PRECISE_DIVISION)
		addVectors(ones, epsilon1, termTwo, size);
	funcDotProduct(w0, termOne, answer, size, precision);
	if (PRECISE_DIVISION)
		funcDotProduct(answer, termTwo, answer, size, precision);

	RSSVectorMyType scaledA(size);
	multiplyByScalar(a, (1 << (alpha + 1)), scaledA);
	funcDotProduct(answer, scaledA, quotient, size, (precision + 2*alpha + 2));	
}



//Chunk wise maximum of a vector of size rows*columns and maximum is caclulated of every 
//column number of elements. max is a vector of size rows. maxIndex contains the index of 
//the maximum value.
//PARTY_A, PARTY_B start with the shares in a and {A,B} and {C,D} have the results in 
//max and maxIndex.
void funcMaxMPC(RSSVectorMyType &a, RSSVectorMyType &max, RSSVectorMyType &maxIndex, 
							size_t rows, size_t columns)
{
	log_print("funcMaxMPC");

/******************************** TODO ****************************************/
	// if (THREE_PC)
	// {
	// 	RSSVectorMyType diff(rows), diffIndex(rows), rp(rows), indexShares(rows*columns, 0);

	// 	for (size_t i = 0; i < rows; ++i)
	// 	{
	// 		max[i] = a[i*columns];
	// 		maxIndex[i] = 0;
	// 	}

	// 	for (size_t i = 0; i < rows; ++i)
	// 		for (size_t j = 0; j < columns; ++j)
	// 			if (partyNum == PARTY_A)
	// 				indexShares[i*columns + j] = j;

	// 	for (size_t i = 1; i < columns; ++i)
	// 	{
	// 		for (size_t	j = 0; j < rows; ++j)
	// 			diff[j] = max[j] - a[j*columns + i];

	// 		for (size_t	j = 0; j < rows; ++j)
	// 			diffIndex[j] = maxIndex[j] - indexShares[j*columns + i];

	// 		funcRELUPrime(diff, rp, rows);
	// 		funcSelectShares3PC(diff, rp, max, rows);
	// 		funcSelectShares3PC(diffIndex, rp, maxIndex, rows);

	// 		for (size_t	j = 0; j < rows; ++j)
	// 			max[j] = max[j] + a[j*columns + i];

	// 		for (size_t	j = 0; j < rows; ++j)
	// 			maxIndex[j] = maxIndex[j] + indexShares[j*columns + i];
	// 	}
	// }
/******************************** TODO ****************************************/
}


//MaxIndex is of size rows. a is of size rows*columns.
//a will be set to 0's except at maxIndex (in every set of column)
void funcMaxIndexMPC(RSSVectorMyType &a, const RSSVectorMyType &maxIndex, 
						size_t rows, size_t columns)
{
	log_print("funcMaxIndexMPC");

/******************************** TODO ****************************************/
	assert(((1 << (BIT_SIZE-1)) % columns) == 0 && "funcMaxIndexMPC works only for power of 2 columns");
	assert(columns < 257 && "This implementation does not support larger than 257 columns");
	
	// RSSVectorSmallType random(rows);

	// if (PRIMARY)
	// {
	// 	RSSVectorSmallType toSend(rows);
	// 	for (size_t i = 0; i < rows; ++i)
	// 		toSend[i] = (smallType)maxIndex[i] % columns;
		
	// 	populateRandomVector<RSSSmallType>(random, rows, "COMMON", "POSITIVE");
	// 	if (partyNum == PARTY_A)
	// 		addVectors<smallType>(toSend, random, toSend, rows);

	// 	sendVector<RSSSmallType>(toSend, PARTY_C, rows);
	// }

	// if (partyNum == PARTY_C)
	// {
	// 	RSSVectorSmallType index(rows), temp(rows);
	// 	RSSVectorMyType vector(rows*columns, 0), share_1(rows*columns), share_2(rows*columns);
	// 	receiveVector<RSSSmallType>(index, PARTY_A, rows);
	// 	receiveVector<RSSSmallType>(temp, PARTY_B, rows);
	// 	addVectors<RSSSmallType>(index, temp, index, rows);

	// 	for (size_t i = 0; i < rows; ++i)
	// 		index[i] = index[i] % columns;

	// 	for (size_t i = 0; i < rows; ++i)
	// 		vector[i*columns + index[i]] = 1;

	// 	splitIntoShares(vector, share_1, share_2, rows*columns);
	// 	sendVector<RSSMyType>(share_1, PARTY_A, rows*columns);
	// 	sendVector<RSSMyType>(share_2, PARTY_B, rows*columns);
	// }

	// if (PRIMARY)
	// {
	// 	receiveVector<RSSMyType>(a, PARTY_C, rows*columns);
	// 	size_t offset = 0;
	// 	for (size_t i = 0; i < rows; ++i)
	// 	{
	// 		rotate(a.begin()+offset, a.begin()+offset+(random[i] % columns), a.begin()+offset+columns);
	// 		offset += columns;
	// 	}
	// }
/******************************** TODO ****************************************/	
}



/****************************************************************/
/* 							DEBUG 								*/
/****************************************************************/
void debugMatMul()
{
	// size_t rows = 1000; 
	// size_t common_dim = 1000;
	// size_t columns = 1000;
	// size_t transpose_a = 0, transpose_b = 0;

	// RSSVectorMyType a(rows*common_dim, make_pair(1,1)), 
	// 				b(common_dim*columns, make_pair(1,1)), c(rows*columns);

	// funcMatMul(a, b, c, rows, common_dim, columns, transpose_a, transpose_b);

/******************************** TODO ****************************************/	
	size_t rows = 3; 
	size_t common_dim = 2;
	size_t columns = 3;
	size_t transpose_a = 0, transpose_b = 0;

	RSSVectorMyType a(rows*common_dim), b(common_dim*columns), c(rows*columns);
	vector<myType> a_reconst(rows*columns), b_reconst(common_dim*columns), c_reconst(rows*columns); 

	vector<myType> data_a = {floatToMyType(3),floatToMyType(4),
							 floatToMyType(5),floatToMyType(6),
							 floatToMyType(7),floatToMyType(8)};
	vector<myType> data_b = {floatToMyType(4),floatToMyType(5),floatToMyType(6),
							 floatToMyType(7),floatToMyType(8),floatToMyType(9)};
	funcGetShares(a, data_a);
	funcGetShares(b, data_b);

	funcReconstruct(a, a_reconst, rows*common_dim, "a", true);
	funcReconstruct(b, b_reconst, common_dim*columns, "b", true);
	funcMatMul(a, b, c, rows, common_dim, columns, transpose_a, transpose_b);
	funcReconstruct(c, c_reconst, rows*columns, "c", true);
/******************************** TODO ****************************************/	
}

void debugDotProd()
{
	/****************************** myType ***************************/
	// size_t rows = 3; 
	// size_t columns = 3;

	// RSSVectorMyType a(rows*columns, make_pair(0,0)), 
	// 				b(rows*columns, make_pair(0,0)), 
	// 				c(rows*columns);
	// vector<myType> a_reconst(rows*columns), b_reconst(rows*columns), c_reconst(rows*columns); 

	// vector<myType> data = {floatToMyType(3),floatToMyType(4),floatToMyType(5),
	// 						 floatToMyType(6),floatToMyType(7),floatToMyType(8), 
	// 						 floatToMyType(7),floatToMyType(8),floatToMyType(9)};
	// funcAddConstant(a, data);
	// funcAddConstant(b, data);

	// funcReconstruct(a, a_reconst, rows*columns, "a", true);
	// funcReconstruct(b, b_reconst, rows*columns, "b", true);
	// funcDotProduct(a, b, c, rows*columns);
	// funcReconstruct(c, c_reconst, rows*columns, "c", true);

	/****************************** smallType ***************************/
	size_t size = 9; 

	RSSVectorSmallType a(size, make_pair(1,1)), 
					   b(size, make_pair(1,1)), 
					   c(size);

	funcDotProduct(a, b, c, size);
}


void debugPC()
{
	vector<myType> plain_m{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	vector<myType> plain_r{ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; 
	vector<smallType> plain_beta{ 1, 0, 1, 0, 0, 0, 1, 1, 0, 1};
	size_t size = plain_m.size();
	size_t sizeLong = size*BIT_SIZE;
	assert(plain_r.size() == plain_m.size() && "Error in debugPC");

	RSSVectorSmallType beta(size), shares_m(sizeLong);
	vector<smallType> reconst_betaP(size), betaPrime(size);
	funcGetShares(beta, plain_beta);

	vector<smallType> bits_of_m(sizeLong);
	for (int i = 0; i < size; ++i)
		for (int j = 0; j < BIT_SIZE; ++j)
			bits_of_m[i*BIT_SIZE + j] = (smallType)((plain_m[i] >> (BIT_SIZE-1-j)) & 1);

	funcGetShares(shares_m, bits_of_m);
	funcPrivateCompare(shares_m, plain_r, beta, betaPrime, size, BIT_SIZE);
	
	cout << "BetaPrime: \t ";
	for (int i = 0; i < size; ++i)
		cout << (int)betaPrime[i] << " ";
	cout << endl;
	cout << "Beta: \t\t";
	for (int i = 0; i < size; ++i)
		cout << (int)plain_beta[i] << " ";
	cout << endl;
	cout << "m: \t\t";
	for (int i = 0; i < size; ++i)
		cout << (int)plain_m[i] << " ";
	cout << endl;
	cout << "r: \t\t";
	for (int i = 0; i < size; ++i)
		cout << (int)plain_r[i] << " ";
	cout << endl;
	cout << "m-r: \t\t";
	for (int i = 0; i < size; ++i)
		cout << (int)plain_m[i] - (int)plain_r[i] << " ";
	cout << endl;
}

void debugWrap()
{
	size_t size = 2;
	RSSVectorMyType a(size);
	RSSVectorSmallType theta(size);
	vector<smallType> b(size);

	myType interesting = MINUS_ONE/3;
	interesting = (interesting << 1) + (myType)1;

	a[0] = make_pair(interesting, interesting);
	a[1] = make_pair(0, 0);

	funcWrap(a, theta, size);

#if (LOG_DEBUG)
	funcReconstruct(theta, b, size, "Theta", true);
#endif
}


void debugReLUPrime()
{
	size_t size = 2;
	RSSVectorMyType a(size);
	RSSVectorSmallType b(size);
	vector<smallType> reconst_b(size);

	// myType interesting = MINUS_ONE/3;
	myType interesting = LARGEST_NEG - 1;
	// interesting = (interesting << 1) + (myType)0;

	a[0] = make_pair(interesting, interesting);
	a[1] = make_pair(1, 1);

	print_myType(a[0].first, "interesting", "BITS");
	print_myType(a[1].first, "interesting", "BITS");

	funcRELUPrime(a, b, size);
#if (LOG_DEBUG)
	funcReconstructBit(b, reconst_b, size);
#endif
}


void debugReLU()
{
	vector<myType> data_a {0,1,2,3,4,5,6,7};
	size_t size = data_a.size();
	RSSVectorMyType a(size), b(size);
	vector<myType> reconst_b(size);

	funcGetShares(a, data_a);
	for (int i = size/2; i < size; ++i)
	{
		a[i].first = a[i].first << 61;
		a[i].second = a[i].second << 61;
	}

	// print_myType(a[0].first, "a[0]", "BITS");
	// print_myType(a[1].first, "a[1]", "BITS");
	// print_myType(a[4].first, "a[4]", "BITS");
	// print_myType(a[5].first, "a[5]", "BITS");

	funcRELU(a, b, size);
#if (LOG_DEBUG)
	funcReconstruct(b, reconst_b, size, "ReLU", true);
#endif
}


void debugDivision()
{
	vector<myType> data_a = {1<<13}, data_b = {4<<13};
	size_t size = data_a.size();
	RSSVectorMyType a(size), b(size), quotient(size);
	vector<myType> reconst(size);

	funcGetShares(a, data_a);
	funcGetShares(b, data_b);
	funcDivisionMPC(a, b, quotient, size);

#if (LOG_DEBUG)
	funcReconstruct(a, reconst, size, "a", true);
	funcReconstruct(b, reconst, size, "b", true);
	funcReconstruct(quotient, reconst, size, "Quotient", true);
	print_myType(reconst[0], "Quotient[0]", "FLOAT");
#endif	
	
	
/******************************** TODO ****************************************/
	// size_t size = 10;
	// RSSVectorMyType numerator(size);
	// RSSVectorMyType denominator(size);
	// RSSVectorMyType quotient(size,0);
	
	// for (size_t i = 0; i < size; ++i)
	// 	numerator[i] = 50;

	// for (size_t i = 0; i < size; ++i)
	// 	denominator[i] = 50*size;

	// funcDivisionMPC(numerator, denominator, quotient, size);

	// if (PRIMARY)
	// {
	// 	funcReconstruct2PC(numerator, size, "Numerator");
	// 	funcReconstruct2PC(denominator, size, "Denominator");
	// 	funcReconstruct2PC(quotient, size, "Quotient");
	// }
/******************************** TODO ****************************************/	
}

void debugMax()
{

/******************************** TODO ****************************************/
	// size_t rows = 1;
	// size_t columns = 10;
	// RSSVectorMyType a(rows*columns, 0);

	// if (partyNum == PARTY_A or partyNum == PARTY_C){
	// 	a[0] = 0; a[1] = 1; a[2] = 0; a[3] = 4; a[4] = 5; 
	// 	a[5] = 3; a[6] = 10; a[7] = 6, a[8] = 41; a[9] = 9;
	// }

	// RSSVectorMyType max(rows), maxIndex(rows);
	// funcMaxMPC(a, max, maxIndex, rows, columns);

	// if (PRIMARY)
	// {
	// 	funcReconstruct2PC(a, columns, "a");
	// 	funcReconstruct2PC(max, rows, "max");
	// 	funcReconstruct2PC(maxIndex, rows, "maxIndex");
	// 	cout << "-----------------" << endl;
	// }
/******************************** TODO ****************************************/	
}


void debugSS()
{

/******************************** TODO ****************************************/	
	// size_t size = 10;
	// RSSVectorMyType inputs(size, 0), outputs(size, 0);

	// if (THREE_PC)
	// {
	// 	RSSVectorMyType selector(size, 0);

	// 	if (partyNum == PARTY_A)
	// 		for (size_t i = 0; i < size; ++i)
	// 			selector[i] = (myType)(aes_indep->getBit() << FLOAT_PRECISION);

	// 	if (PRIMARY)
	// 		funcReconstruct2PC(selector, size, "selector");

	// 	if (partyNum == PARTY_A)
	// 		for (size_t i = 0; i < size; ++i)
	// 			inputs[i] = (myType)aes_indep->get8Bits();

	// 	funcSelectShares3PC(inputs, selector, outputs, size);

	// 	if (PRIMARY)
	// 	{
	// 		funcReconstruct2PC(inputs, size, "inputs");
	// 		funcReconstruct2PC(outputs, size, "outputs");
	// 	}
	// }
/******************************** TODO ****************************************/	
}




void debugMaxIndex()
{

/******************************** TODO ****************************************/
	// size_t rows = 10;
	// size_t columns = 4;

	// RSSVectorMyType maxIndex(rows, 0);
	// if (partyNum == PARTY_A)
	// 	for (size_t i = 0; i < rows; ++i)
	// 		maxIndex[i] = (aes_indep->get8Bits())%columns;

	// RSSVectorMyType a(rows*columns);	
	// funcMaxIndexMPC(a, maxIndex, rows, columns);

	// if (PRIMARY)
	// {
	// 	funcReconstruct2PC(maxIndex, maxIndex.size(), "maxIndex");
		
	// 	RSSVectorMyType temp(rows*columns);
	// 	if (partyNum == PARTY_B)
	// 		sendVector<RSSMyType>(a, PARTY_A, rows*columns);

	// 	if (partyNum == PARTY_A)
	// 	{
	// 		receiveVector<RSSMyType>(temp, PARTY_B, rows*columns);
	// 		addVectors<myType>(temp, a, temp, rows*columns);
		
	// 		cout << "a: " << endl;
	// 		for (size_t i = 0; i < rows; ++i)
	// 		{
	// 			for (int j = 0; j < columns; ++j)
	// 			{
	// 				print_linear(temp[i*columns + j], DEBUG_PRINT);
	// 			}
	// 			cout << endl;
	// 		}
	// 		cout << endl;
	// 	}
	// }
/******************************** TODO ****************************************/	
}




/******************************** Test ********************************/

void testMatMul(size_t rows, size_t common_dim, size_t columns, size_t iter)
{

/******************************** TODO ****************************************/	
	// RSSVectorMyType a(rows*common_dim, 1);
	// RSSVectorMyType b(common_dim*columns, 1);
	// RSSVectorMyType c(rows*columns);

	// 	for (int runs = 0; runs < iter; ++runs)
	// 		funcMatMul(a, b, c, rows, common_dim, columns, 0, 0);
}


void testConvolution(size_t iw, size_t ih, size_t fw, size_t fh, size_t C, size_t D, size_t iter)
{

/******************************** TODO ****************************************/	
	// size_t sx = 1, sy = 1, B = MINI_BATCH_SIZE;
	// RSSVectorMyType w(fw*fh*C*D, 0);
	// RSSVectorMyType act(iw*ih*C*B, 0);
	// size_t p_range = (ih-fh+1);
	// size_t q_range = (iw-fw+1);
	// size_t size_rw = fw*fh*C*D;
	// size_t rows_rw = fw*fh*C;
	// size_t columns_rw = D;


	// for (int runs = 0; runs < iter; ++runs)
	// {
	// 	//Reshape weights
	// 	RSSVectorMyType reshapedWeights(size_rw, 0);
	// 	for (int i = 0; i < size_rw; ++i)
	// 		reshapedWeights[(i%rows_rw)*columns_rw + (i/rows_rw)] = w[i];

	// 	//reshape activations
	// 	size_t size_convo = (p_range*q_range*B) * (fw*fh*C); 
	// 	RSSVectorMyType convShaped(size_convo, 0);
	// 	convolutionReshape(act, convShaped, iw, ih, C, B, fw, fh, 1, 1);


	// 	//Convolution multiplication
	// 	RSSVectorMyType convOutput(p_range*q_range*B*D, 0);

	// 	funcMatMul(convShaped, reshapedWeights, convOutput, 
	// 				(p_range*q_range*B), (fw*fh*C), D, 0, 0);
	// }
/******************************** TODO ****************************************/	
}


void testRelu(size_t r, size_t c, size_t iter)
{

/******************************** TODO ****************************************/	
	// RSSVectorMyType a(r*c, 1);
	// RSSVectorSmallType reluPrimeSmall(r*c, 1);
	// RSSVectorMyType reluPrimeLarge(r*c, 1);
	// RSSVectorMyType b(r*c, 0);

	// for (int runs = 0; runs < iter; ++runs)
	// {
	// 	if (STANDALONE)
	// 		for (size_t i = 0; i < r*c; ++i)
	// 			b[i] = a[i] * reluPrimeSmall[i];

	// 	if (FOUR_PC)
	// 		funcSelectShares4PC(a, reluPrimeSmall, b, r*c);

	// 	if (THREE_PC)
	// 		funcSelectShares3PC(a, reluPrimeLarge, b, r*c);
	// }
/******************************** TODO ****************************************/	
}


void testReluPrime(size_t r, size_t c, size_t iter)
{

/******************************** TODO ****************************************/	
	// RSSVectorMyType a(r*c, 1);
	// RSSVectorMyType b(r*c, 0);
	// RSSVectorSmallType d(r*c, 0);

	// for (int runs = 0; runs < iter; ++runs)
	// {
	// 	if (STANDALONE)
	// 		for (size_t i = 0; i < r*c; ++i)
	// 			b[i] = (a[i] < LARGEST_NEG ? 1:0);

	// 	if (THREE_PC)
	// 		funcRELUPrime(a, b, r*c);

	// 	if (FOUR_PC)
	// 		funcRELUPrime4PC(a, d, r*c);
	// }
/******************************** TODO ****************************************/	
}


void testMaxPool(size_t p_range, size_t q_range, size_t px, size_t py, size_t D, size_t iter)
{

/******************************** TODO ****************************************/	
	// size_t B = MINI_BATCH_SIZE;
	// size_t size_x = p_range*q_range*D*B;

	// RSSVectorMyType y(size_x, 0);
	// RSSVectorMyType maxPoolShaped(size_x, 0);
	// RSSVectorMyType act(size_x/(px*py), 0);
	// RSSVectorMyType maxIndex(size_x/(px*py), 0); 

	// for (size_t i = 0; i < iter; ++i)
	// {
	// 	maxPoolReshape(y, maxPoolShaped, p_range, q_range, D, B, py, px, py, px);
		
	// 	funcMaxMPC(maxPoolShaped, act, maxIndex, size_x/(px*py), px*py);
	// }
/******************************** TODO ****************************************/	
}

void testMaxPoolDerivative(size_t p_range, size_t q_range, size_t px, size_t py, size_t D, size_t iter)
{

/******************************** TODO ****************************************/	
	// size_t B = MINI_BATCH_SIZE;
	// size_t alpha_range = p_range/py;
	// size_t beta_range = q_range/px;
	// size_t size_y = (p_range*q_range*D*B);
	// RSSVectorMyType deltaMaxPool(size_y, 0);
	// RSSVectorMyType deltas(size_y/(px*py), 0);
	// RSSVectorMyType maxIndex(size_y/(px*py), 0);

	// size_t size_delta = alpha_range*beta_range*D*B;
	// RSSVectorMyType thatMatrixTemp(size_y, 0), thatMatrix(size_y, 0);


	// for (size_t i = 0; i < iter; ++i)
	// {
	// 	if (STANDALONE)
	// 		for (size_t i = 0; i < size_delta; ++i)
	// 			thatMatrixTemp[i*px*py + maxIndex[i]] = 1;

	// 	if (MPC)
	// 		funcMaxIndexMPC(thatMatrixTemp, maxIndex, size_delta, px*py);
		

	// 	//Reshape thatMatrix
	// 	size_t repeat_size = D*B;
	// 	size_t alpha_offset, beta_offset, alpha, beta;
	// 	for (size_t r = 0; r < repeat_size; ++r)
	// 	{
	// 		size_t size_temp = p_range*q_range;
	// 		for (size_t i = 0; i < size_temp; ++i)
	// 		{
	// 			alpha = (i/(px*py*beta_range));
	// 			beta = (i/(px*py)) % beta_range;
	// 			alpha_offset = (i%(px*py))/px;
	// 			beta_offset = (i%py);
	// 			thatMatrix[((py*alpha + alpha_offset)*q_range) + 
	// 					   (px*beta + beta_offset) + r*size_temp] 
	// 			= thatMatrixTemp[r*size_temp + i];
	// 		}
	// 	}

	// 	//Replicate delta martix appropriately
	// 	RSSVectorMyType largerDelta(size_y, 0);
	// 	size_t index_larger, index_smaller;
	// 	for (size_t r = 0; r < repeat_size; ++r)
	// 	{
	// 		size_t size_temp = p_range*q_range;
	// 		for (size_t i = 0; i < size_temp; ++i)
	// 		{
	// 			index_smaller = r*size_temp/(px*py) + (i/(q_range*py))*beta_range + ((i%q_range)/px);
	// 			index_larger = r*size_temp + (i/q_range)*q_range + (i%q_range);
	// 			largerDelta[index_larger] = deltas[index_smaller];
	// 		}
	// 	}

	// 	funcDotProduct(largerDelta, thatMatrix, deltaMaxPool, size_y);
	// }
/******************************** TODO ****************************************/	
}


