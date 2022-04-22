#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QString>

// Reference to the class defined by the .ui file
namespace Ui
{
class AboutDialog;
}

/**
 * Dialog that will show the current software version and a copyright notice.
 */
class AboutDialog: public QDialog
{
Q_OBJECT
private:
    Ui::AboutDialog *ui; ///< Instance of the .ui file class.

public:
    /**
     * Loads .ui file as set connections.
     * @param parent Parent for qt memory management.
     */
    explicit AboutDialog(QWidget *parent = nullptr, const QString &text = "");
};

#endif // ABOUTDIALOG_H
