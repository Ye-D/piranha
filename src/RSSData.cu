/*
 * RSSData.cu
 */

#include "RSSData.h"

template<typename T>
RSSData<T>::RSSData(size_t n) : shareA(n), shareB(n) {
    // nothing
}

template<typename T>
RSSData<T>::RSSData(const SecretShare<T> &a, const SecretShare<T> &b) :
    shareA(a), shareB(b) {
    // nothing 
}
    
template<typename T>
RSSData<T>::~RSSData() {
    // nothing
}

template<typename T>
size_t RSSData<T>::size() const {
    return shareA.size();
}

template<typename T>
void RSSData<T>::resize(size_t n) const {
    shareA.resize(n);
    shareB.resize(n);
}

template<typename T>
void RSSData<T>::zero() {
    shareA.fill(0);
    shareB.fill(0);
}

template<typenname T>
void RSSData<T>::fillKnown(T val) {
    shareA.fill(partyNum == PARTY_A ? val : 0);
    shareB.fill(0);
}

template<typename T>
void RSSData<T>::unzip(RSSData<T> &even, RSSData<T> &odd) {
    gpu::unzip(shareA, even[0], odd[0]);
    gpu::unzip(shareB, even[1], odd[1]);
}

template<typename T>
void RSSData<T>::zip(RSSData<T> &even, RSSData<T> &odd) {
    gpu::zip(shareA, even[0], odd[0]);
    gpu::zip(shareB, even[1], odd[1]);
}

template<typename T>
SecretShare<T>& RSSData<T>::operator [](int i) {
    return i ? shareB : shareA;
}

// Scalar overloads

template<typename T>
RSSData<T> &RSSData<T>::operator+=(const T rhs) {
    if (partyNum == PARTY_A) {
        this->shareA += rhs;
    }
    return *this;
}

template<typename T>
RSSData<T> &RSSData<T>::operator-=(const T rhs) {
    if (partyNum == PARTY_A) {
        this->shareA -= rhs;
    }
    return *this;
}

template<typename T>
RSSData<T> &RSSData<T>::operator*=(const T rhs) {
    this->shareA *= rhs;
    return *this;
}

template<typename T>
RSSData<T> operator+(RSSData<T> lhs, const T &rhs) {
    lhs += rhs;
    return lhs;    
}

template RSSData<uint32_t> operator+(RSSData<uint32_t> lhs, const uint32_t &rhs);
template RSSData<uint8_t> operator+(RSSData<uint8_t> lhs, const uint8_t &rhs);

template<typename T>
RSSData<T> operator-(RSSData<T> lhs, const T &rhs) {
    lhs -= rhs;
    return lhs;    
}

template RSSData<uint32_t> operator-(RSSData<uint32_t> lhs, const uint32_t &rhs);
template RSSData<uint8_t> operator-(RSSData<uint8_t> lhs, const uint8_t &rhs);

template<typename T>
RSSData<T> operator*(RSSData<T> lhs, const T &rhs) {
    lhs *= rhs;
    return lhs;    
}

template RSSData<uint32_t> operator*(RSSData<uint32_t> lhs, const uint32_t &rhs);
template RSSData<uint8_t> operator*(RSSData<uint8_t> lhs, const uint8_t &rhs);

// Element-wise overloads for public values

template<typename T>
RSSData<T> &RSSData<T>::operator+=(const SecretShare<T>& rhs) {
    if (partyNum == PARTY_A) {
        this->shareA += rhs;
    }
    return *this;
}

template<typename T>
RSSData<T> &RSSData<T>::operator-=(const SecretShare<T>& rhs) {
    if (partyNum == PARTY_A) {
        this->shareA -= rhs;
    }
    return *this;
}

template<typename T>
RSSData<T> &RSSData<T>::operator*=(const SecretShare<T>& rhs) {
    this->shareA *= rhs;
    this->shareB *= rhs;
    return *this;
}

template<typename T>
RSSData<T> operator+(RSSData<T> lhs, const SecretShare<T> &rhs) {
    lhs += rhs;
    return lhs;    
}

