#include "MemoryAllocator.h"
#include <iostream>

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
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[1] = 11;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[2] = 10;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[3] = 11;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[4] = 10;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[5] = 11;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[6] = 10;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[7] = 11;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[8] = 10;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[9] = 11;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[10] = 10;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[11] = 11;
	reinterpret_cast<uint8_t*>(memAlloc.poolList_m.at(3))[12] = 10;

	for (int i = SIZE-1; i >= 10; i--) {
		memAlloc.Free(reinterpret_cast<uint8_t*>(test[i]));
	}

	for (int i = 0; i < memAlloc.poolList_m.size(); i++) {
		std::cout << "Pool" << i << " " << memAlloc.poolList_m.at(i) << "\n";
	}

	std::cout << memAlloc.GetFreeMemBlockCount() << "\n";
}