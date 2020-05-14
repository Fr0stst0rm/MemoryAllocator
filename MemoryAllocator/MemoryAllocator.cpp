#include <cassert>
#include "MemoryAllocator.h"

MemoryAllocator::MemoryAllocator() {
	head = new GPMemBlock();
	AllocPool(head);
}

MemoryAllocator::~MemoryAllocator() {
	DeAllocAllPools();
	delete head;
}

void* MemoryAllocator::Alloc(uint8_t byteSize) {

	assert(byteSize <= CHUNK_SIZE);
	assert(head != nullptr);
	assert(head->next != nullptr);

	void* freeBlock = head->memVoidP;
	head = head->next;

	if (head->next == nullptr) {
		head->next = new GPMemBlock();
		AllocPool(head->next);
	}

	return freeBlock;
}

void MemoryAllocator::AllocPool(GPMemBlock* blockPointer) {
	
	uint8_t* pool = static_cast<uint8_t*>(malloc(POOL_SIZE));

	assert(pool != nullptr);

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

	assert(head != nullptr);

	GPMemBlock* current = head;

	while (current->next != nullptr) {
		count++;
		current = current->next;
	}
	count++;

	return count;
}

void MemoryAllocator::Free(void* memoryP) {

	bool partOfPool = false;
	for (unsigned int i = 0; (!partOfPool) && (i < poolList_m.size()); ++i) {
		if ((memoryP >= (poolList_m.at(i))) && (memoryP <= (reinterpret_cast<uint8_t*>(poolList_m.at(i)) + (POOL_SIZE - 1)))) {
			partOfPool = true;
		}
	}

	if (partOfPool) {
		GPMemBlock* insertAfter = nullptr;
		GPMemBlock* insertBefore = head;

		assert(insertBefore != nullptr);

		while ((insertBefore != nullptr) && (memoryP > insertBefore->memVoidP)) {
			insertAfter = insertBefore;
			assert(insertBefore != nullptr);
			insertBefore = insertBefore->next;
		}

		GPMemBlock* newBlock = new GPMemBlock();
		newBlock->memVoidP = memoryP;

		if (insertAfter != nullptr) { //todo auch assert?
			insertAfter->next = newBlock;
		}

		newBlock->next = insertBefore;

		if (memoryP < head->memVoidP) {
			head = newBlock;
		}


		// Check if pool can be deleted
		if (GetFreeMemBlockCount() >= (2 * (POOL_SIZE / CHUNK_SIZE))) {
			std::cout << "Much free space " << GetFreeMemBlockCount() << " >= " << (2 * (POOL_SIZE / CHUNK_SIZE)) << "\n";

			std::vector<GPMemBlock*>* memCount = new std::vector<GPMemBlock*>[poolList_m.size()];

			GPMemBlock* last;
			for (int i = poolList_m.size() - 1; i >= 0; --i) { //todo problem with unsigned int because of obvious reasons
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

			GPMemBlock* removeFrom = nullptr;
			GPMemBlock* removeTo = nullptr;
			for (unsigned int i = 0; i < poolList_m.size(); ++i)
			{
				//std::cout << "Pool " << i << " Free: " << memCount[i].size() << "\n";

				if (memCount[i].size() == (POOL_SIZE / CHUNK_SIZE)) {
					std::cout << "Deleting pool " << i << "\n";

					GPMemBlock* current = head;
					assert(current != nullptr);
					
					//std::cout << "Head " << head->memVoidP << "\n";
					while (current->next != nullptr) {
						//std::cout << "Current " << current->memVoidP << "\n";
						assert(*memCount[i].begin() != nullptr);

						if ((*memCount[i].begin())->memVoidP == current->next->memVoidP) {
							removeFrom = current;
						}

						assert(*(memCount[i].end() - 1) != nullptr);

						if ((*(memCount[i].end() - 1))->memVoidP == current->memVoidP) {
							removeTo = current->next;
						}
						current = current->next;
					}


					if (removeFrom == nullptr) {
						removeFrom = removeTo;
						head = removeFrom;
					}
					else {
						removeFrom->next = removeTo;
					}

					/*
					std::cout << "From " << removeFrom->memVoidP << "\n";
					std::cout << "From->next " << removeFrom->next->memVoidP << "\n";
					std::cout << "End->To " << (*(memCount[i].end() - 1))->next->memVoidP << "\n";
					std::cout << "To " << removeTo->memVoidP << "\n";
					
					std::cout << "Delete from Start " << (*memCount[i].begin())->memVoidP << "\n";
					std::cout << "Delete to End " << (*(memCount[i].end() - 1))->memVoidP << "\n";
					*/
					assert(*(memCount[i].end() - 1) != nullptr);
					
					(*(memCount[i].end() - 1))->next = nullptr;
					free((*memCount[i].begin())->memVoidP);
					delete* memCount[i].begin();

					poolList_m.erase(poolList_m.begin() + i);
					break;
				}
			}
		}
	}
}

void MemoryAllocator::DeAllocAllPools() {

	for (int i = poolList_m.size() - 1; i >= 0; --i) {
		free(poolList_m.at(i));
		poolList_m.pop_back();
	}
}

GPMemBlock::~GPMemBlock() {
	delete next;
}
