#include "PmergeMe.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>

PmergeMe::PmergeMe() {}

PmergeMe::~PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe& other) {
	*this = other;
}

PmergeMe& PmergeMe::operator=(const PmergeMe& other) {
	if (this != &other) {
		this->_vectorData = other._vectorData;
		this->_dequeData = other._dequeData;
	}
	return *this;
}

bool PmergeMe::parseInput(int argc, char** argv) {
	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		
		// Check if it's a valid positive integer
		if (arg.empty() || arg[0] == '-') {
			std::cerr << "Error" << std::endl;
			return false;
		}

		for (size_t j = 0; j < arg.length(); ++j) {
			if (!isdigit(arg[j])) {
				std::cerr << "Error" << std::endl;
				return false;
			}
		}

		long num = std::atol(arg.c_str());
		if (num < 0 || num > 2147483647) {
			std::cerr << "Error" << std::endl;
			return false;
		}

		_vectorData.push_back(static_cast<int>(num));
		_dequeData.push_back(static_cast<int>(num));
	}

	return !_vectorData.empty();
}

// Generate Jacobsthal sequence for optimal insertion order
void PmergeMe::generateJacobsthalSequence(std::vector<size_t>& sequence, size_t n) {
	if (n == 0)
		return;
	
	// Generate Jacobsthal numbers: J(0)=0, J(1)=1, J(n)=J(n-1)+2*J(n-2)
	std::vector<size_t> jacobsthal;
	jacobsthal.push_back(0);
	jacobsthal.push_back(1);
	
	size_t idx = 2;
	while (true) {
		size_t next = jacobsthal[idx - 1] + 2 * jacobsthal[idx - 2];
		if (next >= n)
			break;
		jacobsthal.push_back(next);
		++idx;
	}
	
	// Build insertion sequence using Jacobsthal numbers
	sequence.clear();
	size_t pos = 1;
	
	for (size_t i = 2; i < jacobsthal.size(); ++i) {
		size_t current = jacobsthal[i];
		size_t prev = jacobsthal[i - 1];
		
		// Add indices from current down to prev + 1
		for (size_t j = current; j > prev && j < n; --j) {
			sequence.push_back(j);
		}
	}
	
	// Add remaining elements
	size_t last = jacobsthal.empty() ? 0 : jacobsthal.back();
	for (size_t i = last + 1; i < n; ++i) {
		sequence.push_back(i);
	}
}

// Binary search to find insertion position in sorted array
int PmergeMe::binarySearchVector(const std::vector<int>& arr, int item, int left, int right) {
	while (left <= right) {
		int mid = left + (right - left) / 2;
		
		if (arr[mid] == item)
			return mid;
		else if (arr[mid] < item)
			left = mid + 1;
		else
			right = mid - 1;
	}
	return left;
}

// Ford-Johnson algorithm (merge-insertion sort) for vector
void PmergeMe::fordJohnsonSortVector(std::vector<int>& arr) {
	size_t n = arr.size();
	if (n <= 1)
		return;
	
	// Handle small arrays with simple insertion
	if (n <= 20) {
		for (size_t i = 1; i < n; ++i) {
			int key = arr[i];
			int pos = binarySearchVector(arr, key, 0, i - 1);
			
			// Shift elements
			for (int j = i; j > pos; --j) {
				arr[j] = arr[j - 1];
			}
			arr[pos] = key;
		}
		return;
	}
	
	// Step 1: Create pairs and sort each pair
	std::vector<std::pair<int, int> > pairs;
	bool hasStraggler = (n % 2 == 1);
	int straggler = hasStraggler ? arr[n - 1] : 0;
	
	for (size_t i = 0; i + 1 < n; i += 2) {
		if (arr[i] > arr[i + 1])
			pairs.push_back(std::make_pair(arr[i], arr[i + 1]));
		else
			pairs.push_back(std::make_pair(arr[i + 1], arr[i]));
	}
	
	// Step 2: Recursively sort pairs by their larger element
	std::vector<int> largers;
	for (size_t i = 0; i < pairs.size(); ++i) {
		largers.push_back(pairs[i].first);
	}
	fordJohnsonSortVector(largers);
	
	// Reorder pairs based on sorted larger elements
	std::vector<std::pair<int, int> > sortedPairs;
	for (size_t i = 0; i < largers.size(); ++i) {
		for (size_t j = 0; j < pairs.size(); ++j) {
			if (pairs[j].first == largers[i]) {
				sortedPairs.push_back(pairs[j]);
				break;
			}
		}
	}
	
	// Step 3: Build main chain with larger elements
	std::vector<int> mainChain;
	std::vector<int> pend;
	
	for (size_t i = 0; i < sortedPairs.size(); ++i) {
		mainChain.push_back(sortedPairs[i].first);
		pend.push_back(sortedPairs[i].second);
	}
	
	// Step 4: Insert first small element at beginning
	if (!pend.empty()) {
		mainChain.insert(mainChain.begin(), pend[0]);
		pend.erase(pend.begin());
	}
	
	// Step 5: Insert remaining elements using Jacobsthal sequence
	if (!pend.empty()) {
		std::vector<size_t> insertionOrder;
		generateJacobsthalSequence(insertionOrder, pend.size());
		
		for (size_t i = 0; i < insertionOrder.size(); ++i) {
			size_t idx = insertionOrder[i];
			if (idx < pend.size()) {
				int item = pend[idx];
				int pos = binarySearchVector(mainChain, item, 0, mainChain.size() - 1);
				mainChain.insert(mainChain.begin() + pos, item);
			}
		}
	}
	
	// Insert straggler if exists
	if (hasStraggler) {
		int pos = binarySearchVector(mainChain, straggler, 0, mainChain.size() - 1);
		mainChain.insert(mainChain.begin() + pos, straggler);
	}
	
	// Copy back to original array
	arr = mainChain;
}

