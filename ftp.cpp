#include "ftp.h"

#include <QDebug>

Ftp::Ftp()
{
    lists.clear();

    connect(this, &QFtp::commandFinished,       this, &Ftp::ftp_command_finished);
    connect(this, &QFtp::listInfo,              this, &Ftp::ftp_list);
//    connect(this, &QFtp::dataTransferProgress,  this, &Ftp::ftp_progress);

    file_download   = NULL;
    file_upload     = NULL;
}

Ftp::~Ftp()
{
}

/**
 * @brief   ftp命令回调函数
 */
void Ftp::ftp_command_finished(int id, bool error)
{
    int command = currentCommand();

    if (error)
    {
        abort();
        //qDebug() << "command id =" << id << "command" << command << "error =" << error;
    }

    switch (command)
    {
        case ConnectToHost:
        {
            break;
        }
        case Login:
        {
            emit connect_finished(error);
            break;
        }
        case Cd:
        {
            lists.clear();
            list();
            break;
        }
        case List:
        {
            emit file_lists(lists);
            break;
        }
        case Get:
        {
            file_download->close();
            break;
        }
        case Put:
        {
            file_upload->close();
            cd(".");
            break;
        }
        default:
            break;
    }
}

/**
 * @brief   ftp文件列表
 */
void Ftp::ftp_list(const QUrlInfo &url_info)
{
    file_info info;

    info.name = url_info.name();
    info.size = url_info.size();
    info.is_dir = url_info.isDir();
    info.lastTime = url_info.lastModified().toString("yy/MM/dd hh:mm");

    lists.append(info);
}

/**
 * @brief   下载进度
 */
void Ftp::ftp_progress(qint64 read, qint64 total)
{
    qDebug("the total size if %d ",total);
    emit progress(read, total);
}

/**
 * @brief   连接ftp服务器
 */
bool Ftp::connect_host(QString ip, QString username, QString password)
{
    connectToHost(ip);
    login(username, password);

    return true;
}

/**
 * @brief   断开ftp连接
 */
void Ftp::disconnect_host()
{
    close();
}

/**
 * @brief   进入子目录
 */
bool Ftp::cd_dir(QString dir)
{
    cd(dir);
    //list();

    return true;
}

/**
 * @brief   获取ftp文件列表
 */
QList<file_info> Ftp::get_list()
{
    return lists;
}

bool Ftp::download(QString filename, QString rename)
{
    bool ret = false;

    if (!filename.isEmpty())
    {
        if (file_download)
            delete file_download;

        if (!rename.isEmpty())
            file_download = new QFile(rename);
        else
            file_download = new QFile(filename);

        if (file_download->open(QIODevice::WriteOnly))
        {
            get(filename, file_download);
            ret = true;
        }
    }

    return ret;
}

bool Ftp::upload(QString filename)
{
    bool ret = false;

    if (!filename.isEmpty())
    {
        if (file_upload)
            delete file_upload;

        file_upload = new QFile(filename);
        QFileInfo file_info = QFileInfo(*file_upload);

        if (file_upload->open(QIODevice::ReadOnly))
        {
            put(file_upload, file_info.fileName());
            ret = true;
        }
    }

    return ret;
}

bool Ftp::del(QString filename)
{
    bool ret = false;

    if (!filename.isEmpty())
    {
        remove(filename);
        ret = true;
    }

    return ret;
}
