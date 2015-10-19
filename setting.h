#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>

namespace Ui {
class setting;
}

class setting : public QWidget
{
    Q_OBJECT

public:
    explicit setting(QWidget *parent = 0);
    ~setting();

private:
    Ui::setting *ui;

public:
    //版本
    QLineEdit *ui_version_arm;
    QLineEdit *ui_version_dsp;
    QLineEdit *lineedit_freq;

    //时钟和滤波
    QComboBox *combobox_clock;
    QLineEdit *lineedit_iir_pass1;
    QLineEdit *lineedit_iir_pass2;
    QLineEdit *lineedit_ntp_ip;
    QLineEdit *lineedit_ntp_interval;
    QLineEdit *ui_rtc_adj;
    QLineEdit *ui_transmode;

    //IP
    QLineEdit *lineedit_ip;
    QLineEdit *lineedit_mask;
    QLineEdit *lineedit_gateway;
    QLineEdit *ui_master_ip;
    QLineEdit *ui_slave_ip;
    QLineEdit *ui_server_auth;

    //安装信息
    QComboBox *ui_inst_mode;
    QComboBox *ui_inst_dir1;
    QComboBox *ui_inst_dir2;
    QLineEdit *ui_inst_dir3;
    QLineEdit *ui_inst_longitude;
    QLineEdit *ui_inst_latitude;
    QLineEdit *ui_inst_height;
    QLineEdit *ui_inst_field;




private slots:
    void on_pushButton_applay_clicked();
    void on_pushButton_cancel_clicked();
};

#endif // SETTING_H
