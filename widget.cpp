#include "widget.h"
#include "ui_widget.h"
#include <QMenu>
#include "modify.h"

QStringList curve_title = QStringList()
        << "LAW-UD"
        << "LAW-EW"
        << "LAW-SN"
        ;
QStringList clock_source = QStringList()
        << "NONE"
        << "RTC"
        << "NTP"
        << "BD"
        << "IRIG";

QStringList triggle_list = QStringList()
        << "NONE"
        << "Intensity"
        << "STA/LTA";

QStringList inst_mode_list = QStringList()
        << "水平"
        << "垂直";

QStringList inst_dir1_list = QStringList()
        << "正北"
        << "北偏东"
        << "正东"
        << "东偏南"
        << "正南"
        << "南偏西"
        << "正西"
        << "西偏北";

QStringList inst_dir2_list = QStringList()
        << "偏北"
        << "偏东"
        << "偏南"
        << "偏西";

QStringList save_wave_type_list = QStringList()
        << "RAW1"
        << "RAW2"
        << "IIR";

//static Option *option_copy;
static TcpClient *tcpclient_copy;
static Widget *this_obj;
static QMutex *mutex_copy;

static int heart_cnt;
static void setCurTime(uint64_t waveTime);

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    int i;
    ui->setupUi(this);
    tableWidgetCur = ui->tableView;
    textLog = ui->textCurLog;
//    textSysInfo = ui->textEditSysinfo;
    textShakeInfo = ui->textEditShakeinfo;
    textDnlog2 = ui->textEditDnlog2;
    lineFileName = ui->lineEditFileName;
//    textWaveStatus = ui->waveStatus;
    textParseLog = ui->parseLog;

//    timeCur = ui->textEditTime;
    xScale = ui->doubleSpinBoxX;
    yScale = ui->doubleSpinBoxY;
    localCBox = ui->localCbox;
    remoteCBox = ui->remoteCbox;
    localTable = ui->localTableView;
    remoteTable = ui->remoteTableView;
    downloadTable = ui->tableView_download;
    logHide = ui->pushButton_hide;
    fileSize = ui->lineEdit_size;
    progressBar = ui->progressBar;
//    start = ui->conncet;
//    edit_ip = ui->hostLine;
//    edit_port = ui->portLine;
    lcdNumberTime = ui->lcdNumber;
    reflush = ui->pushButtonReflush;
//    fileCtrl = new FileCtrl();
    textLog->setContextMenuPolicy(Qt::CustomContextMenu);
    textDnlog2->setContextMenuPolicy(Qt::CustomContextMenu);

    //变量初始化
    this_obj = this;
    plot_time = 0;
    wave_time = 0;
    is_save = false;
    sample_freq = 100;
    time_step = 1.0 / sample_freq;
    remotePath = "/";
    ui->pushButton_disp->setStyleSheet("QPushButton{text-align : right;}");
    logHide->setStyleSheet("QPushButton{text-align : left;}");
    ftpActionType = TYPE_IDLE;
    remoteDrag.flag = false;
    localDrag.flag = false;
    sac_file = new SacFormat();

    //准备数据模型
    table_model = new QStandardItemModel(tableWidgetCur);
    table_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("Name")));
    table_model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Ip")));
    table_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("Port")));
    //利用setModel()方法将数据模型与QTableView绑定
    tableWidgetCur->setModel(table_model);

    //设置列宽不可变动，即不能通过鼠标拖动增加列宽
    tableWidgetCur->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    tableWidgetCur->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
//    tableWidgetCur->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    tableWidgetCur->hideColumn(2);      // 隐藏Port列

    //设置表格的各列的宽度值
    tableWidgetCur->setColumnWidth(0,80);
    tableWidgetCur->setColumnWidth(1,120);

    //设置选中时为整行选中
    tableWidgetCur->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置表格的单元为只读属性，即不能编辑
    tableWidgetCur->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //如果你用在QTableView中使用右键菜单，需启用该属性
    tableWidgetCur->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableWidgetCur, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showCurTableMenu(const QPoint&)));

    download_model = new QStandardItemModel(downloadTable);
    download_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("Name")));
    download_model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("Ip")));
    download_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("user")));
    download_model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("password")));
    //利用setModel()方法将数据模型与QTableView绑定
    downloadTable->setModel(download_model);

    //设置列宽不可变动，即不能通过鼠标拖动增加列宽
    downloadTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    downloadTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
//    tableWidgetCur->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    downloadTable->hideColumn(2);      // 隐藏user列
    downloadTable->hideColumn(3);      // 隐藏password列

    //设置表格的各列的宽度值
    downloadTable->setColumnWidth(0,80);
    downloadTable->setColumnWidth(1,120);

    //设置选中时为整行选中
    downloadTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    //设置表格的单元为只读属性，即不能编辑
    downloadTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //如果你用在QTableView中使用右键菜单，需启用该属性
    downloadTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(downloadTable, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showDownloadTableMenu(const QPoint&)));

    settings = new QSettings("./config.ini", QSettings::IniFormat);
    load_conf();    // 加载参数

    downloadSet = new QSettings("./downloadConfig.ini", QSettings::IniFormat);
    load_downloadConf();    // 加载参数

    listView_ana = ui->listView_ana;
    listViewModel = new QStringListModel(listView_ana);
    listView_ana->setModel(listViewModel);
    listView_ana->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listView_ana, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showListViewMenu(const QPoint&)));

    // 实时波形设置
    interval_xCur = new QwtInterval(0, 10);
    interval_yCur = new QwtInterval(-1, 1);
    plotCur[0].plot = new Plot(ui->qwtPlot_ud);
    plotCur[1].plot = new Plot(ui->qwtPlot_ew);
    plotCur[2].plot = new Plot(ui->qwtPlot_sn);

    for (i=0; i<AXIS_MAX; i++)
    {
        plotCur[i].plot->set_title("Time(s)", "g");
        plotCur[i].plot->set_x_scale(interval_xCur->minValue(), interval_xCur->maxValue());
        plotCur[i].plot->set_y_scale(interval_yCur->minValue(), interval_yCur->maxValue());

        plotCur[i].busy = false;
    }

    for (i=0; i<WAVE_MAX; i++)
    {
        curveCur[i].plot = NULL;
        curveCur[i].id   = -1;
        curveCur[i].max.setX(0);
        curveCur[i].max.setY(0);
    }

    btn_wave[0] = NULL;//ui->pushButton_ud;
    btn_wave[1] = NULL;//ui->pushButton_ew;
    btn_wave[2] = NULL;//ui->pushButton_sn;
    for (i=0; i<AXIS_MAX; i++)
    {
//        connect(btn_wave[i], &QPushButton::toggled, this, &Widget::wave_btn_toggled);
        btn_wave_checked[i] = false;
    }

    // 波形分析设置
    interval_xAna = new QwtInterval(0, 10);
    interval_yAna = new QwtInterval(-1, 1);
    plotAna[0].plot = new Plot(ui->qwtPlot_ud1);
    plotAna[1].plot = new Plot(ui->qwtPlot_ew1);
    plotAna[2].plot = new Plot(ui->qwtPlot_sn1);

    for (i=0; i<3; i++)
    {
        plotAna[i].plot->set_title("Time(s)", "Y");
        plotAna[i].plot->set_x_scale(interval_xAna->minValue(), interval_xAna->maxValue());
        plotAna[i].plot->set_y_scale(interval_yAna->minValue(), interval_yAna->maxValue());

        plotAna[i].busy = false;
    }

    for (i=0; i<12; i++)
    {
        curveAna[i].plot = NULL;
        curveAna[i].id   = -1;
        curveAna[i].max.setX(0);
        curveAna[i].max.setY(0);
    }

    // 连接超时计时器
    tcp_timer = new QTimer(this);
    connect(tcp_timer, SIGNAL(timeout()), this, SLOT(tcp_timeOut()));

    ftp_timer = new QTimer(this);
//    connect(ftp_timer, SIGNAL(timeout()), this, SLOT(ftp_timeOut()));
//    timer->start(1000);
    lcdNumberTime = ui->lcdNumber;
    lcdNumberTime->setDigitCount(sizeof(DISPTIME_FORMAT));
    lcdNumberTime->setMode(QLCDNumber::Dec);
    lcdNumberTime->setSegmentStyle(QLCDNumber::Flat);
    setCurTime(0);

    // 设置
    sysSet = new setting();
    lineedit_sta            = ui->lineEdit_sta;
    lineedit_lta            = ui->lineEdit_lta;
    lineedit_sta_lta_h      = ui->lineEdit_sta_lta_h;
    lineedit_sta_lta_l      = ui->lineEdit_sta_lta_l;
    lineedit_intensity_h    = ui->lineEdit_intensity_h;
    lineedit_intensity_l    = ui->lineEdit_intensity_l;
    lineedit_freq           = ui->lineEdit_freq;
    lineedit_iir_pass1      = ui->lineEdit_iir_pass1;
    lineedit_iir_pass2      = ui->lineEdit_iir_pass2;
    lineedit_ntp_ip         = ui->lineEdit_ntp_ip;
    lineedit_ntp_interval   = ui->lineEdit_ntp_interval;
    lineedit_pre_time       = ui->lineEdit_pre_time;
    lineedit_aft_time       = ui->lineEdit_aft_time;
    lineedit_ip             = ui->lineEdit_ip;
    lineedit_mask           = ui->lineEdit_mask;
    lineedit_gateway        = ui->lineEdit_gateway;
    ui_save_wave_type       = ui->comboBox_save_wave_type;

    // 保存波形数据暂时不启用
    ui_save_wave_type->setVisible(false);
    ui->label_33->setVisible(false);

    combobox_clock      = ui->comboBox_clock;
    combobox_triggle    = ui->comboBox_triggle;

    ui_master_ip        = ui->lineEdit_master_ip;
    ui_slave_ip         = ui->lineEdit_slave_ip;
    ui_server_auth      = ui->lineEdit_server_auth;

    ui_inst_mode        = ui->comboBox_inst_mode;
    ui_inst_dir1        = ui->comboBox_inst_dir1;
    ui_inst_dir2        = ui->comboBox_inst_dir2;
    ui_inst_dir3        = ui->lineEdit_inst_dir3;
    ui_inst_longitude   = ui->lineEdit_inst_longitude;
    ui_inst_latitude    = ui->lineEdit_inst_latitude;
    ui_inst_height      = ui->lineEdit_inst_height;
    ui_inst_field       = ui->lineEdit_inst_field;

    ui_inst_dir2->setVisible(false);

    ui_version_arm      = ui->lineEdit_version_arm;
    ui_version_dsp      = ui->lineEdit_version_dsp;

    ui_devid_station    = ui->lineEdit_station;
    ui_devid_location   = ui->lineEdit_location;
    ui_devid_network    = ui->lineEdit_network;

    ui_rtc_adj          = ui->lineEdit_rtc_adj;
    ui_transmode        = ui->lineEdit_transmode;

    ui_inst_mode->addItems(inst_mode_list);
    ui_inst_dir1->addItems(inst_dir1_list);
    ui_inst_dir2->addItems(inst_dir2_list);

    sysSet->ui_inst_mode->addItems(inst_mode_list);
    sysSet->ui_inst_dir1->addItems(inst_dir1_list);
    sysSet->ui_inst_dir2->addItems(inst_dir2_list);

