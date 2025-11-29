#ifndef RPN_HPP
#define RPN_HPP

#include <string>
#include <stack>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

class RPN {
private:
	std::stack<int> _stack;

	RPN(const RPN& other);
	RPN& operator=(const RPN& other);

	bool isOperator(const std::string& token) const;
	bool isNumber(const std::string& token) const;
	int performOperation(int a, int b, const std::string& op) const;

public:
	RPN();
	~RPN();

	int evaluate(const std::string& expression);
};

#endif
