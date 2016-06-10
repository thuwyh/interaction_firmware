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
|测试震动电机|0x02|1byte
|设置RLD寄存器|0x03+RLD_P+RLD_N|3byte
|配制成噪声测量模式|0x04|1byte
|配制成方波测量模式|0x05|1byte
|配制成正常测量模式|0x06|1byte
|开始采样|0x10|1byte
|停止采样|0x11|1byte
|读寄存器|0x20 + addr|2byte
|写寄存器|0x40 + addr + value|3byte
|重启|0x80|1byte