//    tcpclient = client;
//    tcpclient_copy = client;

    // TCP客户端
    tcpclient = new TcpClient();
    is_client_connected = false;
    client_connected_index = 0;
    connect(tcpclient, &TcpClient::connected,      this, &Widget::tcpClientConnected);
    connect(tcpclient, &TcpClient::disconnected,   this, &Widget::tcpClientDiconnected);
    connect(tcpclient, &TcpClient::readyRead,      this, &Widget::tcpClientRead);
    tcpclient_copy = tcpclient;

    // ftp
    ftp = NULL;
//    ftp = new Ftp();
//    connect(ftp, &Ftp::connect_finished,    this, &Widget::ftp_connect);
//    connect(ftp, &Ftp::file_lists,          this, &Widget::ftp_lists);
    is_ftp_connected = false;
    ftp_connected_index = 0;

    // 选项，读写终端参数
//    option = new Option(tcpclient);
//    option_copy = option;

    /* 波形下载设置*/
    //本地站点设置
    localCBox->setEditable(true);
    connect(localCBox,SIGNAL(activated(QString)),this,SLOT(setLocalPath(QString)));

    local_model = new QStandardItemModel(localTable);
    setLocalHeaderItem();

    //利用setModel()方法将数据模型与QTableView绑定
    localTable->setModel(local_model);
    //设置选中时为整行选中
    localTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    localTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    localTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    localTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    localTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    // 设置拖动放置模式
//    localTable->setDragDropMode(QAbstractItemView::InternalMove);
    //设置表格的单元为只读属性，即不能编辑
    localTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //如果你用在QTableView中使用右键菜单，需启用该属性
    localTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(localTable, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showLocalMenu(const QPoint&)));

    //远程站点设置
//    remoteCBox->setEditable(true);
    connect(remoteCBox,SIGNAL(activated(QString)),this,SLOT(setRemotePath(QString)));
    remote_model = new QStandardItemModel(remoteTable);
    setRemoteHeaderItem();
    //利用setModel()方法将数据模型与QTableView绑定
    remoteTable->setModel(remote_model);
    //设置选中时为整行选中
    remoteTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    //设置拖动放置模式
//    remoteTable->setDragDropMode(QAbstractItemView::InternalMove);
    //设置表格的单元为只读属性，即不能编辑
    remoteTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //如果你用在QTableView中使用右键菜单，需启用该属性
    remoteTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(remoteTable, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(showRemoteMenu(const QPoint&)));


//    connect(xScale,SIGNAL(valueChanged(double)),this,SLOT(setXScale(double)));
//    connect(yScale,SIGNAL(valueChanged(double)),this,SLOT(setYScale(double)));
    connect(localTable,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openLocalFolder(QModelIndex)));
    connect(remoteTable,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openRemoteFolder(QModelIndex)));

    // 心跳
    mutex = new QMutex();
    heart_timer = new QTimer(this);
    heart_timer->setInterval(5000);		// 5000ms
    mutex_copy = mutex;
    heart_cnt = 0;
    connect(heart_timer, &QTimer::timeout, this, &Widget::heart_time_out);

    //不显示设置页
    ui->tabWidget->removeTab(3);

    // 拖拽使能
//    setAcceptDrops(true);

}

Widget::~Widget()
{
    if (is_client_connected)
        tcpclient->disconnect_host();
    // 保存参数
    save_conf();
    save_downloadConf();

    delete table_model;
    delete local_model;
    delete remote_model;
    delete tcp_timer;
    delete mutex;
    delete heart_timer;
    delete tcpclient;
    delete sac_file;
    delete sac_a_file;
    delete ftp;
    delete sysSet;
    delete ui;

}

/**
 * @brief   本地文件列表的表头设置
 */
void Widget::setLocalHeaderItem()
{
    local_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("文件名")));
    local_model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("文件类型")));
    local_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("文件大小")));
    local_model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("最近修改")));
}

/**
 * @brief   远程文件列表的表头设置
 */
void Widget::setRemoteHeaderItem()
{
    remote_model->setHorizontalHeaderItem(0, new QStandardItem(QObject::tr("文件名")));
    remote_model->setHorizontalHeaderItem(1, new QStandardItem(QObject::tr("文件类型")));
    remote_model->setHorizontalHeaderItem(2, new QStandardItem(QObject::tr("文件大小")));
    remote_model->setHorizontalHeaderItem(3, new QStandardItem(QObject::tr("最近修改")));
}
static void setCurTime(uint64_t waveTime)
{
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(waveTime);
    QString time_str = dateTime.toString(DISPTIME_FORMAT);
    this_obj->lcdNumberTime->display(time_str);
}

void Widget::disp_all_wave(uint8_t *p)
{
    int i;
    double wave_data[WAVE_MAX] = {0};
    QPointF wave_point[WAVE_MAX];
    uint32_t sec  = 0;
    uint32_t usec = 0;
    bool need_update = false;
    static int frame = 0;       // 实时波形抽帧显示，200hz/frame

    sec  = read_dword(p);  p += 4;
    usec = read_dword(p);  p += 4;

    frame ++;
    for (i=0; i<WAVE_MAX; i++)
    {
        wave_data[i]  = read_float(p);    p += 4;
        wave_point[i] = QPointF(plot_time, wave_data[i]);

//        if(frame != 10)
//            continue;

//        if(i == WAVE_MAX - 1)
//            frame = 0;
        if (curveCur[i].plot)
        {
            curveCur[i].plot->add_point(curveCur[i].id, wave_point[i]);
            need_update = true;
        }

        // 记录最大值
        if (wave_data[i] > curveCur[i].max.y())
        {
            curveCur[i].max.setX(plot_time);
            curveCur[i].max.setY(wave_data[i]);
        }

        // 记录最小值
        if (wave_data[i] < curveCur[i].min.y())
        {
            curveCur[i].min.setX(plot_time);
            curveCur[i].min.setY(wave_data[i]);
        }
    }

    // 更新时间轴
    if (need_update)
    {
        double min = interval_xCur->minValue();
        double max = interval_xCur->maxValue();

        //if ((plot_time > max) || (plot_time < min))
        {
            double min_update = plot_time - (max - min);
            double max_update = plot_time ;

            if (min_update < 0)
            {
                min_update = 0;
                max_update = max - min;
            }

            interval_xCur->setMinValue(min_update);
            interval_xCur->setMaxValue(max_update);

            plotCur[0].plot->set_x_scale(interval_xCur->minValue(), interval_xCur->maxValue());
//            if (is_plot_split)
            {
                plotCur[1].plot->set_x_scale(interval_xCur->minValue(), interval_xCur->maxValue());
                plotCur[2].plot->set_x_scale(interval_xCur->minValue(), interval_xCur->maxValue());
            }
        }
    }

    // 保存原始波形数据
    if (is_save)
    {
        sac_point_t sac_point = {wave_data[0], wave_data[1], wave_data[2]};
        sac_point_t sac_a_point = {wave_data[3], wave_data[4], wave_data[5]};

        sac_file->add_point(&sac_point);
        sac_a_file->add_point(&sac_a_point);
    }


    wave_time = (uint64_t)sec*1000 + usec/1000;
    plot_time += time_step;
    setCurTime(wave_time);
    // 波形数据限制
    if (plot_time > PLOT_TIME_MAX)
    {
        for (i=0; i<AXIS_MAX; i++)
            plotCur[i].plot->clear_all();

        plot_time = 0;
    }

   // for (i=0; i<AXIS_MAX; i++)
    {
//        if (checked)
        {
            if (!btn_wave_checked[0])
            {
                add_curve(0);
                btn_wave_checked[0] = true;
            }
        }
    }
}
/**
 * @brief   协议发送回调函数
 */
static int do_prot_send_cb(uint8_t *buff, int n)
{
    return tcpclient_copy->send_data(QByteArray((char *)buff, n));
}

void Widget::heart_time_out()
{
    mutex->lock();
    heart_cnt++;
    //qDebug() << "heart:" << heart_cnt;
    if (heart_cnt > 3)
    {
        heart_cnt = 0;
        tcpclient->disconnect_host();
    }
    mutex->unlock();

    prot_send(CMD_HEARTBEAT,   NULL, 0, do_prot_send_cb);
}

/**
 * @brief
 */
void Widget::itemClicked(QModelIndex index)
{
//    qDebug() << index.data().toString();
}

/**
 * @brief   时间显示更新
 */
void Widget::tcp_timeOut()
{
    tcp_timer->stop();

    if(!is_client_connected)
    {
        textLog->append("TCP连接超时");
    }
}

void Widget::ftp_timeOut()
{
    ftp_timer->stop();
    if(!is_ftp_connected)
    {
        if(ftp)
        {
            delete ftp;
            ftp = NULL;
        }
        textDnlog2->append("FTP连接超时");
    }
}

/**
 * @brief   实时波形的X轴设置
 */
void Widget::setXScale(double val)
{
    qDebug()<<__func__;
    if (val != interval_xCur->width())
    {
        interval_xCur->setMaxValue(interval_xCur->minValue() + val);

        plotCur[0].plot->set_x_scale(interval_xCur->minValue(), interval_xCur->maxValue());
        plotCur[1].plot->set_x_scale(interval_xCur->minValue(), interval_xCur->maxValue());
        plotCur[2].plot->set_x_scale(interval_xCur->minValue(), interval_xCur->maxValue());
    }
}

/**
 * @brief   实时波形的Y轴设置
 */
void Widget::setYScale(double val)
{
    if (val != interval_yCur->width())
    {
        interval_yCur->setMinValue(-val);
        interval_yCur->setMaxValue(val);

        plotCur[0].plot->set_y_scale(interval_yCur->minValue(), interval_yCur->maxValue());
        plotCur[1].plot->set_y_scale(interval_yCur->minValue(), interval_yCur->maxValue());
        plotCur[2].plot->set_y_scale(interval_yCur->minValue(), interval_yCur->maxValue());
    }
}

/**
 * @brief   波形分析的X轴设置
 */
void Widget::setXScaleAna(double val)
{
    if (val != interval_xAna->width())
    {
        interval_xAna->setMaxValue(interval_xAna->minValue() + val);

        plotCur[0].plot->set_x_scale(interval_xAna->minValue(), interval_xAna->maxValue());
        plotCur[1].plot->set_x_scale(interval_xAna->minValue(), interval_xAna->maxValue());
        plotCur[2].plot->set_x_scale(interval_xAna->minValue(), interval_xAna->maxValue());
    }
}

/**
 * @brief   波形分析的Y轴设置
 */
void Widget::setYScaleAna(double val)
{
    if (val != interval_yAna->width())
    {
        interval_yAna->setMinValue(-val);
        interval_yAna->setMaxValue(val);

        plotAna[0].plot->set_y_scale(interval_yAna->minValue(), interval_yAna->maxValue());
        plotAna[1].plot->set_y_scale(interval_yAna->minValue(), interval_yAna->maxValue());
        plotAna[2].plot->set_y_scale(interval_yAna->minValue(), interval_yAna->maxValue());
    }
}

/**
 * @brief   设置本地路径的文件列表
 */
void Widget::setLocalPath(QString path)
{
    int fileCount = 0;
    int folderCount = 0;
    int i,j;
    fileCount = fileCtrl.getLocateFiles(path);
    folderCount = fileCtrl.getLocateFolders();
    local_model->clear();
    setLocalHeaderItem();
    local_model->setItem(0, new QStandardItem("..(上级目录)"));

    for(i = 0; i < folderCount; i++)
    {
        local_model->setItem(i+1,0,new QStandardItem(fileCtrl.m_locateFolders.at(i).fileName()));
        local_model->setItem(i+1,1,new QStandardItem("目录"));
        local_model->setItem(i+1,3,new QStandardItem(fileCtrl.m_locateFolders.at(i).lastModified().toString("yy/MM/dd hh:mm")));
    }

    for(j = 0; j < fileCount; j++)
    {
        local_model->setItem(j+i+1,0,new QStandardItem(fileCtrl.m_locateFiles.at(j).fileName()));
        local_model->setItem(j+i+1,2,new QStandardItem(QString("%1").arg(fileCtrl.m_locateFiles.at(j).size())));
        local_model->setItem(j+i+1,1,new QStandardItem("文件"));
        local_model->setItem(j+i+1,3,new QStandardItem(fileCtrl.m_locateFiles.at(j).lastModified().toString("yy/MM/dd hh:mm")));
    }
}

