# interaction_firmware

手势识别项目单片机端程序


## 数据包格式

|名称|定义|长度|
|-|-|-|
|头| 0xff| 1byte|
|长度| |1byte|
|数据正文|||
|校验|sum(数据正文)，自动溢出|1byte|

## 通信协议