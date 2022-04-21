#include "SaveScenarioDialog.h"
#include "ui_savescenariodialog.h"

#include <QPushButton>
#include <QSpinBox>

SaveScenarioDialog::SaveScenarioDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::SaveScenarioDialog)
{
    this->ui->setupUi(this);

    for (auto &item : this->ui->btn_dialog->buttons()){
      item->setIcon(QIcon());
    }

    connect(this->ui->btn_dialog, &QDialogButtonBox::rejected, this, &SaveScenarioDialog::reject);

    connect(this->ui->btn_dialog, &QDialogButtonBox::accepted, this,
            &SaveScenarioDialog::onAcceptButtonPressed);
    connect(this->ui->btn_dialog, &QDialogButtonBox::accepted, this, &SaveScenarioDialog::accept);

    connect(this->ui->btn_browse, &QToolButton::pressed, this,
            &SaveScenarioDialog::onBrowseButtonPressed);

    connect(this->ui->spinner_from_frame, QOverload<int>::of(&QSpinBox::valueChanged), this->ui->spinner_to_frame,
            &QSpinBox::setMinimum);
    connect(this->ui->spinner_to_frame, QOverload<int>::of(&QSpinBox::valueChanged), this->ui->spinner_from_frame,
            &QSpinBox::setMaximum);
}

void SaveScenarioDialog::onBrowseButtonPressed()
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
    if (dir.isEmpty() || !dir.isLocalFile())
        return;

    // Update line edit
    this->ui->edit_browse->setText(dir.toLocalFile());
}

void SaveScenarioDialog::onAcceptButtonPressed()
{
    this->m_fromFrame = this->ui->spinner_from_frame->value();
    this->m_toFrame = this->ui->spinner_to_frame->value();
    this->m_scenarioRootPath.setPath(this->ui->edit_browse->text());
    this->m_scenarioName = this->ui->edit_name->text();
    this->m_export_full_trajectories = this->ui->radio_full_trajectory->isChecked();
}
void SaveScenarioDialog::suggestInput(const QString &name, const QDir &targetDirectory)
{
    this->ui->edit_name->setText(name);
    this->ui->edit_browse->setText(targetDirectory.absolutePath());
}
void SaveScenarioDialog::setFrameLimits(long fromFrame, long toFrame)
{
    this->ui->spinner_from_frame->setMinimum(static_cast<int>(fromFrame));
    this->ui->spinner_from_frame->setValue(static_cast<int>(fromFrame));
    this->ui->spinner_to_frame->setMaximum(static_cast<int>(toFrame));
    this->ui->spinner_to_frame->setValue(static_cast<int>(toFrame));
}
const QString &SaveScenarioDialog::scenarioName() const
{
    return m_scenarioName;
}
const QDir &SaveScenarioDialog::scenarioRootPath() const
{
    return m_scenarioRootPath;
}
size_t SaveScenarioDialog::fromFrame() const
{
    return m_fromFrame;
}
size_t SaveScenarioDialog::toFrame() const
{
    return m_toFrame;
}
bool SaveScenarioDialog::exportFullTrajectories() const
{
    return m_export_full_trajectories;
}