/**
 * @brief   设置远程路径的文件列表
 */
void Widget::setRemotePath(QString path)
{
    int fileCount = 0;
    int folderCount = 0;
    int i,j;
    fileCount = fileCtrl.getRemoteFiles(path);
    folderCount = fileCtrl.getRemoteFolders();
    remote_model->clear();
    setRemoteHeaderItem();
    remote_model->setItem(0, new QStandardItem("..(上级目录)"));


    for(i = 0; i < folderCount; i++)
    {
        remote_model->setItem(i+1,0,new QStandardItem(fileCtrl.m_remoteFolders.at(i).fileName()));
        remote_model->setItem(i+1,2,new QStandardItem("目录"));
        remote_model->setItem(i+1,3,new QStandardItem(fileCtrl.m_remoteFolders.at(i).lastModified().toString("yy/MM/dd hh:mm")));
    }

    for(j = 0; j < fileCount; j++)
    {
        remote_model->setItem(j+i+1,0,new QStandardItem(fileCtrl.m_remoteFiles.at(j).fileName()));
        remote_model->setItem(j+i+1,1,new QStandardItem(QString("%1").arg(fileCtrl.m_remoteFiles.at(j).size())));
        remote_model->setItem(j+i+1,2,new QStandardItem("文件"));
        remote_model->setItem(j+i+1,3,new QStandardItem(fileCtrl.m_remoteFiles.at(j).lastModified().toString("yy/MM/dd hh:mm")));

    }
}

/**
 * @brief   打开本地目录
 */
void Widget::openLocalFolder(QModelIndex index)
{
    QString path;
    if(index.row()>fileCtrl.m_locateFolders.count())
        return ;
    if(index.row() == 0)
    {
        // 打开上一层目录
        path = fileCtrl.getLocatePrePath();
        setLocalPath(path);
    }
    else
    {
        // 打开下一层目录
        path = fileCtrl.getLocatePath() + "\\" + fileCtrl.m_locateFolders.at(index.row()-1).fileName();
        setLocalPath(path);
    }
}

/**
 * @brief   打开远程目录
 */
void Widget::openRemoteFolder(QModelIndex index)
{
    if(remotePath != "/" && index.row() == 0)
    {
        int lastIndex = remotePath.lastIndexOf("/");
        if(lastIndex != 0)
            remotePath = remotePath.mid(0,lastIndex);
        else
           remotePath = remotePath.mid(0,lastIndex+1);
        ftp->cd_dir(remotePath);
        return ;
    }
    QString type = remote_model->item(index.row(),1)->text();   // get type
    if(type == "目录")
    {
        if(remotePath.size() != remotePath.lastIndexOf("/") + 1)
        {
           remotePath += "/";
        }
        remotePath += remote_model->item(index.row())->text();  // get name
        ftp->cd_dir(remotePath);
    }
}

/**
 * @brief   显示本地菜单
 */
void Widget::showLocalMenu(const QPoint &pos)
{
    QMenu *cmenu = new QMenu();

    QAction *openAction = cmenu->addAction("打开");
//    QAction *downloadAction = cmenu->addAction("下载");
    QAction *uploadAction = cmenu->addAction("上传");
//    QAction *attrAction = cmenu->addAction("属性");


//    downloadAction->setEnabled(false);
    openAction->setEnabled(isOpenLocalEnable());
    uploadAction->setEnabled(isUploadEnable());
//    attrAction->setEnabled(isUploadEnable());
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openLocalFolder()));
    connect(uploadAction, SIGNAL(triggered(bool)), this, SLOT(uploadFile()));
//    connect(attrAction, SIGNAL(triggered(bool)), this, SLOT(localFileInfo()));

    cmenu->exec(QCursor::pos());//在当前鼠标位置显示
}

/**
 * @brief   显示远程菜单
 */
void Widget::showRemoteMenu(const QPoint &pos)
{
    QMenu *cmenu = new QMenu();

    QAction *openAction = cmenu->addAction("打开");
    QAction *downloadAction = cmenu->addAction("下载");
//    QAction *uploadAction = cmenu->addAction("上传");
    QAction *delAction = cmenu->addAction("删除");
//    QAction *attrAction = cmenu->addAction("属性");

//    uploadAction->setEnabled(false);
    openAction->setEnabled(isOpenRemoteEnable());
    downloadAction->setEnabled(isDownloadEnable());
//    attrAction->setEnabled(isDownloadEnable());
    delAction->setEnabled(isDownloadEnable());
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openRemoteFolder()));
    connect(downloadAction, SIGNAL(triggered(bool)), this, SLOT(downloadFile()));
//    connect(attrAction, SIGNAL(triggered(bool)), this, SLOT(remoteFileInfo()));
    connect(delAction, SIGNAL(triggered(bool)), this, SLOT(delRemoteFile()));

    cmenu->exec(QCursor::pos());//在当前鼠标位置显示
}


void Widget::showCurTableMenu(const QPoint &pos)
{
    QMenu *cmenu = new QMenu();

    QAction *addAction = cmenu->addAction("新规");
    QAction *modifyAction = cmenu->addAction("编辑");
    QAction *delAction = cmenu->addAction("删除");
    QAction *setAction = cmenu->addAction("设置");

    modifyAction->setEnabled(isModifyEnable());
    delAction->setEnabled(isModifyEnable());
    setAction->setEnabled(isSetEnable());
    connect(addAction, SIGNAL(triggered(bool)), this, SLOT(addCurTableModel()));
    connect(modifyAction, SIGNAL(triggered(bool)), this, SLOT(modifyCurTableModel()));
    connect(delAction, SIGNAL(triggered(bool)), this, SLOT(delCurTableModel()));
    connect(setAction, SIGNAL(triggered(bool)), this, SLOT(setCurTableModel()));

    cmenu->exec(QCursor::pos());
}

void Widget::showDownloadTableMenu(const QPoint &pos)
{
    QMenu *cmenu = new QMenu();

    QAction *addAction = cmenu->addAction("新规");
    QAction *modifyAction = cmenu->addAction("编辑");
    QAction *delAction = cmenu->addAction("删除");
//    QAction *setAction = cmenu->addAction("设置");

    modifyAction->setEnabled(isModifyEnable());
    delAction->setEnabled(isModifyEnable());
//    setAction->setEnabled(isSetEnable());
    connect(addAction, SIGNAL(triggered(bool)), this, SLOT(addDownloadTableModel()));
    connect(modifyAction, SIGNAL(triggered(bool)), this, SLOT(modifyCurTableModel()));
    connect(delAction, SIGNAL(triggered(bool)), this, SLOT(delCurTableModel()));
//    connect(setAction, SIGNAL(triggered(bool)), this, SLOT(setCurTableModel()));

    cmenu->exec(QCursor::pos());
}

void Widget::showListViewMenu(const QPoint &pos)
{
    QMenu *cmenu = new QMenu();

    QAction *addAction = cmenu->addAction("添加");
    QAction *delAction = cmenu->addAction("删除");

    delAction->setEnabled(listView_ana->currentIndex().row()==-1?false:true);
    connect(addAction, SIGNAL(triggered(bool)), this, SLOT(addListViewModel()));
    connect(delAction, SIGNAL(triggered(bool)), this, SLOT(delListViewModel()));

    cmenu->exec(QCursor::pos());
}
/**
 * @brief   打开本地目录
 */
void Widget::openLocalFolder()
{
    QModelIndex index = localTable->currentIndex();
    openLocalFolder(index);
}

/**
 * @brief   打开FTP目录
 */
void Widget::openRemoteFolder()
{
    QModelIndex index = remoteTable->currentIndex();
    openRemoteFolder(index);
}

/**
 * @brief FTP文件下载到本地
 */
void Widget::downloadFile()
{
    bool ret = false;
    QString filename = remote_model->item(remoteTable->currentIndex().row())->text();
    if (!filename.isEmpty())
    {
        QString newfile = QFileDialog::getSaveFileName(this, "另存为...", filename);
        if (!newfile.isEmpty())
        {
            if (newfile == filename)
                ret = ftp->download(filename);
            else
                ret = ftp->download(filename, newfile);
            ftpActionType = TYPE_DOWNLOAD;
        }
    }
    if(ret)
        textDnlog2->append(filename+" 开始下载...");
    else
        textDnlog2->append(filename+" 下载失败!!!");

}

/**
 * @brief 本地文件上传到FTP
 */
void Widget::uploadFile()
{
    bool ret = false;
    QString filename = fileCtrl.getLocatePath() + "/" + local_model->item(localTable->currentIndex().row())->text();
    if (!filename.isEmpty())
    {
        ret = ftp->upload(filename);
        ftpActionType = TYPE_UPLOAD;
    }

    if(ret)
        textDnlog2->append(local_model->item(localTable->currentIndex().row())->text()+" 开始上传...");
    else
        textDnlog2->append(local_model->item(localTable->currentIndex().row())->text()+" 上传失败!!!");
}

/**
 * @brief
 */
void Widget::localFileInfo()
{

}

/**
 * @brief
 */
void Widget::remoteFileInfo()
{

}

/**
 * @brief FTP文件删除
 */
void Widget::delRemoteFile()
{
    bool ret = false;
    QString filename = remote_model->item(remoteTable->currentIndex().row())->text();
    if (!filename.isEmpty())
    {
        ret = ftp->del(filename);

    }
    if(ret)
    {
        textDnlog2->append(filename+" 删除成功");
        ftp->cd_dir(remotePath);
    }
    else
        textDnlog2->append(filename+" 删除失败");
}

/**
 * @brief 实时波形列表，新规表内容
 */
void Widget::addCurTableModel()
{
    Modify *addTable = new Modify();
    addTable->setWindowTitle("新规");
    addTable->exec();
    QString newName = addTable->getName();
    QString newIp = addTable->getIp();

    if(!newName.isEmpty() && !newIp.isEmpty())
    {
        if(newIp.contains(QRegExp(IP_REG_EXP)))
            appendTableCur(newIp,newName);
        else
            QMessageBox::about(this, "警告", "IP地址格式不正确 新规无效");
    }
    delete addTable;
    addTable = NULL;
}

/**
 * @brief 波形下载列表，新规表内容
 */
void Widget::addDownloadTableModel()
{
    Modify *addTable = new Modify();
    addTable->setWindowTitle("新规");
    addTable->exec();
    QString newName = addTable->getName();
    QString newIp = addTable->getIp();

    if(!newName.isEmpty() && !newIp.isEmpty())
    {
        if(newIp.contains(QRegExp(IP_REG_EXP)))
            appendTableDownload(newIp,newName);
        else
            QMessageBox::about(this, "警告", "IP地址格式不正确 新规无效");
    }
    delete addTable;
    addTable = NULL;
}

/**
 * @brief 实时波形列表，修改表内容
 */
