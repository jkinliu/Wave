#include "setting.h"
#include "ui_setting.h"

setting::setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setting)
{
    ui->setupUi(this);

    /*****版本*****/
    ui_version_arm = ui->lineEdit_version_arm;              //ARM版本
    ui_version_dsp = ui->lineEdit_version_dsp;              //DSP版本
    lineedit_freq = ui->lineEdit_freq;                      //采样率

    /*****时钟和滤波*****/
    combobox_clock = ui->comboBox_clock;                    //时钟源
    lineedit_iir_pass1 = ui->lineEdit_iir_pass1;            //滤波器参数1
    lineedit_iir_pass2 = ui->lineEdit_iir_pass2;            //滤波器参数2
    lineedit_ntp_ip = ui->lineEdit_ntp_ip;                  //服务器时钟源
    lineedit_ntp_interval = ui->lineEdit_ntp_interval;      //时钟源对时间隔
    ui_rtc_adj = ui->lineEdit_rtc_adj;                      //RTC校正
    ui_transmode = ui->lineEdit_transmode;                  //传输模式

    /*****IP*****/
    lineedit_ip = ui->lineEdit_ip;                          //设备IP
    lineedit_mask = ui->lineEdit_mask;                      //设备Mask
    lineedit_gateway = ui->lineEdit_gateway;                //设备网关

    ui_master_ip = ui->lineEdit_master_ip;                  //主服务IP
    ui_slave_ip = ui->lineEdit_slave_ip;                    //从服务IP
    ui_server_auth = ui->lineEdit_server_auth;              //认证密码

    /*****安装信息*****/
    ui_inst_mode = ui->comboBox_inst_mode;                  //安装方式
    ui_inst_dir1 = ui->comboBox_inst_dir1;                  //安装方位1
    ui_inst_dir2 = ui->comboBox_inst_dir2;                  //安装方位2
    ui_inst_dir3 = ui->lineEdit_inst_dir3;                  //安装角度
    ui_inst_longitude = ui->lineEdit_inst_longitude;        //经度
    ui_inst_latitude = ui->lineEdit_inst_latitude;          //纬度
    ui_inst_height = ui->lineEdit_inst_height;              //高度
    ui_inst_field = ui->lineEdit_inst_field;                //场地类型

}

setting::~setting()
{
    delete ui;
}

/**
 *  @brief 点击【确认】按钮时调用的槽函数
 */
void setting::on_pushButton_applay_clicked()
{
    QMessageBox::about(this, "提示", "Write device Success");
    close();
}

/**
 *  @brief 点击【取消】按钮是调用的槽函数
 */
void setting::on_pushButton_cancel_clicked()
{
    close();
}
