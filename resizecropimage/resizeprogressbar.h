#ifndef RESIZEPROGRESSBAR_H
#define RESIZEPROGRESSBAR_H
#include "resizecropschema.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QProgressBar>
#include <QFileInfo>
#include <QListWidget>
#include <QSignalMapper>

class ResizeProgressBarDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResizeProgressBarDialog(
            QWidget *parent = 0,
            ResizeCropArgs *resizecropArgs = NULL
            );
    ~ResizeProgressBarDialog();
    void setState(QString);
    QListWidget *clipInfoListWidget;
    QLabel *resizeCropProgressLabel;
    QProgressBar *resizeCropProgressBar;
    QLabel *currentresizeCropProgressLabel;
    QProgressBar *currentresizeCropProgressBar;

signals:
    void clipCancelOk();
    void clipCancel();
private:
    // status
    enum State {
        RUNNING,
        FINISH,
    };
    State(state);
    QStackedWidget *stateStackedWidget;
    QVBoxLayout *resizeCropFinishLayout;
    QSignalMapper *selectPathSignalMapper;
    QVBoxLayout *resizeCropRunLayout;

    // run
    QPushButton *extendButton;
    QPushButton *cancelRunButton;
    QPushButton *determineRunButton;
    int sourceTotal;
    int clipNum;
    QString targetPath;
    QString sourcePath;

    QList<QFileInfo> sourceFileInfo;
    ResizeCropArgs *resizecropArgs;

protected:
    void paintEvent(QPaintEvent*) override;
    void reject() override;

private slots:
    void onExtendButtonClicked();
    void onCancelButtonClicked();
    void onDetermineButtonClicked();
    void onGoToPathButtonClicked(QString pathType);
    void onCheckProgressBar();
    void onRunDetermineButtonClicked();
};

#endif // RESIZEPROGRESSBAR_H
