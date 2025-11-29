#include "RPN.hpp"


RPN::RPN() {}

RPN::~RPN() {}

RPN::RPN(const RPN& other) {
	*this = other;
}

RPN& RPN::operator=(const RPN& other) {
	if (this != &other) {
		this->_stack = other._stack;
	}
	return *this;
}

bool RPN::isOperator(const std::string& token) const {
	return token == "+" || token == "-" || token == "*" || token == "/";
}

bool RPN::isNumber(const std::string& token) const {
	if (token.empty())
		return false;
	
	size_t start = 0;
	if (token[0] == '-' || token[0] == '+')
		start = 1;
	
	if (start >= token.length())
		return false;
	
	for (size_t i = start; i < token.length(); ++i) {
		if (!isdigit(token[i]))
			return false;
	}
	return true;
}

int RPN::performOperation(int a, int b, const std::string& op) const {
	if (op == "+")
		return a + b;
	else if (op == "-")
		return a - b;
	else if (op == "*")
		return a * b;
	else if (op == "/") {
		if (b == 0)
			throw std::runtime_error("Error: division by zero");
		return a / b;
	}
	throw std::runtime_error("Error: invalid operator");
}

int RPN::evaluate(const std::string& expression) {
	std::istringstream iss(expression);
	std::string token;

	while (iss >> token) {
		if (isNumber(token)) {
			int num = std::atoi(token.c_str());
			_stack.push(num);
		} else if (isOperator(token)) {
			if (_stack.size() < 2)
				throw std::runtime_error("Error");

			int b = _stack.top();
			_stack.pop();
			int a = _stack.top();
			_stack.pop();

			int result = performOperation(a, b, token);
			_stack.push(result);
		} else {
			throw std::runtime_error("Error");
		}
	}

	if (_stack.size() != 1)
		throw std::runtime_error("Error");

	return _stack.top();
}