void PmergeMe::mergeInsertSortVector(std::vector<int>& arr) {
	fordJohnsonSortVector(arr);
}

// Binary search for deque
int PmergeMe::binarySearchDeque(const std::deque<int>& arr, int item, int left, int right) {
	while (left <= right) {
		int mid = left + (right - left) / 2;
		
		if (arr[mid] == item)
			return mid;
		else if (arr[mid] < item)
			left = mid + 1;
		else
			right = mid - 1;
	}
	return left;
}

// Ford-Johnson algorithm (merge-insertion sort) for deque
void PmergeMe::fordJohnsonSortDeque(std::deque<int>& arr) {
	size_t n = arr.size();
	if (n <= 1)
		return;
	
	// Handle small arrays with simple insertion
	if (n <= 20) {
		for (size_t i = 1; i < n; ++i) {
			int key = arr[i];
			int pos = binarySearchDeque(arr, key, 0, i - 1);
			
			// Shift elements
			for (int j = i; j > pos; --j) {
				arr[j] = arr[j - 1];
			}
			arr[pos] = key;
		}
		return;
	}
	
	// Step 1: Create pairs and sort each pair
	std::vector<std::pair<int, int> > pairs;
	bool hasStraggler = (n % 2 == 1);
	int straggler = hasStraggler ? arr[n - 1] : 0;
	
	for (size_t i = 0; i + 1 < n; i += 2) {
		if (arr[i] > arr[i + 1])
			pairs.push_back(std::make_pair(arr[i], arr[i + 1]));
		else
			pairs.push_back(std::make_pair(arr[i + 1], arr[i]));
	}
	
	// Step 2: Recursively sort pairs by their larger element
	std::deque<int> largers;
	for (size_t i = 0; i < pairs.size(); ++i) {
		largers.push_back(pairs[i].first);
	}
	fordJohnsonSortDeque(largers);
	
	// Reorder pairs based on sorted larger elements
	std::vector<std::pair<int, int> > sortedPairs;
	for (size_t i = 0; i < largers.size(); ++i) {
		for (size_t j = 0; j < pairs.size(); ++j) {
			if (pairs[j].first == largers[i]) {
				sortedPairs.push_back(pairs[j]);
				break;
			}
		}
	}
	
	// Step 3: Build main chain with larger elements
	std::deque<int> mainChain;
	std::vector<int> pend;
	
	for (size_t i = 0; i < sortedPairs.size(); ++i) {
		mainChain.push_back(sortedPairs[i].first);
		pend.push_back(sortedPairs[i].second);
	}
	
	// Step 4: Insert first small element at beginning
	if (!pend.empty()) {
		mainChain.push_front(pend[0]);
		pend.erase(pend.begin());
	}
	
	// Step 5: Insert remaining elements using Jacobsthal sequence
	if (!pend.empty()) {
		std::vector<size_t> insertionOrder;
		generateJacobsthalSequence(insertionOrder, pend.size());
		
		for (size_t i = 0; i < insertionOrder.size(); ++i) {
			size_t idx = insertionOrder[i];
			if (idx < pend.size()) {
				int item = pend[idx];
				int pos = binarySearchDeque(mainChain, item, 0, mainChain.size() - 1);
				mainChain.insert(mainChain.begin() + pos, item);
			}
		}
	}
	
	// Insert straggler if exists
	if (hasStraggler) {
		int pos = binarySearchDeque(mainChain, straggler, 0, mainChain.size() - 1);
		mainChain.insert(mainChain.begin() + pos, straggler);
	}
	
	// Copy back to original array
	arr = mainChain;
}

void PmergeMe::mergeInsertSortDeque(std::deque<int>& arr) {
	fordJohnsonSortDeque(arr);
}

void PmergeMe::sort() {
	struct timeval start, end;

	// Display before sorting
	std::cout << "Before: ";
	for (size_t i = 0; i < _vectorData.size() && i < 5; ++i) {
		std::cout << _vectorData[i] << " ";
	}
	if (_vectorData.size() > 5)
		std::cout << "[...]";
	std::cout << std::endl;

	// Sort with vector
	gettimeofday(&start, NULL);
	mergeInsertSortVector(_vectorData);
	gettimeofday(&end, NULL);
	double vectorTime = (end.tv_sec - start.tv_sec) * 1000000.0 + 
	                    (end.tv_usec - start.tv_usec);

	// Sort with deque
	gettimeofday(&start, NULL);
	mergeInsertSortDeque(_dequeData);
	gettimeofday(&end, NULL);
	double dequeTime = (end.tv_sec - start.tv_sec) * 1000000.0 + 
	                   (end.tv_usec - start.tv_usec);

	displayResults(vectorTime, dequeTime);
}

void PmergeMe::displayResults(double vectorTime, double dequeTime) const {
	std::cout << "After:  ";
	for (size_t i = 0; i < _vectorData.size() && i < 5; ++i) {
		std::cout << _vectorData[i] << " ";
	}
	if (_vectorData.size() > 5)
		std::cout << "[...]";
	std::cout << std::endl;

	std::cout << "Time to process a range of " << _vectorData.size() 
	          << " elements with std::vector : " << vectorTime << " us" << std::endl;
	std::cout << "Time to process a range of " << _dequeData.size() 
	          << " elements with std::deque : " << dequeTime << " us" << std::endl;
}
