# STM32F1\_SSD1306\_IIC

STM32F1/StdPeriph_Lib 标准固件库/OLED/SSD1306 IIC/DMA方式/支持16x16 GBK汉字/移植u8g2

[toc]

## 介绍

这是一个基于STM32F10x及其IIC接口和DMA控制器设计的SSD1306驱动的显示屏驱动代码，支持16x16 GBK汉字，并移植了u8g2第三方图形库，使用StdPeriph_Lib（标准固件库）编程。默认显示屏大小为128x64，并提供了测试样例。在默认的12864上，传输帧率可达43FPS，更小的屏幕意味着更少的数据量，传输帧率更高，实际帧率还会受渲染速度、刷新率等因素的影响。

注释和文档正在更新中

## 1. 快速上手 
### 1.1 克隆、硬件连接、编译
使用git命令下载本代码及子模块，u8g2的子模块c-periphery等可以不下载
```
git clone --recursive https://github.com/cnZhiran/STM32F1_SSD1306_IIC.git
```
如果你没有安装git，下载压缩包并解压。再点击 **[u8g2](https://github.com/olikraus/u8g2)** 下载压缩包，解压至u8g2\文件夹下。

本代码默认`PB6--SCL` `PB7--SDA` 。如果您使用杜邦线，请注意连接的可靠性
```
 _______________
/    stm32f1    \    ___________
|               |   /  SSD1306  \
|           PB6 |---| SCL       |
|           PB7 |---| SDA       |
|               |   \___________/
\_______________/
``` 
使用keil5打开项目文件，就可直接编译，注意ARM编译器版本为V5。

其他集成开发环境正在更新，请移步第二章

## 2. 移植到您的项目
### 2.1 不移植u8g2库
如果您不需要移植u8g2，那么您只需要复制`Driver\src\oled.c` `Driver\inc\oled.h` `Driver\inc\font.h`三个文件到您的项目中。

接着将`oled.h`第41行的宏定义
```
#define SUPPORT_U8G2
```
注释掉

如果您需要计算传输帧率，您需要使用定时器做一个随系统时间增长的计数器，并修改`oled.h`第59行到61行的宏定义。您可以参考`User\main.c` 和 `User\ctrl.c`。

### 2.2 移植u8g2库
在完成2.1的基础上，将u8g2库中的`u8g2\csrc\ `下的源文件和头文件添加到项目中。为了减少程序的大小，根据您的需要选择要添加的文件。

其中，必要的文件有

1. `u8g2_`开头的源文件，这是u8g2绘制图形的模块
2. `u8x8_d_ssd1306_128x64_noname.c`，u8x8_d_开头的源文件，只需要这一个，内部记录着ssd1306及同类屏幕的初始化及屏幕信息。
3. `u8x8_`开头的源文件，不包括u8x8_d_开头的源文件，这是u8g2控制总线并通过页写更新屏幕的模块。

您可以调用测试用例函数来查看移植成功与否。

### 2.3 u8g2库的其他功能
#### 显示utf-8编码的中文
网上关于这一功能的资料极多，本文不再陈述。
#### u8log功能

### 2.4 定制化编译选项
`#define OLED_IICx` OLED使用的IIC接口，可用`I2C1`和`I2C2`

`#define OLED_IIC_GPIOx` OLED使用的GPIO端口，默认`GPIOB`

`#define OLED_IIC_PIN` OLED使用的GPIO引脚，默认`GPIO_Pin_6|GPIO_Pin_7`

`#define OLED_IIC_REMAP` 用于引脚重定义，默认不启用，若启用，内容为`GPIO_Remap_I2C1`

`#define OLED_DMA1_Channel6` 使用I2C1时，OLED使用的DMA及其通道

`#define OLED_DMA1_Channel4` 使用I2C2时，OLED使用的DMA及其通道
 
`#define OLED_SIZE_X` OLED的屏幕宽度，单位像素，默认`128`

`#define OLED_SIZE_Y` OLED的屏幕宽度，单位像素，默认`64`

`#define OLED_PAGE_NUM` OLED的页数，默认`OLED_SIZE_Y/8`

`#define USE_GRAM` 启用图形缓存，以及它的基本控制和绘制函数

`#define USE_PAGE` 启用页写函数（非DMA方式）

`#define SUPPORT_U8G2` 启用u8g2第三方图形库

`#define USE_GRAM_DRAW_BASE_FUNCTION` 使用图形缓存基本绘制函数

`#define OLED_COMPUT_TRANS_FPS` 计算传输帧率

`#define USE_GRAM_EXAMPLE` 启用图形缓存的绘制测试用例

`#define OLED_SYSTIME_VARIABLE`	系统时间计数变量名称，默认`times`

`#define OLED_SYSTIME_PERIOD` 计数一次的周期，单位微秒us，默认`10`

`#define OLED_SYSTIME_SIZE` 计数变量的范围，默认`0`，因为`UINT32_MAX+1`等价于`0`

## 3. 编程资料
### 3.1 原理介绍
![原理图](./doc/原理图.png "原理介绍")
使用DMA接口控制OLED，可以极大的解放CPU资源，节省的资源能够增强CPU渲染或轮询的速率和灵敏度。

如图所示，通过将u8g2的回调函数指针，都指向一个空的回调函数，本程序实现了u8g2库的移植。空的回调函数什么事情也不做，这意味着u8g2对OLED的所有控制都是无效的，包括页写、初始化等。u8g2能做的，只有在图形缓存中绘制图形，包括文字。
### 3.2 API介绍
#### 初始化函数
`void OLED_Init(void)` 初始化

`void OLED_Display_On(void)` 打开屏幕

#### 页写入函数
`void OLED_ShowString(uint8_t page, uint8_t col, char *str)` 将字符串转化的字模，向SSD1306按照页写入

#### 显存绘制函数
`void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t mode)` 在显存中任意一个位置画一个点，可以点亮、熄灭、翻转

`void OLED_DrawString(uint8_t x, uint8_t y, char *str, uint8_t size, uint8_t mode)` 在显存中任意一个位置画字符串

#### 显存操作函数
`int OLED_Refresh_Gram(int sync);` 以DMA方式，向SSD1306更新一次缓存，`sync`控制是否等待更新结束

`int OLED_Continuous_Refresh_Start(void)` 开启DMA方式的连续显存更新

#### u8g2库支持函数
`u8g2_t *OLED_U8G2_Init(void);` u8g2库初始化，提供一个u8g2接口必需的u8g2_t型的结构体

#### 测试用例函数
`void OLED_Frame_Example(void)` 帧率测试用例，有4个不同帧率的数字向右移动，通过观察流畅度判断屏幕的实际帧率

`void OLED_Simulated_Noise_Example(void)` 模拟噪声用例，显示测量的传输帧率，并通过观察噪声分析CPU资源的利用率

`void OLED_CNZH_Example(void)` 中文测试用例，包含作者的母校名称和github名称

`void OLED_U8G2_Draw_Example(u8g2_t *u8g2)` u8g2的官方测试用例，检查u8g2库是否移植成功

`void OLED_Example_Loop(u8g2_t *u8g2)`或`void OLED_Example_Loop(void)` 混合测试用例，每隔5秒，顺序切换上述测试用例

## 4. 编程建议
### 4.1 抑制画面撕裂的方法