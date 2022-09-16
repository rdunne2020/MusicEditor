#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QRegularExpression>

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

private slots:
    void on_actionOpen_Folder_triggered();
    void fill_fileList(QStringList& rFilesForList);

    void on_actionRefresh_triggered();

signals:
    void send_songs(QStringList& rSelectedSongs);

private:
    Ui::MainWindow *ui;
    QStringList _filesInDir;
    QString _fileExt = "";
};
#endif // MAINWINDOW_H