template RSSData<uint32_t> operator+<uint32_t>(RSSData<uint32_t> lhs, const SecretShare<uint32_t> &rhs);
template RSSData<uint8_t> operator+<uint8_t>(RSSData<uint8_t> lhs, const SecretShare<uint8_t> &rhs);

template<typename T>
RSSData<T> operator-(RSSData<T> lhs, const SecretShare<T> &rhs) {
    lhs -= rhs;
    return lhs;
}

template RSSData<uint32_t> operator-<uint32_t>(RSSData<uint32_t> lhs, const SecretShare<uint32_t> &rhs);
template RSSData<uint8_t> operator-<uint8_t>(RSSData<uint8_t> lhs, const SecretShare<uint8_t> &rhs);

template<typename T>
RSSData<T> operator*(RSSData<T> lhs, const SecretShare<T> &rhs) {
    lhs *= rhs;
    return lhs;    
}

template RSSData<uint32_t> operator*<uint32_t>(RSSData<uint32_t> lhs, const SecretShare<uint32_t> &rhs);
template RSSData<uint8_t> operator*<uint8_t>(RSSData<uint8_t> lhs, const SecretShare<uint8_t> &rhs);

// Element-wise overloads

template<typename T>
RSSData<T> &RSSData<T>::operator+=(const RSSData<T>& rhs) {
    this->shareA += rhs.shareA;
    this->shareB += rhs.shareB;
    return *this;
}

template<typename T>
RSSData<T> &RSSData<T>::operator-=(const RSSData<T>& rhs) {
    this->shareA -= rhs.shareA;
    this->shareB -= rhs.shareB;
    return *this;
}

template<typename T>
RSSData<T> &RSSData<T>::operator*=(const RSSData<T>& rhs) {
    SecretShare<T> c = this->shareA * rhs->shareA;
    c += this->shareB * rhs->shareA;
    c += this->shareA * rhs->shareB;

    NEW_funcReshare(c, *this);
    return *this;
}

template<typename T>
RSSData<T> &RSSData<T>::operator^=(const RSSData<T>& rhs) {
    this->shareA ^= rhs.shareA;
    this->shareB ^= rhs.shareB;
    return *this;
}

template<typename T>
RSSData<T> operator+(RSSData<T> lhs, const RSSData<T> &rhs) {
    lhs += rhs;
    return lhs;    
}

template RSSData<uint32_t> operator+<uint32_t>(RSSData<uint32_t> lhs, const RSSData<uint32_t> &rhs);
template RSSData<uint8_t> operator+<uint8_t>(RSSData<uint8_t> lhs, const RSSData<uint8_t> &rhs);

template<typename T>
RSSData<T> operator-(RSSData<T> lhs, const RSSData<T> &rhs) {
    lhs -= rhs;
    return lhs;    
}

template RSSData<uint32_t> operator-<uint32_t>(RSSData<uint32_t> lhs, const RSSData<uint32_t> &rhs);
template RSSData<uint8_t> operator-<uint8_t>(RSSData<uint8_t> lhs, const RSSData<uint8_t> &rhs);

template<typename T>
RSSData<T> operator*(RSSData<T> lhs, const RSSData<T> &rhs) {
    lhs *= rhs;
    return lhs;    
}

template RSSData<uint32_t> operator*<uint32_t>(RSSData<uint32_t> lhs, const RSSData<uint32_t> &rhs);
template RSSData<uint8_t> operator*<uint8_t>(RSSData<uint8_t> lhs, const RSSData<uint8_t> &rhs);

template<typename T>
RSSData<T> operator^(RSSData<T> lhs, const RSSData<T> &rhs) {
    lhs ^= rhs;
    return lhs;
}

template RSSData<uint32_t> operator^<uint32_t>(RSSData<uint32_t> lhs, const RSSData<uint32_t> &rhs);
template RSSData<uint8_t> operator^<uint8_t>(RSSData<uint8_t> lhs, const RSSData<uint8_t> &rhs);

template class RSSData<uint32_t>;
template class RSSData<uint8_t>;
