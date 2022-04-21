#ifndef SCENARIOHANDLER_H
#define SCENARIOHANDLER_H

#include <memory>

#include <QObject>

#include <cpm_scenario/Scenario.h>
#include <cpm_scenario/ScenarioWriter.h>
#include <cpm_scenario/ScenarioParser.h>

#include "visualisation/ScenarioVisualization.h"

class ScenarioHandler: public QObject
{
Q_OBJECT
private:
    // Dialog information
    QString m_exportRootDirectory; ///< Scenario directory
    QString m_scenarioName; ///< Scenario name
    size_t m_fromFrame = 0; ///< First frame for the temporal transformation
    size_t m_toFrame = 0; ///< Last frame for the temporal transformation
    bool m_exportFullTrajectories = false; ///< Flag indication if a trajectory purge should be performed

    ScenarioVisualization *const m_visualization = nullptr; ///< Source of all non dialog information

    cpm_scenario::ScenarioWriterPtr m_scenarioWriter; ///< Copy of current scenario that will be clamped and transformed
    cpm_scenario::ScenarioParserPtr m_scenarioParser; ///< Copy of current scenario that will be clamped and transformed

    cpm_scenario::ScenarioPtr m_loadedScenario; ///< Buffer for the loaded scenario
public:
    /**
     * Creates the handler.
     * @param visualization Source for information
     * @param parent Possible parent or qt pointer destruction.
     */
    explicit ScenarioHandler(ScenarioVisualization *visualization, QObject *parent = nullptr);

    [[nodiscard]] const cpm_scenario::ScenarioPtr &loadedScenario() const;

public slots:
    /**
     * Transforms and writes scenario to the disk.
     * @param name Name of the scenario
     * @param rootDirectoryPath Root directory.
     * @param fromFrame Starting frame for temporal shift.
     * @param toFrame End frame for temporal shift.
     * @param exportFullTrajectories Flag to indicate a trajectory purge.
     */
    void writeScenario(QString name,
                       QString rootDirectoryPath,
                       size_t fromFrame,
                       size_t toFrame,
                       bool exportFullTrajectories);

    /**
     * Loads a scenario from the disk.
     * @param name Name of the scenario.
     * @param rootDirectoryPath Directory to search in.
     */
    void loadScenario(QString name, QString rootDirectoryPath);

signals:
    /**
     * Indicator for the current progress. Send periodically from the worker to the main thread.
     * @param currentTask Current progress.
     * @param numberOfTasks Number of tasks ahead.
     */
    void progress(qint16 currentTask, qint16 numberOfTasks);

    /**
     * Indicates error. Aborts process.
     * @param message Error message.
     */
    void error(QString message);
    /**
     * Loading is finished and no errors occurred.
     */
    void loaded();
    /**
     * Storing is finished and no errors occurred.
     */
    void stored();
};

#endif // SCENARIOHANDLER_H
