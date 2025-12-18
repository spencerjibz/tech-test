#include "PricingConfigLoader.h"
#include "PricingEngineConfig.h"
#include "PricingEngineConfigItem.h"
#include <stdexcept>
#include <sstream>
#include<iostream>
#include <fstream>

 // look up each attribute at att="...";
std::string extract_atrribute(const std::string& line,  const std::string& att) {
  std::string searchKey = att+ "=\"";
  // the type can't be intered as size_t here
  size_t start = line.find(searchKey);
  if (start == std::string::npos) return "";
  start += searchKey.length();
   // find the closing literal mark form start
  size_t end  = line.find("\"", start);
  return line.substr(start, end - start);
     
}

std::string PricingConfigLoader::getConfigFile() const {
    return configFile_;
}

void PricingConfigLoader::setConfigFile(const std::string& file) {
    configFile_ = file;
}
PricingEngineConfig PricingConfigLoader::parseXml(const std::string& content) {
    std::stringstream ss(content);
    PricingEngineConfig config;
    
    int lineCount = 0;
    std::string line;
    while (std::getline(ss, line)) { 
    if (line.find("<Engine") != std::string::npos) {
       PricingEngineConfigItem item;
       item.setAssembly(extract_atrribute(line, "assembly"));
       item.setTradeType(extract_atrribute(line, "tradeType"));
       item.setTypeName(extract_atrribute(line, "pricingEngine"));
        config.push_back(item);
       
    }
  }
  return config;
} 

PricingEngineConfig PricingConfigLoader::loadConfig() {
    auto filename  = getConfigFile();
    std::ifstream stream(filename);
    if (!stream.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::stringstream buffer;
     buffer << stream.rdbuf();
 // std::cout << "Parsing line for attribute: " << buffer.str() << std::endl;
      return parseXml(buffer.str());
}
