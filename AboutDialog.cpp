#include "AboutDialog.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::AboutDialog)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_btnGetSource_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/Experiment5X/BIG-File-Extractor"));
}

void AboutDialog::on_btnOK_clicked()
{
    close();
}