void Widget::modifyCurTableModel()
{
    QString oldName = table_model->item(tableWidgetCur->currentIndex().row(),0)->text();
    QString oldIp = table_model->item(tableWidgetCur->currentIndex().row(),1)->text();
    Modify *modifyTable = new Modify();
    modifyTable->setWindowTitle("编辑");
    modifyTable->setName(oldName);
    modifyTable->setIp(oldIp);
    modifyTable->exec();
    QString newName = modifyTable->getName();
    QString newIp = modifyTable->getIp();

    if(!newName.isEmpty() && !newIp.isEmpty()
            &&(newName.compare(oldName)||newIp.compare(oldIp)))
    {
        if(newIp.contains(QRegExp(IP_REG_EXP)))
        {
            table_model->setItem(tableWidgetCur->currentIndex().row(),0,new QStandardItem(newName));
            table_model->setItem(tableWidgetCur->currentIndex().row(),1,new QStandardItem(newIp));
        }
        else
            QMessageBox::about(this, "警告", "IP地址格式不正确 修改无效");
    }

    delete modifyTable;
    modifyTable = NULL;
}

/**
 * @brief 实时波形列表，删除表内容
 */
void Widget::delCurTableModel()
{
    table_model->removeRow(tableWidgetCur->currentIndex().row());
    if (is_client_connected &&(client_connected_index == tableWidgetCur->currentIndex().row()))
    {
        tcpclient->disconnect_host();
        setCurTime(0);
    }
}

/**
 * @brief 波形分析，添加新文件
 */
void Widget::addListViewModel()
{
    QStringList curList = listViewModel->stringList();
    QStringList fileList = QFileDialog::getOpenFileNames(this, "选择波形文件...","/","Files (*.mseed *.sac)");

    int curSize = curList.size();
    int fileSize = fileList.size();
    bool isExit = false;
    if(fileSize != 0)
    {
        if(curSize == 0)
            listViewModel->setStringList(fileList);
        else
        {
            for(int i = 0;i<fileSize;i++)
            {
                for(int j = 0;j<curSize;j++)
                {
                    // 去重，判断列表中是否已经存在
                    if(curList.at(j) == fileList.at(i))
                    {
                        isExit = true;
                        break;
                    }

                }
                if(isExit != true)
                    curList.append(fileList.at(i));
                else
                    isExit = false;
            }
            listViewModel->setStringList(curList);
        }
    }
}

/**
 * @brief 波形分析，删除文件
 */
void Widget::delListViewModel()
{

    listViewModel->removeRow(listView_ana->currentIndex().row());
}

/**
 * @brief 实时波形，TCP连接上后，设置菜单按下时处理。
 */
void Widget::setCurTableModel()
{
    if (is_client_connected)
    {
        if (!read_option())
            QMessageBox::about(this, "错误", "Read device fail");
    }

    sysSet->show();

}

/**
 * @brief 波形下载页面的Log内容清除
 */
void Widget::clearTextDnLog()
{
    textDnlog2->clear();
}

/**
 * @brief 实时波形页面的Log内容清除
 */
void Widget::clearTextLog()
{
    textLog->clear();
}

/**
 *  @brief  设置本地菜单的"打开"选项是否使能
 */
bool Widget::isOpenLocalEnable()
{
    // 当前光标所在行的文件类型是目录时
    if((localTable->currentIndex().row() != -1) &&
            (localTable->currentIndex().row()<= fileCtrl.m_locateFolders.count()))
        return true;
    return false;
}

/**
 *  @brief  设置远程菜单的"打开"选项是否使能
 */
bool Widget::isOpenRemoteEnable()
{
    QString type;
    // 当前不是根目录并且光标在第一行
    if(remoteTable->currentIndex().row() == 0 && remotePath != "/")
    {
        return true;
    }
    // 表中没有内容
    else if(remoteTable->currentIndex().row() == -1)
    {
        return false;
    }
    else
    {
        type = remote_model->item(remoteTable->currentIndex().row(),1)->text();
        if(type == "目录")
            return true;
        return false;
    }
}

/**
 *  @brief  设置远程菜单的"下载"和"属性"选项是否使能
 */
bool Widget::isDownloadEnable()
{
    QString type;
    // 当前不是根目录并且光标在第一行
    if(remoteTable->currentIndex().row() == 0 && remotePath != "/")
    {
        return false;
    }
    // 表中没有内容
    else if(remoteTable->currentIndex().row() == -1)
    {
        return false;
    }
    else
    {
        type = remote_model->item(remoteTable->currentIndex().row(),1)->text();
        if(type == "文件")
            return true;
        return false;
    }
}

/**
 *  @brief  设置本地菜单的"上传"和"属性"选项是否使能
 */
bool Widget::isUploadEnable()
{
    if(localTable->currentIndex().row() <= 0)
        return false;
    QString type = local_model->item(localTable->currentIndex().row(),1)->text();
    if(type == "文件")
        return true;
    return false;
}

bool Widget::isModifyEnable()
{
    if(tableWidgetCur->currentIndex().row() != -1)
        return true;
    return false;
}

bool Widget::isSetEnable()
{
    if(tableWidgetCur->currentIndex().row() != -1
            && client_connected_index == tableWidgetCur->currentIndex().row()
            && is_client_connected)
        return true;
    return false;
}

/**
 * @brief   建立连接
 */
void Widget::tcpClientConnected()
{
    int i = 0;

    is_client_connected = true;
    if(is_ftp_connected)
        tcp_timer->stop();
    heart_cnt = 0;
    heart_timer->start();


    // 获取采样频率
    textLog->append("TCP连接成功");
    prot_send(CMD_SAMPLE,   NULL, 0, do_prot_send_cb);
}

/**
 * @brief   断开连接
 */
void Widget::tcpClientDiconnected()
{
    is_client_connected = false;

    // 保存波形文件
    if (is_save)
    {
        sac_file->close_file();
        sac_a_file->close_file();

        is_save = false;
    }

    heart_timer->stop();
}

/**
 * @brief   协议解析成功后的回调函数
 */
static void do_prot_recv_cb(prot_t *prot)
{
    uint8_t *p = prot->data;

//    qDebug("cmd = %x, len = %d", prot->cmd, prot->len);
    switch (prot->cmd)
    {
        case CMD_ALL_WAVE:
        {
            this_obj->disp_all_wave(p);
            break;
        }
        case CMD_LAW_WAVE:
        case CMD_A_WAVE:
        case CMD_V_WAVE:
        case CMD_D_WAVE:
        {
            break;
        }
        case CMD_SAMPLE:
        {
            if (prot->len > 0)
            {
                int freq = read_dword(p);  p += 4;

                /*option_copy->*/
                this_obj->set_sample(freq);
                this_obj->time_step = 1.0 / freq;
                this_obj->sample_freq = freq;

//                this_obj->sample->setText(QString("采样率: %1 sps").arg(freq));
            }
            break;
        }
        case CMD_IIR:
        {
            if (prot->len > 0)
            {
                float iir_pass1 = read_float(p);     p += 4;
                float iir_pass2 = read_float(p);     p += 4;

                /*option_copy->*/
                this_obj->set_iir(iir_pass1, iir_pass2);
            }
            break;
        }
        case CMD_CLOCK:
        {
            if (prot->len > 0)
            {
                uint32_t ip_val = 0;
                uint32_t interval = 0;
                int clock = 0;

                clock = read_dword(p);  p += 4;
                if (prot->len >= 12)
                {
                    ip_val   = read_dword(p);   p += 4;
                    interval = read_dword(p);   p += 4;
                }
                /*option_copy->*/
                this_obj->set_clock(clock, ip_val, interval);
            }
            break;
        }
        case CMD_TRIGGLE:
        {
            if (prot->len > 0)
            {
                int type = 0;
                int sta  = 0;
                int lta  = 0;
                float sta_lta_h = 0;
                float sta_lta_l = 0;
                float intensity_h = 0;
                float intensity_l = 0;

                type = read_dword(p);   p += 4;
                if (prot->len >= 20)
                {
                    sta = read_dword(p);          p += 4;
                    lta = read_dword(p);          p += 4;
                    sta_lta_h = read_float(p);    p += 4;
                    sta_lta_l = read_float(p);    p += 4;
                }
                else if (prot->len >= 12)
                {
                    intensity_h = read_float(p);  p += 4;
                    intensity_l = read_float(p);  p += 4;
                }

                /*option_copy->*/
                this_obj->set_triggle(type, sta, lta, sta_lta_h, sta_lta_l, intensity_h, intensity_l);
            }
            break;
        }
        case CMD_ADJUST:
        {
            if (prot->len > 0)
            {
                float ud_ud = read_float(p);     p += 4;
                float ud_ew = read_float(p);     p += 4;
                float ud_sn = read_float(p);     p += 4;

                float ew_ud = read_float(p);     p += 4;
                float ew_ew = read_float(p);     p += 4;
                float ew_sn = read_float(p);     p += 4;

                float sn_ud = read_float(p);     p += 4;
                float sn_ew = read_float(p);     p += 4;
                float sn_sn = read_float(p);     p += 4;

                /*option_copy->*/
                this_obj->set_adjust(ud_ud, ud_ew, ud_sn,
                                          ew_ud, ew_ew, ew_sn,
                                          sn_ud, sn_ew, sn_sn);
            }
            break;
        }
        case CMD_EXPORT:
        {
            break;
        }
        case CMD_SAVE:
        {
            if (prot->len > 0)
            {
                uint8_t wave_type = read_byte(p);   p += 1;
                int pre_time = read_dword(p);       p += 4;
                int aft_time = read_dword(p);       p += 4;

                /*option_copy->*/
                this_obj->set_save(wave_type, pre_time, aft_time);
            }
            break;
        }
        case CMD_IP:
        {
            if (prot->len > 0)
            {
                uint32_t ip     = read_dword(p);    p += 4;
                uint32_t mask   = read_dword(p);    p += 4;
                uint32_t gateway= read_dword(p);    p += 4;

                /*option_copy->*/
                this_obj->set_ip(ip, mask, gateway);
            }
            break;
        }
        case CMD_HEARTBEAT:
        {
            break;
        }
        case CMD_SERVER:
        {
            if (prot->len > 0)
            {
                uint32_t master_ip = read_dword(p);    p += 4;
                uint32_t slave_ip  = read_dword(p);    p += 4;

                /*option_copy->*/
                this_obj->set_server(master_ip, slave_ip);
            }
            break;
        }
        case CMD_INSTALL:
        {
            if (prot->len > 0)
            {
                char field[100] = {0};
                uint8_t mode    = read_byte(p);     p += 1;
                float angle     = read_float(p);    p += 4;
                float longitude = read_float(p);    p += 4;
                float latitude  = read_float(p);    p += 4;
                float height    = read_float(p);    p += 4;

                memcpy(field, p, 32);               p += 32;

                /*option_copy->*/
                this_obj->set_install_info(mode, angle, longitude, latitude, height, QString::fromUtf8(field));
            }
            break;
        }
        case CMD_VERSION:
        {
            if (prot->len > 0)
            {
                uint32_t arm    = read_dword(p);    p += 4;
                uint32_t dsp    = read_dword(p);    p += 4;

                /*option_copy->*/
                this_obj->set_version(arm, dsp);
            }
            break;
        }
        case CMD_DEV_ID:
        {
            if (prot->len > 0)
            {
                char station[10]  = {0};
                char location[10] = {0};
                char channel[10]  = {0};
                char network[10]  = {0};

                memcpy(station, p, 5);      p += 5;
                memcpy(location, p, 2);     p += 2;
                memcpy(channel, p, 3);      p += 3;
                memcpy(network, p, 2);      p += 2;

                /*option_copy->*/
                this_obj->set_devid(QString(station), QString(location), QString(channel), QString(network));
            }
            break;
        }
        case CMD_SERVER_AUTH:
        {
            if (prot->len > 0)
            {
                char password[50] = {0};

                memcpy(password, p, 32);    p += 32;

                /*option_copy->*/
                this_obj->set_server_auth(QString(password));
            }
            break;
        }
        case CMD_RTC_ADJ:
        {
            if (prot->len > 0)
            {
                int adjust = read_dword(p); p += 4;

                /*option_copy->*/
                this_obj->set_rtc_adj(adjust);
            }
            break;
        }
        case CMD_TRANSMODE:
        {
            if (prot->len > 0)
            {
                int mode = read_word(p); p += 2;

                /*option_copy->*/
                this_obj->set_transmode(mode);
            }
            break;
        }
        default:
            break;
    }
    mutex_copy->lock();
    heart_cnt = 0;
    mutex_copy->unlock();
}

