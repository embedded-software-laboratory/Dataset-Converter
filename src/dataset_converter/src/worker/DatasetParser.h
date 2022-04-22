#ifndef DATASETPARSER_H
#define DATASETPARSER_H

#include <memory>

#include <QObject>

#include <dataset_converter_lib/DatasetParser.h>

/**
 * Worker class that load data sets using the dataset_converter_lib.
 */
class DatasetParser: public QObject
{
Q_OBJECT
private:
    dataset_converter_lib::DatasetParser *m_datasetParser = nullptr; ///< Data set specific parser from the lib
    cpm_scenario::ScenarioPtrs m_scenarios; ///< Loaded scenarios
    QString m_datasetName; ///< Name of data set
    QString m_datasetRootDirectory; ///< Root directory to search in

    /**
     * Initialises the parser and directly afterwards executes the parsing afterwards.
     */
    void initialise();

    /**
     * Executed the parsing process and with error or loaded.
     */
    void run();

public:
    /**
     * Creates the parser.
     * @param parent Possible parent or qt pointer destruction.
     */
    explicit DatasetParser(QObject *parent = nullptr);

    /**
     * Getter for the parsed scenarios.
     * @return parsed scenarios.
     */
    cpm_scenario::ScenarioPtrs loadedScenarios();

    /**
     * Getter for the parsed data set name.
     * @return Parsed data set name.
     */
    [[nodiscard]] const QString &datasetName() const;

public slots:

    /**
     * Start the parsing process.
     * @param datasetName Data set to parse.
     * @param datasetRootDirectory Directory to look in.
     */
    void loadScenariosFromDataset(QString datasetName, QString datasetRootDirectory);

signals:

    /**
     * Indicator for the current progress. Send periodically from the worker to the main thread.
     * @param currentTask Current progress.
     * @param numberOfTasks Number of tasks ahead.
     */
    void progress(quint16 currentTask, quint16 numberOfTasks);

    /**
     * Indicates error. Aborts process.
     * @param message Error message.
     */
    void error(QString message);

    /**
     * Loading is finished and no errors occurred.
     */
    void loaded();
};

#endif // DATASETPARSER_H
