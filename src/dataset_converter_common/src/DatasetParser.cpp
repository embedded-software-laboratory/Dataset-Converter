#include "dataset_converter_lib/DatasetParser.h"
namespace dataset_converter_common {

void DatasetParser::ParseAll(const std::string &dataset_root_directory) {
  for (const auto &dataset : this->scenarios_) {
    dataset->Parse(dataset_root_directory);
  }
}

long DatasetParser::ParseNext(const std::string &dataset_root_directory) {
  if (this->scenario_counter_ >= this->scenarios_.size())return -1;
  this->scenarios_.at(this->scenario_counter_)->Parse(dataset_root_directory);
  this->scenario_counter_++;
  return static_cast<long>(this->scenarios_.size()) - this->scenario_counter_;
}

void DatasetParser::AddScenario(const DatasetScenarioPtr &scenario) {
  this->scenarios_.push_back(scenario);
}

const DatasetScenarioPtrs &DatasetParser::GetScenarios() const {
  return scenarios_;
}

size_t DatasetParser::GetNumberOfScenarios() {
  return this->scenarios_.size();
}

}
