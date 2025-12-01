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

void PmergeMe::generateJacobsthalSequence(std::vector<size_t>& sequence, size_t n) {
	if (n == 0)
		return;
	
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
	
	sequence.clear();
	
	for (size_t i = 2; i < jacobsthal.size(); ++i) {
		size_t current = jacobsthal[i];
		size_t prev = jacobsthal[i - 1];
		
		for (size_t j = current; j > prev && j < n; --j) {
			sequence.push_back(j);
		}
	}

	size_t last = jacobsthal.empty() ? 0 : jacobsthal.back();
	for (size_t i = last + 1; i < n; ++i) {
		sequence.push_back(i);
	}
}


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


void PmergeMe::mergeInsertSortVector(std::vector<int>& arr) {
	size_t n = arr.size();
	if (n <= 1)
		return;
	
	if (n <= 20) {
		for (size_t i = 1; i < n; ++i) {
			int key = arr[i];
			int pos = binarySearchVector(arr, key, 0, i - 1);
			
			for (int j = i; j > pos; --j) {
				arr[j] = arr[j - 1];
			}
			arr[pos] = key;
		}
		return;
	}
	
	std::vector<std::pair<int, int> > pairs;
	bool hasStraggler = (n % 2 == 1);
	int straggler = hasStraggler ? arr[n - 1] : 0;

	for (size_t i = 0; i + 1 < n; i += 2) {
		if (arr[i] > arr[i + 1])
			pairs.push_back(std::make_pair(arr[i], arr[i + 1]));
		else
			pairs.push_back(std::make_pair(arr[i + 1], arr[i]));
	}
	
	std::vector<int> largers;
	for (size_t i = 0; i < pairs.size(); ++i) {
		largers.push_back(pairs[i].first);
	}
	mergeSortVector(largers);
	
	std::vector<std::pair<int, int> > sortedPairs;
	for (size_t i = 0; i < largers.size(); ++i) {
		for (size_t j = 0; j < pairs.size(); ++j) {
			if (pairs[j].first == largers[i]) {
				sortedPairs.push_back(pairs[j]);
				break;
			}
		}
	}
	
	std::vector<int> mainChain;
	std::vector<int> pend;
	
	for (size_t i = 0; i < sortedPairs.size(); ++i) {
		mainChain.push_back(sortedPairs[i].first);
		pend.push_back(sortedPairs[i].second);
	}
	
	if (!pend.empty()) {
		mainChain.insert(mainChain.begin(), pend[0]);
	}
	
	std::vector<size_t> insertionOrder;
	generateJacobsthalSequence(insertionOrder, pend.size());
	
	for (size_t i = 0; i < insertionOrder.size(); ++i) {
		size_t idx = insertionOrder[i];
		if (idx > 0 && idx < pend.size()) {
			int item = pend[idx];
			int pos = binarySearchVector(mainChain, item, 0, mainChain.size() - 1);
			mainChain.insert(mainChain.begin() + pos, item);
		}
	}
	
	if (hasStraggler) {
		int pos = binarySearchVector(mainChain, straggler, 0, mainChain.size() - 1);
		mainChain.insert(mainChain.begin() + pos, straggler);
	}
	
	arr = mainChain;
}


void PmergeMe::mergeVector(std::vector<int>& arr, int left, int mid, int right) {
	int n1 = mid - left + 1;
	int n2 = right - mid;
	
	std::vector<int> leftArr(n1);
	std::vector<int> rightArr(n2);
	
	for (int i = 0; i < n1; ++i)
		leftArr[i] = arr[left + i];
	for (int j = 0; j < n2; ++j)
		rightArr[j] = arr[mid + 1 + j];
	
	int i = 0, j = 0, k = left;
	
	while (i < n1 && j < n2) {
		if (leftArr[i] <= rightArr[j]) {
			arr[k] = leftArr[i];
			++i;
		} else {
			arr[k] = rightArr[j];
			++j;
		}
		++k;
	}
	
	while (i < n1) {
		arr[k] = leftArr[i];
		++i;
		++k;
	}
	
	while (j < n2) {
		arr[k] = rightArr[j];
		++j;
		++k;
	}
}

void PmergeMe::mergeSortVector(std::vector<int>& arr) {
	int n = arr.size();
	if (n <= 1)
		return;
	
	if (n <= 20) {
		for (int i = 1; i < n; ++i) {
			int key = arr[i];
			int pos = binarySearchVector(arr, key, 0, i - 1);
			
			for (int j = i; j > pos; --j) {
				arr[j] = arr[j - 1];
			}
			arr[pos] = key;
		}
		return;
	}
	
	int mid = n / 2;
	std::vector<int> left(arr.begin(), arr.begin() + mid);
	std::vector<int> right(arr.begin() + mid, arr.end());
	
	mergeSortVector(left);
	mergeSortVector(right);
	
	for (int i = 0; i < mid; ++i)
		arr[i] = left[i];
	for (int i = 0; i < n - mid; ++i)
		arr[mid + i] = right[i];
	
	mergeVector(arr, 0, mid - 1, n - 1);
}


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

