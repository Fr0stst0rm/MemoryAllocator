#pragma once


#ifndef MEMORY_ALLOC_H_
#define MEMORY_ALLOC_H_

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <vector>

constexpr uint16_t POOL_SIZE = 32/8; //1024
constexpr uint16_t CHUNK_SIZE = 8/8;

struct GPMemBlock {
	~GPMemBlock();
	union {
		void* memVoidP = nullptr; //todo mem leaks?
		uint8_t* memByteP;
	};

	GPMemBlock* next = nullptr;
};


class MemoryAllocator {

public:
	MemoryAllocator();
	~MemoryAllocator();

	void* Alloc(uint8_t byteSize, uint8_t alignment); //TODO Alignment
	void Free(void* memoryP);

	uint32_t GetFreeMemBlockCount() const;
	
	GPMemBlock* head; // TODO Private
private:

	void DeAllocAllPools();
	void AllocPool(GPMemBlock * blockPointer);


	std::vector<void*> poolList_m;
};

#endif // !MEMORY_ALLOC_H_
