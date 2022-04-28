#include "resizeprogressbar.h"
#include "resizecropschema.h"
#include "utils.h"
#include "resizeprogressthread.h"

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPainter>
#include <QApplication>
#include <QWidget>
#include <iostream>
#include <QStackedWidget>
#include <QDir>
#include <QElapsedTimer>
#include <QSignalMapper>
#include <QDesktopServices>
#include <QUrl>
#include <QListWidget>
#include <QTime>
#include <QFuture>
#include <QFutureInterface>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QImage>
#include <QImageReader>
#include <windows.h>

ResizeProgressBarDialog::ResizeProgressBarDialog(
        QWidget *parent,
        ResizeCropArgs *resizecropArgs
        )
    : QDialog(parent)
{
    this->selectPathSignalMapper = new QSignalMapper(this);
    // run
    this->resizecropArgs = resizecropArgs;
    this->clipInfoListWidget = new QListWidget;
    this->clipInfoListWidget->setVisible(false);
    this->sourcePath = resizecropArgs->sourcePath;
    this->targetPath = resizecropArgs->targetPath;
    //this->sourceFileInfo = new QList<QFileInfo>;
    QSet<QByteArray> validFormat = QImageReader::supportedImageFormats().toSet();
    recursiveAllFiles(sourcePath, this->sourceFileInfo, &validFormat);
    this->sourceTotal = sourceFileInfo.count();
    this->resizeCropProgressLabel = new QLabel(tr("Image Converting Progress(0/1)"));
    this->resizeCropProgressLabel->setText(QString("Image Converting Progress(0/%1)").arg(sourceTotal));
    this->resizeCropProgressBar = new QProgressBar;
    this->resizeCropProgressBar->setRange(0, this->sourceTotal);
    this->currentresizeCropProgressLabel = new QLabel(tr("Current Picture: "));
    this->currentresizeCropProgressBar = new QProgressBar;
    this->extendButton = new QPushButton(tr("Extend"));
    connect(extendButton, &QPushButton::clicked, this, &ResizeProgressBarDialog::onExtendButtonClicked);

    this->cancelRunButton = new QPushButton(tr("Cancel"));
    this->determineRunButton = new QPushButton(tr("Ok"));
    this->determineRunButton->setDisabled(true);
    connect(determineRunButton, &QPushButton::clicked, this, &ResizeProgressBarDialog::onRunDetermineButtonClicked);

    QHBoxLayout *currentresizeCropProgressLayout = new QHBoxLayout;
    currentresizeCropProgressLayout->addWidget(currentresizeCropProgressBar);
    currentresizeCropProgressLayout->addWidget(extendButton);

    this->resizeCropRunLayout = new QVBoxLayout;
    this->resizeCropRunLayout->addWidget(resizeCropProgressLabel);
    this->resizeCropRunLayout->addWidget(resizeCropProgressBar);
    this->resizeCropRunLayout->addWidget(currentresizeCropProgressLabel);
    this-> resizeCropRunLayout->addLayout(currentresizeCropProgressLayout);
    this->resizeCropRunLayout->addWidget(this->clipInfoListWidget);
    this-> resizeCropRunLayout->addWidget(cancelRunButton);
    this->resizeCropRunLayout->addWidget(determineRunButton);

    this->state = ResizeProgressBarDialog::RUNNING;

    // finish
    QLabel *resizeCropFinishLabel = new QLabel(tr("Image Converting Finished!"));
    QLineEdit *sourcePathEdit = new QLineEdit;
    sourcePathEdit->setText(sourcePath);
    sourcePathEdit->setDisabled(true);
    QLabel *sourceLabel = new QLabel(tr("Source Path: "));
    QPushButton *sourcePathButton = new QPushButton(tr("Go To the directory"));
    sourcePathButton->setStyleSheet("font-weight: bold; color: blue; text-decoration: underline");
    connect(sourcePathButton, SIGNAL(clicked()), selectPathSignalMapper, SLOT(map()));
    selectPathSignalMapper->setMapping(sourcePathButton, "Source");

    QLineEdit *targetPathEdit = new QLineEdit;
    targetPathEdit->setDisabled(true);
    targetPathEdit->setText(targetPath);
    QLabel *targetLabel = new QLabel(tr("Target Path: "));
    QPushButton *targetPathButton = new QPushButton(tr("Go To the directory"));
    targetPathButton->setStyleSheet("font-weight: bold; color: blue; text-decoration: underline");
    connect(targetPathButton, SIGNAL(clicked()), selectPathSignalMapper, SLOT(map()));
    selectPathSignalMapper->setMapping(targetPathButton, "Target");
    connect(selectPathSignalMapper, SIGNAL(mapped (QString)), this, SLOT(onGoToPathButtonClicked(QString)));

    //QPushButton *cancelFinishButton = new QPushButton(tr("Cancel"));
    QPushButton *determineFinishButton = new QPushButton(tr("Ok"));
    connect(determineFinishButton, &QPushButton::clicked, this, &ResizeProgressBarDialog::reject);
    QHBoxLayout *sourcePathLayout = new QHBoxLayout;
    sourcePathLayout->addWidget(sourceLabel);
    sourcePathLayout->addWidget(sourcePathEdit);
    sourcePathLayout->addWidget(sourcePathButton);

    QHBoxLayout *targetPathLayout = new QHBoxLayout;
    targetPathLayout->addWidget(targetLabel);
    targetPathLayout->addWidget(targetPathEdit);
    targetPathLayout->addWidget(targetPathButton);

    this->resizeCropFinishLayout = new QVBoxLayout;
    this->resizeCropFinishLayout->addWidget(resizeCropFinishLabel);
    this->resizeCropFinishLayout->addLayout(sourcePathLayout);
    this->resizeCropFinishLayout->addLayout(targetPathLayout);
    //this->resizeCropFinishLayout->addWidget(cancelFinishButton);
    this->resizeCropFinishLayout->addWidget(determineFinishButton);

    this->stateStackedWidget = new QStackedWidget(this);
    QWidget *runWidget = new QWidget;
    runWidget->setLayout(this->resizeCropRunLayout);

    QWidget *finishWidget = new QWidget;
    finishWidget->setLayout(this->resizeCropFinishLayout);
    // 添加页面（用于切换）
    this->stateStackedWidget->addWidget(runWidget);
    this->stateStackedWidget->addWidget(finishWidget);

    // mainlayout
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(this->stateStackedWidget);
    this->setLayout(mainLayout);

    ResizeProgressThread *progressThread = new ResizeProgressThread(this, this->sourceFileInfo, this->resizecropArgs);
    progressThread->start();
    connect(progressThread, &ResizeProgressThread::reportFinished, this, [=]() {
        // important: delete watcher again
        // jump into finish layout
        this->setState("FINISH");
        this->determineRunButton->setEnabled(true);
    });
    connect(cancelRunButton, &QPushButton::clicked, this, [=](){
        if (this->state == ResizeProgressBarDialog::FINISH){
            QMessageBox::question(this, "Cancel Task",
                                  tr("The task has canceled, don't canceled again!\n"),
                                  QMessageBox::Yes,
                                  QMessageBox::Yes);
        }
        else {
            emit clipCancel();
        }
    });
    connect(this, &ResizeProgressBarDialog::clipCancel, this, [=](){
        progressThread->Cancel();
        while (progressThread->isRunning())
        emit clipCancelOk();
        qDebug() << "clipCancelOk";
        delete progressThread;
    });
}

