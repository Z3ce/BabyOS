/**
 * \file        b_drv_qmc5883p.c
 * \version     v0.0.1
 * \date        2023-11-06
 * \author      miniminiminini (405553848@qq.com)
 * \brief
 *
 * Copyright (c) 2023 by miniminiminini. All Rights Reserved.
 */

/* Includes ----------------------------------------------*/
#include "drivers/inc/b_drv_qmc5883p.h"

#include <string.h>

#include "utils/inc/b_util_log.h"

/**
 * \defgroup QMC5883P_Private_TypesDefinitions
 * \{
 */

/**
 * }
 */

/**
 * \defgroup QMC5883P_Private_Defines
 * \{
 */
#define DRIVER_NAME QMC5883P

#define QMC5883P_ID 0x80

#define CHIP_ID_REG 0X00
#define MAG_X_REG_L 0X01
#define MAG_X_REG_H 0X02
#define MAG_Y_REG_L 0X03
#define MAG_Y_REG_H 0X04
#define MAG_Z_REG_L 0X05
#define MAG_Z_REG_H 0X06
#define STATUS_REG 0X09
#define CONTROL_1_REG 0X0A
#define CONTROL_2_REG 0X0B

#define MAG_DATA_LEN 6

enum
{
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_Z = 2,

    AXIS_TOTAL
};

typedef struct
{
    signed char   sign[3];
    unsigned char map[3];
} QMC5883P_map;

static QMC5883P_map c_map;

/**
 * }
 */

/**
 * \defgroup QMC5883P_Private_Macros
 * \{
 */
#define U82U16(msb, lsb) ((((msb) & 0xffff) << 8) | (((lsb) & 0xffff)))

/**
 * }
 */

/**
 * \defgroup QMC5883P_Private_Variables
 * \{
 */
bDRIVER_HALIF_TABLE(bQMC5883P_HalIf_t, DRIVER_NAME);

/**
 * }
 */

/**
 * \defgroup QMC5883P_Private_FunctionPrototypes
 * \{
 */

/**
 * }
 */

/**
 * \defgroup QMC5883P_Private_Functions
 * \{
 */

static void qmc5883p_set_layout(int layout)
{
    if (layout == 0)
    {
        c_map.sign[AXIS_X] = 1;
        c_map.sign[AXIS_Y] = 1;
        c_map.sign[AXIS_Z] = 1;
        c_map.map[AXIS_X]  = AXIS_X;
        c_map.map[AXIS_Y]  = AXIS_Y;
        c_map.map[AXIS_Z]  = AXIS_Z;
    }
    else if (layout == 1)
    {
        c_map.sign[AXIS_X] = -1;
        c_map.sign[AXIS_Y] = 1;
        c_map.sign[AXIS_Z] = 1;
        c_map.map[AXIS_X]  = AXIS_Y;
        c_map.map[AXIS_Y]  = AXIS_X;
        c_map.map[AXIS_Z]  = AXIS_Z;
    }
    else if (layout == 2)
    {
        c_map.sign[AXIS_X] = -1;
        c_map.sign[AXIS_Y] = -1;
        c_map.sign[AXIS_Z] = 1;
        c_map.map[AXIS_X]  = AXIS_X;
        c_map.map[AXIS_Y]  = AXIS_Y;
        c_map.map[AXIS_Z]  = AXIS_Z;
    }
    else if (layout == 3)
    {
        c_map.sign[AXIS_X] = 1;
        c_map.sign[AXIS_Y] = -1;
        c_map.sign[AXIS_Z] = 1;
        c_map.map[AXIS_X]  = AXIS_Y;
        c_map.map[AXIS_Y]  = AXIS_X;
        c_map.map[AXIS_Z]  = AXIS_Z;
    }
    else if (layout == 4)
    {
        c_map.sign[AXIS_X] = -1;
        c_map.sign[AXIS_Y] = 1;
        c_map.sign[AXIS_Z] = -1;
        c_map.map[AXIS_X]  = AXIS_X;
        c_map.map[AXIS_Y]  = AXIS_Y;
        c_map.map[AXIS_Z]  = AXIS_Z;
    }
    else if (layout == 5)
    {
        c_map.sign[AXIS_X] = 1;
        c_map.sign[AXIS_Y] = 1;
        c_map.sign[AXIS_Z] = -1;
        c_map.map[AXIS_X]  = AXIS_Y;
        c_map.map[AXIS_Y]  = AXIS_X;
        c_map.map[AXIS_Z]  = AXIS_Z;
    }
    else if (layout == 6)
    {
        c_map.sign[AXIS_X] = 1;
        c_map.sign[AXIS_Y] = -1;
        c_map.sign[AXIS_Z] = -1;
        c_map.map[AXIS_X]  = AXIS_X;
        c_map.map[AXIS_Y]  = AXIS_Y;
        c_map.map[AXIS_Z]  = AXIS_Z;
    }
    else if (layout == 7)
    {
        c_map.sign[AXIS_X] = -1;
        c_map.sign[AXIS_Y] = -1;
        c_map.sign[AXIS_Z] = -1;
        c_map.map[AXIS_X]  = AXIS_Y;
        c_map.map[AXIS_Y]  = AXIS_X;
        c_map.map[AXIS_Z]  = AXIS_Z;
    }
    else
    {
        c_map.sign[AXIS_X] = 1;
        c_map.sign[AXIS_Y] = 1;
        c_map.sign[AXIS_Z] = 1;
        c_map.map[AXIS_X]  = AXIS_X;
        c_map.map[AXIS_Y]  = AXIS_Y;
        c_map.map[AXIS_Z]  = AXIS_Z;
    }
}

