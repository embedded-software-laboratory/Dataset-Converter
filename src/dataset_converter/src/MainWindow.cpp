#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QProgressDialog>
#include <QDoubleSpinBox>
#include <QSettings>
#include <QFileDialog>
#include <QGraphicsBlurEffect>

#include "worker/DatasetParser.h"
#include "dialog/AboutDialog.h"
#include "dialog/ErrorDialog.h"

void MainWindow::restoreWindowStates()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName(), this);
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("window_state").toByteArray());
}

void MainWindow::storeWindowState()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName(), this);
    settings.setValue("geometry", saveGeometry());
    settings.setValue("window_state", saveState());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    this->ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, false);

    // Setup progress dialog
    this->m_progressDialog = new QProgressDialog(this);
    this->m_progressDialog->setWindowModality(Qt::WindowModal);
    this->m_progressDialog->setCancelButton(nullptr);
    this->m_progressDialog->setMinimumDuration(-1);
    this->m_progressDialog->setMinimum(0);
    this->m_progressDialog->setMaximum(0);
    this->m_progressDialog->setValue(0);

    // Setup visualization manager
    this->m_scenarioVisualization = new ScenarioVisualization(this->ui->canvas, this);
    this->m_laneletVisualisation = new LaneletVisualisation(this->ui->canvas, this);

    // Setup graphics view handler
    this->m_graphicsViewZoomHandler = new GraphicsViewZoomHandler(this->ui->canvas);
    this->m_graphicsViewClickHandler = new GraphicsViewClickHandler(this->ui->canvas);

    // Setup tool button
    this->ui->buttonGroup->setId(this->ui->btn_selection, static_cast<int>(EditorMode::SELECT));
    this->ui->buttonGroup->setId(this->ui->btn_add_node, static_cast<int>(EditorMode::NODE_TOOL));
    this->ui->buttonGroup->setId(this->ui->btn_add_path, static_cast<int>(EditorMode::WAY_TOOL));
    this->ui->buttonGroup->setId(this->ui->btn_add_lanelet,
                                 static_cast<int>(EditorMode::LANELET_TOOL));
    connect(this->ui->buttonGroup, &QButtonGroup::idClicked, this,
            &MainWindow::onToolSelectionChanged);

    // Setup dataset loading dialog
    this->m_loadDatasetDialog = new LoadDatasetDialog(this);

    // Setup save scenario dialog
    this->m_saveScenarioDialog = new SaveScenarioDialog(this);

    // Setup save scenario dialog
    this->m_loadScenarioDialog = new LoadScenarioDialog(this);

    // Setup input methods
    connect(this->ui->spinner_shift_x, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this->m_scenarioVisualization, &ScenarioVisualization::setScenarioShiftX);
    connect(this->ui->spinner_shift_y, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this->m_scenarioVisualization, &ScenarioVisualization::setScenarioShiftY);
    connect(this->ui->spinner_rotation, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this->m_scenarioVisualization, &ScenarioVisualization::setScenarioRotation);

    connect(this->ui->btn_skip_forward, &QToolButton::pressed, this,
            &MainWindow::onSkipFramesForward);
    connect(this->ui->btn_skip_backward, &QToolButton::pressed, this,
            &MainWindow::onSkipFramesBackward);
    connect(this->ui->btn_play_pause, &QToolButton::pressed, this,
            &MainWindow::onStartStopPlayback);

    connect(this->ui->slider_time, &QAbstractSlider::valueChanged, this->m_scenarioVisualization,
            &ScenarioVisualization::setFrame);
    connect(this->ui->slider_alpha_scenario, &QAbstractSlider::valueChanged,
            this->m_scenarioVisualization, [this](qint32 value)
            {
                this->m_scenarioVisualization->setScenarioTransparency(value / 100.0);
            });
    connect(this->ui->slider_alpha_background, &QAbstractSlider::valueChanged,
            this->m_scenarioVisualization, [this](qint32 value)
            {
                this->m_scenarioVisualization->setLabTransparency(value / 100.0);
            });

    connect(this->ui->btn_spline, &QToolButton::pressed, this, [this]()
    {
        this->m_laneletVisualisation->splineSelection(this->ui->slider_spline_factor->value()
                                                          / 100.0);
    });

    // Setup actions
    connect(this->ui->action_about, &QAction::triggered, this, &MainWindow::onAboutDialogRequested);
    connect(this->ui->action_load_dataset, &QAction::triggered, this,
            &MainWindow::onLoadDatasetDialogRequested);
    connect(this->ui->action_load_scenario, &QAction::triggered, this,
            &MainWindow::onLoadScenarioDialogRequested);
    connect(this->ui->action_load_lanelet_map, &QAction::triggered, this,
            &MainWindow::onLoadLaneletMapDialogRequested);
    connect(this->ui->action_save_lanelet_map, &QAction::triggered, this,
            &MainWindow::onSaveLaneletMapDialogRequested);
    connect(this->ui->action_export_as_svg, &QAction::triggered, this,
            &MainWindow::onExportLaneletMapDialogRequested);
    connect(this->ui->action_quit, &QAction::triggered, this, &MainWindow::close);
    connect(this->ui->action_node_tool, &QAction::triggered, this->ui->btn_add_node,
            &QToolButton::animateClick);
    connect(this->ui->action_select_tool, &QAction::triggered, this->ui->btn_selection,
            &QToolButton::animateClick);
    connect(this->ui->action_path_tool, &QAction::triggered, this->ui->btn_add_path,
            &QToolButton::animateClick);
    connect(this->ui->action_lanelet_tool, &QAction::triggered, this->ui->btn_add_lanelet,
            &QToolButton::animateClick);
    connect(this->ui->action_spline_path, &QAction::triggered, this->ui->btn_spline,
            &QToolButton::animateClick);
    connect(this->ui->action_save_transformation, &QAction::triggered, this,
            &MainWindow::onSaveTransformationDialogRequested);
    connect(this->ui->action_load_transformation, &QAction::triggered, this,
            &MainWindow::onLoadTransformationDialogRequested);
    connect(this->ui->action_save_scenario, &QAction::triggered, this,
            &MainWindow::onSaveScenarioDialogRequested);

    // Setup dataset parser
    m_datasetParser = new DatasetParser();
    m_datasetParser->moveToThread(&this->m_workerThread);
    connect(&m_workerThread, &QThread::finished, m_datasetParser, &QObject::deleteLater);
    connect(this, &MainWindow::requestDataset, m_datasetParser, &DatasetParser::loadScenariosFromDataset);
    connect(m_datasetParser, &DatasetParser::loaded, this, &MainWindow::onDatasetLoaded);
    connect(m_datasetParser, &DatasetParser::progress, this,
            &MainWindow::onProgressDuringLoading);
    connect(m_datasetParser, &DatasetParser::error, this, &MainWindow::onErrorDuringLoading);

    // Setup lanelet parser
    m_laneletHandler = new LaneletHandler();
    m_laneletHandler->moveToThread(&this->m_workerThread);
    connect(&m_workerThread, &QThread::finished, m_laneletHandler, &QObject::deleteLater);
    connect(this, &MainWindow::requestLaneletMap, m_laneletHandler, &LaneletHandler::parseLanelet);
    connect(this, &MainWindow::storeLaneletMap, m_laneletHandler, &LaneletHandler::writeLanelet);
    connect(this, &MainWindow::exportLaneletMap, m_laneletHandler,
            &LaneletHandler::writeLaneletImage);
    connect(m_laneletHandler, &LaneletHandler::loaded, this, &MainWindow::onLaneletMapLoaded);
    connect(m_laneletHandler, &LaneletHandler::progress, this,
            &MainWindow::onProgressDuringLoading);
    connect(m_laneletHandler, &LaneletHandler::error, this, &MainWindow::onErrorDuringLoading);

    // Setup scenario handler
    m_scenarioHandler = new ScenarioHandler(this->m_scenarioVisualization);
    m_scenarioHandler->moveToThread(&this->m_workerThread);
    connect(&m_workerThread, &QThread::finished, m_scenarioHandler, &QObject::deleteLater);
    connect(this, &MainWindow::storeScenario, m_scenarioHandler, &ScenarioHandler::writeScenario);
    connect(this, &MainWindow::requestScenario, m_scenarioHandler, &ScenarioHandler::loadScenario);
    connect(m_scenarioHandler, &ScenarioHandler::stored, this, &MainWindow::onScenarioStored);
    connect(m_scenarioHandler, &ScenarioHandler::loaded, this, &MainWindow::onScenarioLoaded);
    connect(m_scenarioHandler, &ScenarioHandler::progress, this, &MainWindow::onProgressDuringLoading);
    connect(m_scenarioHandler, &ScenarioHandler::error, this, &MainWindow::onErrorDuringLoading);
    // Add new
    connect(this->m_graphicsViewClickHandler, &GraphicsViewClickHandler::requestAddNode,
            this->m_laneletHandler, &LaneletHandler::addNode);
    connect(m_laneletHandler, &LaneletHandler::nodeAdded, this->m_laneletVisualisation,
            &LaneletVisualisation::visualizeNode);

    connect(this->m_graphicsViewClickHandler, &GraphicsViewClickHandler::requestNewWay,
            this->m_laneletHandler, &LaneletHandler::addWayWithNode);
    connect(m_laneletHandler, &LaneletHandler::wayAdded, this->m_laneletVisualisation,
            &LaneletVisualisation::visualizeWay);

    connect(this->m_graphicsViewClickHandler, &GraphicsViewClickHandler::requestNewLanelet,
            this->m_laneletHandler, &LaneletHandler::addLaneletWithPath);
    connect(m_laneletHandler, &LaneletHandler::laneletAdded, this->m_laneletVisualisation,
            &LaneletVisualisation::visualizeLanelet);

    // Add to existing
    connect(this->m_graphicsViewClickHandler, &GraphicsViewClickHandler::requestAddNodeToWay,
            this->m_laneletVisualisation, &LaneletVisualisation::addNodeToWay);

    connect(this->m_graphicsViewClickHandler, &GraphicsViewClickHandler::requestAddWayToLanelet,
            this->m_laneletVisualisation, &LaneletVisualisation::addWayToLanelet);

    // Removal
    connect(this->ui->action_delete_node, &QAction::triggered, this->m_graphicsViewClickHandler,
            &GraphicsViewClickHandler::actionToRemoveNodeTriggered);
    connect(this->m_graphicsViewClickHandler, &GraphicsViewClickHandler::requestRemoveNode,
            this->m_laneletVisualisation, &LaneletVisualisation::removeNode);
    connect(this->m_laneletVisualisation, &LaneletVisualisation::nodeRemovedFromVisualisation,
            this->m_laneletHandler, &LaneletHandler::removeNode);

    connect(this->ui->action_delete_way, &QAction::triggered, this->m_graphicsViewClickHandler,
            &GraphicsViewClickHandler::actionToRemoveWayTriggered);
    connect(this->m_graphicsViewClickHandler, &GraphicsViewClickHandler::requestRemoveWay,
            this->m_laneletVisualisation, &LaneletVisualisation::removeWay);
    connect(this->m_laneletVisualisation, &LaneletVisualisation::wayRemovedFromVisualisation,
            this->m_laneletHandler, &LaneletHandler::removeWay);

    connect(this->ui->action_delete_lanelet, &QAction::triggered, this->m_graphicsViewClickHandler,
            &GraphicsViewClickHandler::actionToRemoveLaneletTriggered);
    connect(this->m_graphicsViewClickHandler, &GraphicsViewClickHandler::requestRemoveLanelet,
            this->m_laneletVisualisation, &LaneletVisualisation::removeLanelet);
    connect(this->m_laneletVisualisation, &LaneletVisualisation::laneletRemovedFromVisualisation,
            this->m_laneletHandler, &LaneletHandler::removeLanelet);

    // Enable disable
    connect(this->m_graphicsViewClickHandler, &GraphicsViewClickHandler::selectedLaneletChanged,
            this, [this](LaneletItem *item)
            { this->ui->menu_lanelet->setEnabled(item); });
    connect(this->m_graphicsViewClickHandler, &GraphicsViewClickHandler::selectedPathChanged, this,
            [this](WayItem *item)
            { this->ui->menu_way->setEnabled(item); });
    connect(this->m_graphicsViewClickHandler, &GraphicsViewClickHandler::selectedNodeChanged, this,
            [this](NodeItem *item)
            { this->ui->menu_node->setEnabled(item); });

    // Additional actions
    connect(this->ui->action_flip_direction, &QAction::triggered, this->m_graphicsViewClickHandler,
            &GraphicsViewClickHandler::actionToFlipLaneletTriggered);

    // Start secondary thread
    m_workerThread.start();

    // Connect scenario list
    this->m_scenarioListModel = new QStringListModel(this);
    this->m_scenarioListModel->setStringList(QStringList() << "No Scenarios loaded");
    this->ui->list_scenario->setModel(this->m_scenarioListModel);
    connect(this->ui->list_scenario->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWindow::onSelectedScenarioChanged);

    this->m_playbackTimer = new QTimer(this);
    connect(this->m_playbackTimer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::onPlaybackTimer));

    // Move window to last position and restore full screen state
    this->restoreWindowStates();
}
MainWindow::~MainWindow()
{
    // Make sure that worker thread
    this->m_workerThread.quit();
    this->m_workerThread.wait();
    QMainWindow::~QMainWindow();
}

