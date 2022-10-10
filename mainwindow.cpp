/****************************************************************************
 * Music Editor: Let's see if this works
 * TODO:
 * Add a logger
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
    ui->saveButton->setStyleSheet("background-color : rgb(87,87,87); border: 1px solid grey; border-radius: 4px");
    ui->saveAsButton->setStyleSheet("background-color : rgb(87,87,87); border: 1px solid grey; border-radius: 4px");
    ui->manualButton->setStyleSheet("background-color : rgb(87,87,87); border: 1px solid grey; border-radius: 4px");
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
        // TODO: Make this pass the formatted names, not the ones from the dir, they are wrong
        pManualEditor->setupList(_filesInDir);
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

void MainWindow::readFileData(const QStringList &rFileList)
{
    // Allocate the vector of byte arrays
    _songData.clear();
    _songData.resize(rFileList.size());
    // TODO: Under no circumstances can I read the whole directory into memory at once, it's a recipe for disaster
    // You should read then write only after the program is certain you want to be writing the bytes
    for(int i = 0; i < rFileList.size();  ++i)
    {
        // TODO: Clean up path generation, has to be a better way than using + operators
        QString songName = _songDir.absolutePath() + "/" + rFileList[i] + "." + _fileExt;
        QFile songData(songName);
        songData.open(QIODevice::ReadOnly);
        //std::cout << "Reading in " << rFileList[i].toStdString() << std::endl;
        std::cout << "Reading in " << songName.toStdString() << std::endl;
        while(!songData.atEnd())
        {
            _songData[i].append(songData.read(sizeof(char)));
        }
        std::cout << "Read in " << _songData[i].size() << " bytes for: " << rFileList[i].toStdString() << std::endl;
    }
}

/*****************************************
 * Writes files
 * Loops thru all the files, writes the old data to the new file names
 * Returns the number of bytes written, zero means an error
*****************************************/
unsigned long MainWindow::writeNewFiles()
{
    // TODO: Error handle the directory?
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
            // TODO: Clean up path generation, has to be a better way than using + operators
            QString songName = saveDir + "/" + ui->formattedNames->item(i)->text() + "." + _fileExt;
            std::cout << songName.toStdString() << std::endl;
            QFile newSong(songName);
            // Song Failed to write, print out the failure, don't stop the rest of it
            if (!newSong.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                std::cerr << "Couldn't write new file: " << songName.toStdString() << std::endl;
            }
            qint64 numBytesWritten = newSong.write(_songData[i]);
            std::cout << "Wrote " << numBytesWritten << " bytes to " << songName.toStdString();
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
    // Now that we have read in all the files, load their data into memory
    readFileData(_filesInDir);
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
