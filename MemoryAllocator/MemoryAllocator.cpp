#include "assert.h"
#include "MemoryAllocator.h"

MemoryAllocator::MemoryAllocator() {
	head = new GPMemBlock();
	AllocPool(head);
}

MemoryAllocator::~MemoryAllocator() {
	DeAllocAllPools();
	delete head;
}

void* MemoryAllocator::Alloc(uint8_t byteSize, uint8_t alignment) {

	if (byteSize > CHUNK_SIZE) {
		std::cout << "Error! Byte size to big\n";
		return nullptr;
	}

	assert(head != nullptr);
	void* freeBlock = head->memVoidP;
	
	assert(head->next != nullptr);
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
	for (int i = 0; (!partOfPool) && (i < poolList_m.size()); i++) {
		if ((memoryP >= (poolList_m.at(i))) && (memoryP <= (reinterpret_cast<uint8_t*>(poolList_m.at(i)) + (POOL_SIZE - 1)))) {
			partOfPool = true;
		}
	}

	if (partOfPool) {
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
		if (GetFreeMemBlockCount() >= (2 * (POOL_SIZE / CHUNK_SIZE))) {
			std::cout << "Much free space " << GetFreeMemBlockCount() << " >= " << (2 * (POOL_SIZE / CHUNK_SIZE)) << "\n";

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

			GPMemBlock* removeFrom = nullptr;
			GPMemBlock* removeTo = nullptr;
			for (int i = 0; i < poolList_m.size(); i++)
			{
				//std::cout << "Pool " << i << " Free: " << memCount[i].size() << "\n";

				if (memCount[i].size() == (POOL_SIZE / CHUNK_SIZE)) {
					std::cout << "Deleting pool " << i << "\n";

					GPMemBlock* current = head;
					//std::cout << "Head " << head->memVoidP << "\n";
					while (current->next != nullptr) {
						//std::cout << "Current " << current->memVoidP << "\n";
						if ((*memCount[i].begin())->memVoidP == current->next->memVoidP) {
							removeFrom = current;
						}
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

					(*(memCount[i].end() - 1))->next = nullptr;
					delete* memCount[i].begin();

					poolList_m.erase(poolList_m.begin() + i);
					break;
				}
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
