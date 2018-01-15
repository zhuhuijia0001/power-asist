
#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#define MAX_PACKET_LEN              40

//header 
#define PACKET_HEADER               0xA5

//protocol version
#define PROTOCOL_VERSION            1

//data type
#define TYPE_ENABLE_TRANSFER        1
#define TYPE_TRANSFER_DATA          2
#define TYPE_ENABLE_QC20            3
#define TYPE_ENABLE_QC30            4
#define TYPE_ENABLE_PD              5
#define TYPE_TRIGGER_QUICK_CHARGE   6
#define TYPE_AUTO_DETECT            7
#define TYPE_SET_TIME               8
#define TYPE_QUERY_PARAM            9
#define TYPE_SET_SAMPLE             10
#define TYPE_SET_BLE_NAME           11
#define TYPE_SET_PEAK_DURATION      12
#define TYPE_QUERY_CHARGE_MODE      13

//packet data len
#define SIZE_ENABLE_TRANSFER        2
#define SIZE_ENABLE_QUICK_CHARGE    2
#define SIZE_ENABLE_TRIGGER         3
#define SIZE_SET_TIME               7
#define SIZE_SET_SAMPLE             2
#define SIZE_SET_BLE_NAME           21
#define SIZE_SET_PEAK_DURATION      2


//data bitmap
#define DATA_VOLTAGE_POS            0
#define DATA_VOLTAGE_MASK           (1 << 0)
#define DATA_CURRENT_POS            1
#define DATA_CURRENT_MASK           (1 << 1)
#define DATA_POWER_POS              2
#define DATA_POWER_MASK             (1 << 2)
#define DATA_WH_POS                 3
#define DATA_WH_MASK                (1 << 3)
#define DATA_AH_POS                 4
#define DATA_AH_MASK                (1 << 4)
#define DATA_DP_POS                 5
#define DATA_DP_MASK                (1 << 5)
#define DATA_DM_POS                 6
#define DATA_DM_MASK                (1 << 6)

//mode 
#define MODE_NORMAL                 1
#define MODE_APPLE_1A               2
#define MODE_APPLE_2_1A             3
#define MODE_QC20                   4
#define MODE_QC30                   5
#define MODE_PD                     6
#define MODE_DCP                    7

//voltage
#define QC20_5V                     5
#define QC20_9V                     9
#define QC20_12V                    12
#define QC20_20V                    20

//enable / disable
#define QUICK_CHARGE_ENABLE         1
#define QUICK_CHARGE_DISABLE        0

#define PD_ENABLE                   1
#define PD_DISABLE                  0

//support
#define MODE_SUPPORT                1
#define MODE_UNSUPPORT              0

//result
#define RESULT_OK                   0
#define RESULT_FAILED               1

//qc3.0 operation
#define QC30_INCREASE               1
#define QC30_DECREASE               0

//support/not support
#define QUICK_CHARGE_SUPPORTED      1
#define QUICK_CHARGE_NOT_SUPPORTED  0

#endif