/**
 * @brief   客户端接收数据
 */
void Widget::tcpClientRead()
{

    QByteArray data = tcpclient->recv_data();

    if (!data.isEmpty())
    {
        prot_parse((uint8_t *)data.data(), data.size(), do_prot_recv_cb);

    }
}

void Widget::appendTableCur(QString ip, QString name, QString port)
{
    int row = table_model->rowCount();
    table_model->setItem(row, 0, new QStandardItem(name));
    table_model->setItem(row, 1, new QStandardItem(ip));
    table_model->setItem(row, 2, new QStandardItem(port));
}

void Widget::appendTableDownload(QString ip, QString name, QString user,QString password)
{
    int row = download_model->rowCount();
    download_model->setItem(row, 0, new QStandardItem(name));
    download_model->setItem(row, 1, new QStandardItem(ip));
    download_model->setItem(row, 2, new QStandardItem(user));
    download_model->setItem(row, 3, new QStandardItem(password));
}

void Widget::add_curve(int index)
{
    int i;

    for (i=0; i<AXIS_MAX; i++)
    {
        if (!plotCur[i].busy)
        {
            curveCur[i].plot = plotCur[i].plot;
            curveCur[i].id   = curveCur[i].plot->insert_curve(curve_title.at(i),i);

            plotCur[i].busy = true;
//            qDebug()<<"index is "<<index << " i is "<<i << " id is "<<curveCur[index].id;
//            break;
        }
    }
}

void Widget::del_curve(int index)
{
    int i;

    if (curveCur[index].plot)
    {
        curveCur[index].plot->del_curve(curveCur[index].id);

        for (i=0; i<AXIS_MAX; i++)
            if (curveCur[index].plot == plotCur[i].plot)
                plotCur[i].busy = false;

        curveCur[index].plot = NULL;
        curveCur[index].id   = -1;
    }
}

void Widget::load_conf()
{
    // 取得终端数量
    int size = settings->beginReadArray("users");

    // 配置中的终端列表显示在表中
    for(int i = 0;i < size;i++)
    {
        settings->setArrayIndex(i);
        table_model->setItem(i,0,new QStandardItem(settings->value("NAME").toString()));
        table_model->setItem(i,1,new QStandardItem(settings->value("IP").toString()));
        table_model->setItem(i,2,new QStandardItem(settings->value("PORT").toString()));
    }
    settings->endArray();
}

void Widget::save_conf()
{
    QStringList  list;
    int i;

    // 取得当前的终端列表
    for(i = 0;i < table_model->rowCount();i++)
    {
        list.append(table_model->item(i,0)->text());
        list.append(table_model->item(i,1)->text());
        list.append(table_model->item(i,2)->text());
    }

    // 清除配置文件，重新写入
    settings->clear();
    settings->beginWriteArray("users");
    for(i = 0;i<list.size()/3;i++)
    {
        settings->setArrayIndex(i);
        settings->setValue("NAME",list.at(3*i));
        settings->setValue("IP",list.at(3*i+1));
        settings->setValue("PORT",list.at(3*i+2));
    }
    settings->endArray();
    settings->sync();
}

void Widget::load_downloadConf()
{
    // 取得终端数量
    int size = downloadSet->beginReadArray("downloadUsers");

    // 配置中的终端列表显示在表中
    for(int i = 0;i < size;i++)
    {
        downloadSet->setArrayIndex(i);
        download_model->setItem(i,0,new QStandardItem(downloadSet->value("NAME").toString()));
        download_model->setItem(i,1,new QStandardItem(downloadSet->value("IP").toString()));
        download_model->setItem(i,2,new QStandardItem(downloadSet->value("USER").toString()));
        download_model->setItem(i,3,new QStandardItem(downloadSet->value("PASSWORD").toString()));
    }
    downloadSet->endArray();
}

void Widget::save_downloadConf()
{
    QStringList  list;
    int i;

    // 取得当前的终端列表
    for(i = 0;i < download_model->rowCount();i++)
    {
        list.append(download_model->item(i,0)->text());
        list.append(download_model->item(i,1)->text());
        list.append(download_model->item(i,2)->text());
        list.append(download_model->item(i,3)->text());
    }

    // 清除配置文件，重新写入
    downloadSet->clear();
    downloadSet->beginWriteArray("downloadUsers");
    for(i = 0;i<list.size()/4;i++)
    {
        downloadSet->setArrayIndex(i);
        downloadSet->setValue("NAME",list.at(4*i));
        downloadSet->setValue("IP",list.at(4*i+1));
        downloadSet->setValue("USER",list.at(4*i+2));
        downloadSet->setValue("PASSWORD",list.at(4*i+3));
    }
    downloadSet->endArray();
    downloadSet->sync();
}

void Widget::clearWave()
{
    for(int i = 0;i < AXIS_MAX;i++)
    {
        if (curveCur[i].plot)
        {
            curveCur[i].plot->clear();
//            plot_time = 0;
        }
    }
}

void Widget::dragEnterEvent(QDragEnterEvent *event)
{
//    qDebug()<<__func__<<event->mimeData()->text();
//    event->accept();
}

void Widget::dragMoveEvent(QDragMoveEvent *event)
{
//    event->setDropAction(Qt::MoveAction);       // 设置为移动动作
//    event->accept();
}

void Widget::dropEvent(QDropEvent *event)
{
    qDebug()<<"dropEvent is call";
//    if()
//    qDebug()<<event->mimeData()->text();
//    qDebug()<<event->mimeData()->hasUrls();
}

void Widget::on_pushButtonReflush_clicked()
{
    plot_time = 0;
//    for (int i=0; i<AXIS_MAX; i++)
    {
        if (btn_wave_checked[0])
        {
            clearWave();
        }
    }
}

void Widget::wave_btn_toggled(bool checked)
{
    int i;

    for (i=0; i<AXIS_MAX; i++)
    {
        if (checked)
        {
            if (btn_wave[i]->isChecked() && !btn_wave_checked[i])
            {
                add_curve(i);
                btn_wave_checked[i] = true;
            }
        }
        else
        {
            if (!btn_wave[i]->isChecked() && btn_wave_checked[i])
            {
                del_curve(i);
                btn_wave_checked[i] = false;
            }
        }
    }
}


/**
 * @brief   从终端读取配置
 */
