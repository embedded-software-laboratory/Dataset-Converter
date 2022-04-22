/**
 * @file DutScenario.h
 * @authors Simon Schaefer
 * @date 07.05.2021
 */
#ifndef DATASET_CONVERTER_LIB_IND_SCENARIO_H_
#define DATASET_CONVERTER_LIB_IND_SCENARIO_H_

#include <map>
#include "dataset_converter_lib/DatasetScenario.h"

namespace dataset_converter_common {

class InDScenario : public DatasetScenario {
 private:
  std::map<long, cpm_scenario::ExtendedObjectPtr> objects_map_;

  double FRAMES_PER_SECOND = 25.00;
  std::string recording_meta_file_path_;
  std::string tracks_file_path_;
  std::string tracks_meta_file_path_;
  std::string background_file_path_;

  InDScenario(std::string name,
              std::string recording_meta_file_path_,
              std::string tracks_file_path_,
              std::string tracks_meta_file_path_,
              std::string background_file_path_);

  void MakePathsAbsolute(const std::string &dataset_root_directory);

  void ParserRecordingMetaFile();
  void ParserTrackMetaFile();
  void ParserTrackFile();

 public:
  explicit InDScenario(const std::string &name);

  void Parse(const std::string &dataset_root_directory) override;
};

}

#endif //DATASET_CONVERTER_LIB_IND_SCENARIO_H_
