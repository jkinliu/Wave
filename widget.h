#ifndef WIDGET_H
#define WIDGET_H

//#include <QTreeWidget>
#include <qwt_plot.h>
#include <QLCDNumber>
#include <qwt_wheel.h>
#include <qwt_knob.h>
#include <qwt_interval.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_directpainter.h>
#include <qwt_plot_layout.h>
#include <qwt_system_clock.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_zoomer.h>
#include <QTimer>
#include <QTimerEvent>
#include <QMessageBox>
#include <QList>
#include <QComboBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QFileDialog>
#include <QDateTime>
#include <QSettings>
#include <QTimer>
#include <QMutex>
#include <QWidget>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QDoubleSpinBox>
#include <QTableView>
#include <QTableWidget>
#include <QTextEdit>
#include <QDialog>
#include <QProgressBar>
#include <QMouseEvent>
#include <QDropEvent>
#include <sys/time.h>
#include "plot.h"
//#include "option.h"
#include "fileCtrl.h"
#include "sacformat.h"
#include "protocol.h"
#include "global.h"
#include "tcpclient.h"
#include "ftp.h"
#include "setting.h"
#ifdef Q_OS_WIN
    #include <Winsock2.h>
#else
    #include <arpa/inet.h>
#endif

typedef struct
{
    Plot *plot;
    bool busy;
} plot_t;

typedef struct
{
    int id;
    QPointF max;    // 当前最大值
    QPointF min;    // 当前最小值
    Plot *plot;
} curve_t;

typedef struct
{
    bool flag;
    QString name;
}drag_t;

typedef enum
{
    TYPE_IDLE,
    TYPE_UPLOAD,
    TYPE_DOWNLOAD
}FTP_ACTION_TYPE;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void setLocalHeaderItem();
    void setRemoteHeaderItem();
    void disp_all_wave(uint8_t *p);
    void heart_time_out();

    bool read_option();
    int  write_option();

    void set_sample(float freq);
    void set_iir(float pass1, float pass2);

    void set_clock(int clock, uint32_t ntp_ip, uint32_t interval);
    void set_sta_lta(int sta, int lta, float sta_lta_h, float sta_lta_l);
    void set_intensity(float intensity_h, float intensity_l);
    void set_triggle(int type, int sta, int lta, float sta_lta_h, float sta_lta_l, float intensity_h, float intensity_l);

    void set_adjust(float ud_ud, float ud_ew, float ud_sn,
                      float ew_ud, float ew_ew, float ew_sn,
                      float sn_ud, float sn_ew, float sn_sn);

    void set_event(int pre_time, int aft_time);
    void set_save(uint8_t wave_type, int pre_time, int aft_time);
    void set_ip(uint32_t ip, uint32_t mask, uint32_t gw);
    void set_reboot();
    void set_server(uint32_t master_ip, uint32_t slave_ip);
    void set_install_info(uint8_t mode, float angle, float longitude, float latitude, float height, QString field);
    void set_version(uint32_t arm, uint32_t dsp);
    void set_devid(QString station, QString location, QString channel, QString network);
    void set_server_auth(QString password);
    void set_rtc_adj(int adjust);
    void set_transmode(int mode);
    void ftpConnect(QString ip);
    void ftpDisconnect();

private:
    Ui::Widget *ui;




