#include "MemoryAllocator.h"

MemoryAllocator::MemoryAllocator() {
	head = new GPMemBlock();
	AllocPool(head);
}

MemoryAllocator::~MemoryAllocator() {
	DeAllocAllPools();
	delete head;
}

void* MemoryAllocator::Alloc(uint8_t byte, uint8_t alignment) {

	void* freeBlock = head->memVoidP;
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
	currentBlock->memVoidP = pool;
	GPMemBlock* nextBlock;

	for (int n = (CHUNK_SIZE); n < (POOL_SIZE); n += (CHUNK_SIZE)) {
		nextBlock = new GPMemBlock();
		nextBlock->memVoidP = pool + n;
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

	// TODO Check if mem is part of our pools

	GPMemBlock* insertAfter = nullptr;
	GPMemBlock* insertBefore = head;

	while ((insertBefore != nullptr) && (memoryP > insertBefore->memVoidP)) {
		insertAfter = insertBefore;
		insertBefore = insertBefore->next;
	}

	GPMemBlock* newBlock = new GPMemBlock();
	newBlock->memVoidP = memoryP;

	if (insertAfter != nullptr) {
		insertAfter->next = newBlock;
	}

	newBlock->next = insertBefore;

	if (memoryP < head->memVoidP) {
		head = newBlock;
	}


	// Check if pool can be deleted
	std::cout << GetFreeMemBlockCount() << " >= " << (2 * (POOL_SIZE / CHUNK_SIZE)) << "\n";
	if (GetFreeMemBlockCount() >= (2 * (POOL_SIZE / CHUNK_SIZE))) {

		std::vector<GPMemBlock*>* memCount = new std::vector<GPMemBlock*>[poolList_m.size()];

		GPMemBlock* last;
		for (int i = poolList_m.size() - 1; i >= 0; i--) {
			memCount[i] = std::vector <GPMemBlock*>();

			last = head;
			int memIndex = 0;
			while (last != nullptr) {

				//std::cout << last->memory << "\n";
				//std::cout << poolList_m.at(i) << "\n";
				//std::cout << reinterpret_cast<void*>(reinterpret_cast<uint8_t*>(poolList_m.at(i)) + (POOL_SIZE - 1)) << "\n";

				if ((last->memVoidP >= (poolList_m.at(i))) && (last->memVoidP <= (reinterpret_cast<uint8_t*>(poolList_m.at(i)) + (POOL_SIZE - 1)))) {
					memCount[i].push_back(last);
					memIndex++;
				}
				last = last->next;
			}
		}

		insertAfter = nullptr;
		insertBefore = nullptr;
		for (int i = 0; i < poolList_m.size(); i++)
		{
			std::cout << "Pool " << i << " Free: " << memCount[i].size() << "\n";

			if (memCount[i].size() == (POOL_SIZE / CHUNK_SIZE)) {
				std::cout << "Deleting pool " << i << "\n";

				std::cout << "Start " << (*memCount[i].begin())->memVoidP << "\n";
				std::cout << "End " << (*(memCount[i].end()-1))->memVoidP << "\n";

				GPMemBlock* current = head;
				while (current->next != nullptr) {
					std::cout << "Current " << current->memVoidP << "\n";
					if ((*memCount[i].begin())->memVoidP == current->memVoidP) {
						insertAfter = current;
					}
					if ((*(memCount[i].end() - 1))->memVoidP == current->memVoidP) {
						insertBefore = current;
					}
					current = current->next;
				}

				if (insertAfter == nullptr) {
					insertAfter = insertBefore;
				}
				else {
					insertAfter->next = insertBefore;
				}

				(*(memCount[i].end() - 1))->next = nullptr;
				delete* memCount[i].begin();

				poolList_m.erase(poolList_m.begin() + i);
				break;
			}
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
