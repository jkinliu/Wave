#ifndef FILECTRL
#define FILECTRL

#include <QObject>
#include <QString>
#include <QVector>
#include <io.h>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include "ftp.h"

enum FileType{
    IS_FILE,
    IS_FOLDER
};

enum PathType{
    IS_LOCATE,
    IS_REMOTE
};

class FileCtrl//:public QObject
{

public:
    FileCtrl();
    ~FileCtrl();

    int getLocateFiles(QString path);
    int getLocateFolders();
    int getRemoteFiles(QString path);
    int getRemoteFolders();
    QString getFileName(int index,PathType pathType,FileType fileType);
    QString getFileType(int index,PathType pathType,FileType fileType);
    qint64 getFileSize(int index,PathType pathType,FileType fileType);
    QString getFileDate(int index,PathType pathType,FileType fileType);
    QString getLocatePath();
    QString getRemotePath();
    QString getLocatePrePath();
    QString getRemotePrePath();

private:
    QFileInfoList getFileList(QString path);
    QFileInfoList getFolderList(QString path);

public:
    QFileInfoList m_locateFiles;
    QFileInfoList m_locateFolders;
    QFileInfoList m_remoteFiles;
    QFileInfoList m_remoteFolders;
    QString m_locatePath;
    QString m_remotePath;
};


#endif // FILECTRL