void MainWindow::closeEvent(QCloseEvent * /*event*/)
{
    storeWindowState();
}

void MainWindow::onAboutDialogRequested()
{
    QString text =
        R"(<html><head/><body><p align="center"><span style=" font-size:18pt; font-weight:600;">Dataset Converter</span></p><p align="center">)"
            + QApplication::applicationName() + "<br>"
            + "Version: " + QApplication::applicationVersion() + "<br>" + "<br>"
            + "Copyright © 2022 " + QApplication::organizationName()
            + "</p></body></html>";
    AboutDialog aboutDialog(this, text);
    aboutDialog.exec();
}

void MainWindow::onLoadDatasetDialogRequested()
{
    // Cancel if loading dialog is closed
    if (!this->m_loadDatasetDialog->exec())
        return;

    // Extract data from dialog
    QString datasetName = this->m_loadDatasetDialog->datasetName();
    QDir datasetRootDirectoryPath = this->m_loadDatasetDialog->datasetRootDirectory();

    // Check if directory is valid and exists
    if (!datasetRootDirectoryPath.exists()) {
        ErrorDialog
            messageBox(this, "Dataset root directory not found. Make sure the selected directory is a valid path.");
        messageBox.exec();
        return;
    }

    // Make progress dialog
    this->m_progressDialog->setLabelText("<html><b>Loading dataset please wait.</b><br>You may not "
                                         "cancel the loading process.</html>");
    this->m_progressDialog->setValue(0);
    this->m_progressDialog->show();

    // Request dataset from worker thread
    emit requestDataset(datasetName, datasetRootDirectoryPath.absolutePath());
}
void MainWindow::onLoadScenarioDialogRequested()
{
    // Cancel if loading dialog is closed
    if (!this->m_loadScenarioDialog->exec())
        return;
    // Extract data from dialog
    QString datasetName = this->m_loadScenarioDialog->scenarioName();
    QDir datasetRootDirectoryPath = this->m_loadScenarioDialog->scenarioRootDirectory();

    // Check if directory is valid and exists
    if (!datasetRootDirectoryPath.exists()) {
        ErrorDialog
            messageBox(this, "Scenario root directory not found. Make sure the selected directory is a valid path.");
        messageBox.exec();
        return;
    }

    // Make progress dialog
    this->m_progressDialog->setLabelText("<html><b>Loading scenario please wait.</b><br>You may not "
                                         "cancel the loading process.</html>");
    this->m_progressDialog->setValue(0);
    this->m_progressDialog->show();

    // Request dataset from worker thread
    emit requestScenario(datasetName, datasetRootDirectoryPath.absolutePath());
}
void MainWindow::onLoadLaneletMapDialogRequested()
{
    // Open save as dialog
    QFileDialog dialog(this);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter("*.osm");
    dialog.selectNameFilter("*.osm");
    dialog.selectFile(QDir::homePath() + "/Lanelet.osm");

    auto currentScenario = this->m_scenarioVisualization->scenario();
    if (currentScenario) {
        dialog.selectFile(QDir::homePath() + QString::fromStdString("/Lanelet_")
                              + QString::fromStdString(currentScenario->GetName())
                              + QString::fromStdString(".osm"));

        // Assume standard file structure
        QString pathOnSimonsComputer = QString("%1/%2/%3/Lanelet_%4.osm")
            .arg(QDir::homePath(),
                 "Datasets/Converted",
                 this->m_datasetParser->datasetName(),
                 currentScenario->GetName().c_str());
        QFileInfo fileOnSimonsComputer(pathOnSimonsComputer);
        if (fileOnSimonsComputer.exists())
            dialog.selectFile(fileOnSimonsComputer.absoluteFilePath());
    }
    // Cancel if loading dialog is closed
    if (!dialog.exec())
        return;

    // Extract data from dialog
    auto filePath = dialog.selectedFiles().first();
    QFileInfo file(filePath);

    // Check if file is valid and is not a directory
    if (!file.exists() || file.isDir()) {
        ErrorDialog messageBox(this, "Lanelet map file not found. Make sure the selected file is a valid path.");
        messageBox.exec();
        return;
    }

    // Make progress dialog
    this->m_progressDialog->setLabelText("<html><b>Loading dataset please wait.</b><br>You may not "
                                         "cancel the loading process.</html>");
    this->m_progressDialog->setValue(0);
    this->m_progressDialog->show();

    // Request lanelet map form worker thread
    emit requestLaneletMap(file.absoluteFilePath(), this->m_scenarioVisualization->scaleFactor());
}
void MainWindow::onSaveLaneletMapDialogRequested()
{
    // Open save as dialog
    QFileDialog dialog(this);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("*.osm");
    dialog.selectNameFilter("*.osm");
    dialog.selectFile(QDir::homePath() + "/Lanelet.osm");

    auto currentScenario = this->m_scenarioVisualization->scenario();
    if (currentScenario) {
        dialog.selectFile(QString::fromStdString("Lanelet_")
                              + QString::fromStdString(currentScenario->GetName())
                              + QString::fromStdString(".osm"));

        // Assume standard file structure
        QString pathOnSimonsComputer = QString("%1/%2/%3/Lanelet_%4.osm")
            .arg(QDir::homePath(),
                 "Datasets/Converted",
                 this->m_datasetParser->datasetName(),
                 currentScenario->GetName().c_str());
        QFileInfo fileOnSimonsComputer(pathOnSimonsComputer);
        if (fileOnSimonsComputer.exists())
            dialog.selectFile(fileOnSimonsComputer.absoluteFilePath());
    }

    // Abort if cancel is pressed
    if (!dialog.exec())
        return;

    // Get file information from dialog
    QFileInfo file(dialog.selectedFiles().first());

    // Advise worker to write data
    emit storeLaneletMap(file.absoluteFilePath(), this->m_scenarioVisualization->scaleFactor());
}
void MainWindow::onSaveScenarioDialogRequested()
{
    // Cancel if loading dialog is closed
    if (!this->m_saveScenarioDialog->exec()) return;

    // Extract data from dialog
    auto filePath = this->m_saveScenarioDialog->scenarioRootPath().absolutePath();
    auto toFrame = this->m_saveScenarioDialog->toFrame();
    auto fromFrame = this->m_saveScenarioDialog->fromFrame();
    auto scenarioName = this->m_saveScenarioDialog->scenarioName();
    auto exportFullTrajectories = this->m_saveScenarioDialog->exportFullTrajectories();

    qDebug() << filePath;

    QFileInfo file(filePath);
    // Check if file is valid and is not a directory
    if (!file.exists() || !file.isDir()) {
        ErrorDialog messageBox(this, "Export directory not found. Make sure the selected directory is a valid path.");
        messageBox.exec();
        return;
    }

    // Make progress dialog
    this->m_progressDialog->setLabelText("<html><b>Loading dataset please wait.</b><br>You may not "
                                         "cancel the loading process.</html>");
    this->m_progressDialog->setValue(0);
    this->m_progressDialog->show();


    // Request dataset from worker thread
    emit storeScenario(scenarioName, filePath, fromFrame, toFrame, exportFullTrajectories);
}
void MainWindow::updateInformationForSaveScenarioDialog()
{
    auto scenario = m_scenarioVisualization->scenario();
    m_saveScenarioDialog->setFrameLimits(0, scenario->GetNumberOfFrames());

    // Assume standard file structure
    QString pathOnSimonsComputer = QString("%1/%2/%3/")
        .arg(QDir::homePath(), "Datasets/Converted", m_datasetParser->datasetName());

    m_saveScenarioDialog->suggestInput(scenario->GetName().c_str(),
                                       pathOnSimonsComputer);
}

