#ifndef RESIZEPROGRESSTHREAD_H
#define RESIZEPROGRESSTHREAD_H
#include "resizeprogressbar.h"
#include <QThread>
#include <QFileInfo>

class ResizeProgressThread: public QThread
{
    Q_OBJECT
public:
    explicit ResizeProgressThread(
            ResizeProgressBarDialog *gui,
            QList<QFileInfo> sourceFileInfo,
            ResizeCropArgs *resizecropArgs = NULL
            );
    ~ResizeProgressThread();

    void run() override;
    void Cancel();
    bool isCancel;

signals:
    void updateListWidget(const QString &text);
    void updateClipProgressLabel(const QString &text);
    void updateClipPictureProgressLabel(const QString &text);
    void updateClipProgressBar(int);
    void updateClipPictureProgressBar(int);
    void reportFinished();
    void updateClipPictureProgressBarRange(int, int);

public slots:
    void slotUpdateListWidget(const QString &text);
    void slotUpdateClipPictureProgressLabel(const QString &text);
    void slotUpdateClipProgressBar(int);
    void slotUpdateClipPictureProgressBar(int);
    void slotUpdateClipPictureProgressBarRange(int, int);

public:
    ResizeProgressBarDialog *pMain;

private:
    QList<QFileInfo> sourceFileInfo;
    ResizeCropArgs *resizecropArgs;
    void setCanceled(bool value);

};

#endif // RESIZEPROGRESSTHREAD_H
