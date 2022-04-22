#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QProgressDialog>
#include <QStringListModel>
#include <worker/ScenarioHandler.h>
#include <dialog/LoadScenarioDialog.h>
#include <QTimer>

#include "visualisation/GraphicsViewZoomHandler.h"
#include "visualisation/GraphicsViewClickHandler.h"
#include "dialog/LoadDatasetDialog.h"
#include "dialog/SaveScenarioDialog.h"
#include "worker/DatasetParser.h"
#include "worker/LaneletHandler.h"
#include "visualisation/ScenarioVisualization.h"

namespace Ui
{
class MainWindow;
}

/**
 * Main class that communicates with all sub classes.
 */
class MainWindow: public QMainWindow
{
Q_OBJECT
private:
    Ui::MainWindow *ui; ///< Ui from .ui file

    LoadDatasetDialog *m_loadDatasetDialog; ///< Load data set dialog
    LoadScenarioDialog *m_loadScenarioDialog; ///< Load scenario dialog
    SaveScenarioDialog *m_saveScenarioDialog; ///< Save scenario dialog
    QProgressDialog *m_progressDialog; ///< Progress dialog

    DatasetParser *m_datasetParser; ///< Data set parser worker class
    LaneletHandler *m_laneletHandler; ///< Lanelet handler worker class
    ScenarioHandler *m_scenarioHandler; ///< Scenario handler worker class
    QThread m_workerThread; ///< Worker thread

    cpm_scenario::ScenarioPtrs m_scenarios; ///< Current scenario

    ScenarioVisualization *m_scenarioVisualization; ///< Scenario and background visualisation service
    LaneletVisualisation *m_laneletVisualisation; ///< Lanelet visualisation service

    QStringListModel *m_scenarioListModel; ///< Accessible scenarios

    GraphicsViewZoomHandler *m_graphicsViewZoomHandler; ///< Handler that manages zooming
    GraphicsViewClickHandler *m_graphicsViewClickHandler; ///< Handler that manages all interaction with the canvas

    QTimer *m_playbackTimer; ///< Timer for playback

    /**
     * Restores the window state form last usage.
     */
    void restoreWindowStates();

    /**
     * Stores the window state for later usage.
     */
    void storeWindowState();

    /**
     * Fills in the save scenario dialog with suggested values.
     */
    void updateInformationForSaveScenarioDialog();

    /**
     * Load a transformation file if one is found.
     * @param file Possible transformation file.
     */
    void loadTransformationInformationFromFile(const QFileInfo &file);

    /**
     * Getter for the default transformation file path.
     * @return Default transformation file path.
     */
    [[nodiscard]] QString getDefaultTransformationFilePath() const;

public:
    /**
     * Creates the instance, initialises the ui and connects the required bindings.
     * @param parent
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * Closes the worker thread.
     */
    ~MainWindow() override;

protected:

    /**
     * Makes sure the current window state is stored.
     * @param event Possible reasons how the window was closed.
     */
    void closeEvent(QCloseEvent *event) override;

private slots:
    /**
     * Open the about dialog if requested by the user.
     */
    void onAboutDialogRequested();
    /**
     * Open the load dataset dialog if requested by the user.
     */
    void onLoadDatasetDialogRequested();
    /**
     * Open the load scenario dialog if requested by the user.
     */
    void onLoadScenarioDialogRequested();
    /**
     * Open the load lanelet map dialog if requested by the user.
     */
    void onLoadLaneletMapDialogRequested();
    /**
     * Open the load transformation dialog if requested by the user.
     */
    void onLoadTransformationDialogRequested();

    /**
     * Open the save lanelet map dialog if requested by the user.
     */
    void onSaveLaneletMapDialogRequested();
    /**
     * Open the save scenario map dialog if requested by the user.
     */
    void onSaveScenarioDialogRequested();
    /**
     * Open the export lanelet map dialog if requested by the user.
     */
    void onExportLaneletMapDialogRequested();
    /**
     * Open the save transformation dialog if requested by the user.
     */
    void onSaveTransformationDialogRequested();

    /**
     * Triggered if the user clicks the skip frame forward button.
     */
    void onSkipFramesForward();
    /**
     * Triggered if the user clicks the skip frame backward button.
     */
    void onSkipFramesBackward();
    /**
     * Triggered by the playback timer every time the next frame should be displayed.
     */
    void onPlaybackTimer();
    /**
     * Triggered if the user clicks the play/pause button.
     */
    void onStartStopPlayback();

    /**
     * Triggered if the user changes the current selected edit tool.
     */
    void onToolSelectionChanged(int id);

    /**
     * Triggered if the user changes the current selected scenario.
     */
    void onSelectedScenarioChanged(const QItemSelection &selection);

    /*
     * Slot triggered if the worker thread has finished a task.
     */
    void onDatasetLoaded();
    void onLaneletMapLoaded();
    void onScenarioLoaded();
    void onScenarioStored();

    /*
     * Slots to monitor the work done by the worker thread.
     */
    void onProgressDuringLoading(int current, int max);
    void onErrorDuringLoading(const QString &message);

signals:
    /*
     * Signals to trigger a task for the worker thread.
     */
    void requestDataset(QString datasetName, QString datasetRootDirectoryPath);
    void requestScenario(QString datasetName, QString datasetRootDirectoryPath);
    void requestLaneletMap(QString laneletMapFilePath, qreal scaleFactor);
    void storeLaneletMap(QString laneletMapFilePath, qreal scaleFactor);
    void storeScenario(QString name,
                       QString rootDirectoryPath,
                       size_t fromFrame,
                       size_t toFrame,
                       bool exportFullTrajectories);
    void exportLaneletMap(QString laneletMapFilePath, QGraphicsScene *scene);

};

#endif // MAINWINDOW_H
