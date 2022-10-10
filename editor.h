#ifndef EDITOR_H
#define EDITOR_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class Editor;
}

class Editor : public QDialog
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    void setupList(const QStringList& formattedNames);
    ~Editor();

private slots:
    void editSong(QListWidgetItem* pUpdatedSong);
    void propagateChange(int rowChanged);

signals:
    void sendDataToMain(QString newSongName, int songNum);

private:
    Ui::Editor *ui;
};

#endif // EDITOR_H
