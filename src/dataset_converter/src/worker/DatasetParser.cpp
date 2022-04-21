#include "DatasetParser.h"
#include <QDir>

#include <dataset_converter_lib/DUT/DutParser.h>
#include <dataset_converter_lib/inD/InDParser.h>
#include <dataset_converter_lib/rounD/RounDParser.h>

DatasetParser::DatasetParser(QObject *parent) : QObject(parent)
{}
cpm_scenario::ScenarioPtrs DatasetParser::loadedScenarios()
{
    return this->m_scenarios;
}
void DatasetParser::loadScenariosFromDataset(QString datasetName, QString datasetRootDirectory)
{
    this->m_datasetName = std::move(datasetName);
    this->m_datasetRootDirectory = std::move(datasetRootDirectory);

    initialise();
}

void DatasetParser::initialise()
{
    delete this->m_datasetParser;
    this->m_scenarios.clear();

    QStringList supportedDatasets = { "DUT", "Ko-Per", "inD", "rounD"};
    if (!supportedDatasets.contains(this->m_datasetName)) {
        emit error("Dataset is not supported.");
        return;
    }
    if (this->m_datasetName == "DUT") {
        this->m_datasetParser = new dataset_converter_lib::DutParser;
    } else if (this->m_datasetName == "inD") {
        this->m_datasetParser = new dataset_converter_lib::InDParser;
    } else if (this->m_datasetName == "rounD") {
        this->m_datasetParser = new dataset_converter_lib::RounDParser;
    }

    QDir datasetRootDirectory(this->m_datasetRootDirectory);
    if (!datasetRootDirectory.exists()) {
        emit error("Dataset root directory does not exists.");
        return;
    }
    this->run();
}
void DatasetParser::run()
{
    qInfo("Dataset: %s", qUtf8Printable(this->m_datasetName));
    qInfo("Dataset root: %s", qUtf8Printable(this->m_datasetRootDirectory));
    QDir datasetRootDirectory(this->m_datasetRootDirectory);

    try {
        long numberOfScenarios = m_datasetParser->GetNumberOfScenarios();
        emit progress(0, numberOfScenarios);
        while (true) {
            long numberOfDatasetsLeft =
                m_datasetParser->ParseNext(datasetRootDirectory.absolutePath().toStdString());
            emit progress(numberOfScenarios - numberOfDatasetsLeft, numberOfScenarios);
            if (numberOfDatasetsLeft == 0)
                break;
            if (numberOfDatasetsLeft < 0) {
                emit error("Parsing of the data set failed.");
                return;
            }
        }
        for (const auto &scenario: this->m_datasetParser->GetScenarios()) {
            this->m_scenarios.push_back(scenario);
        }
        emit loaded();
    } catch (std::exception e){
        emit error("Parsing of the data set failed.");
    }
}
const QString &DatasetParser::datasetName() const
{
    return m_datasetName;
}
