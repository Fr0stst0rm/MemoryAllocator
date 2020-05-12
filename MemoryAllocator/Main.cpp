#include "MemoryAllocator.h"
#include <iostream>

constexpr uint8_t SIZE = 15;

void main() {
	MemoryAllocator memAlloc;

	std::cout << memAlloc.GetFreeMemBlockCount() << "\n";
	
	uint32_t** test = new uint32_t*[SIZE];
	

	for (int i = 0; i < SIZE; i++) {
		test[i] = reinterpret_cast<uint32_t*>(memAlloc.Alloc());
		*test[i] = i + 1;
	}

	for (int i = SIZE-1; i >= 10; i--) {
		memAlloc.Free(reinterpret_cast<uint32_t*>(test[i]));
	}

	std::cout << memAlloc.GetFreeMemBlockCount() << "\n";
}