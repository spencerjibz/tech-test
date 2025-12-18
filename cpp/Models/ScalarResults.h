#ifndef SCALARRESULTS_H
#define SCALARRESULTS_H

#include "IScalarResultReceiver.h"
#include "ScalarResult.h"
#include <map>
#include <vector>
#include <optional>
#include <string>
#include <iterator>
#include <algorithm>

class ScalarResults : public IScalarResultReceiver {
public:
    virtual ~ScalarResults();
    std::optional<ScalarResult> operator[](const std::string& tradeId) const;

    bool containsTrade(const std::string& tradeId) const;

    virtual void addResult(const std::string& tradeId, double result) override;

    virtual void addError(const std::string& tradeId, const std::string& error) override;

    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = ScalarResult;
        using difference_type = std::ptrdiff_t;
        using pointer = ScalarResult*;
        using reference = ScalarResult&;

        Iterator() = default;

        // Iterator must be constructable from ScalarResults parent
        Iterator(std::vector<std::string>::const_iterator iter, const ScalarResults* parent_class) : iter_(iter), parent_(parent_class) {}

        Iterator& operator++() {
         ++iter_;
         return *this;
    }
        ScalarResult operator*() const {
             auto opt = (*parent_)[*iter_];
          return *opt;
    }
        bool operator!=(const Iterator& other) const {
             return iter_ != other.iter_;
    }
private:
        std::vector<std::string>::const_iterator iter_;
        const ScalarResults* parent_ = nullptr;
    };

    Iterator begin() const {

        for (auto &[tradeId, _]: results_) allcurrentIds_.push_back(tradeId); 
        for (auto &[id, _]: errors_)  {
           if (std::find(allcurrentIds_.begin(), allcurrentIds_.end(), id) == allcurrentIds_.end()) {
             allcurrentIds_.push_back(id);
      }
    }
        return Iterator(allcurrentIds_.cbegin(), this);
  }
    Iterator end() const {
     return Iterator(allcurrentIds_.cend(), this);
  }

private:
    std::map<std::string, double> results_;
    std::map<std::string, std::string> errors_;
    mutable std::vector<std::string> allcurrentIds_;
};

#endif // SCALARRESULTS_H
