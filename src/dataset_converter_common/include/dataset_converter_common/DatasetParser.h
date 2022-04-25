/**
 * @authors Simon Schaefer
 * @date 05.05.2021
 */
#ifndef DATASET_CONVERTER_LIB_DATASET_PARSER_H_
#define DATASET_CONVERTER_LIB_DATASET_PARSER_H_

#include <vector>

#include "dataset_converter_common/DatasetScenario.h"

namespace dataset_converter_common {

/**
 * Abstract implementation of a dataset parser that all specific implementations will inherit from.
 */
class DatasetParser {
 protected:
  DatasetScenarioPtrs scenarios_; ///< Collection of recordings/scenarios parsed from the dataset
  long scenario_counter_ = 0; ///< Counter to keep track which scenario was parsed already

 public:
  /**
   * Parse all scenarios as one single task.
   * @param dataset_root_directory Directory to parse from.
   */
  void ParseAll(const std::string &dataset_root_directory);
  /**
   * Parse the next scenario in line.
   * @param dataset_root_directory Directory to parse from.
   */
  long ParseNext(const std::string &dataset_root_directory);

  /**
   * Get total number of scenarios.
   * @return
   */
  size_t GetNumberOfScenarios();

  /**
   * Add scenario to be parsed.
   * @param scenario New scenario.
   */
  void AddScenario(const DatasetScenarioPtr &scenario);

  /**
   * Get list of scenarios.
   * @return List of scenarios.
   */
  [[nodiscard]] const DatasetScenarioPtrs &GetScenarios() const;
};

}
#endif //DATASET_CONVERTER_LIB_DATASET_PARSER_H_
