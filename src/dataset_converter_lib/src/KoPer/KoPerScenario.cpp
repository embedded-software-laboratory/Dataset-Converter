#include "CpmDatasetConverter/KoPer/KoPerScenario.h"

#include <map>
#include <iostream>

#include <mat/MatfileReader.h>

namespace CpmDatasetConverter {
void KoPerScenario::MakePathsAbsolute(const std::string &dataset_root_directory) {
  this->labels_file_path_ = dataset_root_directory + this->labels_file_path_;
  this->first_recording_part_file_path_ = dataset_root_directory + this->first_recording_part_file_path_;
  this->second_recording_part_file_path_ = dataset_root_directory + this->second_recording_part_file_path_;
}

void KoPerScenario::ParseLabelFile() {
  MatfileReader matfile_reader(this->labels_file_path_);

  matfile_reader.parseHeader();
  matfile_reader.gotoData();
  matfile_reader.parseDataElement();
  std::vector<DataElement *> des = matfile_reader.dataElements();

  MatrixDataElement *de;
  CompressedDataElement *cde;
  if (des[0]->dataType() != miCOMPRESSED) return;

  cde = dynamic_cast<CompressedDataElement *>(des[0]);
  assert(cde);
  de = dynamic_cast<MatrixDataElement *>(cde->reparse());

  auto matrix_type = de->arrayFlags()->klass();
  if (matrix_type != mxSTRUCT_CLASS) return;
  auto *data = dynamic_cast<Struct *>(cde->reparse());

  std::cout << "Field names:" << std::endl;
  std::cout << "\t";
  for(const auto& field_name : data->fieldNames()->fieldNames()) {
    std::cout << field_name << " ";
  }
  std::cout << std::endl;
}

KoPerScenario::KoPerScenario(const std::string &name) :
    KoPerScenario(name,
                  "/" + name + "/3_BoxfittingLabels_REF_GP_eff_part1of4.mat",
                  "/" + name + "/1_Rec20131118135501_TS_merged_part1of4.mat",
                  "/" + name + "/2_Rec20131118135501_TS_merged_part1of4.mat") {}

KoPerScenario::KoPerScenario(std::string name,
                             std::string labels_file_path,
                             std::string first_recording_part_file_path,
                             std::string second_recording_part_file_path)
    : CpmScenario(std::move(name)),
      labels_file_path_(std::move(labels_file_path)),
      first_recording_part_file_path_(std::move(first_recording_part_file_path)),
      second_recording_part_file_path_(std::move(second_recording_part_file_path)) {}

void KoPerScenario::Parse(const std::string &dataset_root_directory) {
  this->MakePathsAbsolute(dataset_root_directory);
  this->ParseLabelFile();
}

}