void MainWindow::onExportLaneletMapDialogRequested()
{
    // Open save as dialog
    QFileDialog dialog(this);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("*.svg");
    dialog.selectNameFilter("*.svg");
    dialog.selectFile(QDir::homePath() + "/Lanelet.svg");

    auto currentScenario = this->m_scenarioVisualization->scenario();
    if (currentScenario) {
        dialog.selectFile(QString::fromStdString("Lanelet_")
                              + QString::fromStdString(currentScenario->GetName())
                              + QString::fromStdString(".svg"));

        // Assume standard file structure
        QString pathOnSimonsComputer = QString("%1/%2/%3/Lanelet_%4.svg")
            .arg(QDir::homePath(),
                 "Datasets/Converted",
                 this->m_datasetParser->datasetName(),
                 currentScenario->GetName().c_str());
        QFileInfo fileOnSimonsComputer(pathOnSimonsComputer);
        if (fileOnSimonsComputer.exists())
            dialog.selectFile(fileOnSimonsComputer.absoluteFilePath());
    }

    // Abort if cancel is pressed
    if (!dialog.exec())
        return;

    // Get file information from dialog
    QFileInfo file(dialog.selectedFiles().first());

    // Advise worker to write data
    emit exportLaneletMap(file.absoluteFilePath(), this->ui->canvas->scene());
}
void MainWindow::onSaveTransformationDialogRequested()
{
    // Open save as dialog
    QFileDialog dialog(this);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("*.ini");
    dialog.selectNameFilter("*.ini");
    dialog.selectFile(QDir::homePath() + "/Transformation.ini");

    auto currentScenario = this->m_scenarioVisualization->scenario();
    if (currentScenario) {
        dialog.selectFile(QString::fromStdString("Transformation_")
                              + QString::fromStdString(currentScenario->GetName())
                              + QString::fromStdString(".ini"));

        // Assume standard file structure
        QString pathOnSimonsComputer = QString("%1/%2/%3/Transformation_%4.ini")
            .arg(QDir::homePath(),
                 "Datasets/Converted",
                 this->m_datasetParser->datasetName(),
                 currentScenario->GetName().c_str());
        QFileInfo fileOnSimonsComputer(pathOnSimonsComputer);
        if (fileOnSimonsComputer.exists())
            dialog.selectFile(fileOnSimonsComputer.absoluteFilePath());
    }

    // Abort if cancel is pressed
    if (!dialog.exec())
        return;

    // Get file information from dialog
    QFileInfo file(dialog.selectedFiles().first());

    QSettings transformationSettings(file.absoluteFilePath(), QSettings::IniFormat);
    transformationSettings.setValue("shift_x", this->ui->spinner_shift_x->value());
    transformationSettings.setValue("shift_y", this->ui->spinner_shift_y->value());
    transformationSettings.setValue("rotation", this->ui->spinner_rotation->value());
    transformationSettings.setValue("alpha_scenario", this->ui->slider_alpha_scenario->value());
    transformationSettings.setValue("alpha_background", this->ui->slider_alpha_background->value());
}
void MainWindow::onLoadTransformationDialogRequested()
{
    // Open save as dialog
    QFileDialog dialog(this);
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter("*.ini");
    dialog.selectNameFilter("*.ini");
    dialog.selectFile(QDir::homePath() + "/Transformation.ini");

    dialog.selectFile(getDefaultTransformationFilePath());

    // Abort if cancel is pressed
    if (!dialog.exec())
        return;

    // Get file information from dialog
    QFileInfo file(dialog.selectedFiles().first());

    this->loadTransformationInformationFromFile(file);
}
QString MainWindow::getDefaultTransformationFilePath() const
{
    QString path = "";
    auto currentScenario = m_scenarioVisualization->scenario();
    if (currentScenario) {
        path = (QDir::homePath() + QString::fromStdString("/Transformation_")
            + QString::fromStdString(currentScenario->GetName())
            + QString::fromStdString(".ini"));

        // Assume standard file structure
        QString pathOnSimonsComputer = QString("%1/%2/%3/Transformation_%4.ini")
            .arg(QDir::homePath(),
                 "Datasets/Converted",
                 m_datasetParser->datasetName(),
                 currentScenario->GetName().c_str());
        QFileInfo fileOnSimonsComputer(pathOnSimonsComputer);
        if (fileOnSimonsComputer.exists())
            path = (fileOnSimonsComputer.absoluteFilePath());
    }
    return path;
}
void MainWindow::loadTransformationInformationFromFile(const QFileInfo &file)
{
    QSettings transformationSettings(file.absoluteFilePath(), QSettings::IniFormat);
    ui->spinner_shift_x->setValue(transformationSettings.value("shift_x", 0.0).toDouble());
    ui->spinner_shift_y->setValue(transformationSettings.value("shift_y", 0.0).toDouble());
    ui->slider_alpha_scenario->setValue(
        transformationSettings.value("alpha_scenario", 100).toInt());
    ui->slider_alpha_background->setValue(
        transformationSettings.value("alpha_background", 100).toInt());
    ui->spinner_rotation->setValue(transformationSettings.value("rotation", 0.0).toDouble());
}
void MainWindow::onToolSelectionChanged(int id)
{
    this->m_graphicsViewClickHandler->setEditorMode(static_cast<EditorMode>(id));
}

