#include "AboutDialog.h"
#include "ui_aboutdialog.h"

#include <QPushButton>

AboutDialog::AboutDialog(QWidget *parent, const QString &text)
    : QDialog(parent), ui(new Ui::AboutDialog)
{
    // Load .ui file
    this->ui->setupUi(this);

    this->ui->lbl_description->setText(text);

    // Remove icons from the buttons
    for (auto &item : this->ui->btn_dialog->buttons()) {
        item->setIcon(QIcon());
    }

    // Setup dialog button bar function
    connect(this->ui->btn_dialog, &QDialogButtonBox::rejected, this, &AboutDialog::reject);
    connect(this->ui->btn_dialog, &QDialogButtonBox::accepted, this, &AboutDialog::accept);
}