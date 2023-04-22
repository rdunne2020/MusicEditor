#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QRegularExpression>
#include "flaccontainer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void editTitle(QString& rSongName, int trackNum, bool continuous);
    void parseFiles(QStringList &rFileList);

    QByteArray readFileData(const QString& rFileName);

private slots:
    void on_actionOpen_Folder_triggered();
    void fill_fileList(QStringList& rFilesForList);

    void on_actionRefresh_triggered();

    unsigned long writeNewFiles();

    void launchManualEditor();

    void receiveManuallyChangedName(QString newName, int rowChanged);

signals:
    void send_songs(QStringList& rSelectedSongs);

private:
    Ui::MainWindow *ui;
    QDir _songDir;
    QStringList _filesInDir;
    QStringList _fullFilePath;
    QString _fileExt = "";
    // Vector that holds in depth info for all flac files read in
    // std::vector<FlacContainer> _listOfFlacFiles;
};
#endif // MAINWINDOW_H
