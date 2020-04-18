#ifndef __KVECTOR_H__
#define __KVECTOR_H__

#include "Kernel/Types.h"
#include "Memory/KMemoryManager.h"
#include "Util/Memcpy.h"

#define STARTING_VECTOR_SIZE 10
#define EXTRA_SPACE_ON_SET_CAPACITY 10

template <class T> 
class KVector {
public:
    KVector();
    KVector(const KVector& kVector);
    T get(int index);
    void put(T item, int index);
    void remove(int index);
    int setCapcity(int desiredCapacity);
    void push(T item);
    int size();
private:
    T *buffer;
    int numItems;
    int capacity;

    void shiftLeft(int startIndex);
};

template <class T>
KVector<T>::KVector() {
    buffer = (T*)KMM.kmalloc(STARTING_VECTOR_SIZE * sizeof(T));
    numItems = 0;
    capacity = STARTING_VECTOR_SIZE;
}

template <class T>
KVector<T>::KVector(const KVector& kVector) {
    buffer = (T*)KMM.kmalloc(kVector.capacity * sizeof(T));
    memory_copy((char*)kVector.buffer, (char*)buffer, sizeof(T) * kVector.numItems);
    numItems = kVector.numItems;
    capacity = kVector.capacity;
}

template <class T>
T KVector<T>::get(int index) {
    T basic;
    if (index > numItems) {
        kprint("KVector: Index > numItems");
        return basic;
    }
    return buffer[index];
}

template <class T>
void KVector<T>::put(T item, int index) {
    if (index > numItems) {
        kprint("KVector: Out of bounds\n");
        return;
    }
    if (index > capacity) {
        setCapcity(index + EXTRA_SPACE_ON_SET_CAPACITY);
    }
    buffer[index] = item;
    numItems++;
}

template <class T>
void KVector<T>::push(T item) {
    put(item, numItems);
}
template <class T>
void KVector<T>::remove(int index) {
    if (index >= numItems) {
        kprint("Vector: Out of bounds\n");
    }
    shiftLeft(index);
}

template <class T>
int KVector<T>::setCapcity(int desiredCapacity) {
    T* oldBuffer = buffer;
    buffer = (T*)KMM.kmalloc(desiredCapacity);
    memory_copy((char*)oldBuffer, (char*)buffer, sizeof(T) * numItems);
    KMM.free(oldBuffer);
}

template <class T>
void KVector<T>::shiftLeft(int index) {
    while (index < numItems - 1) {
        buffer[index] = buffer[index + 1];
        index++;
    }
    numItems--;
}

template <class T>
int KVector<T>::size() {
    return numItems;
}


#endif