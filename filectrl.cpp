
#include <QObject>
#include <QString>
#include <QVector>
#include <io.h>
#include <QDir>
#include <QDateTime>
#include <QFileInfo>
#include <iostream>
#include "fileCtrl.h"
#include <QMetaObject>


FileCtrl::FileCtrl()
    :m_locatePath("")
    ,m_remotePath("")
{
    m_locateFiles.clear();
    m_locateFolders.clear();
    m_remoteFiles.clear();
    m_remoteFolders.clear();
}

FileCtrl::~FileCtrl()
{

}

/**
 * @brief   取得本地站点的文件
 */
int FileCtrl::getLocateFiles(QString path)
{

    m_locatePath = path;
    if(-1 == m_locatePath.lastIndexOf('\\'))
    {
        m_locatePath += "\\";
    }
    m_locateFiles = getFileList(m_locatePath);
    return m_locateFiles.size();
}

/**
 * @brief   取得本地站点的目录
 */
int FileCtrl::getLocateFolders()
{
   m_locateFolders = getFolderList(m_locatePath);
   return m_locateFolders.size();
}

/**
 * @brief   取得远程站点的文件
 */
int FileCtrl::getRemoteFiles(QString path)
{
    m_remotePath = path;
    if(-1 == m_remotePath.lastIndexOf('\\'))
    {
        m_remotePath += "\\";
    }
    m_remoteFiles = getFileList(m_remotePath);
    return m_remoteFiles.size();
}

/**
 * @brief   取得远程站点的目录
 */
int FileCtrl::getRemoteFolders()
{
    m_remoteFolders = getFolderList(m_remotePath);
    return m_remoteFolders.size();
}

/**
 * @brief   取得文件名
 */
QString FileCtrl::getFileName(int index,PathType pathType, FileType fileType)
{
    if(pathType == IS_LOCATE)
    {
        if(fileType == IS_FILE)
        {
            return m_locateFiles.at(index).fileName();
        }
        else
        {
            return m_locateFolders.at(index).fileName();
        }
    }
    else
    {
        if(fileType == IS_FILE)
        {
            return m_remoteFiles.at(index).fileName();
        }
        else
        {
            return m_remoteFolders.at(index).fileName();
        }
    }
}

/**
 * @brief   取得文件类型
 */
QString FileCtrl::getFileType(int index,PathType pathType, FileType fileType)
{
    if(pathType == IS_LOCATE)
    {
        if(fileType == IS_FILE)
        {
            return m_locateFiles.at(index).fileName();
        }
        else
        {
            return m_locateFolders.at(index).fileName();
        }
    }
    else
    {
        if(fileType == IS_FILE)
        {
            return m_remoteFiles.at(index).fileName();
        }
        else
        {
            return m_remoteFolders.at(index).fileName();
        }
    }
}

/**
 * @brief   取得文件的大小
 */
qint64 FileCtrl::getFileSize(int index,PathType pathType, FileType fileType)
{
    if(pathType == IS_LOCATE)
    {
        if(fileType == IS_FILE)
        {
            return m_locateFiles.at(index).size();
        }
        else
        {
            return m_locateFolders.at(index).size();
        }
    }
    else
    {
        if(fileType == IS_FILE)
        {
            return m_remoteFiles.at(index).size();
        }
        else
        {
            return m_remoteFolders.at(index).size();
        }
    }
}

/**
 * @brief   取得文件的最近修改时间
 */
QString FileCtrl::getFileDate(int index,PathType pathType, FileType fileType)
{
    if(pathType == IS_LOCATE)
    {
        if(fileType == IS_FILE)
        {
            return m_locateFiles.at(index).lastModified().toString("yy/MM/dd hh:mm");
        }
        else
        {
            return m_locateFolders.at(index).lastModified().toString("yy/MM/dd hh:mm");
        }
    }
    else
    {
        if(fileType == IS_FILE)
        {
            return m_remoteFiles.at(index).lastModified().toString("yy/MM/dd hh:mm");
        }
        else
        {
            return m_remoteFolders.at(index).lastModified().toString("yy/MM/dd hh:mm");
        }
    }
}

/**
 * @brief   取得本地站点的路径
 */
QString FileCtrl::getLocatePath()
{
    return m_locatePath;
}

/**
 * @brief   取得远程站点的路径
 */
QString FileCtrl::getRemotePath()
{
    return m_remotePath;
}

/**
 * @brief   取得本地站点的上级路径
 */
QString FileCtrl::getLocatePrePath()
{
    int position1 = m_locatePath.lastIndexOf('\\');
    int position2 = m_locatePath.indexOf('\\');
    if(position2 < position1)
    {
        m_locatePath = m_locatePath.mid(0,position1);
    }
    else
    {
        m_locatePath += "\\";
    }

    return m_locatePath;
}

/**
 * @brief   取得远程站点的上级路径
 */
QString FileCtrl::getRemotePrePath()
{
    int position1 = m_remotePath.lastIndexOf('\\');
    int position2 = m_remotePath.indexOf('\\');
    if(position2 < position1)
    {
        m_remotePath = m_remotePath.mid(0,position1);
    }
    else
    {
        m_remotePath += "\\";
    }
    return m_remotePath;
}


/**
 * @brief   取得文件列表
 */
QFileInfoList FileCtrl::getFileList(QString path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    return file_list;
}

/**
 * @brief   取得目录列表
 */
QFileInfoList FileCtrl::getFolderList(QString path)
{
    QDir dir(path);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
//  std::cout << qPrintable(QString("%1 %2 %3").arg("folder list name ").arg(i).arg(folder_list.at(i).fileName())) << std::endl;
    return folder_list;
}

