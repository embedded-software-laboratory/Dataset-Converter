#ifndef ERRORDIALOG_H
#define ERRORDIALOG_H

#include <QDialog>
#include <QString>

// Reference to the class defined by the .ui file
namespace Ui
{
class ErrorDialog;
}

/**
 * Dialog that will show an error message.
 */
class ErrorDialog: public QDialog
{
Q_OBJECT
private:
    Ui::ErrorDialog *ui; ///< Instance of the .ui file class.

public:
    /**
     * Loads .ui file as set connections.
     * @param parent Parent for qt memory management.
     */
    explicit ErrorDialog(QWidget *parent = nullptr, const QString &description = "", const QString &title = "");
};

#endif // ERRORDIALOG_H
