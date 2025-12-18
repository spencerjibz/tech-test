#include "BondTradeLoader.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>
#include <stdexcept>

std::string trim(const std::string &str) {
  auto start = std::find_if_not(
      str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); });
  auto end = std::find_if_not(str.rbegin(), str.rend(), [](unsigned char c) {
               return std::isspace(c);
             }).base();
  if (start >= end)
    return "";
  return std::string(start, end);
}

BondTrade *BondTradeLoader::createTradeFromLine(std::string line) {
  std::vector<std::string> items;
  std::stringstream ss(line);
  std::string item;

  while (std::getline(ss, item, separator)) {
    items.push_back(item);
  }

  if (items.size() < 7) {
    throw std::runtime_error("Invalid line format");
  }

  auto bondStr = items[0];
  std::optional<std::string> bondType = std::nullopt;
  if (bondStr == BondTrade::CorpBondTradeType)
    bondType = BondTrade::CorpBondTradeType;

  auto id = trim(items[6]);
  BondTrade *trade = bondType.has_value() ? new BondTrade(id, bondType.value())
                                          : new BondTrade(id);

  std::tm tm = {};
  std::istringstream dateStream(items[1]);
  dateStream >> std::get_time(&tm, "%Y-%m-%d");
  auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  trade->setTradeDate(timePoint);

  trade->setInstrument(items[2]);
  trade->setCounterparty(items[3]);
  trade->setNotional(std::stod(items[4]));
  trade->setRate(std::stod(items[5]));

  return trade;
}

void BondTradeLoader::loadTradesFromFile(std::string filename,
                                         BondTradeList &tradeList) {
  if (filename.empty()) {
    throw std::invalid_argument("Filename cannot be null");
  }

  std::ifstream stream(filename);
  if (!stream.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  int lineCount = 0;
  std::string line;
  while (std::getline(stream, line)) {
    if (lineCount == 0) {
    } else {
      tradeList.add(createTradeFromLine(line));
    }
    lineCount++;
  }
}

std::vector<ITrade *> BondTradeLoader::loadTrades() {
  BondTradeList tradeList;
  loadTradesFromFile(dataFile_, tradeList);

  std::vector<ITrade *> result;
  for (size_t i = 0; i < tradeList.size(); ++i) {
    result.push_back(tradeList[i]);
  }
  return result;
}

std::string BondTradeLoader::getDataFile() const { return dataFile_; }

void BondTradeLoader::setDataFile(const std::string &file) { dataFile_ = file; }
