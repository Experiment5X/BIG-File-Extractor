#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);

    ui->treeWidget->header()->resizeSection(0, 350);

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    filePath = QFileDialog::getOpenFileName(this, "Choose a BIG file", "", "*.big");
    if (filePath == "")
        return;

    // open the file
    try
    {
        FileIO bigFile(filePath.toStdString());
        BIGHeader header;

        if (bigFile.ReadWord() != 'EB')
        {
            QMessageBox::warning(this, "Error", "The file selected is not a supported BIG file.\n");
            bigFile.Close();
            return;
        }

        // parse the header
        header.magic =              'EB';
        header.version =            bigFile.ReadWord();
        header.fileCount =          bigFile.ReadDword();
        header.unknown0 =           bigFile.ReadDword();
        header.fileListingAddr =    bigFile.ReadDword();
        header.unknown1 =           bigFile.ReadDword();
        header.fileNameLen =        bigFile.ReadByte();
        header.unknown2 =           bigFile.ReadByte();
        header.unknown3 =           bigFile.ReadWord();
        header.unknown4 =           bigFile.ReadDword();
        header.fileLength =         bigFile.ReadDword();

        // read all of the file information
        bigFile.SetPosition(0x30);
        for (int i = 0; i < header.fileCount; i++)
        {
            BIGFile file;
            file.address =          bigFile.ReadDword() * 0x10;
            file.compressedSize =   bigFile.ReadDword();
            file.decompressedSize = bigFile.ReadDword();
            file.unknown =          bigFile.ReadDword();
            file.compressed =       file.compressedSize != 0;

            files.push_back(file);

            QApplication::processEvents();
            ui->statusBar->showMessage("Reading file info table...", 3000);
        }

        // read all the file names
        bigFile.SetPosition(header.fileListingAddr);

        // there's a null word at the beginning of each name, we'll just skip over that
        bigFile.ReadWord();
        for (int i = 0; i < header.fileCount; i++)
        {
            files[i].name = QString::fromStdString(bigFile.ReadString(header.fileNameLen));

            QApplication::processEvents();
            ui->statusBar->showMessage("Reading file name table...", 3000);
        }

        // add all of the files to the widget
        foreach (BIGFile file, files)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
            item->setText(0, file.name);
            item->setText(1, file.compressedSize != 0 ? "Yes" : "No");
            item->setText(2, "0x" + QString::number(file.address, 16).toUpper());
            item->setText(3, QString::fromStdString(ByteSizeToString(file.compressedSize)));
            item->setText(4, QString::fromStdString(ByteSizeToString(file.decompressedSize)));
        }

        bigFile.Close();

        ui->treeWidget->setEnabled(true);
        ui->statusBar->showMessage("Parsed successfully", 3000);

        ui->actionOpen->setEnabled(false);
        ui->actionClose->setEnabled(true);
    }
    catch (std::string error)
    {
        QMessageBox::critical(this, "Error", "An error occurred while parsing the BIG file.\n\n" + QString::fromStdString(error));
        ui->statusBar->showMessage("Error parsing", 3000);
    }
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    if (ui->treeWidget->selectedItems().count() < 1)
        return;

    QPoint globalPos = ui->treeWidget->mapToGlobal(pos);
    QMenu contextMenu;

    contextMenu.addAction("Extract File");

    QAction *selectedItem = contextMenu.exec(globalPos);

    if (selectedItem == NULL)
        return;
    else if (selectedItem->text() == "Extract File")
    {
        try
        {
                BIGFile currentFile = files.at(ui->treeWidget->currentIndex().row());

                QString savePath = QFileDialog::getSaveFileName(this, "Choose a place to save the file",
                                             QDesktopServices::storageLocation(QDesktopServices::DesktopLocation).replace("\\", "/") + "/" + currentFile.name);

                if (savePath == "")
                    return;

                // create a file to write the bytes to
                FileIO outFile(savePath.toStdString(), true);

                // open the file again and seek to the internal file's address
                FileIO bigFile(filePath.toStdString());
                bigFile.SetPosition(currentFile.address);

                BYTE *buffer = new BYTE[0x10000];
                long bytesRemaining = (currentFile.compressedSize == 0) ? currentFile.decompressedSize : currentFile.compressedSize;

                // extract the bytes to the out file in 0x10000 byte chunks
                int i = 1;
                while (bytesRemaining >= 0x10000)
                {
                    bigFile.ReadBytes(buffer, 0x10000);
                    outFile.WriteBytes(buffer, 0x10000);

                    bytesRemaining -= 0x10000;

                    QApplication::processEvents();
                    ui->statusBar->showMessage(QString::number(bytesRemaining) + " bytes left to extract", 3000);
                }

                // if there are some more bytes left, then clean those up
                if (bytesRemaining > 0)
                {
                    bigFile.ReadBytes(buffer, bytesRemaining);
                    outFile.WriteBytes(buffer, bytesRemaining);
                }

                ui->statusBar->showMessage("Extracted successfully", 3000);

                // cleanup
                outFile.Close();
                bigFile.Close();
                delete[] buffer;

                // if the file is compressed then we need to decompress it
                if (!currentFile.compressed)
                    return;

                QFileInfo fileInfo(savePath);

                QStringList args;
                args.push_back(QDir::currentPath() + "\\unzip.bms");
                args.push_back(savePath);
                args.push_back(fileInfo.path());

                QProcess quickBMS(this);
                quickBMS.start(QDir::currentPath() + "\\quickbms.exe", args);

                ui->statusBar->showMessage("Decompressing...", 3000);
                if (quickBMS.waitForFinished())
                    ui->statusBar->showMessage("Decompressed successfully", 3000);

                // delete the original
                QFile::remove(savePath);

                // rename the unpacked one to the original
                QFile::rename(fileInfo.path() + "/" + fileInfo.baseName() + "_unpacked" + "." + fileInfo.suffix(), savePath);
        }
        catch (std::string error)
        {
            QMessageBox::critical(this, "Error", "An error occurred while extracting the file.\n\n" + QString::fromStdString(error));
            ui->statusBar->showMessage("Error extracting", 3000);
        }
    }
}

void MainWindow::on_actionClose_triggered()
{
    ui->treeWidget->clear();
    ui->treeWidget->setEnabled(false);
    ui->actionOpen->setEnabled(true);
    ui->actionClose->setEnabled(false);
    files.clear();
    filePath = "";
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dialog;
    dialog.exec();
}
