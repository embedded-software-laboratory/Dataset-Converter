#ifndef SAVESCENARIODIALOG_H
#define SAVESCENARIODIALOG_H

#include <QDialog>
#include <QDir>
#include <QString>
#include <QFileDialog>

// Reference to the class defined by the .ui file
namespace Ui {
class SaveScenarioDialog;
}

/**
 * Dialog that will open if the user wants to load a dataset.
 */
class SaveScenarioDialog : public QDialog
{
    Q_OBJECT
private:
    Ui::SaveScenarioDialog *ui; ///< Instance of the .ui file class.

    QString m_scenarioName; ///< Name to export scenario with
    QDir m_scenarioRootPath; ///< Selected root directory
    size_t m_fromFrame = 0; ///< Frame to start scenario with
    size_t m_toFrame = 0; ///< Frame to stop scenario with
    bool m_export_full_trajectories = false; ///< Flag if the full trajectory should be exported

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
    explicit SaveScenarioDialog(QWidget *parent = nullptr);

    /**
     * Getter for the selected scenario name.
     * @return Scenario name.
     */
    [[nodiscard]] const QString &scenarioName() const;

    /**
     * Getter for the selected scenario directory.
     * @return Scenario directory.
     */
    [[nodiscard]] const QDir &scenarioRootPath() const;

    /**
     * Getter for the selected starting frame.
     * @return Starting frame.
     */
    [[nodiscard]] size_t fromFrame() const;

    /**
     * Getter for the selected end frame.
     * @return Starting frame.
     */
    [[nodiscard]] size_t toFrame() const;

    /**
     * Getter for the selected trajectory mode.
     * @return Trajectory mode.
     */
    [[nodiscard]] bool exportFullTrajectories() const;

public slots:

    /**
     * Will prefill the field with the suggested name and directory.
     * @param name Suggested name.
     * @param targetDirectory Suggested directory.
     */
    void suggestInput(const QString& name, const QDir& targetDirectory);

    /**
     * Will limit the frame that can be selected to the source scenario length.
     * @param fromFrame Min frame.
     * @param toFrame Max frame.
     */
    void setFrameLimits(long fromFrame, long toFrame);
};

#endif // SAVESCENARIODIALOG_H
