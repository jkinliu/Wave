#ifndef PROTOCOL_H
#define PROTOCOL_H

#ifdef __cpluscplus
extern "C" {
#endif

#include <stdint.h>

/**
 * v0.2.2
 *
 * @brief	烈度仪与上位机通讯协议
 *			大端字节序
 *			LEN=0则为查询指令
 * +----+-------+-------+-------+-------+
 * | 7E | CMD   | LEN   | DATA  | 7E    |
 * +----+-------+-------+-------+-------+
 * | 1  | 1     | 2     | n		| 1     |
 * +----+-------+-------+-------+-------+
 *
 * 转义
 *		0x7E -> 0x7D 0x02
 *		0x7D -> 0x7D 0x01
 *
 *
 * @brief	波形数据
 * +--------+-----------+-------+-------+-------+-------+
 * | time   | time_use	|  ud   | ew    | sn    | ...   |
 * +--------+-----------+-------+-------+-------+-------+
 * | uint32 | uint32	| float | float | float	| ...   |
 * +--------+-----------+-------+-------+-------+-------+
 *
 *
 * @brief	CMD_SAMPLE
 *			采样率
 * +--------+
 * | freq	|
 * +--------+
 * | int	|
 * +--------+
 *
 *
 * @brief	CMD_IIR
 *			滤波器参数
 * +--------+-----------+
 * | pass1	| pass2		|
 * +--------+-----------+
 * | float	| float		|
 * +--------+-----------+
 *
 *
 * @brief	CMD_CLOCK
 *			时钟源
 * +----------------+-----------+
 * | type	| ip	| interval	|
 * +----------------+-----------+
 * | int	| uint32| int		|
 * +----------------+-----------+
 *	type
 *		0 -- TIME_SOURCE_NONE
 *		1 -- TIME_SOURCE_RTC
 *		2 -- TIME_SOURCE_NTP
 *		3 -- TIME_SOURCE_BD
 *		4 -- TIME_SOURCE_IRIG
 *
 *
 * @brief	CMD_TRIGGLE
 *			触发方式
 * +--------+-------+-------+-----------+-----------+---------------+---------------+
 * |type    | sta	| lta	| sta_lta_h	| sta_lta_l	| intersity_h	| intersity_l	|
 * +--------+-------+-------+-----------+-----------+---------------+---------------+
 * | int	| int	| float	| float		| float		| float			| float			|
 * +--------+-------+-------+-----------+-----------+---------------+---------------+
 *	type
 *		0 -- EVENT_NONE
 *		1 -- EVENT_INTENSITY
 *		2 -- EVENT_STA_LTA
 *
 *
 * @brief	CMD_ADJUST
 *			校正参数
 * +--------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+
 * | ud_ud	| ud_ew		| ud_sn		| ew_ud		| ew_ew		| ew_sn		| sn_ud		| sn_ew		| sn_sn		|
 * +--------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+
 * | float	| float		| float		| float		| float		| float		| float		| float		| float		|
 * +--------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+-----------+
 *
 *
 * @brief	CMD_EXPORT
 *			导出终端文件
 *          not use
 * +--------+
 * | type	|
 * +--------+
 * | int	|
 * +--------+
 *	type
 *		1 -- log
 *		2 -- sac
 *
 *
 * @brief	CMD_SAVE
 *			事件存储
 * +--------+-----------+-----------+
 * | type	| pre_time	| aft_time	|
 * +--------+-----------+-----------+
 * | uint8	| int		| int		|
 * +--------+-----------+-----------+
 * type
 *		存储波形类型
 *		0 -- RAW
 *		1 -- A
 *		2 -- V
 *		3 -- D
 * pre_time
 *		震前时长
 * aft_time
 *		震后时长
 *
 *
 * @brief	CMD_REBOOT
 *			重启
 *          无参数
 * +--------+
 * | None   |
 * +--------+
 *
 *
 * @brief   CMD_IP
 *          设置IP
 * +--------+-------+-----------+
 * | ip     | mask  | gateway   |
 * +--------+-------+-----------+
 * | uint32 | uint32| uint32    |
 * +--------+-------+-----------+
 *
 *
 * @brief   CMD_HEARTBEET
 * 			心跳包
 *          无参数
 * +--------+
 * | None   |
 * +--------+
 *
 *
 * @brief	CMD_SERVER
 *			设置服务器IP和端口
 * +------------+-----------+
 * | master ip	| slave ip	|
 * +------------+-----------+
 * | uint32		| uint32	|
 * +------------+-----------+
 *
 *
 * @brief	CMD_INSTALL
 *			设置安装信息
 * +--------+-------+-----------+-----------+-----------+---------------+
 * | mode	| angle	| longitude	| latitude	| height	|  field		|
 * +--------+-------+-----------+-----------+-----------+---------------+
 * | uint8	| float | float		| float		| float		| char[32]		|
 * +--------+-------+-----------+-----------+-----------+---------------+
 * mode     安装类型
 *		0 -- 水平
 *		1 -- 垂直
 * angle    安装方位
 *		以北顺时针方向角度
 * longitude	经度
 * latitude		纬度
 * field		场地类型
 *
 *
 * @brief	CMD_DEV_ID
 *			设备标识
 * +-----------+-----------+-----------+-----------+
 * | station   | location  | channel   | network   |
 * +-----------+-----------+-----------+-----------+
 * | char[5]   | char[2]   | char[3]   | char[2]   |
 * +-----------+-----------+-----------+-----------+
 * station		台站标识符
 * location		位置标识符
 * channel		通道标识符
 * network		台网编码
 *
 * 
 * @brief	CMD_VERSION
 *			获取版本
 * +--------+-------+
 * | ARM	| DSP	|
 * +--------+-------+
 * | uint32	| uint32|
 * +--------+-------+
 * ARM
 *		[23..16]	major
 *		[15..8]		minor
 *		[7..0]		patch
 * DSP
 *		[23..16]	major
 *		[15..8]		minor
 *		[7..0]		patch
 *
 *
 * @brief   CMD_SERVER_AUTH
 *          认证密码
 * +------------+
 * | PASSWORD   |
 * +------------+
 * | char(32)	|
 * +------------+
 *
 *
 * @brief   CMD_RTC_ADJ
 *          RTC校正
 * +--------+
 * | adjust	|
 * +--------+
 * | int32	|
 * +--------+
 *
 *
 * @brief   CMD_TRANSMODE
 *          传输模式
 * +--------+
 * | mod	|
 * +--------+
 * | int16	|
 * +--------+
 */

#define DATA_LEN_MAX		200

#define PROT_LEN_MIN		5

#define PROT_START			0x7E
#define PROT_END			0x7E

// 波形
#define CMD_ALL_WAVE		0x00
#define CMD_LAW_WAVE		0x01
#define CMD_A_WAVE			0X02
#define CMD_V_WAVE			0x03
#define CMD_D_WAVE			0x04

// 终端配置
#define CMD_SAMPLE			0x10	// 采样率
#define CMD_IIR				0x11	// 滤波器参数
#define CMD_CLOCK			0x12	// 时钟源
#define CMD_TRIGGLE			0x13	// 触发方式
#define CMD_ADJUST			0x14	// 校正参数
#define CMD_EXPORT			0x15	// 导出终端文件
#define CMD_SAVE			0x16	// 数据存储
#define CMD_REBOOT			0x17    // 重启
#define CMD_IP				0x18    // 设置IP
#define CMD_HEARTBEAT		0x19	// 心跳包
#define CMD_SERVER			0x20	// 服务器IP和端口
#define CMD_INSTALL			0x21	// 安装信息
#define CMD_DEV_ID			0x22	// 设备标识
#define CMD_VERSION			0x23	// 版本
#define CMD_SERVER_AUTH     0x24    // 认证密码
#define CMD_RTC_ADJ         0x25	// RTC校正
#define CMD_TRANSMODE       0x26	// 传输模式
#define CMD_SETTIME         0x27    // 时钟设定

// 应答
// not use
#define CMD_ACK				0xE0
#define CMD_NACK			0xE1


#if 1
inline static uint8_t read_byte(uint8_t *p)
{
    return p[0];
}

inline static uint16_t read_word(uint8_t *p)
{
    return (((uint16_t)p[0]<<8)  | p[1]);
}

inline static uint32_t read_dword(uint8_t *p)
{
    return (((uint32_t)p[0]<<24) | ((uint32_t)p[1]<<16) | ((uint32_t)p[2]<<8) | p[3]);
}

inline static float read_float(uint8_t *p)
{
    float *tmp;
    uint32_t val = read_dword(p);

    tmp = (float *)&val;

    return *tmp;
}

inline static void write_byte(uint8_t *p, uint8_t val)
{
    uint8_t *tmp = (uint8_t *)&val;

    p[0] = tmp[0];
}

inline static void write_word(uint8_t *p, uint16_t val)
{
    uint8_t *tmp = (uint8_t *)&val;

    p[0] = tmp[1];
    p[1] = tmp[0];
}

inline static void write_dword(uint8_t *p, uint32_t val)
{
    uint8_t *tmp = (uint8_t *)&val;

    p[0] = tmp[3];
    p[1] = tmp[2];
    p[2] = tmp[1];
    p[3] = tmp[0];
}

inline static void write_float(uint8_t *p, float val)
{
    uint8_t *tmp = (uint8_t *)&val;

    p[0] = tmp[3];
    p[1] = tmp[2];
    p[2] = tmp[1];
    p[3] = tmp[0];
}

typedef struct
{
	uint8_t  start;		// 0x7E
	uint8_t  cmd;
	uint16_t len;
	uint8_t  data[DATA_LEN_MAX];
	uint8_t  end;		// 0x7E
} prot_t;

typedef void (*prot_recv_cb)(prot_t *prot);

typedef int  (*prot_send_cb)(uint8_t *buff, int n);

/**
 * @brief	协议解析
 */
void prot_parse(uint8_t *buff, int n, prot_recv_cb cb);

/**
 * @brief	发送协议
 */
int prot_send(uint8_t cmd, uint8_t *data, int len, prot_send_cb cb);
#endif


#ifdef __cpluscplus
}
#endif

#endif