/**
 * \brief        读寄存器并且判定是否iic失败
 * \param pdrv
 * \param reg
 * \param data
 * \param len
 * \return int -1:失败,0:正常
 */
static int _bQMC5883PReadCheckRegs(bDriverInterface_t *pdrv, uint8_t reg, uint8_t *data,
                                   uint16_t len)
{
    bDRIVER_GET_HALIF(_if, bQMC5883P_HalIf_t, pdrv);

    if (bHalI2CMemRead(_if, reg, 1, data, len) < 0)
    {
        return -1;
    }

    return len;
}

/**
 * \brief        写寄存器后再读寄存器,看是否配置成功,若iic失败或者写后再读值不一致则返回失败
 * \param pdrv
 * \param reg
 * \param data
 * \param len
 * \return int -1:失败,0:正常
 */
static int _bQMC5883PWriteCheckRegs(bDriverInterface_t *pdrv, uint8_t reg, uint8_t *data,
                                    uint16_t len)
{
    uint8_t read_buf[len];
    bDRIVER_GET_HALIF(_if, bQMC5883P_HalIf_t, pdrv);
    memset(read_buf, 0, sizeof(read_buf));

    if (bHalI2CMemWrite(_if, reg, 1, data, len) < 0)
    {
        return -1;
    }

    if (bHalI2CMemRead(_if, reg, 1, read_buf, len) < 0)
    {
        return -1;
    }

    for (uint16_t i = 0; i < len; i++)
    {
        if (read_buf[i] != data[i])
        {
            return -2;
        }
    }

    return len;
}

static int _bQMC5883PGetID(bDriverInterface_t *pdrv, uint8_t *id)
{
    if (_bQMC5883PReadCheckRegs(pdrv, CHIP_ID_REG, id, 1) < 0)
    {
        return -1;
    }
    // b_log("QMC5883P id:0x%x\n", id);
    return 0;
}

static int _bQMC5883PDefaultCfg(bDriverInterface_t *pdrv)
{
    uint8_t control_0x0a_reg_val = 0x00;
    uint8_t control_0x0b_reg_val = 0x00;
    uint8_t control_0x0d_reg_val = 0x00;
    uint8_t control_0x29_reg_val = 0x00;

    control_0x0b_reg_val = 0x80;
    if (_bQMC5883PWriteCheckRegs(pdrv, 0x0b, &control_0x0b_reg_val, 1) < 0)
    {
        return -1;
    }

    bHalDelayMs(5);

    control_0x0b_reg_val = 0x00;
    if (_bQMC5883PWriteCheckRegs(pdrv, 0x0b, &control_0x0b_reg_val, 1) < 0)
    {
        return -1;
    }

    bHalDelayMs(5);

    qmc5883p_set_layout(3);

    control_0x0a_reg_val = 0;
    if (_bQMC5883PWriteCheckRegs(pdrv, 0x0a, &control_0x0a_reg_val, 1) < 0)
    {
        return -1;
    }

    bHalDelayMs(1);

    control_0x0d_reg_val = 0x40;
    if (_bQMC5883PWriteCheckRegs(pdrv, 0x0d, &control_0x0d_reg_val, 1) < 0)
    {
        return -1;
    }
    control_0x29_reg_val = 0x06;
    if (_bQMC5883PWriteCheckRegs(pdrv, 0x29, &control_0x29_reg_val, 1) < 0)
    {
        return -1;
    }
    //+-8G
    control_0x0b_reg_val = 0x08;
    if (_bQMC5883PWriteCheckRegs(pdrv, 0x0b, &control_0x0b_reg_val, 1) < 0)
    {
        return -1;
    }
    // Normal Mode,ODR = 200Hz,
    control_0x0a_reg_val = 0xcd;
    if (_bQMC5883PWriteCheckRegs(pdrv, 0x0a, &control_0x0a_reg_val, 1) < 0)
    {
        return -1;
    }

    bHalDelayMs(1);

    return 0;
}

