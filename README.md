# interaction_firmware

手势识别项目单片机端程序


## 数据包格式

|名称|定义|长度|
|-----|-----|-----|
|头| 0xff| 1byte|
|长度| |1byte|
|数据正文|command+data|可变长度|
|校验|sum(数据正文)，自动溢出|1byte|

## Commands

|名称|格式|长度|
|-----|-----|-----|
|握手|0x00|1byte
|进入AT|0x01|1byte
|开始采样|0x10|1byte
|停止采样|0x11|1byte
|读寄存器|0x20 + addr|2byte
|写寄存器|0x40 + addr + value|3byte
|重启|0x80|1byte