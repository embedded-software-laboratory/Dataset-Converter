#include "ErrorDialog.h"
#include "ui_errordialog.h"

#include <QPushButton>

ErrorDialog::ErrorDialog(QWidget *parent, const QString &description, const QString &title)
    : QDialog(parent), ui(new Ui::ErrorDialog)
{
    // Load .ui file
    this->ui->setupUi(this);

    // Update text
    this->ui->lbl_title->setText(title);
    this->ui->lbl_description->setText(description);

    // Remove icons from the buttons
    for (auto &item : this->ui->btn_dialog->buttons()) {
        item->setIcon(QIcon());
    }

    // Setup dialog button bar function
    connect(this->ui->btn_dialog, &QDialogButtonBox::rejected, this, &ErrorDialog::reject);
    connect(this->ui->btn_dialog, &QDialogButtonBox::accepted, this, &ErrorDialog::accept);
}