void ResizeProgressBarDialog::reject()
{
    QMessageBox::StandardButton resBtn = QMessageBox::Yes;
    if (this->state == ResizeProgressBarDialog::RUNNING) {
        resBtn = QMessageBox::question( this, "Resize&Crop",
                                        tr("Are you sure exit?The task is running\n"),
                                        QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                        QMessageBox::Yes);
    }
    if (resBtn == QMessageBox::Yes) {
        if (this->state == ResizeProgressBarDialog::RUNNING)
            emit clipCancel();
            connect(this, &ResizeProgressBarDialog::clipCancelOk, this, [=](){
            QDialog::reject();
            qDebug() << " QDialog::reject();";
            });
        QDialog::reject();
    }
}

ResizeProgressBarDialog::~ResizeProgressBarDialog(){
    resizecropArgs = nullptr;
    delete selectPathSignalMapper;
    qDebug() << "~ResizeProgressBarDialog\n";
}

void ResizeProgressBarDialog::onExtendButtonClicked(){
    if (this->clipInfoListWidget->isVisible()){
        this->clipInfoListWidget->hide();
        this->extendButton->setText(tr("Extend"));
    }
    else {
        this->clipInfoListWidget->show();
        this->extendButton->setText(tr("Hidden"));
    }
}

void ResizeProgressBarDialog::onCancelButtonClicked(){

}

void ResizeProgressBarDialog::onDetermineButtonClicked(){

}

void ResizeProgressBarDialog::onGoToPathButtonClicked(QString pathType){
    QString path;
    if (QString::compare(pathType, "Target") == 0)
        path = this->targetPath;
    else
        path = this->sourcePath;
    qDebug() << "path: "<< path;
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}



void ResizeProgressBarDialog::onRunDetermineButtonClicked(){
    this->setState("FINISH");
    this->stateStackedWidget->setCurrentIndex(1);
}

void ResizeProgressBarDialog::paintEvent(QPaintEvent *e){
    this->setWindowTitle(QString(tr("Resize Pictures Tool")));
    if (this->state == this->RUNNING){
        this->stateStackedWidget->setCurrentIndex(0);
    }
    else {
        this->stateStackedWidget->setCurrentIndex(1);
    }
}

void ResizeProgressBarDialog::onCheckProgressBar(){

}

void ResizeProgressBarDialog::setState(QString state){
    if (QString::compare(state, "RUNNING") == 0){
        this->state = ResizeProgressBarDialog::RUNNING;
    }
    else{
         this->state = ResizeProgressBarDialog::FINISH;
    }
}
