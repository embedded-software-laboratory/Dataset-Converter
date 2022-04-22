#include "LoadDatasetDialog.h"
#include "ui_loaddatasetdialog.h"

#include <QPushButton>

LoadDatasetDialog::LoadDatasetDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoadDatasetDialog)
{
    // Load .ui file
    this->ui->setupUi(this);

    // Remove icons from the buttons
    for (auto &item : this->ui->btn_dialog->buttons()) {
        item->setIcon(QIcon());
    }

    // Setup dialog button bar function
    connect(this->ui->btn_dialog, &QDialogButtonBox::rejected, this, &LoadDatasetDialog::reject);
    connect(this->ui->btn_dialog, &QDialogButtonBox::accepted, this, &LoadDatasetDialog::accept);

    // Copy values from interface than trigger accept
    connect(this->ui->btn_dialog, &QDialogButtonBox::accepted, this, &LoadDatasetDialog::onAcceptButtonPressed);

    // Open the systems file browser then button is pressed
    connect(this->ui->btn_browse, &QToolButton::pressed, this, &LoadDatasetDialog::onBrowseButtonPressed);
}
void LoadDatasetDialog::onBrowseButtonPressed()
{
    // Assume home directory is fine
    QDir suggestedDir = QDir::homePath();

    // If directory is valid and exists use this value as the suggested directory
    QFileInfo fileInfo(this->ui->edit_browse->text());
    if (fileInfo.exists() && fileInfo.isDir())
        suggestedDir.setPath(fileInfo.absoluteFilePath());

    // Open file dialog and lets the user select a directory
    QUrl dir = QFileDialog::getExistingDirectoryUrl(this,
                                                    tr("Open Dataset"),
                                                    suggestedDir.absolutePath(),
                                                    QFileDialog::ShowDirsOnly);

    // Terminate if no file is selected
    if (dir.isEmpty() || !dir.isLocalFile()) return;

    // Update line edit
    this->ui->edit_browse->setText(dir.toLocalFile());
}

void LoadDatasetDialog::onAcceptButtonPressed()
{
    // Copy data from the ui to the data model
    this->m_dataset_name = this->ui->combo_dataset->currentText();
    this->m_dataset_root_directory.setPath(this->ui->edit_browse->text());
}

const QDir &LoadDatasetDialog::datasetRootDirectory() const
{
    return m_dataset_root_directory;
}

const QString &LoadDatasetDialog::datasetName() const
{
    return m_dataset_name;
}
