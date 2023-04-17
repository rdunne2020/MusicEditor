/****************************************************************************
 * Music Editor: Let's see if this works
 * TODO:
 * Add a logger
 * Use dialogs to show errors
****************************************************************************/

#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "editor.h"
#include <iostream>
#include <QDir>
#include <QFile>
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->saveButton->setStyleSheet("background-color: rgb(87,87,87); color: white; border: 1px solid grey; border-radius: 4px");
    ui->saveAsButton->setStyleSheet("background-color: rgb(87,87,87); color: white; border: 1px solid grey; border-radius: 4px");
    ui->manualButton->setStyleSheet("background-color: rgb(87,87,87); color: white; border: 1px solid grey; border-radius: 4px");
    connect(ui->saveButton, &QPushButton::pressed, this, &MainWindow::writeNewFiles);
    connect(ui->saveAsButton, &QPushButton::pressed, this, &MainWindow::writeNewFiles);
    connect(ui->manualButton, &QPushButton::pressed, this, &MainWindow::launchManualEditor);
    connect(this, &MainWindow::send_songs, this, &MainWindow::fill_fileList);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::launchManualEditor()
{
    if(_filesInDir.count() < 1)
    {
        std::cout << "Please open a directory with songs in it first" << std::endl;
    }
    else
    {
        std::unique_ptr<Editor> pManualEditor = std::make_unique<Editor>();
        QStringList formattedFileNames;
        for(int i = 0; i < ui->formattedNames->count(); ++i)
        {
            formattedFileNames.append(ui->formattedNames->item(i)->text());
        }
        pManualEditor->setupList(formattedFileNames);
        connect(pManualEditor.get(), &Editor::sendDataToMain, this, &MainWindow::receiveManuallyChangedName);
        pManualEditor->exec();
        disconnect(pManualEditor.get(), &Editor::sendDataToMain, this, &MainWindow::receiveManuallyChangedName);
        pManualEditor.release();
    }
}

void MainWindow::receiveManuallyChangedName(QString newName, int rowChanged)
{
    ui->formattedNames->item(rowChanged)->setText(newName);
}

QByteArray MainWindow::readFileData(const QString &rFileName)
{
    // TODO: Clean up path generation, has to be a better way than using + operators
    QString songName = _songDir.absolutePath() + "/" + rFileName + "." + _fileExt;
    QFile songData(songName);
    QByteArray binData;
    songData.open(QIODevice::ReadOnly);
    while(!songData.atEnd())
    {
        binData.append(songData.read(sizeof(char)));
    }
    std::cout << "Read in " << binData.size() << " bytes for: " << rFileName.toStdString() << std::endl;
    return binData;
}

/*****************************************
 * Writes files
 * Loops thru all the files, writes the old data to the new file names
 * Returns the number of bytes written, zero means an error
*****************************************/
unsigned long MainWindow::writeNewFiles()
{
    if(_filesInDir.size() < 1 || ui->formattedNames->count() < 1)
    {
        std::cout << "Open Directory With Music Files in it First" << std::endl;
        return 0;
    }
    else
    {
        QString saveDir = _songDir.absolutePath();
        QString buttonText = static_cast<QPushButton*>(QObject::sender())->text();
        bool pressedSaveAs = (buttonText == "Save As");

        // If save as was activated, open a dialog to select a new directory
        if(pressedSaveAs)
        {
            QString new_directory_selected = QFileDialog::getExistingDirectory(this, "Save New Files As", "./", QFileDialog::Option::ShowDirsOnly);
            if(new_directory_selected.isEmpty())
            {
                std::cout << "Select a directory to save your files to" << std::endl;
                return 0;
            }
            saveDir = new_directory_selected;
        }

        std::cout << "Saving new songs to : " << saveDir.toStdString() << std::endl;
        unsigned long totalNumMbWritten = 0;
        for(int i = 0; i < ui->formattedNames->count(); ++i)
        {
            // Get the filename to read the data into memory and write it
            QString unformattedName = ui->unformattedNames->item(i)->text();
            QByteArray currentBinaryData = readFileData(unformattedName);
            // TODO: Clean up path generation, has to be a better way than using + operators
            QString songName = saveDir + "/" + ui->formattedNames->item(i)->text() + "." + _fileExt;
            QFile newSong(songName);
            // Song Failed to write, print out the failure, don't stop the rest of it
            if (!newSong.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                std::cerr << "Couldn't write new file: " << songName.toStdString() << std::endl;
            }
            //qint64 numBytesWritten = newSong.write(_songData[i]);
            qint64 numBytesWritten = newSong.write(currentBinaryData);
            std::cout << "Wrote " << numBytesWritten << " bytes to " << songName.toStdString() << "\n";
            totalNumMbWritten += (static_cast<double>(numBytesWritten) / 1000000.0);
        }
        return totalNumMbWritten;
    }
}

