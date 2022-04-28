#ifndef RESIZECROPSCHEMA_H
#define RESIZECROPSCHEMA_H
#include <QString>
#include <QRect>

struct ResizeCropArgs{
    QString sourcePath;
    QString targetPath;
    bool isResize;
    bool isCrop;
    int resizeHeight;
    int resizeWidth;
    QRect cropRect;
    QString pipeMode;
};
#endif // RESIZECROPSCHEMA_H
