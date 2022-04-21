#include "dataset_converter_lib/inD/InDScenario.h"

#include <map>

#include <CSV/csv.hpp>
#include <utility>
namespace dataset_converter_lib {

InDScenario::InDScenario(std::string name,
                         std::string recording_meta_file_path,
                         std::string tracks_file_path,
                         std::string tracks_meta_file_path,
                         std::string background_file_path)
    : DatasetScenario(std::move(name)),
      recording_meta_file_path_(std::move(recording_meta_file_path)),
      tracks_file_path_(std::move(tracks_file_path)),
      tracks_meta_file_path_(std::move(tracks_meta_file_path)),
      background_file_path_(std::move(background_file_path)) {}

void InDScenario::MakePathsAbsolute(const std::string &dataset_root_directory) {
  this->recording_meta_file_path_ = dataset_root_directory + this->recording_meta_file_path_;
  this->tracks_file_path_ = dataset_root_directory + this->tracks_file_path_;
  this->tracks_meta_file_path_ = dataset_root_directory + this->tracks_meta_file_path_;
  this->background_file_path_ = dataset_root_directory + this->background_file_path_;
}

void InDScenario::ParserRecordingMetaFile() {
  csv::CSVReader csv_reader(this->recording_meta_file_path_);
  for (csv::CSVRow &row: csv_reader) {
    auto ortho_px_to_meter = row["orthoPxToMeter"].get<double>() * 12.0;
    this->SetBackgroundImageScaleFactor(1.0/ortho_px_to_meter);
    this->FRAMES_PER_SECOND = row["frameRate"].get<double>();
  }
}

void InDScenario::ParserTrackMetaFile() {
  csv::CSVReader csv_reader(this->tracks_meta_file_path_);

  for (csv::CSVRow &row: csv_reader) {
    auto track_id = row["trackId"].get<long>();
    auto width = row["width"].get<double>();
    auto length = row["length"].get<double>();
    auto class_string = row["class"].get<std::string>();
    cpm_scenario::ExtendedObjectType type = cpm_scenario::ExtendedObjectType::UNKNOWN;
    if (class_string == "car") {
      type = cpm_scenario::ExtendedObjectType::CAR;
    } else if (class_string == "truck_bus") {
      type = cpm_scenario::ExtendedObjectType::TRUCK_BUS;
    } else if (class_string == "bicycle") {
      type = cpm_scenario::ExtendedObjectType::BICYCLE_MOTORCYCLES;
      length = 1.5;
      width = 0.75;
    } else if (class_string == "pedestrian") {
      type = cpm_scenario::ExtendedObjectType::PEDESTRIAN;
      length = 0.75;
      width = 0.75;
    }
    if (!objects_map_[track_id]) objects_map_[track_id] = std::make_shared<cpm_scenario::ExtendedObject>(track_id, Eigen::Vector2d(length, width), type);
  }

  // Push to class storage and fill orientation
  for(auto element : objects_map_){
    this->AddObject(element.second);
  }

  std::cout << "Preprocessed " << objects_map_.size() << " objects from inD scenario " << this->GetName() << "." << std::endl;
}

void InDScenario::ParserTrackFile() {
  long max_frame = 0;
  csv::CSVReader csv_reader(this->tracks_file_path_);

  for (csv::CSVRow &row: csv_reader) {
    auto track_id = row["trackId"].get<long>();
    auto frame = row["frame"].get<long>();
      long timestamp = static_cast<long>((frame / FRAMES_PER_SECOND) * 1e9);
    auto x = row["xCenter"].get<double>();
    auto y = -row["yCenter"].get<double>();
    auto vx = row["xVelocity"].get<double>();
    auto vy = -row["yVelocity"].get<double>();
    auto orientation = row["heading"].get<double>();
    cpm_scenario::ExtendedObjectPtr object = objects_map_[track_id];
    auto state = std::make_shared<cpm_scenario::ObjectState>();
    state->SetPosition({x, y});
    state->SetVelocity({vx, vy});
    state->SetTimestamp(timestamp);
    state->SetFrame(frame);
    state->SetOrientation(-orientation * M_PI / 180.0);
    object->AddState(state);
    if(frame > max_frame) max_frame = frame;
  }

  // Update number of frames
  if(max_frame > this->GetNumberOfFrames()) this->SetNumberOfFrames(max_frame);

  std::cout << "Parsed " << objects_map_.size() << " objects from inD scenario " << this->GetName() << "." << std::endl;
}

InDScenario::InDScenario(const std::string &name) :
    InDScenario(name,
                "/data/" + name + "_recordingMeta.csv",
                "/data/" + name + "_tracks.csv",
                "/data/" + name + "_tracksMeta.csv",
                "/data/" + name + "_background.png") {}

void InDScenario::Parse(const std::string &dataset_root_directory) {
  this->MakePathsAbsolute(dataset_root_directory);
  this->SetBackgroundImageSourcePath(this->background_file_path_);
  this->ParserRecordingMetaFile();
  this->ParserTrackMetaFile();
  this->ParserTrackFile();
}

}