void MainWindow::editTitle(QString &rSongName, int trackNum, bool continuous)
{
    // Capitalize the start of every appropriate word, else lowercase it
    QStringList wordsInTitle = rSongName.split(" ", Qt::SplitBehaviorFlags::SkipEmptyParts);
    // Remove the first element since it's a bad track#
    wordsInTitle.removeAt(0);

    bool alreadyHasArrow = false;

    // Used for detecting when it's the first word in a song title and not lowercasing it if need be
    int word_count = 0;
    for(auto& word : wordsInTitle)
    {
        // TODO: Clean this up
        QRegularExpression badUppercase("(^a$|^an$|^and$|^but$|^for$|^to$|^at$|^by$)", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch toLowercase = badUppercase.match(word);
        if(toLowercase.hasMatch())
        {
            // Don't lowercase it if it's the first word in the title
            if(word_count > 0)
            {
                word.replace(0,1, word[0].toLower());
            }
        }
        else
        {
            // Replace the first char in Word with an upper case variant
            word.replace(0,1, word[0].toUpper());
        }
        ++word_count;
        // If the song title already has an arrow,
        if(word.contains(">"))
        {
            alreadyHasArrow = true;
        }
    }
    // Increase the trackNum by one to start from 1
    QString wc = QString::fromStdString(std::to_string(trackNum+1));
    wordsInTitle.prepend(wc);

    if(continuous && !alreadyHasArrow)
    {
        wordsInTitle.append(">");
    }

    rSongName = wordsInTitle.join(" ");

    // Make sure a > are in the continued songs
}

void MainWindow::parseFiles(QStringList &rFileList)
{
    QStringList formattedNames;
    // Set the song continuation to off for all the songs by default
    for(int i = 0; i < ui->unformattedNames->count(); ++i)
    {
        QListWidgetItem *pSongName = ui->unformattedNames->item(i);
        if(pSongName)
        {
            QString formattedName = pSongName->text();
            editTitle(formattedName, i, pSongName->checkState());
            formattedNames.push_back(formattedName);
        }
    }
    ui->formattedNames->addItems(formattedNames);
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    //QStringList directory_selected;
    QDir directory_selected = QFileDialog::getExistingDirectory(this);
    std::cout << "Path: " << directory_selected.absolutePath().toStdString() << std::endl;
    QStringList audio_file_filter;
    audio_file_filter << "*.mp3" << "*.alac" << "*.flac" << "*.wav" << "*.m4a";
    QStringList files_in_dir = directory_selected.entryList(audio_file_filter, QDir::Files);
    _songDir = directory_selected;
    for(auto& rFile : files_in_dir)
    {
        QStringList tmpSplit = rFile.split(".");
        _fileExt = tmpSplit.at(1);
        rFile = tmpSplit.at(0);
        std::cout << rFile.toStdString() << std::endl;
    }
    _filesInDir = files_in_dir;
    emit(send_songs(_filesInDir));
}

void MainWindow::fill_fileList(QStringList& rFilesForList)
{
    QStringList formattedNames;
    // Clear the current lists
    ui->unformattedNames->clear();
    ui->formattedNames->clear();
    ui->unformattedNames->addItems(rFilesForList);
    QRegularExpression continuous(".*>", QRegularExpression::DotMatchesEverythingOption);
    // Set the song continuation to off for all the songs by default
    for(int i = 0; i < ui->unformattedNames->count(); ++i)
    {
        QListWidgetItem *pSongName = ui->unformattedNames->item(i);
        if(pSongName)
        {
            QRegularExpressionMatch isContinuous = continuous.match(pSongName->text());
            // We only want the user to be able to set continuous for files without arrows
            if(! isContinuous.hasMatch())
            {
                pSongName->setCheckState(Qt::Unchecked);
            }

            QString formattedName = pSongName->text();
            editTitle(formattedName, i, pSongName->checkState());
            formattedNames.push_back(formattedName);
        }
    }
    ui->formattedNames->addItems(formattedNames);
}

void MainWindow::on_actionRefresh_triggered()
{
    if(ui->unformattedNames->count() > 0 && ui->formattedNames->count() > 0)
    {
        ui->formattedNames->clear();
        parseFiles(_filesInDir);
    }
}
