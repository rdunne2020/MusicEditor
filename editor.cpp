#include "editor.h"
#include "ui_editor.h"

Editor::Editor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);
    // Since the text is white as set in main.cpp, make the edit box black
    ui->songNames->setStyleSheet("QLineEdit{ background:black; };");
    connect(this->ui->songNames, &QListWidget::itemDoubleClicked, this, &Editor::editSong);
    connect(this->ui->songNames, &QListWidget::currentRowChanged, this, &Editor::getChangedRow);
    connect(this, &QDialog::accepted, this, &Editor::propagateChange);
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

void Editor::getChangedRow(int rowChanged)
{
    _changedRows.push_back(rowChanged);
}

void Editor::propagateChange()
{
    for(auto row : _changedRows)
    {
        QString newFileName = ui->songNames->item(row)->text();
        emit(sendDataToMain(newFileName, row));
    }
    _changedRows.clear();
}

Editor::~Editor()
{
    delete ui;
}
