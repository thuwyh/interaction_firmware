# interaction_firmware

手势识别项目单片机端程序


## 数据包格式

### 主--->从

|名称|定义|长度|
|-----|-----|-----|
|头| 0xff| 1byte|
|长度| |1byte|
|数据正文|command+data|可变长度|
|校验|sum(数据正文)，自动溢出|1byte|

### 从--->主

|名称|定义|长度|
|-----|-----|-----|
|头| 0xff 0xff| 2byte|
|长度| |1byte|
|数据类型|0x01 for emg数据; 0x02 for imu数据; 0x03 命令回复|1byte|
|回复正文|见下表|可变长度|
|校验|sum(回复正文)，自动溢出|1byte|


## Commands

|名称|格式|长度|回复正文|
|-----|-----|-----|-----|
|握手|0x00|1byte|字符串|
|进入AT|0x01|1byte|none|
|测试震动电机|0x02|1byte|none|
|设置RLD寄存器|0x03+RLD_P+RLD_N|3byte|1byte 0xff or 0xee|
|配制成噪声测量模式|0x04|1byte|1byte 0xff or 0xee|
|配制成方波测量模式|0x05|1byte|1byte 0xff or 0xee|
|配制成正常测量模式|0x06|1byte|1byte 0xff or 0xee|
|开始采样|0x10|1byte|none|
|停止采样|0x11|1byte|none|
|读寄存器|0x20 + addr|2byte|1byte 寄存器值|
|写寄存器|0x40 + addr + value|3byte|1byte 寄存器值|
|重启|0x80|1byte|1byte 0xff or 0xee|
|emg数据|3byte为一个通道，前3byte为状态数据|27byte||
|imu数据|第1byte为地址，后接16byte为数据，4byte一个浮点数，顺序为xyzw|17byte||


## Changelog

###2016.6.11

- 完善命令应答，覆盖常用所有命令
- 修改尝试机制，避免死循环

