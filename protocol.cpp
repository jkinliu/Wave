#include <string.h>
#include "protocol.h"

typedef struct
{
	int n;
    uint8_t data[DATA_LEN_MAX];
} frame_t;

static frame_t frame;
static prot_t  prot;

int get_frame(uint8_t ch)
{
	static int state = 0;
	int ret = -1;

	switch (state)
	{
		case 0:		// start
		{
			if (ch == PROT_START)
			{
				frame.n = 0;

				frame.data[frame.n++] = ch;
				state = 1;
			}

			break;
		}
		case 1:		// data
		{
			if (ch == PROT_END)		// end
			{
				frame.data[frame.n++] = ch;
				state = 0;
				ret = 0;
			}
			else if (ch == 0x7D)
				state = 2;
			else
				frame.data[frame.n++] = ch;

			break;
		}
		case 2:		// 转义
		{
			state = 1;

			if (ch == 0x01)
				frame.data[frame.n++] = 0x7D;
			else if (ch == 0x02)
				frame.data[frame.n++] = 0x7E;
			else
				state = 0;

			break;
		}
	}

	return ret;
}

int is_frame_valid(void)
{
	int ret = -1;

    if (frame.n >= PROT_LEN_MIN)
	{
		if ((frame.data[0] == PROT_START) && (frame.data[frame.n-1] == PROT_END))
		{
			prot.start	= PROT_START;
			prot.end	= PROT_END;

			prot.cmd	= frame.data[1];
            prot.len	= (frame.data[2]<<8) | frame.data[3];

			if (((prot.len + PROT_LEN_MIN) == frame.n) && (prot.len < DATA_LEN_MAX))
			{
				memcpy(prot.data, frame.data+4, prot.len);
				ret = 0;
			}
		}
	}

	return ret;
}

/**
 * @brief	协议解析
 */
void prot_parse(uint8_t *buff, int n, prot_recv_cb cb)
{
	int i;

	for (i=0; i<n; i++)
	{
		if ((get_frame(buff[i])==0) && (is_frame_valid()==0))
		{
			if (cb)
				cb(&prot);
		}
	}
}

/**
 * @brief	发送协议
 */
int prot_send(uint8_t cmd, uint8_t *data, int len, prot_send_cb cb)
{
	int i;
	int n = 0;
	int ret = -1;
	uint8_t tmp;
	uint8_t buff[200] = {0};

	buff[n++] = PROT_START;
	buff[n++] = cmd;

    // len
    tmp = (len >> 8) & 0xFF;
    if (tmp == 0x7D)
    {
        buff[n++] = 0x7D;
        buff[n++] = 0x01;
    }
    else if (tmp == 0x7E)
    {
        buff[n++] = 0x7D;
        buff[n++] = 0x02;
    }
    else
        buff[n++] = tmp;

	// len
	tmp = len & 0xFF;
	if (tmp == 0x7D)
	{
		buff[n++] = 0x7D;
		buff[n++] = 0x01;
	}
	else if (tmp == 0x7E)
	{
		buff[n++] = 0x7D;
		buff[n++] = 0x02;
	}
	else
		buff[n++] = tmp;

	// data
	for (i=0; i<len; i++)
	{
		if (data[i] == 0x7D)
		{
			buff[n++] = 0x7D;
			buff[n++] = 0x01;
		}
		else if (data[i] == 0x7E)
		{
			buff[n++] = 0x7D;
			buff[n++] = 0x02;
		}
		else
			buff[n++] = data[i];
	}

	buff[n++] = PROT_END;

	if (cb)
		ret = cb(buff, n);

	return ret;
}
