#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QProcess>
#include <QDebug>

#include <IO/FileIO.h>
#include <Stfs/StfsDefinitions.h>

#include "AboutDialog.h"

namespace Ui {
class MainWindow;
}

struct BIGHeader
{
    WORD magic;     // EB
    WORD version;   // not really sure
    DWORD fileCount;
    DWORD unknown0;
    DWORD fileListingAddr;
    DWORD unknown1;
    BYTE fileNameLen;
    BYTE unknown2;
    WORD unknown3;
    DWORD unknown4;
    DWORD fileLength;
};

struct BIGFile
{
    DWORD address;
    DWORD compressedSize;
    DWORD decompressedSize;
    DWORD unknown;
    QString name;
    bool compressed;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionOpen_triggered();
    void showContextMenu(const QPoint &pos);

    void on_actionClose_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    QList<BIGFile> files;
    QString filePath;
};

#endif // MAINWINDOW_H
