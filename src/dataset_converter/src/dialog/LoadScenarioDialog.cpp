#include "LoadScenarioDialog.h"
#include "ui_loadscenariodialog.h"

#include <QPushButton>

LoadScenarioDialog::LoadScenarioDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoadScenarioDialog)
{
    this->ui->setupUi(this);

    for (auto &item : this->ui->btn_dialog->buttons()){
      item->setIcon(QIcon());
    }

    connect(this->ui->btn_dialog, &QDialogButtonBox::rejected, this, &LoadScenarioDialog::reject);

    // Copy values from interface than trigger accept
    connect(this->ui->btn_dialog, &QDialogButtonBox::accepted, this, &LoadScenarioDialog::onAcceptButtonPressed);
    connect(this->ui->btn_dialog, &QDialogButtonBox::accepted, this, &LoadScenarioDialog::accept);

    // Open the systems file browser
    connect(this->ui->btn_browse, &QToolButton::pressed, this, &LoadScenarioDialog::onBrowseButtonPressed);
}
void LoadScenarioDialog::onBrowseButtonPressed()
{
    QDir suggestedDir = QDir::homePath();

    // If directory is valid and exists use this value as the suggested directory
    QFileInfo fileInfo(this->ui->edit_browse->text());
    if (fileInfo.exists() && fileInfo.isDir())
        suggestedDir.setPath(fileInfo.absoluteFilePath());

    // Open file dialog and lets the user select a directory
    QUrl dir = QFileDialog::getExistingDirectoryUrl(this, tr("Scenario Root Directory"),
                                                    suggestedDir.absolutePath(),
                                                    QFileDialog::ShowDirsOnly);

    // Terminate if no file is selected
    if (dir.isEmpty() || !dir.isLocalFile()) return;

    // Update line edit
    this->ui->edit_browse->setText(dir.toLocalFile());
}

void LoadScenarioDialog::onAcceptButtonPressed()
{
    this->m_dataset_name = this->ui->edit_name->text();
    this->m_dataset_root_directory.setPath(this->ui->edit_browse->text());
}
const QDir &LoadScenarioDialog::scenarioRootDirectory() const
{
    return m_dataset_root_directory;
}
const QString &LoadScenarioDialog::scenarioName() const
{
    return m_dataset_name;
}
