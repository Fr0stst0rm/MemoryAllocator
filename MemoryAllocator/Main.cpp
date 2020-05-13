#include "MemoryAllocator.h"
#include <iostream>

constexpr uint8_t SIZE = 15;

void main() {
	MemoryAllocator memAlloc;

	std::cout << memAlloc.GetFreeMemBlockCount() << "\n";
	
	uint8_t** test = new uint8_t*[SIZE];
	

	for (int i = 0; i < SIZE; i++) {
		test[i] = reinterpret_cast<uint8_t*>(memAlloc.Alloc(sizeof(uint8_t),1));
		*test[i] = i + 1;
	}

	for (int i = SIZE-1; i >= 10; i--) {
		memAlloc.Free(reinterpret_cast<uint8_t*>(test[i]));
	}

	std::cout << memAlloc.GetFreeMemBlockCount() << "\n";
}