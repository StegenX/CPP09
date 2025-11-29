#include "PmergeMe.hpp"
#include <iostream>

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Error" << std::endl;
		return 1;
	}

	PmergeMe sorter;
	if (!sorter.parseInput(argc, argv)) {
		return 1;
	}

	sorter.sort();
	return 0;
}
