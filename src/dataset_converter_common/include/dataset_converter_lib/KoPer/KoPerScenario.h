/**
 * @file DutScenario.h
 * @authors Simon Schaefer
 * @date 07.05.2021
 */
#ifndef DATASET_CONVERTER_LIB_KO_PER_SCENARIO_H_
#define DATASET_CONVERTER_LIB_KO_PER_SCENARIO_H_

#include "CpmDatasetConverter/CpmScenario.h"

namespace CpmDatasetConverter {

class KoPerScenario : public CpmScenario {
 private:
  std::string labels_file_path_;
  std::string first_recording_part_file_path_;
  std::string second_recording_part_file_path_;

  KoPerScenario(std::string name,
                std::string labels_file_path,
                std::string first_recording_part_file_path,
                std::string second_recording_part_file_path);

  void MakePathsAbsolute(const std::string& dataset_root_directory);

  void ParseLabelFile();

 public:
  explicit KoPerScenario(const std::string& name);

  void Parse(const std::string& dataset_root_directory) override;

};

}

#endif //DATASET_CONVERTER_LIB_KO_PER_SCENARIO_H_