void PmergeMe::mergeDeque(std::deque<int>& arr, int left, int mid, int right) {
	int n1 = mid - left + 1;
	int n2 = right - mid;
	
	std::deque<int> leftArr(n1);
	std::deque<int> rightArr(n2);
	
	for (int i = 0; i < n1; ++i)
		leftArr[i] = arr[left + i];
	for (int j = 0; j < n2; ++j)
		rightArr[j] = arr[mid + 1 + j];
	
	int i = 0, j = 0, k = left;
	
	while (i < n1 && j < n2) {
		if (leftArr[i] <= rightArr[j]) {
			arr[k] = leftArr[i];
			++i;
		} else {
			arr[k] = rightArr[j];
			++j;
		}
		++k;
	}
	
	while (i < n1) {
		arr[k] = leftArr[i];
		++i;
		++k;
	}
	
	while (j < n2) {
		arr[k] = rightArr[j];
		++j;
		++k;
	}
}

void PmergeMe::mergeSortDeque(std::deque<int>& arr) {
	int n = arr.size();
	if (n <= 1)
		return;
	
	if (n <= 20) {
		for (int i = 1; i < n; ++i) {
			int key = arr[i];
			int pos = binarySearchDeque(arr, key, 0, i - 1);
			
			for (int j = i; j > pos; --j) {
				arr[j] = arr[j - 1];
			}
			arr[pos] = key;
		}
		return;
	}
	
	int mid = n / 2;
	std::deque<int> left(arr.begin(), arr.begin() + mid);
	std::deque<int> right(arr.begin() + mid, arr.end());
	
	mergeSortDeque(left);
	mergeSortDeque(right);
	
	for (int i = 0; i < mid; ++i)
		arr[i] = left[i];
	for (int i = 0; i < n - mid; ++i)
		arr[mid + i] = right[i];
	
	mergeDeque(arr, 0, mid - 1, n - 1);
}

void PmergeMe::mergeInsertSortDeque(std::deque<int>& arr) {
	size_t n = arr.size();
	if (n <= 1)
		return;
	
	if (n <= 20) {
		for (size_t i = 1; i < n; ++i) {
			int key = arr[i];
			int pos = binarySearchDeque(arr, key, 0, i - 1);
			
			for (int j = i; j > pos; --j) {
				arr[j] = arr[j - 1];
			}
			arr[pos] = key;
		}
		return;
	}
	
	std::vector<std::pair<int, int> > pairs;
	bool hasStraggler = (n % 2 == 1);
	int straggler = hasStraggler ? arr[n - 1] : 0;
	
	for (size_t i = 0; i + 1 < n; i += 2) {
		if (arr[i] > arr[i + 1])
			pairs.push_back(std::make_pair(arr[i], arr[i + 1]));
		else
			pairs.push_back(std::make_pair(arr[i + 1], arr[i]));
	}
	
	std::deque<int> largers;
	for (size_t i = 0; i < pairs.size(); ++i) {
		largers.push_back(pairs[i].first);
	}
	mergeSortDeque(largers);
	
	std::vector<std::pair<int, int> > sortedPairs;
	for (size_t i = 0; i < largers.size(); ++i) {
		for (size_t j = 0; j < pairs.size(); ++j) {
			if (pairs[j].first == largers[i]) {
				sortedPairs.push_back(pairs[j]);
				break;
			}
		}
	}
	
	std::deque<int> mainChain;
	std::vector<int> pend;
	
	for (size_t i = 0; i < sortedPairs.size(); ++i) {
		mainChain.push_back(sortedPairs[i].first);
		pend.push_back(sortedPairs[i].second);
	}
	
	if (!pend.empty()) {
		mainChain.push_front(pend[0]);
	}
	
	std::vector<size_t> insertionOrder;
	generateJacobsthalSequence(insertionOrder, pend.size());
	
	for (size_t i = 0; i < insertionOrder.size(); ++i) {
		size_t idx = insertionOrder[i];
		if (idx > 0 && idx < pend.size()) {
			int item = pend[idx];
			int pos = binarySearchDeque(mainChain, item, 0, mainChain.size() - 1);
			mainChain.insert(mainChain.begin() + pos, item);
		}
	}
	
	if (hasStraggler) {
		int pos = binarySearchDeque(mainChain, straggler, 0, mainChain.size() - 1);
		mainChain.insert(mainChain.begin() + pos, straggler);
	}
	
	arr = mainChain;
}

void PmergeMe::sort() {
	struct timeval start, end;

	std::cout << "Before: ";
	for (size_t i = 0; i < _vectorData.size() && i < 5; ++i) {
		std::cout << _vectorData[i] << " ";
	}
	if (_vectorData.size() > 5)
		std::cout << "[...]";
	std::cout << std::endl;

	gettimeofday(&start, NULL);
	mergeInsertSortVector(_vectorData);
	gettimeofday(&end, NULL);
	double vectorTime = (end.tv_sec - start.tv_sec) * 1000000.0 + 
	                    (end.tv_usec - start.tv_usec);

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
