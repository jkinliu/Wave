#ifndef GLOBAL_H
#define GLOBAL_H

#define FTP_USER        "root"
#define FTP_PWD         "root"

#define IP_REG_EXP       "^(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])$" //"^(\\d{1,3}.){3}\\d{1,3}"     // 匹配IP正则表达式

#define TIME_FORMAT     "yyyy.MM.dd-hh.mm.ss"
#define DISPTIME_FORMAT     "hh:mm:ss.zzz"

#define AXIS_MAX        3
#define WAVE_MAX        12

#define PLOT_TIME_MAX   600     // s
#define CONNECT_TIME_OUT 30000  // 30s

#define WAVE_TIME_STEP  0.005

#define CURWAVE_TAB     0       // 实时波形页面
#define DOWNLOAD_TAB    1       // 波形下载页面
#define ANAWAVE_TAB     2       // 波形分析页面
#define SETTING_TAB     3       // 设置页面
#define EXIT_TAB        4       // 退出页面

#define SPIN_UP         (1+0.1) // 放大10%
#define SPIN_DOWN       (1-0.1) // 缩小10%

#endif // GLOBAL_H

