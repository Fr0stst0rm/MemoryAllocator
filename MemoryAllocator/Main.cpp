#include "MemoryAllocator.h"
#include <iostream>
#include <cassert>

constexpr uint8_t SIZE = 15;

void main() {
	MemoryAllocator memAlloc;

	std::cout << memAlloc.GetFreeMemBlockCount() << "\n";
	
	uint8_t** test = new uint8_t*[SIZE];
	

	for (int i = 0; i < SIZE; i++) {
		test[i] = reinterpret_cast<uint8_t*>(memAlloc.Alloc(sizeof(uint8_t)));
		*test[i] = i + 1;
	}

	for (int i = 0; i < memAlloc.poolList_m.size(); i++) {
		std::cout << "Pool" << i << " " << memAlloc.poolList_m.at(i) << "\n";
	} 

	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[0] = 10;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[100] = 11;


	for (int i = SIZE-1; i >= 10; i--) {
		memAlloc.Free(reinterpret_cast<uint8_t*>(test[i]));
	}

	for (int i = 0; i < memAlloc.poolList_m.size(); i++) {
		std::cout << "Pool" << i << " " << memAlloc.poolList_m.at(i) << "\n";
	}

	std::cout << memAlloc.GetFreeMemBlockCount() << "\n";

	//Raffi test
	MemoryAllocator testPool;

	uint8_t* v1 = reinterpret_cast<uint8_t*>(testPool.Alloc(sizeof(uint8_t)));
	*v1 = 1;

	uint8_t* v2 = reinterpret_cast<uint8_t*>(testPool.Alloc(sizeof(uint8_t)));
	*v2 = 2;

	assert(v2 == v1 + sizeof(uint8_t));
}