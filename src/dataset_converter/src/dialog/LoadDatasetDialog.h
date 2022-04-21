#ifndef LOADDATASETDIALOG_H
#define LOADDATASETDIALOG_H

#include <QDialog>
#include <QDir>
#include <QString>
#include <QFileDialog>

// Reference to the class defined by the .ui file
namespace Ui {
class LoadDatasetDialog;
}

/**
 * Dialog that will open if the user wants to load a dataset.
 */
class LoadDatasetDialog : public QDialog {
 Q_OBJECT
 private:
  Ui::LoadDatasetDialog *ui; ///< Instance of the .ui file class.

  QDir m_dataset_root_directory; ///< Selected root directory
  QString m_dataset_name; ///< Selected dataset

 private slots:
  /**
   * Triggered if user uses the browse button to set the dataset root directory.
   */
  void onBrowseButtonPressed();

  /**
   * Triggered if user clicks the load button.
   */
  void onAcceptButtonPressed();

 public:
  /**
   * Loads .ui file as set connections.
   * @param parent Parent for qt memory management.
   */
  explicit LoadDatasetDialog(QWidget *parent = nullptr);

  /**
   * Getter for selected root directory.
   * @return Selected root directory.
   */
  [[nodiscard]] const QDir &datasetRootDirectory() const;

  /**
   * Getter for selected dataset.
   * @return Selected dataset.
   */
  [[nodiscard]] const QString &datasetName() const;
};

#endif // LOADDATASETDIALOG_H
