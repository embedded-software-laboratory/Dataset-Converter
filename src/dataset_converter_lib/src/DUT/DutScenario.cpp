#include "dataset_converter_lib/DUT/DutScenario.h"

#include <map>

#include <CSV/csv.hpp>
namespace dataset_converter_lib {
void DutScenario::MakePathsAbsolute(const std::string &dataset_root_directory) {
  this->background_file_path_ = dataset_root_directory + this->background_file_path_;
  this->pedestrian_trajectory_file_path_ = dataset_root_directory + this->pedestrian_trajectory_file_path_;
  this->vehicle_trajectory_file_path_ = dataset_root_directory + this->vehicle_trajectory_file_path_;
  this->background_ratio_file_path_ = dataset_root_directory + this->background_ratio_file_path_;
}
void DutScenario::InterpolateOrientation(const cpm_scenario::ExtendedObjectPtr & object) {
  const auto &states = object->GetStates();
  if (states.empty()) return;

  auto last_state = states.begin()->second;
  double last_orientation = 0.0;

  for (const auto& element : states) {
    auto state = element.second;
    Eigen::Vector2d normal = state->GetPosition() - last_state->GetPosition();

    double orientation = atan2(normal.y(), normal.x());
    if (normal.norm() < 0.01) {
      orientation = last_orientation;
    }
    state->SetOrientation(orientation);

    last_state = state;
    last_orientation = orientation;
  }
}
void DutScenario::ParsePedestrianFile() {
  long max_frame = 0;
  csv::CSVReader csv_reader(this->pedestrian_trajectory_file_path_);
  std::map<long, cpm_scenario::ExtendedObjectPtr> objects;
  for (csv::CSVRow &row: csv_reader) {
    auto id = row["id"].get<long>();
    auto frame = row["frame"].get<long>();
    long timestamp = static_cast<long>((FRAMES_PER_SECOND / frame) * 1000000000.0);
    auto x = row["x_est"].get<double>();
    auto y = row["y_est"].get<double>();
    auto vx = row["vx_est"].get<double>();
    auto vy = row["vy_est"].get<double>();
    if (!objects[id])objects[id] = std::make_shared<cpm_scenario::ExtendedObject>(id, Eigen::Vector2d(0.75, 0.75), cpm_scenario::ExtendedObjectType::PEDESTRIAN);
    cpm_scenario::ExtendedObjectPtr object = objects[id];
    auto state = std::make_shared<cpm_scenario::ObjectState>();
    state->SetPosition({x, y});
    state->SetVelocity({vx, vy});
    state->SetTimestamp(timestamp);
    state->SetFrame(frame);
    state->SetOrientation(0);
    object->AddState(state);
    if (frame > max_frame) max_frame = frame;
  }
  // Push to class storage and fill orientation
  for (const auto& element : objects) {
    this->InterpolateOrientation(element.second);
    this->AddObject(element.second);
  }

  // Update number of frames
  if (max_frame > this->GetNumberOfFrames()) this->SetNumberOfFrames(max_frame);

  std::cout << "Parsed " << objects.size() << " pedestrians from dut scenario " << this->GetName() << "." << std::endl;
}
void DutScenario::ParseVehicleFile() {
  long max_frame = 0;
  csv::CSVReader csv_reader(this->vehicle_trajectory_file_path_);
  std::map<long, cpm_scenario::ExtendedObjectPtr> objects;
  for (csv::CSVRow &row: csv_reader) {
    auto id = row["id"].get<long>();
    auto frame = row["frame"].get<long>();
    long timestamp = static_cast<long>((FRAMES_PER_SECOND / frame) * 1000000000.0);
    auto x = row["x_est"].get<double>();
    auto y = row["y_est"].get<double>();
    auto psi = row["psi_est"].get<double>();
    auto speed = row["vel_est"].get<double>();
    if (!objects[id])objects[id] = std::make_shared<cpm_scenario::ExtendedObject>(id, Eigen::Vector2d(4.0, 2.0), cpm_scenario::ExtendedObjectType::CAR);
    cpm_scenario::ExtendedObjectPtr object = objects[id];
    auto state = std::make_shared<cpm_scenario::ObjectState>();
    state->SetPosition({x, y});
    auto orientation = Eigen::Rotation2Dd(psi);
    Eigen::Vector2d velocity = {speed, 0.0};
    velocity = orientation * velocity;
    state->SetVelocity(velocity);
    state->SetTimestamp(timestamp);
    state->SetFrame(frame);
    state->SetOrientation(psi);
    object->AddState(state);
    if (frame > max_frame) max_frame = frame;
  }
  // Push to class storage
  for (const auto& element : objects) {
    this->AddObject(element.second);
  }

  // Update number of frames
  if (max_frame > this->GetNumberOfFrames()) this->SetNumberOfFrames(max_frame);

  std::cout << "Parsed " << objects.size() << " vehicle from dut scenario " << this->GetName() << "." << std::endl;
}
void DutScenario::ParseBackgroundRatioFile() {
  csv::CSVReader csv_reader(this->background_ratio_file_path_);
  std::string ratio_string = csv_reader.get_col_names().front();
  double ratio = std::stod(ratio_string);
  this->SetBackgroundImageScaleFactor(ratio);
}

DutScenario::DutScenario(const std::string &name) :
    DutScenario(name,
                "/data/trajectories_filtered/" + name + "_traj_ped_filtered.csv",
                "/data/trajectories_filtered/" + name + "_traj_veh_filtered.csv",
                "/data/backgrounds/" + name + "_background_GSOC.png",
                "/data/ratios/" + name + "_ratio_pixel2meter.txt") {}

DutScenario::DutScenario(std::string name,
                         std::string pedestrian_trajectory_file_path,
                         std::string vehicle_trajectory_file_path,
                         std::string background_file_path,
                         std::string background_ratio_file_path)
    : DatasetScenario(std::move(name)),
      pedestrian_trajectory_file_path_(std::move(pedestrian_trajectory_file_path)),
      vehicle_trajectory_file_path_(std::move(vehicle_trajectory_file_path)),
      background_file_path_(std::move(background_file_path)),
      background_ratio_file_path_(std::move(background_ratio_file_path)) {}

void DutScenario::Parse(const std::string &dataset_root_directory) {
  this->MakePathsAbsolute(dataset_root_directory);
  this->SetBackgroundImageSourcePath(this->background_file_path_);
  this->ParseBackgroundRatioFile();
  this->ParsePedestrianFile();
  this->ParseVehicleFile();
}

const std::string &DutScenario::GetPedestrianTrajectoryFilePath() const {
  return pedestrian_trajectory_file_path_;
}
void DutScenario::SetPedestrianTrajectoryFilePath(const std::string &pedestrian_trajectory_file_path) {
  pedestrian_trajectory_file_path_ = pedestrian_trajectory_file_path;
}
const std::string &DutScenario::GetVehicleTrajectoryFilePath() const {
  return vehicle_trajectory_file_path_;
}
void DutScenario::SetVehicleTrajectoryFilePath(const std::string &vehicle_trajectory_file_path) {
  vehicle_trajectory_file_path_ = vehicle_trajectory_file_path;
}
const std::string &DutScenario::GetBackgroundFilePath() const {
  return background_file_path_;
}
void DutScenario::SetBackgroundFilePath(const std::string &background_file_path) {
  background_file_path_ = background_file_path;
}
}