bool Widget::read_option()
{
    int n;
    bool ret = false;

    if (tcpclient->is_valid())
    {
        n = prot_send(CMD_SAMPLE,   NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        n = prot_send(CMD_IIR,      NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        n = prot_send(CMD_CLOCK,    NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        n = prot_send(CMD_TRIGGLE,  NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

#if 0
        n = prot_send(CMD_ADJUST,   NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;
#endif

        n = prot_send(CMD_SAVE,    NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        n = prot_send(CMD_IP,       NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        n = prot_send(CMD_SERVER,   NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        n = prot_send(CMD_INSTALL,  NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        n = prot_send(CMD_DEV_ID,   NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        n = prot_send(CMD_VERSION,  NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        n = prot_send(CMD_SERVER_AUTH,  NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        n = prot_send(CMD_RTC_ADJ,  NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        n = prot_send(CMD_TRANSMODE,  NULL, 0, do_prot_send_cb);
        if (n < 0)
            return false;

        ret = true;
    }

    return ret;
}

/**
 * @brief   将配置写入终端
 */
int Widget::write_option()
{
    int n;
    int ret = 2;

    if (tcpclient->is_valid())
    {
        uint8_t data[100] = {0};
        uint8_t *p = data;

        // 采样率
        QString freq = lineedit_freq->text().trimmed();
        if (freq != freq_old)
        {
            p = data;

            write_dword(p, freq.toInt());   p += 4;

            n = prot_send(CMD_SAMPLE, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            freq_old = freq;
            ret = 0;
        }

        // IIR参数
        QString iir_pass1 = lineedit_iir_pass1->text().trimmed();
        QString iir_pass2 = lineedit_iir_pass2->text().trimmed();
        if ((iir_pass1_old != iir_pass1) ||
                (iir_pass2_old != iir_pass2))
        {
            p = data;

            write_float(p, iir_pass1.toFloat());  p += 4;
            write_float(p, iir_pass2.toFloat());  p += 4;

            n = prot_send(CMD_IIR, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            iir_pass1_old = iir_pass1;
            iir_pass2_old = iir_pass2;
            ret = 0;
        }

        // 时钟源
        QString clock    = combobox_clock->currentText().trimmed();
        QString ntp_ip   = lineedit_ntp_ip->text().trimmed();
        QString ntp_interval = lineedit_ntp_interval->text().trimmed();
        if ((clock != clock_old) ||
                (ntp_ip != ntp_ip_old) ||
                (ntp_interval != ntp_interval_old))
        {
            int type = clock_source.indexOf(clock);
            uint32_t ip = inet_addr(ntp_ip.toLocal8Bit().data());

            p = data;

            write_dword(p, type);               p += 4;
            if (clock == QString("NTP"))
            {
                if (!ntp_ip.contains(QRegExp(IP_REG_EXP)))
                {
                    QMessageBox::about(this, "错误", "NTP IP invalid");
                    return -1;
                }

                write_dword(p, ip);             p += 4;
                write_dword(p, ntp_interval.toInt());   p += 4;
            }

            n = prot_send(CMD_CLOCK, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            clock_old = clock;
            ntp_ip_old = ntp_ip;
            ntp_interval_old = ntp_interval;
            ret = 0;
        }

        // 触发方式
        QString triggle = combobox_triggle->currentText().trimmed();
        QString sta     = lineedit_sta->text().trimmed();
        QString lta     = lineedit_lta->text().trimmed();
        QString sta_lta_h = lineedit_sta_lta_h->text().trimmed();
        QString sta_lta_l = lineedit_sta_lta_l->text().trimmed();
        QString intensity_h = lineedit_intensity_h->text().trimmed();
        QString intensity_l = lineedit_intensity_l->text().trimmed();
        if ((triggle != triggle_old) ||
                (sta != sta_old) ||
                (lta != lta_old) ||
                (sta_lta_h != sta_lta_h_old) ||
                (sta_lta_l != sta_lta_l_old) ||
                (intensity_h != intensity_h_old) ||
                (intensity_l != intensity_l_old))
        {
            int type = triggle_list.indexOf(triggle);

            p = data;

            write_dword(p, type);           p += 4;
            if (triggle == QString("STA/LTA"))
            {
                write_dword(p, sta.toInt());        p += 4;
                write_dword(p, lta.toInt());        p += 4;
                write_float(p, sta_lta_h.toFloat());  p += 4;
                write_float(p, sta_lta_l.toFloat());  p += 4;
            }
            else if (triggle == QString("Intensity"))
            {
                write_float(p, intensity_h.toFloat());    p += 4;
                write_float(p, intensity_l.toFloat());    p += 4;
            }

            n = prot_send(CMD_TRIGGLE, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            triggle_old = triggle;
            sta_old = sta;
            lta_old = lta;
            sta_lta_h_old = sta_lta_h;
            sta_lta_l_old = sta_lta_l;
            intensity_h_old = intensity_h;
            intensity_l_old = intensity_l;
            ret = 0;
        }

        // 设置IP
        QString ip_str      = lineedit_ip->text().trimmed();
        QString mask_str    = lineedit_mask->text().trimmed();
        QString gw_str      = lineedit_gateway->text().trimmed();
        if ((ip_str != dev_ip_old) ||
                (mask_str != dev_mask_old) ||
                (gw_str != dev_gw_old))
        {
            p = data;

            uint32_t ip     = inet_addr(ip_str.toLocal8Bit().data());
            uint32_t mask   = inet_addr(mask_str.toLocal8Bit().data());
            uint32_t gw     = inet_addr(gw_str.toLocal8Bit().data());

            if (!ip_str.contains(QRegExp(IP_REG_EXP)) || !mask_str.contains(QRegExp(IP_REG_EXP)) || !gw_str.contains(QRegExp(IP_REG_EXP)))
            {
                QMessageBox::about(this, "错误", "IP invalid");
                return -1;
            }

            write_dword(p, ip);     p += 4;
            write_dword(p, mask);   p += 4;
            write_dword(p, gw);     p += 4;

            n = prot_send(CMD_IP, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            dev_ip_old = ip_str;
            dev_mask_old = mask_str;
            dev_gw_old = gw_str;
            ret = 0;
        }

        // 数据存储
        QString save_wave_type = ui_save_wave_type->currentText().trimmed();
        QString pre_time = lineedit_pre_time->text().trimmed();
        QString aft_time = lineedit_aft_time->text().trimmed();
        if ((save_wave_type != save_wave_type_old) ||
                (pre_time != pre_time_old) ||
                (aft_time != aft_time_old))
        {
            int wave_type = save_wave_type_list.indexOf(save_wave_type);

            if (wave_type == -1)
            {
                QMessageBox::about(this, "错误", "波形数据类型非法");
                return -1;
            }

            p = data;

            write_byte(p, wave_type);           p += 1;
            write_dword(p, pre_time.toInt());   p += 4;
            write_dword(p, aft_time.toInt());   p += 4;

            n = prot_send(CMD_SAVE, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            save_wave_type_old = save_wave_type;
            pre_time_old = pre_time;
            aft_time_old = aft_time;

            ret = 0;
        }

        // 服务器地址
        QString master_ip_str = ui_master_ip->text().trimmed();
        QString slave_ip_str  = ui_slave_ip->text().trimmed();
        if ((master_ip_str != master_ip_old) ||
                (slave_ip_str != slave_ip_old))
        {
            p = data;

            uint32_t master_ip = inet_addr(master_ip_str.toLocal8Bit().data());
            uint32_t slave_ip  = inet_addr(slave_ip_str.toLocal8Bit().data());

            if (!master_ip_str.contains(QRegExp(IP_REG_EXP)) || !slave_ip_str.contains(QRegExp(IP_REG_EXP)))
            {
                QMessageBox::about(this, "错误", "IP invalid");
                return -1;
            }

            write_dword(p, master_ip);  p += 4;
            write_dword(p, slave_ip);   p += 4;

            n = prot_send(CMD_SERVER, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            master_ip_old = master_ip_str;
            slave_ip_old  = slave_ip_str;
            ret = 0;
        }

        // 安装信息
        QString inst_mode       = ui_inst_mode->currentText().trimmed();
        QString inst_longitude  = ui_inst_longitude->text().trimmed();
        QString inst_latitude   = ui_inst_latitude->text().trimmed();
        QString inst_height     = ui_inst_height->text().trimmed();
        QString inst_field      = ui_inst_field->text().trimmed();
        QString inst_dir1       = ui_inst_dir1->currentText().trimmed();
        //QString inst_dir2       = ui_inst_dir2->currentText().trimmed();
        QString inst_dir3       = ui_inst_dir3->text().trimmed();
        if ((inst_mode != inst_mode_old) ||
                (inst_longitude != inst_longitude_old) ||
                (inst_latitude != inst_latitude_old) ||
                (inst_height != inst_height_old) ||
                (inst_field != inst_field_old) ||
                (inst_dir1 != inst_dir1_old) ||
                //(inst_dir2 != inst_dir2_old) ||
                (inst_dir3 != inst_dir3_old))
        {
            int dir1 = inst_dir1_list.indexOf(inst_dir1);
            //int dir2 = inst_dir2_list.indexOf(inst_dir2);
            float dir3 = inst_dir3.toFloat();
            float angle;
            int m;

            //qDebug() << dir1 << dir2 << dir3;
            if (dir1==-1 || dir3<0 || dir3>=90)
            {
                QMessageBox::about(this, "错误", "安装方位格式非法");
                return -1;
            }

            // 转化成以北为准，顺时针角度
            angle = (dir1 / 2) * 90.0 + dir3;

            if (angle > 360)
                angle -= ((int)angle / 360) * 360;

            int mode        = inst_mode_list.indexOf(inst_mode);
            float longitude = inst_longitude.toFloat();
            float latitude  = inst_latitude.toFloat();
            float height    = inst_height.toFloat();

            //qDebug("dir3: %04x, dir: %x", dir3, dir);

            p = data;
            memset(p, 0, sizeof(data));

            write_byte(p, mode);                p += 1;
            write_float(p, angle);              p += 4;
            write_float(p, longitude);          p += 4;
            write_float(p, latitude);           p += 4;
            write_float(p, height);             p += 4;

            QByteArray ba = inst_field.toUtf8();
            m = ba.length()<32 ? ba.length() : 32;
            memcpy(p, ba.data(), m);    p += 32;

            n = prot_send(CMD_INSTALL, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            inst_mode_old       = inst_mode;
            inst_longitude_old  = inst_longitude;
            inst_latitude_old   = inst_latitude;
            inst_height_old     = inst_height;
            inst_dir1_old       = inst_dir1;
            //inst_dir2_old       = inst_dir2;
            inst_dir3_old       = inst_dir3;

            ret = 0;
        }

        // 设备标识
        QString devid_station   = ui_devid_station->text();
        QString devid_location  = ui_devid_location->text().trimmed();
        QString devid_channel   = QString();//ui_devid_channel->text().trimmed();
        QString devid_network   = ui_devid_network->text().trimmed();
        if ((devid_station != devid_station_old) ||
                (devid_location != devid_location_old) ||
//                (devid_channel != devid_channel_old) ||
                (devid_network != devid_network_old))
        {
            int m;

            p = data;
            memset(p, 0, sizeof(data));

            m = devid_station.length()<5 ? devid_station.length() : 5;
            memcpy(p, devid_station.toLatin1().data(), m);      p += 5;

            m = devid_location.length()<2 ? devid_location.length() : 2;
            memcpy(p, devid_location.toLatin1().data(),m);      p += 2;

            m = devid_channel.length()<3 ? devid_channel.length() : 3;
            memcpy(p, devid_channel.toLatin1().data(), m);      p += 3;

            m = devid_network.length()<2 ? devid_network.length() : 2;
            memcpy(p, devid_network.toLatin1().data(), m);      p += 2;

            n = prot_send(CMD_DEV_ID, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            devid_station_old   = devid_station;
            devid_location_old  = devid_location;
            devid_channel_old   = devid_channel;
            devid_network_old   = devid_network;

            ret = 0;
        }

        // 认证密码
        QString server_auth = ui_server_auth->text().trimmed();
        if (server_auth != server_auth_old)
        {
            int m;

            p = data;
            memset(p, 0, sizeof(data));

            m = server_auth.length()<32 ? server_auth.length() : 32;
            memcpy(p, server_auth.toLatin1().data(), m);      p += 32;

            n = prot_send(CMD_SERVER_AUTH, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            server_auth_old   = server_auth;

            ret = 0;
        }

        QString rtc_adjust = ui_rtc_adj->text().trimmed();
        if (rtc_adjust != fac_rtc_adj_old)
        {
            int tmp = rtc_adjust.toInt();

            p = data;

            write_dword(p, tmp);    p += 4;

            n = prot_send(CMD_RTC_ADJ, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            fac_rtc_adj_old = rtc_adjust;

            ret = 0;
        }

        QString transmode = ui_transmode->text().trimmed();
        if (transmode != fac_transmode_old)
        {
            int tmp = transmode.toInt();

            p = data;

            write_word(p, tmp);    p += 2;

            n = prot_send(CMD_TRANSMODE, data, p-data, do_prot_send_cb);
            if (n < 0)
                return -1;

            fac_transmode_old = transmode;

            ret = 0;
        }
    }

    return ret;
}

void Widget::set_reboot()
{
    prot_send(CMD_REBOOT, NULL, 0, do_prot_send_cb);
}

void Widget::set_sample(float freq)
{
    freq_old = QString().setNum(freq);

    lineedit_freq->setText(freq_old);
    sysSet->lineedit_freq->setText(freq_old);
}

void Widget::set_iir(float pass1, float pass2)
{
    iir_pass1_old = QString().setNum(pass1);
    iir_pass2_old = QString().setNum(pass2);

    lineedit_iir_pass1->setText(iir_pass1_old);
    lineedit_iir_pass2->setText(iir_pass2_old);
    sysSet->lineedit_iir_pass1->setText(iir_pass1_old);
    sysSet->lineedit_iir_pass2->setText(iir_pass2_old);
}

void Widget::set_clock(int clock, uint32_t ntp_ip, uint32_t interval)
{
    clock_old = clock_source.at(clock);

    combobox_clock->setCurrentText(clock_old);
    sysSet->combobox_clock->setCurrentText(clock_old);
    if (ntp_ip != 0)
    {
#ifdef Q_OS_WIN
        struct in_addr addr = {0};
        addr.s_addr = ntp_ip;
#else
        struct in_addr addr = {0};
        addr.s_addr = ntp_ip;
#endif
        ntp_ip_old = QString(inet_ntoa(addr));
        ntp_interval_old = QString().setNum(interval);

        lineedit_ntp_ip->setText(QString("%1").arg(inet_ntoa(addr)));
        lineedit_ntp_interval->setText(QString("%1").arg(interval));
        sysSet->lineedit_ntp_interval->setText(QString("%1").arg(interval));
    }
}

void Widget::set_sta_lta(int sta, int lta, float sta_lta_h, float sta_lta_l)
{
    sta_old = QString().setNum(sta);
    lta_old = QString().setNum(lta);
    sta_lta_h_old = QString().setNum(sta_lta_h);
    sta_lta_l_old = QString().setNum(sta_lta_l);

    lineedit_sta->setText(sta_old);
    lineedit_lta->setText(lta_old);
    lineedit_sta_lta_h->setText(sta_lta_h_old);
    lineedit_sta_lta_l->setText(sta_lta_l_old);
}

void Widget::set_intensity(float intensity_h, float intensity_l)
{
    intensity_h_old = QString().setNum(intensity_h);
    intensity_l_old = QString().setNum(intensity_l);

    lineedit_intensity_h->setText(intensity_h_old);
    lineedit_intensity_l->setText(intensity_l_old);
}

void Widget::set_triggle(int type, int sta, int lta, float sta_lta_h, float sta_lta_l, float intensity_h, float intensity_l)
{
    triggle_old = triggle_list.at(type);
    sta_old = QString().setNum(sta);
    lta_old = QString().setNum(lta);
    sta_lta_h_old = QString().setNum(sta_lta_h);
    sta_lta_l_old = QString().setNum(sta_lta_l);
    intensity_h_old = QString().setNum(intensity_h);
    intensity_l_old = QString().setNum(intensity_l);

    combobox_triggle->setCurrentText(triggle_old);

    lineedit_sta->setText(sta_old);
    lineedit_lta->setText(lta_old);
    lineedit_sta_lta_h->setText(sta_lta_h_old);
    lineedit_sta_lta_l->setText(sta_lta_l_old);
    lineedit_intensity_h->setText(intensity_h_old);
    lineedit_intensity_l->setText(intensity_l_old);
}

void Widget::set_adjust(float ud_ud, float ud_ew, float ud_sn,
                          float ew_ud, float ew_ew, float ew_sn,
                          float sn_ud, float sn_ew, float sn_sn)
{
    ud_ud_old = QString().setNum(ud_ud);
    ud_ew_old = QString().setNum(ud_ew);
    ud_sn_old = QString().setNum(ud_sn);

    ew_ud_old = QString().setNum(ew_ud);
    ew_ew_old = QString().setNum(ew_ew);
    ew_sn_old = QString().setNum(ew_sn);

    sn_ud_old = QString().setNum(sn_ud);
    sn_ew_old = QString().setNum(sn_ew);
    sn_sn_old = QString().setNum(sn_sn);

#if 0
    tableview_adjust->setItem(0, 0, new QTableWidgetItem(ud_ud_old));
    tableview_adjust->setItem(0, 1, new QTableWidgetItem(ud_ew_old));
    tableview_adjust->setItem(0, 2, new QTableWidgetItem(ud_sn_old));

    tableview_adjust->setItem(1, 0, new QTableWidgetItem(ew_ud_old));
    tableview_adjust->setItem(1, 1, new QTableWidgetItem(ew_ew_old));
    tableview_adjust->setItem(1, 2, new QTableWidgetItem(ew_sn_old));

    tableview_adjust->setItem(2, 0, new QTableWidgetItem(sn_ud_old));
    tableview_adjust->setItem(2, 1, new QTableWidgetItem(sn_ew_old));
    tableview_adjust->setItem(2, 2, new QTableWidgetItem(sn_sn_old));
#endif
}

void Widget::set_event(int pre_time, int aft_time)
{
    pre_time_old = QString().setNum(pre_time);
    aft_time_old = QString().setNum(aft_time);

    lineedit_pre_time->setText(pre_time_old);
    lineedit_aft_time->setText(aft_time_old);
}

void Widget::set_save(uint8_t wave_type, int pre_time, int aft_time)
{
    if ((wave_type>=0) && (wave_type<save_wave_type_list.size()))
    {
        save_wave_type_old = save_wave_type_list.at(wave_type);
        pre_time_old = QString().setNum(pre_time);
        aft_time_old = QString().setNum(aft_time);

        ui_save_wave_type->setCurrentIndex(wave_type);
        lineedit_pre_time->setText(pre_time_old);
        lineedit_aft_time->setText(aft_time_old);
    }
}

void Widget::set_ip(uint32_t ip, uint32_t mask, uint32_t gw)
{
#ifdef Q_OS_WIN
    struct in_addr ip_addr = {0};
    struct in_addr mask_addr = {0};
    struct in_addr gw_addr = {0};

    ip_addr.s_addr = ip;
    mask_addr.s_addr = mask;
    gw_addr.s_addr = gw;
#else
    struct in_addr ip_addr = {0};
    struct in_addr mask_addr = {0};
    struct in_addr gw_addr = {0};

    ip_addr.s_addr = ip;
    mask_addr.s_addr = mask;
    gw_addr.s_addr = gw;
#endif

    dev_ip_old      = QString(inet_ntoa(ip_addr));
    dev_mask_old    = QString(inet_ntoa(mask_addr));
    dev_gw_old      = QString(inet_ntoa(gw_addr));

    lineedit_ip->setText(dev_ip_old);
    lineedit_mask->setText(dev_mask_old);
    lineedit_gateway->setText(dev_gw_old);

    sysSet->lineedit_ip->setText(dev_ip_old);
    sysSet->lineedit_mask->setText(dev_mask_old);
    sysSet->lineedit_gateway->setText(dev_gw_old);
}

void Widget::set_server(uint32_t master_ip, uint32_t slave_ip)
{
#ifdef Q_OS_WIN
    struct in_addr master_addr = {0};
    struct in_addr slave_addr = {0};

    master_addr.s_addr = master_ip;
    slave_addr.s_addr = slave_ip;
#else
    struct in_addr master_addr = {0};
    struct in_addr slave_addr = {0};

    master_addr.s_addr = master_ip;
    slave_addr.s_addr = slave_ip;
#endif

    master_ip_old = QString(inet_ntoa(master_addr));
    slave_ip_old  = QString(inet_ntoa(slave_addr));

    ui_master_ip->setText(master_ip_old);
    ui_slave_ip->setText(slave_ip_old);
    sysSet->ui_master_ip->setText(master_ip_old);
    sysSet->ui_slave_ip->setText(slave_ip_old);
}

void Widget::set_install_info(uint8_t mode, float angle, float longitude, float latitude, float height, QString field)
{
    int dir1 = (int)angle / 90;
    //int dir2 = (dir1 + 1) % 4;
    float dir3 = angle - dir1 * 90;

    dir1 *= 2;
    if (dir3 >= 0.01)
        dir1 += 1;

    if ((mode>=0) && (mode<inst_mode_list.size()) &&
            (dir1>=0) && (dir1<inst_dir1_list.size()))
    {
        inst_mode_old       = inst_mode_list.at(mode);
        inst_longitude_old  = QString().setNum(longitude);;
        inst_latitude_old   = QString().setNum(latitude);;
        inst_height_old     = QString().setNum(height);
        inst_field_old      = field;
        inst_dir1_old       = inst_dir1_list.at(dir1);
        //inst_dir2_old       = inst_dir2_list.at(dir2);
        inst_dir3_old       = QString().setNum(dir3, 'g', 3);

        ui_inst_mode->setCurrentText(inst_mode_old);
        ui_inst_longitude->setText(inst_longitude_old);
        ui_inst_latitude->setText(inst_latitude_old);
        ui_inst_height->setText(inst_height_old);
        ui_inst_field->setText(inst_field_old);
        ui_inst_dir1->setCurrentText(inst_dir1_old);
        //ui_inst_dir2->setCurrentText(inst_dir2_old);
        ui_inst_dir3->setText(inst_dir3_old);

        ;
        sysSet->ui_inst_mode->setCurrentText(inst_mode_old);
        sysSet->ui_inst_longitude->setText(inst_longitude_old);
        sysSet->ui_inst_latitude->setText(inst_latitude_old);
        sysSet->ui_inst_height->setText(inst_height_old);
        sysSet->ui_inst_field->setText(inst_field_old);
        sysSet->ui_inst_dir1->setCurrentText(inst_dir1_old);
        //ui_inst_dir2->setCurrentText(inst_dir2_old);
        sysSet->ui_inst_dir3->setText(inst_dir3_old);
    }
}

void Widget::set_version(uint32_t arm, uint32_t dsp)
{
    int arm_major   = (arm & 0xFF0000) >> 16;
    int arm_minor   = (arm & 0x00FF00) >> 8;
    int arm_patch   = (arm & 0x0000FF) >> 0;

    int dsp_major   = (dsp & 0xFF0000) >> 16;
    int dsp_minor   = (dsp & 0x00FF00) >> 8;
    int dsp_patch   = (dsp & 0x0000FF) >> 0;

    version_arm_old = QString("%1.%2.%3").arg(arm_major).arg(arm_minor).arg(arm_patch);
    version_dsp_old = QString("%1.%2.%3").arg(dsp_major).arg(dsp_minor).arg(dsp_patch);

    ui_version_arm->setText(version_arm_old);
    ui_version_dsp->setText(version_dsp_old);
    sysSet->ui_version_arm->setText(version_arm_old);
    sysSet->ui_version_dsp->setText(version_dsp_old);
}

void Widget::set_devid(QString station, QString location, QString channel, QString network)
{
    devid_station_old   = station;
    devid_location_old  = location;
    devid_channel_old   = channel;
    devid_network_old   = network;

    ui_devid_station->setText(devid_station_old);
    ui_devid_location->setText(devid_location_old);
//    ui_devid_channel->setText(devid_channel_old);
    ui_devid_network->setText(devid_network_old);
}

void Widget::set_server_auth(QString password)
{
    server_auth_old = password;

    ui_server_auth->setText(server_auth_old);
    sysSet->ui_server_auth->setText(server_auth_old);
}

void Widget::set_rtc_adj(int adjust)
{
    fac_rtc_adj_old = QString("%1").arg(adjust);

    ui_rtc_adj->setText(fac_rtc_adj_old);
    sysSet->ui_rtc_adj->setText(fac_rtc_adj_old);
}

void Widget::set_transmode(int mode)
{
    fac_transmode_old = QString("%1").arg(mode);

    ui_transmode->setText(fac_transmode_old);
    sysSet->ui_transmode->setText(fac_transmode_old);
}

void Widget::ftpConnect(QString ip)
{
    remotePath = "/";
    if(!ftp)
    {
        ftp = new Ftp();
        connect(ftp, &Ftp::connect_finished,    this, &Widget::ftp_connect);
        connect(ftp, &Ftp::file_lists,          this, &Widget::ftp_lists);
//        connect(ftp, &Ftp::dataTransferProgress,this, &Widget::updateDataTransferProgress);
        connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)),this, SLOT(updateDataTransferProgress(qint64,qint64)));
        connect(ftp_timer, SIGNAL(timeout()), this, SLOT(ftp_timeOut()));
    }

    // 判断IP是否合法
    if (ip.contains(QRegExp(IP_REG_EXP)))
    {
        ftp->connect_host(ip, FTP_USER, FTP_PWD);
        ftp->cd_dir(remotePath);
        textDnlog2->append("FTP连接中...");
    }
    else
    {
        QMessageBox::about(this, "错误", "IP invalid");
        textDnlog2->append("IP无效，连接失败，请检查IP");
    }
}

void Widget::ftpDisconnect()
{
    if(ftp)
    {
        ftp->disconnect_host();
        delete ftp;
        ftp = NULL;
    }
    remote_model->clear();
    setRemoteHeaderItem();
    textDnlog2->append("FTP连接断开");
    is_ftp_connected = false;
}

/**
 * @brief   时钟源选择
 */
void Widget::on_comboBox_clock_currentTextChanged(const QString &arg1)
{
    bool set_ntp = false;

    if (arg1 == QString("NTP"))
        set_ntp = true;

    lineedit_ntp_ip->setEnabled(set_ntp);
    lineedit_ntp_interval->setEnabled(set_ntp);
}

/**
 * @brief   触发方式
 */
void Widget::on_comboBox_triggle_currentTextChanged(const QString &arg1)
{
    bool is_intensity = false;

    if (arg1 == QString("Intensity"))
        is_intensity = true;

    if (is_intensity)
    {
        ui->groupBox_sta_lta->setEnabled(false);
        ui->groupBox_intensity->setEnabled(true);
    }
    else
    {
        ui->groupBox_sta_lta->setEnabled(true);
        ui->groupBox_intensity->setEnabled(false);
    }
}

/**
 * @brief   Applay
 */
void Widget::on_pushButton_applay_clicked()
{
    int ret = write_option();

    if (ret == 0)
	{
        QMessageBox::about(this, "提示", "Write device Success");

        // 获取采样率，主界面显示
        prot_send(CMD_SAMPLE, NULL, 0, do_prot_send_cb);
	}
    else if (ret == 2)
        QMessageBox::about(this, "提示", "No change");
    else
        QMessageBox::about(this, "错误", "Write device fail");
}

/**
 * @brief   Cancel
 */
void Widget::on_pushButton_cancel_clicked()
{
    close();
}

void Widget::on_comboBox_inst_dir1_currentIndexChanged(int index)
{
    if (index % 2 == 0)
    {
        ui_inst_dir3->setText("0");
        ui_inst_dir3->setEnabled(false);
    }
    else
        ui_inst_dir3->setEnabled(true);
}

void Widget::on_tabWidget_currentChanged(int index)
{
//    switch(index)
//    {
//    case SETTING_TAB:
//        if (is_client_connected)
//        {
//            if (!read_option())
//                QMessageBox::about(this, "错误", "Read device fail");
//        }
//        else
//            QMessageBox::about(this, "错误", "未连接");
//        break;
//    case EXIT_TAB:
//        close();
//        break;
//    default:
//        break;
//    }

}

void Widget::on_tableView_doubleClicked(const QModelIndex &index)
{
    if (is_client_connected)
    {
        tcpclient->disconnect_host();
        setCurTime(0);
        // 断开ftp，清空ftp列表
//        if(is_ftp_connected)
//            ftpDisconnect();

        textLog->append("TCP断开");
        if(index.row() != client_connected_index)
        {
            client_connected_index = index.row();
            QString ip = table_model->item(index.row(),1)->text();
            quint16 port = table_model->item(index.row(),2)->text().toInt();

            // 判断IP和端口是否合法
            if (ip.contains(QRegExp(IP_REG_EXP)) && (port > 1024))
                tcpclient->connect_host(ip, port);

            // 连接ftp
//            ftpConnect(ip);
            textLog->append("TCP连接中...");
            tcp_timer->start(CONNECT_TIME_OUT);
        }
    }
    else
    {
        client_connected_index = index.row();

        QString ip = table_model->item(index.row(),1)->text();
        quint16 port = table_model->item(index.row(),2)->text().toInt();

        // 判断IP和端口是否合法
        if (ip.contains(QRegExp(IP_REG_EXP)) && (port > 1024))
            tcpclient->connect_host(ip, port);

        // 连接ftp
//        if(is_ftp_connected)
//            ftp->cd_dir("/");
//        else
//            ftpConnect(ip);
        textLog->append("TCP连接中...");
        tcp_timer->start(CONNECT_TIME_OUT);
    }
}

void Widget::ftp_connect(bool error)
{
    if(error)
    {
        ftpDisconnect();
    }
}

void Widget::ftp_lists(QList<file_info> &info)
{
    textDnlog2->append("取得FTP目录成功");
    is_ftp_connected = true;

    ftp_timer->stop();

    int i,j;
    int fileSize = info.size();

//    if(fileSize == 0)
//    {
//        ftpDisconnect();
//        return;
//    }

    remote_model->clear();
    setRemoteHeaderItem();
    j = 0;
    if(remotePath != "/")
    {
        remote_model->setItem(0, new QStandardItem("[上级目录]"));
        j = 1;
    }

    for(i = 0;i < fileSize; i++,j++)
    {
        remote_model->setItem(j,0,new QStandardItem(info.at(i).name));
        if(info.at(i).is_dir)
        {
            remote_model->setItem(j,1,new QStandardItem("目录"));
        }
        else
        {
            remote_model->setItem(j,1,new QStandardItem("文件"));
            remote_model->setItem(j,2,new QStandardItem(QString("%1").arg(info.at(i).size)));
        }
        remote_model->setItem(j,3,new QStandardItem(info.at(i).lastTime));
    }
}

void Widget::on_textEditDnlog2_customContextMenuRequested(const QPoint &pos)
{
    QMenu *cmenu = new QMenu();

    QAction *clearAction = cmenu->addAction("清除");
    clearAction->setEnabled(textDnlog2->toPlainText().isEmpty()?false:true);
    connect(clearAction, SIGNAL(triggered(bool)), this, SLOT(clearTextDnLog()));

    cmenu->exec(QCursor::pos());
}

void Widget::on_pushButton_disp_toggled(bool checked)
{
    if(checked)
    {
        ui->pushButton_disp->setText("∧");
        textLog->hide();
    }
    else
    {
        ui->pushButton_disp->setText("∨");
        textLog->show();
    }
}

void Widget::on_textCurLog_customContextMenuRequested(const QPoint &pos)
{
    QMenu *cmenu = new QMenu();

    QAction *clearAction = cmenu->addAction("清除");

    clearAction->setEnabled(textLog->toPlainText().isEmpty()?false:true);
    connect(clearAction, SIGNAL(triggered(bool)), this, SLOT(clearTextLog()));

    cmenu->exec(QCursor::pos());
}

void Widget::on_pushButton_hide_toggled(bool checked)
{
    if(checked)
    {
        logHide->setText("∧");
        textDnlog2->hide();
    }
    else
    {
        logHide->setText("∨");
        textDnlog2->show();
    }
}

void Widget::updateDataTransferProgress(qint64 curSize, qint64 totalSize)
{
    int persent = 0;

    if(totalSize != 0)
        persent = curSize*100/totalSize;

    fileSize->setText("Size:" + QString().setNum(totalSize) + "Bytes");
    progressBar->setValue(persent);
    if(persent == 100)
    {
        if(ftpActionType == TYPE_UPLOAD)
            textDnlog2->append("本地文件上传完成");
        else
            textDnlog2->append("FTP文件下载完成");
    }
}

void Widget::on_tableView_download_doubleClicked(const QModelIndex &index)
{
    if (is_ftp_connected)
    {
        // 断开ftp，清空ftp列表
        if(is_ftp_connected)
            ftpDisconnect();

        if(index.row() != ftp_connected_index)
        {
            ftp_connected_index = index.row();
            QString ip = download_model->item(index.row(),1)->text();

            // 连接ftp
            ftpConnect(ip);
            ftp_timer->start(CONNECT_TIME_OUT);
        }
    }
    else
    {
        ftp_connected_index = index.row();

        QString ip = table_model->item(index.row(),1)->text();

        // 连接ftp
        ftpConnect(ip);
        ftp_timer->start(CONNECT_TIME_OUT);
    }
}

void Widget::on_doubleSpinBoxX_valueChanged(double arg1)
{
    qDebug()<<__func__;
    xScale->setSingleStep(arg1/10);
    if (arg1 != interval_xCur->width())
    {
        interval_xCur->setMaxValue(interval_xCur->minValue() + arg1);

        plotCur[0].plot->set_x_scale(interval_xCur->minValue(), interval_xCur->maxValue());
        plotCur[1].plot->set_x_scale(interval_xCur->minValue(), interval_xCur->maxValue());
        plotCur[2].plot->set_x_scale(interval_xCur->minValue(), interval_xCur->maxValue());
    }
}

void Widget::on_pushButtonSettime_clicked()
{
    // 设置终端时间为PC端时间
    struct timeval time;

    uint8_t data[8] = {0};
    uint8_t *p = data;
    int ret = -1;
    gettimeofday(&time,NULL);
    write_dword(p, time.tv_sec);   p += 4;
    write_dword(p, time.tv_usec);   p += 4;

    ret = prot_send(CMD_SETTIME, data, p-data, do_prot_send_cb);
    if(ret != -1)
        QMessageBox::about(this,"提示","本地时间同步成功。");
    else
        QMessageBox::about(this,"错误","本地时间同步错误！");
}

//void Widget::on_remoteTableView_pressed(const QModelIndex &index)
//{
//    qDebug()<<__func__;
//    remoteDrag.flag = true;
//    remoteDrag.name = remote_model->item(index.row())->text();

//}

//void Widget::on_localTableView_pressed(const QModelIndex &index)
//{
//    qDebug()<<__func__;
//    localDrag.flag = true;
//    localDrag.name = local_model->item(index.row())->text();
//}


void Widget::on_listView_ana_doubleClicked(const QModelIndex &index)
{
    float timeAna = 0;
    QString fileName = listViewModel->index(index.row()).data().toString();
    lineFileName->setText(fileName);

    if (!fileName.isEmpty())
    {
        if (sac_file->open_file(fileName))
        {
            sac_point_t sac_point;
            SACHeader_t header;

            sac_file->read_header(&header);

            // 显示sac波形
            int freq = header.delta;

            if ((freq!=50) && (freq!=200))
                freq = 100;

            for(int i = 0;i<AXIS_MAX;i++)
            {
                if (!plotAna[i].busy)
                {
                    curveAna[i].plot = plotAna[i].plot;
                    curveAna[i].id   = curveAna[i].plot->insert_curve(curve_title.at(i),i);

                    plotAna[i].busy = true;
//                    qDebug()<<"i is "<<i<<" index is "<<index<<" id is "<<curveAna[i].id;
                }
            }
            while (sac_file->read_point(&sac_point))
            {
                timeAna += time_step;
                QPointF ud_point = QPointF(timeAna, sac_point.ud);
                QPointF ew_point = QPointF(timeAna, sac_point.ew);
                QPointF sn_point = QPointF(timeAna, sac_point.sn);
                curveAna[0].plot->add_point(curveAna[0].id, ud_point);
                curveAna[1].plot->add_point(curveAna[1].id, ew_point);
                curveAna[2].plot->add_point(curveAna[2].id, sn_point);
            }
            sac_file->close_file();
        }
    }

}

void Widget::on_doubleSpinBoxX_ana_valueChanged(double arg1)
{
    ui->doubleSpinBoxX_ana->setSingleStep(arg1/10);
    if (arg1 != interval_xAna->width())
    {
        interval_xAna->setMaxValue(interval_xAna->minValue() + arg1);

        plotAna[0].plot->set_x_scale(interval_xAna->minValue(), interval_xAna->maxValue());
        plotAna[1].plot->set_x_scale(interval_xAna->minValue(), interval_xAna->maxValue());
        plotAna[2].plot->set_x_scale(interval_xAna->minValue(), interval_xAna->maxValue());
    }
}

void Widget::on_doubleSpinBoxY_ana_valueChanged(double arg1)
{
    ui->doubleSpinBoxY_ana->setSingleStep(arg1/10);
    if (arg1 != interval_yCur->width())
    {
        interval_yAna->setMinValue(-arg1);
        interval_yAna->setMaxValue(arg1);

        plotAna[0].plot->set_y_scale(interval_yAna->minValue(), interval_yAna->maxValue());
        plotAna[1].plot->set_y_scale(interval_yAna->minValue(), interval_yAna->maxValue());
        plotAna[2].plot->set_y_scale(interval_yAna->minValue(), interval_yAna->maxValue());
    }
}

void Widget::on_doubleSpinBoxY_valueChanged(double arg1)
{
    ui->doubleSpinBoxY->setSingleStep(arg1/10);
    if (arg1 != interval_yCur->width())
    {
        interval_yCur->setMinValue(-arg1);
        interval_yCur->setMaxValue(arg1);

        plotCur[0].plot->set_y_scale(interval_yCur->minValue(), interval_yCur->maxValue());
        plotCur[1].plot->set_y_scale(interval_yCur->minValue(), interval_yCur->maxValue());
        plotCur[2].plot->set_y_scale(interval_yCur->minValue(), interval_yCur->maxValue());
    }
}
