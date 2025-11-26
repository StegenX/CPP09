#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <string>
#include <map>

class BitcoinExchange {
private:
	std::map<std::string, double> _database;

	BitcoinExchange(const BitcoinExchange& other);
	BitcoinExchange& operator=(const BitcoinExchange& other);

	bool isValidDate(const std::string& date) const;
	bool isValidValue(const std::string& value) const;
	double stringToDouble(const std::string& str) const;
	std::string trim(const std::string& str) const;

public:
	BitcoinExchange();
	~BitcoinExchange();

	bool loadDatabase(const std::string& filename);
	void processInputFile(const std::string& filename);
	double getExchangeRate(const std::string& date) const;
};

#endif