void MainWindow::onDatasetLoaded()
{
    // Dataset is loaded close the progress dialog
    this->m_progressDialog->close();

    // Get scenarios and push to selection list
    QStringList listScenarioNames = this->m_scenarioListModel->stringList();
    if (listScenarioNames.first() == "No Scenarios loaded") {
        listScenarioNames.clear();
    }
    for (const auto &scenario : this->m_datasetParser->loadedScenarios()) {
        this->m_scenarios.push_back(scenario);
        listScenarioNames.push_back(QString::fromStdString(scenario->GetName()));
    }
    this->m_scenarioListModel->setStringList(listScenarioNames);

    // Trigger selection so that first scenario is loaded
    this->ui->list_scenario->setCurrentIndex(this->m_scenarioListModel->index(0, 0));

    this->ui->action_save_transformation->setEnabled(true);
    this->ui->action_load_transformation->setEnabled(true);
    this->ui->scenario_widget->setEnabled(true);
    this->ui->lbl_scenario->setEnabled(true);
    this->ui->list_scenario->setEnabled(true);
    this->ui->btn_play_pause->setEnabled(true);
    this->ui->btn_skip_backward->setEnabled(true);
    this->ui->btn_skip_forward->setEnabled(true);

    this->ui->action_save_scenario->setEnabled(true);
}
void MainWindow::onLaneletMapLoaded()
{
    // Lanelet map is loaded close the progress dialog
    this->m_progressDialog->close();
    this->ui->action_export_as_svg->setEnabled(true);
    this->ui->action_save_lanelet_map->setEnabled(true);
    // TODO Add back in
}
void MainWindow::onProgressDuringLoading(int current, int max)
{
    this->m_progressDialog->setMaximum(max);
    this->m_progressDialog->setValue(current);
}

