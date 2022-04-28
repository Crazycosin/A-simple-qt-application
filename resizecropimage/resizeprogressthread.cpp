#include "resizeprogressthread.h"
#include "resizeprogressbar.h"
#include <QDir>
#include <QTime>
#include <windows.h>
#include <QImageReader>

ResizeProgressThread::~ResizeProgressThread()
{
    pMain = nullptr;
    resizecropArgs = nullptr;
}

ResizeProgressThread::ResizeProgressThread(
        ResizeProgressBarDialog *gui,
        QList<QFileInfo> sourceFileInfo,
        ResizeCropArgs *resizecropArgs
        )
{
    pMain=gui;
    isCancel = false;
    this->resizecropArgs = resizecropArgs;
    this->sourceFileInfo = sourceFileInfo;
    connect(this, SIGNAL(updateClipPictureProgressBar(int)), this,
            SLOT(slotUpdateClipPictureProgressBar(int)));
    connect(this, SIGNAL(updateClipPictureProgressLabel(const QString &)), this,
            SLOT(slotUpdateClipPictureProgressLabel(const QString &)));
    connect(this, SIGNAL(updateListWidget(const QString &)), this,
            SLOT(slotUpdateListWidget(const QString &)));
    connect(this,SIGNAL(updateClipProgressBar(int)), this,
            SLOT(slotUpdateClipProgressBar(int)));
    connect(this, SIGNAL(updateClipPictureProgressBarRange(int, int)), this,
            SLOT(slotUpdateClipPictureProgressBarRange(int, int)));
}

void ResizeProgressThread::run()
{
    QDir targetDir(resizecropArgs->targetPath);
    int clipNum = 0;
    QString tplClipInfo = "%1 %2 %3s";
    int totalClipProgressSize = 3 + resizecropArgs->isCrop + resizecropArgs->isResize;
    for (int i=0; i<sourceFileInfo.count(); i++)
        {
            if (this->isCancel){
                // get cancel signal
                this->setCanceled(false);
                break;
            }
           // Sleep(5000);
            emit updateClipPictureProgressBarRange(0, totalClipProgressSize);
            QTime timeRecorder;
            timeRecorder.start();
            int progress = 1;
            QFileInfo thisFile = sourceFileInfo.at(i);
            QString fileName = thisFile.fileName();
            emit updateClipPictureProgressLabel(QString(tr("Current Picture: %1")).arg(fileName));
            emit updateClipPictureProgressBar(progress);
            QString sourceFilePath = thisFile.filePath();
            QString targetFilePath = targetDir.absoluteFilePath(
                        QString("%1_OK.%2").
                        arg(thisFile.baseName()).
                        arg(thisFile.suffix()));
            if (!sourceFileInfo.at(i).isFile())
                continue;
            QImage sourceImage;
            QImage targetImage;
            QImageReader reader;
            reader.setFileName(sourceFilePath);
            QSize sourceSize = reader.size();
            // resize
            if (resizecropArgs->isResize){
                // resizing
                sourceSize.scale(
                            resizecropArgs->resizeWidth,
                            resizecropArgs->resizeHeight,
                            Qt::IgnoreAspectRatio
                            );
                progress += 1;
                emit updateClipPictureProgressBar(progress);
                reader.setScaledSize(sourceSize);
            }
            sourceImage = reader.read();
            if (resizecropArgs->isCrop){
                // clipping
                targetImage = sourceImage.copy(resizecropArgs->cropRect);
                progress += 1;
                emit updateClipPictureProgressBar(progress);
            }
            else{
                // simple copy saving
                targetImage = sourceImage.copy(0, 0, sourceSize.width(), sourceSize.height());
            }
            if (QString::compare(resizecropArgs->pipeMode, "RGB") == 0)
                targetImage = targetImage.convertToFormat(QImage::Format_BGR888);
            else
                targetImage = targetImage.convertToFormat(QImage::Format_Grayscale8);
            progress += 1;
            emit updateClipPictureProgressBar(progress);
            if (targetImage.save(targetFilePath)){
                progress += 1;
                emit updateClipPictureProgressBar(progress);
                clipNum += 1;
                emit updateListWidget(tplClipInfo.arg(fileName).
                                      arg("Processed Success").
                                      arg(timeRecorder.elapsed() / 1000));
            }
            else{
                QString noOkTargetFilePath = targetDir.absoluteFilePath(
                            QString("%1_NOK.%2").
                            arg(thisFile.baseName()).
                            arg(thisFile.suffix()));
                emit updateListWidget(tplClipInfo.arg(fileName).
                                      arg("Processed Failed").
                                      arg(timeRecorder.elapsed() / 1000));
                // create empty file
                QFile tfile(noOkTargetFilePath);
                tfile.open(QIODevice::WriteOnly);
                tfile.close();
            }
            emit updateClipProgressBar(clipNum);
    }
    emit reportFinished();
}

void ResizeProgressThread::slotUpdateListWidget(const QString &text){
    pMain->clipInfoListWidget->addItem(text);
}

void ResizeProgressThread::slotUpdateClipPictureProgressLabel(const QString &text){
    pMain->currentresizeCropProgressLabel->setText(text);
}

void ResizeProgressThread::slotUpdateClipProgressBar(int progress){
    pMain->resizeCropProgressBar->setValue(progress);
    int sourceTotal = pMain->resizeCropProgressBar->maximum();
    pMain->resizeCropProgressLabel->setText(QString("Pictures Cut Out Progress(%1/%2)").
                                        arg(progress).
                                        arg(sourceTotal));
}

void ResizeProgressThread::slotUpdateClipPictureProgressBar(int progress){
    pMain->currentresizeCropProgressBar->setValue(progress);
}

void ResizeProgressThread::slotUpdateClipPictureProgressBarRange(int min, int max){
    pMain->currentresizeCropProgressBar->setRange(min, max);
}

void ResizeProgressThread::Cancel(){
   this->isCancel = true;
}

void ResizeProgressThread::setCanceled(bool value){
   this->isCancel = value;
}
