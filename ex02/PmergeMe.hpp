#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <string>

class PmergeMe {
private:
	std::vector<int> _vectorData;
	std::deque<int> _dequeData;

	PmergeMe(const PmergeMe& other);
	PmergeMe& operator=(const PmergeMe& other);

	void mergeInsertSortVector(std::vector<int>& arr);
	int binarySearchVector(const std::vector<int>& arr, int item, int left, int right);
	void generateJacobsthalSequence(std::vector<size_t>& sequence, size_t n);

	void mergeInsertSortDeque(std::deque<int>& arr);
	int binarySearchDeque(const std::deque<int>& arr, int item, int left, int right);

public:
	PmergeMe();
	~PmergeMe();

	bool parseInput(int argc, char** argv);
	void sort();
	void displayResults(double vectorTime, double dequeTime) const;
};

#endif
