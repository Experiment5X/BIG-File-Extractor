#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();
    
private slots:
    void on_btnGetSource_clicked();

    void on_btnOK_clicked();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
