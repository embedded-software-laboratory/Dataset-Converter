#include "dataset_converter_lib/rounD/RounDParser.h"
#include "dataset_converter_lib/rounD/RounDScenario.h"

namespace dataset_converter_lib {

RounDParser::RounDParser() {
  this->scenarios_.push_back(std::make_shared<RounDScenario>("00"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("01"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("02"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("03"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("04"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("05"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("06"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("07"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("08"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("09"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("10"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("11"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("12"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("13"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("14"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("15"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("16"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("17"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("18"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("19"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("20"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("21"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("22"));
  this->scenarios_.push_back(std::make_shared<RounDScenario>("23"));
}
}