static int _bQMC5883PRead(bDriverInterface_t *pdrv, uint32_t off, uint8_t *pbuf, uint32_t len)
{
    float              hw_d[3] = {0};
    float              raw_c[3];
    uint8_t            mag_data[6];
    bQMC5883P_3Axis_t *ptmp = (bQMC5883P_3Axis_t *)pbuf;

    if (len < sizeof(bQMC5883P_3Axis_t))
    {
        return -1;
    }

    if (_bQMC5883PReadCheckRegs(pdrv, MAG_X_REG_L, &mag_data[0], MAG_DATA_LEN) < 0)
    {
        return -1;
    }

    hw_d[0] = (float)((short)(U82U16(mag_data[1], mag_data[0])));
    hw_d[1] = (float)((short)(U82U16(mag_data[3], mag_data[2])));
    hw_d[2] = (float)((short)(U82U16(mag_data[5], mag_data[4])));

    // Unit:mG  1G = 100uT = 1000mG
    // printf("Hx=%d, Hy=%d, Hz=%d\n",hw_d[0],hw_d[1],hw_d[2]);
    raw_c[AXIS_X] = (int)(c_map.sign[AXIS_X] * hw_d[c_map.map[AXIS_X]]);
    raw_c[AXIS_Y] = (int)(c_map.sign[AXIS_Y] * hw_d[c_map.map[AXIS_Y]]);
    raw_c[AXIS_Z] = (int)(c_map.sign[AXIS_Z] * hw_d[c_map.map[AXIS_Z]]);

    ptmp->mag_arr[0] = (float)raw_c[0] / 37.5f;
    ptmp->mag_arr[1] = (float)raw_c[1] / 37.5f;
    ptmp->mag_arr[2] = (float)raw_c[2] / 37.5f;

    return 0;
}

/**
 * \brief
 * \param pdrv
 * \param cmd
 * \param param
 * \return int
 * -1代表期间iic错误
 * 0在bCMD_QMC5883P_WHETHER_NEWDATA_READY下表示有数据
 * 1在bCMD_QMC5883P_WHETHER_NEWDATA_READY下表示无数据
 */
static int _bQMC5883PCtl(struct bDriverIf *pdrv, uint8_t cmd, void *param)
{
    int     retval   = -1;
    uint8_t read_dat = 0;
    switch (cmd)
    {
        case bCMD_QMC5883P_WHETHER_NEWDATA_READY:
        {
            if (_bQMC5883PReadCheckRegs(pdrv, STATUS_REG, &read_dat, 1) < 0)
            {
                return -1;
            }

            if (read_dat & 0x01)
            {
                retval = QMC5883P_NEWDATA_READY;
            }
            else
            {
                retval = QMC5883P_NEWDATA_NOT_READY;
            }
        }
        break;

        case bCMD_QMC5883P_SET_STATUS_ERR:
        {
            pdrv->status = -1;
            retval       = 0;
        }
        break;

        default:
            retval = -1;
            break;
    }
    return retval;
}
/**
 * }
 */

/**
 * \defgroup QMC5883P_Exported_Functions
 * \{
 */
// int bQMC5883P_Init_0(bDriverInterface_t *pdrv)
// {
//     _bQMC5883PSOFTRESET(pdrv);
//     bHalDelayMs(50);
//     return 0;
// }

int bQMC5883P_Init(bDriverInterface_t *pdrv)
{
    uint8_t id = 0;
    bDRIVER_STRUCT_INIT(pdrv, DRIVER_NAME, bQMC5883P_Init);
    pdrv->read = _bQMC5883PRead;
    pdrv->ctl  = _bQMC5883PCtl;

    if (_bQMC5883PGetID(pdrv, &id) < 0)
    {
        return -1;
    }

    if (id != QMC5883P_ID)
    {
        return -1;
    }

    if (_bQMC5883PDefaultCfg(pdrv) < 0)
    {
        return -1;
    }

    return 0;
}

// bDRIVER_REG_INIT_0(B_DRIVER_QMC5883P, bQMC5883P_Init_0);
bDRIVER_REG_INIT(B_DRIVER_QMC5883P, bQMC5883P_Init);

/**
 * }
 */

/**
 * }
 */

/**
 * }
 */

/**
 * }
 */

/***** Copyright (c) 2023 miniminiminini *****END OF FILE*****/
