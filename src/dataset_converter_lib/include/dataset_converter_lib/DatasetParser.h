/**
 * @authors Simon Schaefer
 * @date 05.05.2021
 */
#ifndef DATASET_CONVERTER_LIB_DATASET_PARSER_H_
#define DATASET_CONVERTER_LIB_DATASET_PARSER_H_

#include <vector>

#include "dataset_converter_lib/DatasetScenario.h"

namespace dataset_converter_lib {

class DatasetParser {
 protected:
  DatasetScenarioPtrs scenarios_;
  long scenario_counter_ = 0;

 public:
  void ParseAll(const std::string &dataset_root_directory);
  long ParseNext(const std::string &dataset_root_directory);

  size_t GetNumberOfScenarios();

  void AddScenario(const DatasetScenarioPtr& scenario);
  [[nodiscard]] const DatasetScenarioPtrs &GetScenarios() const;
};

}
#endif //DATASET_CONVERTER_LIB_DATASET_PARSER_H_
