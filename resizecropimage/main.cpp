#include "mainwindow.h"

#include <QApplication>
#include "resizeimagedialog.h"

#include <QPushButton>
#include <QDebug>
#include <QtCore>
#include <QHBoxLayout>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QWidget wdg(&w);
    QPushButton tool("Resize&Crop");
    tool.setMinimumSize(20, 10);
    QVBoxLayout vLayout;
    vLayout.addWidget(&tool);
    wdg.setLayout(&vLayout);
    w.setCentralWidget(&wdg);
    w.setMaximumSize(400, 400);
    QObject::connect(&tool, &QPushButton::clicked, &w, [=]{
        qDebug() << "hello click!";
        ResizeImageDialog *dialog = new ResizeImageDialog;
        if (dialog->exec()){}
        delete dialog;
        dialog = nullptr;
    });
    w.show();
    return a.exec();
}