private slots:
    void itemClicked(QModelIndex index);
    void tcp_timeOut();
    void ftp_timeOut();
    void setXScale(double val);
    void setYScale(double val);
    void setXScaleAna(double val);
    void setYScaleAna(double val);
    void setLocalPath(QString path);
    void setRemotePath(QString path);
    void openLocalFolder(QModelIndex index);
    void openRemoteFolder(QModelIndex index);
    void showLocalMenu(const QPoint& pos);
    void showRemoteMenu(const QPoint& pos);
    void showCurTableMenu(const QPoint& pos);
    void showDownloadTableMenu(const QPoint& pos);
    void showListViewMenu(const QPoint &pos);
    void openLocalFolder();
    void openRemoteFolder();
    void downloadFile();
    void uploadFile();
    void localFileInfo();
    void remoteFileInfo();
    void delRemoteFile();
    void addCurTableModel();
    void addDownloadTableModel();
    void modifyCurTableModel();
    void delCurTableModel();
    void setCurTableModel();
    void addListViewModel();
    void delListViewModel();
    void clearTextDnLog();
    void clearTextLog();

    void on_pushButtonReflush_clicked();

    void wave_btn_toggled(bool checked);
    void on_comboBox_clock_currentTextChanged(const QString &arg1);

    void on_pushButton_applay_clicked();
    void on_pushButton_cancel_clicked();

    void on_comboBox_triggle_currentTextChanged(const QString &arg1);

    void on_comboBox_inst_dir1_currentIndexChanged(int index);

    void on_tabWidget_currentChanged(int index);

    void on_tableView_doubleClicked(const QModelIndex &index);
    void ftp_connect(bool error);
    void ftp_lists(QList<file_info> &info);
    void on_textEditDnlog2_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_disp_toggled(bool checked);

    void on_textCurLog_customContextMenuRequested(const QPoint &pos);

    void on_pushButton_hide_toggled(bool checked);
    void updateDataTransferProgress(qint64 curSize,qint64 totalSize);

    void on_tableView_download_doubleClicked(const QModelIndex &index);

    void on_doubleSpinBoxX_valueChanged(double arg1);

    void on_pushButtonSettime_clicked();

//    void on_remoteTableView_pressed(const QModelIndex &index);

//    void on_localTableView_pressed(const QModelIndex &index);


    void on_listView_ana_doubleClicked(const QModelIndex &index);

    void on_doubleSpinBoxX_ana_valueChanged(double arg1);

    void on_doubleSpinBoxY_ana_valueChanged(double arg1);

    void on_doubleSpinBoxY_valueChanged(double arg1);

private:
    bool isOpenLocalEnable();
    bool isOpenRemoteEnable();
    bool isDownloadEnable();
    bool isUploadEnable();
    bool isModifyEnable();
    bool isSetEnable();
    void tcpClientConnected();
    void tcpClientDiconnected();
    void tcpClientRead();
    void appendTableCur(QString ip,QString name,QString port = "6300");
    void appendTableDownload(QString ip, QString name, QString user="root",QString password="root");
    void add_curve(int index);
    void del_curve(int index);
    void load_conf();
    void save_conf();
    void load_downloadConf();
    void save_downloadConf();
    void clearWave();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
//    void startDrag(Qt::DropActions supportedActions);


public:
//    QwtPlotZoomer* zoomer_ud;
    QwtInterval *interval_xCur;
    QwtInterval *interval_yCur;
    QTextEdit *textLog;
    plot_t plotCur[3];
    curve_t curveCur[12];
    QDoubleSpinBox *xScale;
    QDoubleSpinBox *yScale;

    QwtInterval *interval_xAna;
    QwtInterval *interval_yAna;
    plot_t plotAna[3];
    curve_t curveAna[12];

    QPushButton *btn_wave[AXIS_MAX];
    bool btn_wave_checked[AXIS_MAX];

    // tcp
    TcpClient *tcpclient;
    bool is_client_connected;
    int client_connected_index;
    // ftp
    Ftp *ftp;
    bool is_ftp_connected;
    int ftp_connected_index;

    QDoubleSpinBox *spinBoxX;
    QDoubleSpinBox *spinBoxY;
    QStandardItemModel *standardItemModel;
    // 设置
    setting *sysSet;
    QLineEdit *lineedit_sta;
    QLineEdit *lineedit_lta;
    QLineEdit *lineedit_sta_lta_h;
    QLineEdit *lineedit_sta_lta_l;
    QLineEdit *lineedit_intensity_h;
    QLineEdit *lineedit_intensity_l;
    QLineEdit *lineedit_freq;
    QLineEdit *lineedit_iir_pass1;
    QLineEdit *lineedit_iir_pass2;
    QLineEdit *lineedit_ntp_ip;
    QLineEdit *lineedit_ntp_interval;
    QLineEdit *lineedit_pre_time;
    QLineEdit *lineedit_aft_time;
    QComboBox *ui_save_wave_type;

    QLineEdit *lineedit_ip;
    QLineEdit *lineedit_mask;
    QLineEdit *lineedit_gateway;

