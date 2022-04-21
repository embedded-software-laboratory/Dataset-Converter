/**
 * @file main.cpp
 * @authors Simon Schaefer
 * @date 05.05.2021
 */
#define APPLICATION_NAME "CpmDatasetConverter"
#define ORGANIZATION_NAME "Lehrstuhl Informatik 11 - Embedded Software"
#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define REVISION 1

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTimer>
#include <QMessageBox>

#include "MainWindow.h"
#include "visualisation/graphics_items/ColorDefinition.h"

#ifdef Q_OS_DARWIN
#include "platform/OSXHideTitleBar.h"
#endif

void load_application_information()
{
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(QString(
        QString::number(MAJOR_VERSION) + "." + QString::number(MINOR_VERSION) + "." + QString::number(REVISION)));

    //QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
}

void load_style()
{
    QApplication::setWindowIcon(QIcon(":/resources/logo.png"));

    QApplication::setStyle("Fusion");

    //QColor accent(255, 81, 47); // Red
    QColor accent = ColorDefinitions::ORANGE; // Orange

    QPalette darkPalette = QApplication::palette();
    darkPalette.setColor(QPalette::Window, Qt::black);
    darkPalette.setColor(QPalette::WindowText, Qt::white);

    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(75, 75, 75));

    darkPalette.setColor(QPalette::ToolTipBase, QColor(251, 211, 23));
    darkPalette.setColor(QPalette::ToolTipText, Qt::black);

    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(180, 180, 180));
    darkPalette.setColor(QPalette::PlaceholderText, QColor(200, 200, 200));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(180, 180, 180));

    darkPalette.setColor(QPalette::Button, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::Disabled, QPalette::Button, QColor(10, 10, 10));

    darkPalette.setColor(QPalette::Dark, QColor(10, 10, 10));
    darkPalette.setColor(QPalette::Mid, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::Midlight, QColor(50, 50, 50));
    darkPalette.setColor(QPalette::Light, QColor(70, 70, 70));

    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(180, 180, 180));

    darkPalette.setColor(QPalette::Highlight, accent);
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);

    darkPalette.setColor(QPalette::BrightText, accent);
    darkPalette.setColor(QPalette::Link, accent);
    QApplication::setPalette(darkPalette);
}

void register_metadata()
{
    qRegisterMetaType<size_t>("size_t");
}

QString getStyleSheet()
{
    QFile file(":resources/style.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    return styleSheet;
}

int main(int argc, char *argv[])
{
    qInfo("Executing %s, version %i.%i.%i", APPLICATION_NAME, MAJOR_VERSION, MINOR_VERSION, REVISION);
    load_application_information();

    QApplication application(argc, argv);

    QCommandLineParser commandLineParser;
    commandLineParser.setApplicationDescription(
        "Will convert the file structure of known datasets to a common form that is supported by the CPM-Remote scenario generation.");
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();

    QCommandLineOption datasetNameOption(QStringList() << "d" << "dataset", "Name of the dataset", "<dataset>", "");
    QCommandLineOption
        datasetRootDirectoryOption(QStringList() << "i" << "input", "Dataset root path", "<dataset_root_path>", "");
    QCommandLineOption outputDirectoryOption(QStringList() << "o" << "output", "Output path", "<output_path>", "");
    commandLineParser.addOption(datasetNameOption);
    commandLineParser.addOption(datasetRootDirectoryOption);
    commandLineParser.addOption(outputDirectoryOption);

    commandLineParser.process(application);

    // Get values from the command line parser
    QString outputDirectoryPath = commandLineParser.value(outputDirectoryOption);
    QString datasetRootDirectoryPath = commandLineParser.value(datasetRootDirectoryOption);
    QString datasetName = commandLineParser.value(datasetNameOption);

    qInfo("[CpmSC] Register metadata.");
    register_metadata();

    qInfo("[CpmSC] Loading style.");
    load_style();

    qInfo("[CpmSC] Executing Gui.");
    auto *mainWindow = new MainWindow;
    mainWindow->setWindowTitle(QApplication::applicationName());
    mainWindow->setStyleSheet(getStyleSheet());
    mainWindow->show();

#ifdef Q_OS_DARWIN
    OsxTitleBarHandler::HideTitleBar(mainWindow->winId());
#endif
    return QCoreApplication::exec();
}