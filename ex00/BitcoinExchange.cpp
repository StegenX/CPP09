#include "BitcoinExchange.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::~BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other) {
	*this = other;
}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other) {
	if (this != &other) {
		this->_database = other._database;
	}
	return *this;
}

std::string BitcoinExchange::trim(const std::string& str) const {
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}

double BitcoinExchange::stringToDouble(const std::string& str) const {
	char* end;
	double value = std::strtod(str.c_str(), &end);
	if (*end != '\0' && *end != ' ' && *end != '\t')
		return -1;
	return value;
}

bool BitcoinExchange::isValidDate(const std::string& date) const {
	if (date.length() != 10 || date[4] != '-' || date[7] != '-')
		return false;

	for (size_t i = 0; i < date.length(); ++i) {
		if (i == 4 || i == 7)
			continue;
		if (!isdigit(date[i]))
			return false;
	}

	int year = std::atoi(date.substr(0, 4).c_str());
	int month = std::atoi(date.substr(5, 2).c_str());
	int day = std::atoi(date.substr(8, 2).c_str());

	if (year < 2008 || month < 1 || month > 12 || day < 1 || day > 31)
		return false;

	return true;
}

bool BitcoinExchange::isValidValue(const std::string& value) const {
	std::string trimmedValue = trim(value);
	if (trimmedValue.empty())
		return false;

	double val = stringToDouble(trimmedValue);
	return val >= 0 && val <= 1000;
}

bool BitcoinExchange::loadDatabase(const std::string& filename) {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		std::cerr << "Error: could not open database file." << std::endl;
		return false;
	}

	std::string line;
	std::getline(file, line);

	while (std::getline(file, line)) {
		size_t pipe = line.find('|');
		if (pipe == std::string::npos)
			continue;

		std::string date = trim(line.substr(0, pipe));
		std::string value = trim(line.substr(pipe + 1));

		if (isValidDate(date)) {
			double rate = stringToDouble(value);
			_database[date] = rate;
		}
	}

	file.close();
	return true;
}

double BitcoinExchange::getExchangeRate(const std::string& date) const {
	std::map<std::string, double>::const_iterator it = _database.find(date);
	if (it != _database.end())
		return it->second;

	it = _database.lower_bound(date);
	if (it == _database.begin())
		return 0;
	--it;
	return it->second;
}

void BitcoinExchange::processInputFile(const std::string& filename) {
	std::ifstream file(filename.c_str());
	if (!file.is_open()) {
		std::cerr << "Error: could not open file." << std::endl;
		return;
	}

	std::string line;
	std::getline(file, line);

	while (std::getline(file, line)) {
		size_t pipe = line.find('|');
		if (pipe == std::string::npos) {
			std::cerr << "Error: bad input => " << trim(line) << std::endl;
			continue;
		}

		std::string date = trim(line.substr(0, pipe));
		std::string valueStr = trim(line.substr(pipe + 1));

		if (!isValidDate(date)) {
			std::cerr << "Error: bad input => " << date << std::endl;
			continue;
		}

		if (valueStr.empty() || valueStr[0] == '-') {
			std::cerr << "Error: not a positive number." << std::endl;
			continue;
		}

		double value = stringToDouble(valueStr);
		if (value < 0) {
			std::cerr << "Error: not a positive number." << std::endl;
			continue;
		}
		if (value > 1000) {
			std::cerr << "Error: too large a number." << std::endl;
			continue;
		}

		double rate = getExchangeRate(date);
		std::cout << date << " => " << value << " = " << (value * rate) << std::endl;
	}

	file.close();
}