//    QLineEdit *edit_ip;
//    QLineEdit *edit_port;

    QComboBox *combobox_clock;
    QComboBox *combobox_triggle;

    QLCDNumber *lcdNumberTime;

    QLineEdit *ui_master_ip;
    QLineEdit *ui_slave_ip;
    QLineEdit *ui_server_auth;

    QComboBox *ui_inst_mode;
    QComboBox *ui_inst_dir1;
    QComboBox *ui_inst_dir2;
    QLineEdit *ui_inst_dir3;
    QLineEdit *ui_inst_longitude;
    QLineEdit *ui_inst_latitude;
    QLineEdit *ui_inst_height;
    QLineEdit *ui_inst_field;

    QLineEdit *ui_version_arm;
    QLineEdit *ui_version_dsp;

    QLineEdit *ui_devid_station;
    QLineEdit *ui_devid_location;
//    QLineEdit *ui_devid_channel;
    QLineEdit *ui_devid_network;

    QLineEdit *ui_rtc_adj;
    QLineEdit *ui_transmode;

//    QTableWidget *tableview_adjust;

    QComboBox *localCBox;
    QComboBox *remoteCBox;
    QTableView *localTable;
    QTableView *remoteTable;
    QTableView *downloadTable;
    FileCtrl fileCtrl;
    QStandardItemModel *local_model;
    QStandardItemModel *remote_model;
    QTableView *tableWidgetCur;
    QStandardItemModel *table_model;
    QStandardItemModel *download_model;

    QPushButton *reflush;
    QPushButton *logHide;
    QLineEdit *fileSize;
    QProgressBar *progressBar;

    // 终端配置参数
    // 采样率
    QString freq_old;

    // IIR参数
    QString iir_pass1_old;
    QString iir_pass2_old;

    // 时钟源
    QString clock_old;
    QString ntp_ip_old;
    QString ntp_interval_old;

    // 触发方式
    QString triggle_old;
    QString sta_old;
    QString lta_old;
    QString sta_lta_h_old;
    QString sta_lta_l_old;
    QString intensity_h_old;
    QString intensity_l_old;

    // 设备ip
    QString dev_ip_old;
    QString dev_mask_old;
    QString dev_gw_old;

    // 校正参数
    QString ud_ud_old;
    QString ud_ew_old;
    QString ud_sn_old;
    QString ew_ud_old;
    QString ew_ew_old;
    QString ew_sn_old;
    QString sn_ud_old;
    QString sn_ew_old;
    QString sn_sn_old;

    // 数据存储
    QString save_wave_type_old;
    QString pre_time_old;
    QString aft_time_old;

    // 服务器IP和端口
    QString master_ip_old;
    QString slave_ip_old;
    QString server_auth_old;

    // 安装信息
    QString inst_mode_old;
    QString inst_dir1_old;
    QString inst_dir2_old;
    QString inst_dir3_old;
    QString inst_longitude_old;
    QString inst_latitude_old;
    QString inst_height_old;
    QString inst_field_old;

    // 软件版本
    QString version_arm_old;
    QString version_dsp_old;

    // 设备标识
    QString devid_station_old;
    QString devid_location_old;
    QString devid_channel_old;
    QString devid_network_old;

    QString fac_rtc_adj_old;
    QString fac_transmode_old;

    SacFormat *sac_file;
    SacFormat *sac_a_file;
    QSettings *settings;
    QSettings *downloadSet;

    QTimer *tcp_timer;
    QTimer *ftp_timer;
    QTimer *heart_timer;
    QMutex *mutex;

    QTextEdit *textSysInfo;
    QTextEdit *textShakeInfo;
    QTextEdit *textDnlog2;
    QLineEdit *lineFileName;
    QTextEdit *textWaveStatus;
    QTextEdit *textParseLog;

    QListView *listView_ana;
    QStringListModel *listViewModel;

    uint64_t wave_time;     // ms
    uint64_t wave_time_old; // ms
    int sample_freq;
    double plot_time;       // s
    double time_step;
    bool is_save;

    QString remotePath;
    FTP_ACTION_TYPE ftpActionType;
    drag_t remoteDrag;
    drag_t localDrag;
};

#endif // WIDGET_H