void MainWindow::onErrorDuringLoading(const QString &message)
{
    // Error occurred
    this->m_progressDialog->close();

    ErrorDialog messageBox(this, "<html><b>Process aborted with message:</b><br>" + message + "</html>");
    messageBox.exec();
}
void MainWindow::onSelectedScenarioChanged(const QItemSelection &selection)
{
    // Get selected scenario
    int item = selection.indexes().first().row();
    auto scenario = this->m_scenarios.at(item);

    // Extract meta information
    int numberOfFrames = static_cast<int>(scenario->GetNumberOfFrames());

    this->m_scenarioVisualization->setScenario(scenario);

    // Update interface
    this->ui->lbl_start_time->setText("0");
    this->ui->lbl_stop_time->setText(QString::number(numberOfFrames));

    this->ui->slider_time->setRange(0, numberOfFrames);
    this->ui->slider_alpha_background->setValue(100);
    this->ui->slider_alpha_scenario->setValue(100);
    this->ui->spinner_shift_x->setValue(0);
    this->ui->spinner_shift_y->setValue(0);
    this->ui->spinner_rotation->setValue(0);

    this->m_scenarioVisualization->setScenarioShiftX(0);
    this->m_scenarioVisualization->setScenarioShiftY(0);
    this->m_scenarioVisualization->setScenarioRotation(0);
    this->m_scenarioVisualization->setScenarioTransparency(1);
    this->m_scenarioVisualization->setFrame(0);

    this->ui->control_bar->setEnabled(true);
    this->ui->sidebar->setEnabled(true);

    // Update dialogs
    this->updateInformationForSaveScenarioDialog();

    // Load transformation of stored
    QFileInfo file(getDefaultTransformationFilePath());
    this->loadTransformationInformationFromFile(file);
}
void MainWindow::onScenarioStored()
{
    // Dataset is loaded close the progress dialog
    this->m_progressDialog->close();
}
void MainWindow::onScenarioLoaded()
{
    // Dataset is loaded close the progress dialog
    this->m_progressDialog->close();

    // Get scenarios and push to selection list
    QStringList listScenarioNames = this->m_scenarioListModel->stringList();
    if (listScenarioNames.first() == "No Scenarios loaded") {
        listScenarioNames.clear();
    }
    auto scenario = this->m_scenarioHandler->loadedScenario();
    listScenarioNames.push_back(QString::fromStdString(scenario->GetName()));
    this->m_scenarios.push_back(scenario);
    this->m_scenarioListModel->setStringList(listScenarioNames);

    // Trigger selection so that first scenario is loaded
    this->ui->list_scenario->setCurrentIndex(this->m_scenarioListModel->index(0, 0));

    this->ui->action_save_transformation->setEnabled(true);
    this->ui->action_load_transformation->setEnabled(true);
    this->ui->scenario_widget->setEnabled(true);
    this->ui->lbl_scenario->setEnabled(true);
    this->ui->list_scenario->setEnabled(true);
    this->ui->btn_play_pause->setEnabled(true);
    this->ui->btn_skip_backward->setEnabled(true);
    this->ui->btn_skip_forward->setEnabled(true);

    this->ui->action_save_scenario->setEnabled(true);
}
void MainWindow::onSkipFramesForward()
{
    this->ui->slider_time->setValue(this->ui->slider_time->value() + 100);
}
void MainWindow::onSkipFramesBackward()
{
    this->ui->slider_time->setValue(this->ui->slider_time->value() - 100);
}
void MainWindow::onPlaybackTimer()
{
    this->ui->slider_time->setValue(this->ui->slider_time->value() + 1);
}
void MainWindow::onStartStopPlayback()
{
    if (this->ui->btn_play_pause->isChecked()) {
        // Pause
        this->ui->btn_play_pause->setIcon(QIcon(":resources/icons/play.svg"));
        this->m_playbackTimer->stop();
    }
    else {
        // Play
        this->ui->btn_play_pause->setIcon(QIcon(":resources/icons/pause.svg"));
        auto timeBetweenStates =
            this->m_scenarioVisualization->scenario()->GetObjects().front()->GetTimeBetweenStates() * 1e-6;
        this->m_playbackTimer->start(timeBetweenStates / this->ui->spinner_playback->value());
    }
}

