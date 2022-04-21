/**
 * @authors Simon Schaefer
 * @date 10.06.2021
 */
#ifndef DATASET_CONVERTER_LIB_DATASET_SCENARIO_H_
#define DATASET_CONVERTER_LIB_DATASET_SCENARIO_H_

#include <cpm_scenario/Scenario.h>
namespace dataset_converter_lib {

class DatasetScenario : public cpm_scenario::Scenario {
 public:
  DatasetScenario(const std::string &name);
  virtual void Parse(const std::string &dataset_root_directory) = 0;
};
typedef std::shared_ptr<DatasetScenario> DatasetScenarioPtr;
typedef std::vector<DatasetScenarioPtr> DatasetScenarioPtrs;
}
#endif //DATASET_CONVERTER_LIB_DATASET_SCENARIO_H_
