#include "SerialPricer.h"
#include "../Pricers/CorpBondPricingEngine.h"
#include "../Pricers/FxPricingEngine.h"
#include "../Pricers/GovBondPricingEngine.h"
#include <stdexcept>
SerialPricer::~SerialPricer() {}
//
constexpr unsigned int hash_str(const char *s, int off = 0) {
    return !s[off] ? 5381 : (hash_str(s, off + 1) * 33) ^ s[off];
}
void SerialPricer::loadPricers() {
    PricingConfigLoader pricingConfigLoader;
    pricingConfigLoader.setConfigFile("./PricingConfig/PricingEngines.xml");
    PricingEngineConfig pricerConfig = pricingConfigLoader.loadConfig();

    for (const auto &configItem : pricerConfig) {
        IPricingEngine *pricingEngine;
        auto typeName = configItem.getTypeName();
        auto tradeType = configItem.getTradeType();
        switch (hash_str(typeName.c_str())) {
        case hash_str("HmxLabs.TechTest.Pricers.GovBondPricingEngine"):
            pricingEngine = new GovBondPricingEngine();
            break;
        case (hash_str("HmxLabs.TechTest.Pricers.CorpBondPricingEngine")):
            pricingEngine = new CorpBondPricingEngine();
            break;
        case hash_str("HmxLabs.TechTest.Pricers.FxPricingEngine"):
            pricingEngine = new FxPricingEngine();
            break;
        default:
            throw std::runtime_error("Unsupported: " + typeName);
        }
        pricers_[tradeType] = pricingEngine;
    }
}

void SerialPricer::price(
    const std::vector<std::vector<ITrade *>> &tradeContainers,
    IScalarResultReceiver *resultReceiver) {
    loadPricers();

    for (const auto &tradeContainer : tradeContainers) {
        for (ITrade *trade : tradeContainer) {
            std::string tradeType = trade->getTradeType();
            if (pricers_.find(tradeType) == pricers_.end()) {
                resultReceiver->addError(
                    trade->getTradeId(),
                    "No Pricing Engines available for this trade type");
                continue;
            }

            IPricingEngine *pricer = pricers_[tradeType];
            pricer->price(trade, resultReceiver);
        }
    }
}
