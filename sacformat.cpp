#include "sacformat.h"

#include <QDebug>

SacFormat::SacFormat()
{
    npts = 0;
}

SacFormat::~SacFormat()
{

}

bool SacFormat::open_file(QString filename)
{
    sac_file = new QFile(filename);
    return sac_file->open(QIODevice::ReadWrite | QIODevice::Unbuffered);
}

void SacFormat::close_file()
{
    if (sac_file)
    {
        sac_file->close();
        delete sac_file;
    }
}

bool SacFormat::write_header(QDateTime &dateTime, int sample_freq)
{
    int ret;
    QDate date = dateTime.date();
    QTime time = dateTime.time();

    SACHeader_t header;
    const char *p = (const char *)&header;
    memset((void *)p, 0, sizeof(SACHeader_t));

    header.nzyear = date.year();
    header.nzjday = QDate(date.year(), 1, 1).daysTo(date);
    header.nzhour = time.hour();
    header.nzmin  = time.minute();
    header.nzsec  = time.second();

    header.delta  = sample_freq;

    ret = sac_file->write(p, sizeof(SACHeader_t));

    return ret==sizeof(SACHeader_t) ? true : false;
}

bool SacFormat::read_header(SACHeader_t *header)
{
    int ret;
    //SACHeader_t header;
    char *p = (char *)header;

    //memset(p, 0, sizeof(SACHeader_t));

    sac_file->seek(0);
    ret = sac_file->read(p, sizeof(SACHeader_t));

    return ret==sizeof(SACHeader_t) ? true : false;
}

bool SacFormat::add_point(sac_point_t *point)
{
    int ret;
    const char *p = (const char *)point;

    ret = sac_file->write(p, sizeof(sac_point_t));
    if (ret == sizeof(sac_point_t))
    {
        npts++;
        return true;
    }
    else
        return false;
}

bool SacFormat::read_point(sac_point_t *point)
{
    int ret;
    //sac_point_t point;
    char *p = (char *)point;

    ret = sac_file->read(p, sizeof(sac_point_t));

    return ret==sizeof(sac_point_t) ? true : false;
}


