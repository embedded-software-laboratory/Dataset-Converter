#include "dataset_converter_lib/inD/InDParser.h"
#include "dataset_converter_lib/inD/InDScenario.h"

namespace dataset_converter_lib {

InDParser::InDParser() {
  this->scenarios_.push_back(std::make_shared<InDScenario>("00"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("01"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("02"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("03"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("04"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("05"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("06"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("07"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("08"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("09"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("10"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("11"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("12"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("13"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("14"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("15"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("16"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("17"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("18"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("19"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("20"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("21"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("22"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("23"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("24"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("25"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("26"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("27"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("28"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("29"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("30"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("31"));
  this->scenarios_.push_back(std::make_shared<InDScenario>("32"));
}
}