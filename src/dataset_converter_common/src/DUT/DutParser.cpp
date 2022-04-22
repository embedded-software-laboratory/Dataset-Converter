#include "dataset_converter_lib/DUT/DutParser.h"
#include "dataset_converter_lib/DUT/DutScenario.h"

namespace dataset_converter_common {

DutParser::DutParser() {
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_01"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_02"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_03"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_04"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_05"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_06"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_07"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_08"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_09"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_10"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_11"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_12"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_13"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_14"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_15"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_16"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("intersection_17"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("roundabout_01"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("roundabout_02"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("roundabout_03"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("roundabout_04"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("roundabout_05"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("roundabout_06"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("roundabout_07"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("roundabout_08"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("roundabout_09"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("roundabout_10"));
  this->scenarios_.push_back(std::make_shared<DutScenario>("roundabout_11"));
}
}
