#include "ScenarioHandler.h"
#include <QDir>
#include <utility>
#include <QtMath>

ScenarioHandler::ScenarioHandler(ScenarioVisualization *visualization, QObject *parent)
    : m_visualization(visualization), QObject(parent)
{}

void ScenarioHandler::writeScenario(QString name,
                                    QString rootDirectoryPath,
                                    size_t fromFrame,
                                    size_t toFrame,
                                    bool exportFullTrajectories)
{
    emit progress(0, 3);
    this->m_exportRootDirectory = std::move(rootDirectoryPath);
    this->m_exportFullTrajectories = exportFullTrajectories;
    this->m_scenarioName = std::move(name);
    this->m_fromFrame = fromFrame;
    this->m_toFrame = toFrame;
    emit progress(1, 3);

    QDir datasetRootDirectory(this->m_exportRootDirectory);
    if (!datasetRootDirectory.exists()) {
        emit error("Dataset root directory does not exists.");
        return;
    }

    // Delete old scenario and make a copy
    this->m_scenarioWriter = std::make_shared<cpm_scenario::ScenarioWriter>(this->m_visualization->scenario());
    this->m_scenarioWriter->SetName(m_scenarioName.toStdString());

    emit progress(2, 3);
    double scaleFactor = this->m_visualization->scaleFactor();
    double shiftXInMeter = this->m_visualization->scenarioShiftX() / scaleFactor;
    double shiftXYnMeter = this->m_visualization->scenarioShiftY() / scaleFactor;
    double rotationInRadians = qDegreesToRadians(this->m_visualization->scenarioRotation());
    std::string scenarioFilePath =
        datasetRootDirectory.absolutePath().toStdString() + "/Scenario_" + this->m_scenarioName.toStdString() + ".xml";

    // Perform transformation into the cpm lab
    this->m_scenarioWriter->Transform(Eigen::Rotation2Dd(rotationInRadians),
                                      Eigen::Vector2d(shiftXInMeter, shiftXYnMeter),
                                      Eigen::AlignedScaling2d(1.0 / 18.0, 1.0 / 18.0));
    // Flip the inverted axis from the visualisation
    this->m_scenarioWriter
        ->Transform(Eigen::Rotation2Dd(0), Eigen::Vector2d(0, -4), Eigen::AlignedScaling2d(1.0, -1.0));
    // Remove all elements not in the scenario area
    this->m_scenarioWriter->RestrictToArea(Eigen::Vector2d(0.55, 0.55), Eigen::Vector2d(4.5 - 1.1, 4.0 - 1.1));
    // Purge trajectories if required.
    if (!this->m_exportFullTrajectories) this->m_scenarioWriter->RestrictTrajectories();
    // Perform temporal shift
    this->m_scenarioWriter->RestrictToFrames(this->m_fromFrame, this->m_toFrame);
    // Write file
    this->m_scenarioWriter->Write(scenarioFilePath);

    emit progress(3, 3);
    emit stored();
}
const cpm_scenario::ScenarioPtr &ScenarioHandler::loadedScenario() const
{
    return m_loadedScenario;
}
void ScenarioHandler::loadScenario(QString name, QString rootDirectoryPath)
{
    emit progress(0, 3);
    this->m_exportRootDirectory = std::move(rootDirectoryPath);
    this->m_scenarioName = std::move(name);
    emit progress(1, 3);

    QDir datasetRootDirectory(this->m_exportRootDirectory);
    if (!datasetRootDirectory.exists()) {
        emit error("Dataset root directory does not exists.");
        return;
    }

    try {
        // Delete old scenario and make a copy
        this->m_scenarioParser = std::make_shared<cpm_scenario::ScenarioParser>(this->m_scenarioName.toStdString());
        emit progress(2, 3);

        std::string scenarioFilePath =
            datasetRootDirectory.absolutePath().toStdString() + "/Scenario_" + this->m_scenarioName.toStdString()
                + ".xml";
        // Parse scenario
        this->m_scenarioParser->Parse(scenarioFilePath);
        // Flip the axis from the visualisation
        this->m_scenarioParser
            ->Transform(Eigen::Rotation2Dd(0), Eigen::Vector2d(0, -4), Eigen::AlignedScaling2d(1.0, -1.0));
        // Scale the scenario to real world meters
        this->m_scenarioParser
            ->Transform(Eigen::Rotation2Dd(0), Eigen::Vector2d(0, 0), Eigen::AlignedScaling2d(18.0, 18.0));

        this->m_loadedScenario = this->m_scenarioParser->GetScenario();
        emit progress(3, 3);
        emit loaded();
    }
    catch (std::exception e) {
        emit error("Parsing of the scenario failed.");
    }
}
