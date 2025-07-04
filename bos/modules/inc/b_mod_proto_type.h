/**
 *!
 * \file        b_mod_proto_type.h
 * \version     v0.1.0
 * \date        2020/03/15
 * \author      Bean(notrynohigh@outlook.com)
 *******************************************************************************
 * @attention
 *
 * Copyright (c) 2020 Bean
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *******************************************************************************
 */
#ifndef __B_MOD_PROTO_TYPE_H__
#define __B_MOD_PROTO_TYPE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*Includes ----------------------------------------------*/
#include <stdint.h>

#include "b_section.h"
#include "utils/inc/b_util_list.h"

typedef struct
{
    uint8_t  seq;
    uint8_t *dat;
    uint16_t dat_len;
} bXYModemCbParam_t;

typedef struct
{
    uint8_t  slave_addr;
    uint16_t base_reg;      // Little endian
    uint16_t reg_value[1];  // Little endian
} bModbusMasterWriteReg_t;

typedef struct
{
    uint8_t  slave_addr;
    uint16_t base_reg;  // Little endian
    uint16_t reg_num;   // Little endian
    uint8_t  reserved[4];
    uint16_t reg_value[1];  // Little endian
} bModbusMasterWriteRegs_t;

typedef struct
{
    uint8_t  slave_addr;
    uint8_t  reserved;
    uint16_t base_reg;  // Little endian
    uint16_t reg_num;   // Little endian
} bModbusMasterRead_t;

typedef struct
{
    uint8_t   slave_id;
    uint8_t   func_code;
    uint16_t  base_reg;   // Little endian
    uint16_t  reg_num;    // Little endian
    uint16_t *reg_value;  // Little endian
} bModbusCbParm_t;

typedef struct
{
    uint8_t  name[64];
    uint32_t size;
    uint32_t fcrc32;
} bProtoFileInfo_t;

typedef struct
{
    uint32_t dev;
    uint32_t offset;
} bProtoFileLocation_t;

typedef struct
{
    uint32_t offset;
    uint32_t size;
} bProtoReqFileData_t;

typedef struct
{
    uint32_t offset;
    uint32_t size;
    uint8_t *dat;
} bProtoFileData_t;

typedef struct
{
    uint32_t utc;
    float    timezone;
} bProtoUtc_t;

typedef struct
{
    uint8_t sn_len;
    uint8_t sn[64];
} bProtoSN_t;

typedef struct
{
    uint8_t version[32];
    uint8_t name[32];
} bProtoDevInfo_t;

typedef struct
{
    uint8_t type;  // 配网方式 1字节  AP配网（0）  BLE配网（1）
    uint8_t ssid[32];
    uint8_t passwd[64];
} bProtoCfgNetMode_t;

typedef struct
{
    uint8_t  ssid[32];  // 连接的热点名
    uint32_t ip;        // 例如：uint32_t ip = 0xC0A80101（对应 192.168.1.1）：
    uint32_t gw;
    uint32_t mask;
} bProtoNetInfo_t;

typedef enum
{
    VOICE_STAT_IDLE,
    VOICE_STAT_LISTEN,
    VOICE_STAT_PLAY
} bProtoVoiceState_t;

#define B_PROTO_TRANS_RESULT_OK 0
#define B_PROTO_TRANS_RESULT_CHECKSUM_FAIL 1
#define B_PROTO_TRANS_RESULT_NAME_ERROR 2
#define B_PROTO_TRANS_RESULT_LEN_INVALID 3
#define B_PROTO_TRANS_RESULT_TIMEOUT 4

