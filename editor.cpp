#include "editor.h"
#include "ui_editor.h"

Editor::Editor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);
    connect(this->ui->songNames, &QListWidget::itemDoubleClicked, this, &Editor::editSong);
    connect(this->ui->songNames, &QListWidget::currentRowChanged, this, &Editor::propagateChange);
}

void Editor::setupList(const QStringList &formattedNames)
{
    for(auto fn : formattedNames)
    {
        ui->songNames->addItem(fn);
    }
}

void Editor::editSong(QListWidgetItem* pUpdatedSong)
{
    pUpdatedSong->setFlags(pUpdatedSong->flags() | Qt::ItemIsEditable);
}

void Editor::propagateChange(int rowChanged)
{
    QString newSongTitle = ui->songNames->item(rowChanged)->text();
    emit(sendDataToMain(newSongTitle, rowChanged));
}

Editor::~Editor()
{
    delete ui;
}
