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
    void getChangedRow(int rowChanged);
    void propagateChange();

signals:
    void sendDataToMain(QString newSongName, int songNum);

private:
    Ui::Editor *ui;
    std::vector<int> _changedRows;
};

#endif // EDITOR_H