typedef enum
{
    B_XYMODEM_CMD_START,  // package [null],
    B_XYMODEM_CMD_STOP,   // package [null]
    B_XYMODEM_DATA,       // callback [bXYModemCbParam_t]

    B_MODBUS_CMD_READ_REG,    // pakage [bModbusMasterRead_t], callback [bModbusCbParm_t]
    B_MODBUS_CMD_WRITE_REG,   // pakage [bModbusMasterWriteRegs_t], callback [bModbusCbParm_t]
    B_MODBUS_CMD_WRITE_REGS,  // pakage [bModbusMasterWriteRegs_t], callback [bModbusCbParm_t]

    B_PROTO_TRANS_FILE_INFO,    // callback [bProtoFileInfo_t]
    B_PROTO_OTA_FILE_INFO,      // callback [bProtoFileInfo_t]
    B_PROTO_SET_FILE_LOCATION,  // callback [bProtoFileLocation_t]
    B_PROTO_REQ_FILE_DATA,      // package [bProtoReqFileData_t]
    B_PROTO_FILE_DATA,          // callback [bProtoFileData_t]
    B_PROTO_TRANS_FILE_RESULT,  // package [uint8_t]  B_PROTO_TRANS_RESULT_xxxx
    B_PROTO_UTC,                // callback [bProtoUtc_t]
    B_PROTO_WRITE_SN,           // callback [bProtoSN_t]
    B_PROTO_DEVICEINFO,         // callback [bProtoDevInfo_t]

    B_PROTO_SETCFGNET_MODE,  // 设置进入配网模式 pakage [bProtoCfgNetMode_t]
    B_PROTO_GET_NETINFO,     // 获取网络信息 callback [bProtoNetInfo_t]

    B_PROTO_SET_VOICE_SWITCH,  // 设置语音开关 pakage [uint8_t]
    B_PROTO_SET_VOICE_VOLUME,  // 设置语音音量 pakage [uint8_t]
    B_PROTO_GET_VOICE_VOLUME,  // 获取语音音量 callback [uint8_t]
    B_PROTO_GET_VOICE_STATE,   // 获取语音状态 callback [uint8_t]  @ref bProtoVoiceState_t
    B_PROTO_TTS_CONTENT,       // 获取语音状态 callback [char *]

    B_PROTO_TSL_INVOKE,  // 物模型调用方法 callback [char *]

    B_PROTO_CMD_NUMBER,
} bProtoCmd_t;

typedef enum
{
    B_PROTO_INFO_DEVICE_ID,  // 获取设备id, uint8_t , uint16_t, uint32_t都有可能，依赖具体场景
    B_PROTO_INFO_MODBUS_REG_PERMISSION,  // 获取modbus寄存器的读写权限 bModbusPerm_t *
    B_PROTO_INFO_MODBUS_SLAVE_ADDR,      // 获取当前设备的从机地址  uint8_t
    B_PROTO_INFO_MODBUS_REG_VALUE,       // 获取modbus寄存器的值（做从机时使用）
                                         // in: 寄存器地址  out:寄存器值 共用buf
    B_PROTO_INFO_MCU_UID,    // 获取MCU UID，放数据至buf： UID长度(1byte)UID内容(nbytes)
    B_PROTO_INFO_DEVICE_SN,  // 设备SN，放数据至buf： SN长度(1byte)SN内容(nbytes)
    B_PROTO_INFO_DEVICE_VERSION,  // 设备版本 char *
    B_PROTO_INFO_DEVICE_NAME,     // 设备型号（名称） char *

    B_PROTO_INFO_GET_SSID,  // 获取设备当前连接的SSID char *
    B_PROTO_INFO_GET_IP,    // 获取设备的IP   uint32_t
    B_PROTO_INFO_GET_GW,    // 获取设备的网关 uint32_t
    B_PROTO_INFO_GET_MASK,  // 获取设备的掩码 uint32_t

    B_PROTO_INFO_GET_VOICE_VOLUME,  // 获取语音音量 uint8_t
    B_PROTO_INFO_GET_VOICE_STAT,    // 获取语音状态 uint8_t @ref bProtoVoiceState_t

    B_PROTO_INFO_NUMBER,
} bProtoInfoType_t;

typedef int (*bProtoUserCallback_t)(bProtoCmd_t cmd, void *param);
typedef int (*bProtoCallback_t)(bProtoCmd_t cmd, void *param, void *arg);
typedef int (*bProtoParse_t)(void *attr, uint8_t *in, uint16_t i_len, uint8_t *out, uint16_t o_len);
typedef int (*bProtoPackage_t)(void *attr, bProtoCmd_t cmd, uint8_t *buf, uint16_t buf_len);
typedef int (*bProtoGetInfo_t)(bProtoInfoType_t type, uint8_t *buf, uint16_t buf_len);

typedef struct
{
    uint32_t             reserved;
    bProtoParse_t        parse;
    bProtoPackage_t      package;
    bProtoGetInfo_t      get_info;
    bProtoUserCallback_t u_callback;
    bProtoCallback_t     callback;
    void                *arg;
    struct list_head     list;
} bProtocolAttr_t;

typedef struct
{
    const char     *name;
    bProtoParse_t   parse;
    bProtoPackage_t package;
} bProtocolInstance_t;

#define bPROTOCOL_REG_INSTANCE(proto_name, _parse, _package)                                     \
    bSECTION_ITEM_REGISTER_FLASH(b_srv_protocol, bProtocolInstance_t, CONCAT_2(do_, _parse)) = { \
        .name = proto_name, .parse = _parse, .package = _package};

#ifdef __cplusplus
}
#endif

#endif

/************************ Copyright (c) 2019 Bean *****END OF FILE****/
