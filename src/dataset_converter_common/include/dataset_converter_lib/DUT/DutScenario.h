/**
 * @file DutScenario.h
 * @authors Simon Schaefer
 * @date 07.05.2021
 */
#ifndef DATASET_CONVERTER_LIB_DUT_SCENARIO_H_
#define DATASET_CONVERTER_LIB_DUT_SCENARIO_H_

#include "dataset_converter_lib/DatasetScenario.h"

namespace dataset_converter_common {

class DutScenario : public DatasetScenario {
 private:
  const double FRAMES_PER_SECOND = 23.98;
  std::string pedestrian_trajectory_file_path_;
  std::string vehicle_trajectory_file_path_;
  std::string background_file_path_;
  std::string background_ratio_file_path_;

  DutScenario(std::string name,
              std::string pedestrian_trajectory_file_path,
              std::string vehicle_trajectory_file_path,
              std::string background_file_path,
              std::string background_ratio_file_path_);

  void MakePathsAbsolute(const std::string &dataset_root_directory);

  static void InterpolateOrientation(const std::shared_ptr<cpm_scenario::ExtendedObject> &object);
  void ParsePedestrianFile();
  void ParseVehicleFile();
  void ParseBackgroundRatioFile();

 public:
  explicit DutScenario(const std::string &name);

  void Parse(const std::string &dataset_root_directory) override;

  [[nodiscard]] const std::string &GetPedestrianTrajectoryFilePath() const;
  void SetPedestrianTrajectoryFilePath(const std::string &pedestrian_trajectory_file_path);
  [[nodiscard]] const std::string &GetVehicleTrajectoryFilePath() const;
  void SetVehicleTrajectoryFilePath(const std::string &vehicle_trajectory_file_path);
  [[nodiscard]] const std::string &GetBackgroundFilePath() const;
  void SetBackgroundFilePath(const std::string &background_file_path);
};

}

#endif //DATASET_CONVERTER_LIB_DUT_SCENARIO_H_
