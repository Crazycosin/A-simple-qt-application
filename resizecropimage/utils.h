#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QByteArray>

void recursiveAllFiles(
        QString strFolder,
        QList<QFileInfo> &fileInfoList,
        QSet<QByteArray> *validFormatSet,
        int needNum = 0
        );

#endif // UTILS_H
