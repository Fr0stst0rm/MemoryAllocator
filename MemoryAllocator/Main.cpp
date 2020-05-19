#include "MemoryAllocator.h"
#include <iostream>
#include <cassert>
#include <chrono>

constexpr uint8_t SIZE = 1000;

void main() {
	auto start = std::chrono::high_resolution_clock::now();
	auto finish = std::chrono::high_resolution_clock::now();

	uint8_t* test[SIZE];

	int tests = 10;

	long allocTime = 0;
	long freeTime = 0;

	std::cout << "MemoryAllocator\n";
	
	// Memlear und speed test
	for (int c = 0; c < tests; c++) {
		MemoryAllocator* memAlloc = new MemoryAllocator();
	
		//std::cout << memAlloc.GetFreeMemBlockCount() << "\n";

		start = std::chrono::high_resolution_clock::now();

		for (int i = 0; i < SIZE; i++) {
			test[i] = reinterpret_cast<uint8_t*>(memAlloc->Alloc(sizeof(uint8_t)));
			*test[i] = i + 1;
		}

		finish = std::chrono::high_resolution_clock::now();
		allocTime += std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
		//std::cout << "Alloc: " << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << "ns\n";
		start = std::chrono::high_resolution_clock::now();

		for (int i = 0; i < SIZE; i++) {
			memAlloc->Free(reinterpret_cast<uint8_t*>(test[i]));
		}

		finish = std::chrono::high_resolution_clock::now();
		freeTime += std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();
		//std::cout << "Free: " << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << "ns\n";
		
		delete memAlloc;
	}

	std::cout << "Avg time " << tests << " tests:\n";
	std::cout << "Alloc: " << (allocTime/tests) << "\n";
	std::cout << "Free: " << (freeTime / tests) << "\n";

	std::cout << "Malloc\n";

	start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < SIZE; i++) {
		test[i] = static_cast<uint8_t*>(malloc(sizeof(uint8_t)));
		*test[i] = i + 1;
	}

	finish = std::chrono::high_resolution_clock::now();
	std::cout << "Alloc: " << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << "ns\n";
	start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < SIZE; i++) {
		free(test[i]);
	}

	finish = std::chrono::high_resolution_clock::now();
	std::cout << "Free: " << std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count() << "ns\n";

	std::cout << "End\n";


	/*
	for (int i = 0; i < memAlloc.poolList_m.size(); i++) {
		std::cout << "Pool" << i << " " << memAlloc.poolList_m.at(i) << "\n";
	}

	std::cout << memAlloc.GetFreeMemBlockCount() << "\n";

	//Byte Order test
	MemoryAllocator testPool;

	uint8_t* v1 = reinterpret_cast<uint8_t*>(testPool.Alloc(sizeof(uint8_t)));
	*v1 = 1;

	uint8_t* v2 = reinterpret_cast<uint8_t*>(testPool.Alloc(sizeof(uint8_t)));
	*v2 = 2;

	assert(v2 == v1 + sizeof(uint8_t));

	/*
	//Byte size test
	MemoryAllocator testPool2;

	uint8_t* v3 = reinterpret_cast<uint8_t*>(testPool2.Alloc(CHUNK_SIZE+1));
	*/
}