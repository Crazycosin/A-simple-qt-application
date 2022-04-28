#include "utils.h"

#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QByteArray>
#include <QSet>

void recursiveAllFiles(
        QString strFolder,
        QList<QFileInfo> &fileInfoList,
        QSet<QByteArray> *validFormatSet,
        int needNum
        )
{
    QDir dir(strFolder);
    QFileInfoList listFileInfos = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    foreach(QFileInfo fileInfo, listFileInfos)
    {
        if (fileInfo.isDir())
        {
            recursiveAllFiles(fileInfo.absoluteFilePath(), fileInfoList, validFormatSet, needNum);
        }
        else
        {
            // filter valid format
            if (validFormatSet->contains(fileInfo.suffix().toUtf8()))
                fileInfoList.push_back(fileInfo);
            if (fileInfoList.count() == needNum)
                return;
        }
    }
}
