#include "MemoryAllocator.h"

MemoryAllocator::MemoryAllocator() {
	head = new GPMemBlock();
	AllocPool(head);
}

MemoryAllocator::~MemoryAllocator() {
	DeAllocAllPools();
	delete head;
}

void* MemoryAllocator::Alloc() {
	void* freeBlock = head->memory;
	head = head->next;

	if (head->next == nullptr) {
		head->next = new GPMemBlock();
		AllocPool(head->next);
	}

	return freeBlock;
}

void MemoryAllocator::AllocPool(GPMemBlock* blockPointer) {
	uint8_t* pool;

	pool = (uint8_t*)malloc(POOL_SIZE);

	if (pool == nullptr) {
		std::cout << "Allocation error";
		exit(1);
	}



	GPMemBlock* currentBlock = blockPointer;
	currentBlock->memory = pool;
	GPMemBlock* nextBlock;

	for (int n = (CHUNK_SIZE); n < (POOL_SIZE); n += (CHUNK_SIZE)) {
		nextBlock = new GPMemBlock();
		nextBlock->memory = pool + n;
		currentBlock->next = nextBlock;
		currentBlock = nextBlock;
	}

	poolList_m.push_back(pool);
}

uint32_t MemoryAllocator::GetFreeMemBlockCount() const {
	uint32_t count = 0;

	GPMemBlock* current = head;

	while (current->next != nullptr) {
		count++;
		current = current->next;
	}
	count++;

	return count;
}

void MemoryAllocator::Free(void* memoryP) {

	GPMemBlock* insertAfter = nullptr;
	GPMemBlock* insertBefore = head;

	while ((memoryP > insertBefore->memory) && (insertBefore != nullptr)) {
		insertAfter = insertBefore;
		insertBefore = insertBefore->next;
	}

	GPMemBlock* newBlock = new GPMemBlock();
	newBlock->memory = memoryP;

	if (insertAfter != nullptr) {
		insertAfter->next = newBlock;
	}

	newBlock->next = insertBefore;

	if (memoryP < head->memory) {
		head = newBlock;
	}


	// Check if pool can be deleted
	std::cout << GetFreeMemBlockCount() << " >= " << (2 * (POOL_SIZE / CHUNK_SIZE)) << "\n";
	if (GetFreeMemBlockCount() >= (2 * (POOL_SIZE / CHUNK_SIZE))) {

		uint32_t* indexCount = new uint32_t[poolList_m.size()];

		GPMemBlock* last;
		for (int i = poolList_m.size() - 1; i >= 0; i--) {
			indexCount[i] = 0;

			last = head;
			while (last != nullptr) {

				//std::cout << last->memory << "\n";
				//std::cout << poolList_m.at(i) << "\n";
				//std::cout << reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(poolList_m.at(i)) + (POOL_SIZE - 1)) << "\n";

				if ((last->memory >= (poolList_m.at(i))) && (last->memory <= (reinterpret_cast<uint8_t*>(poolList_m.at(i)) + (POOL_SIZE - 1)))) {
					indexCount[i] ++;
				}
				last = last->next;
			}
		}

		for (int i = 0; i < poolList_m.size(); i++)
		{
			std::cout << "Pool " << i << " Free: " << indexCount[i] << "\n";
		}
	}
}

void MemoryAllocator::DeAllocAllPools() {

	for (int i = poolList_m.size() - 1; i >= 0; i--) {
		free(poolList_m.at(i));
		poolList_m.pop_back();
	}
}

GPMemBlock::~GPMemBlock() {
	delete next;
}
