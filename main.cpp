#include "mainwindow.h"

#include <QApplication>
#include <QPalette>

void darkMode(QApplication& rApp)
{
    QPalette dark_pal;
    dark_pal.setColor(QPalette::Window, QColor(53,53,53));
    dark_pal.setColor(QPalette::WindowText, QColorConstants::White);
    dark_pal.setColor(QPalette::Base, QColor(25, 25, 25));
    dark_pal.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    dark_pal.setColor(QPalette::ToolTipBase, QColorConstants::Black);
    dark_pal.setColor(QPalette::ToolTipText, QColorConstants::White);
    dark_pal.setColor(QPalette::Text, QColorConstants::White);
    dark_pal.setColor(QPalette::Button, QColor(17, 149, 79));
    dark_pal.setColor(QPalette::ButtonText, QColorConstants::White);
    dark_pal.setColor(QPalette::BrightText, QColorConstants::Red);
    dark_pal.setColor(QPalette::Link, QColor(42, 130, 218));
    dark_pal.setColor(QPalette::Highlight, QColor(42, 130, 218));
    dark_pal.setColor(QPalette::HighlightedText, QColorConstants::Black);

    rApp.setPalette(dark_pal);
}

int main(int argc, char *argv[])
{
    QApplication music_app(argc, argv);
    darkMode(music_app);
    MainWindow w;
    w.show();
    return music_app.exec();
}
