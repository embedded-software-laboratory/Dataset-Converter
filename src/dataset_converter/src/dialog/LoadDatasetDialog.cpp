#include "LoadDatasetDialog.h"
#include "ui_loaddatasetdialog.h"

#include <QPushButton>

LoadDatasetDialog::LoadDatasetDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoadDatasetDialog)
{
    // Load .ui file
    this->ui->setupUi(this);

    connect(this->ui->btn_dialog, &QDialogButtonBox::rejected, this, &LoadDatasetDialog::reject);

    // Copy values from interface than trigger accept
    connect(this->ui->btn_dialog, &QDialogButtonBox::accepted, this, &LoadDatasetDialog::onAcceptButtonPressed);
    connect(this->ui->btn_dialog, &QDialogButtonBox::accepted, this, &LoadDatasetDialog::accept);

    // Open the systems file browser
    connect(this->ui->btn_browse, &QToolButton::pressed, this, &LoadDatasetDialog::onBrowseButtonPressed);
}
void LoadDatasetDialog::onBrowseButtonPressed()
{
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
