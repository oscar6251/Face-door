# Face-Door 智能门禁系统

## 项目简介

Face-Door是一个基于STM32F103C8微控制器的多功能智能门禁系统，集成了RFID卡片识别、指纹识别、密码验证、蓝牙通信和人脸识别等多种身份验证方式。系统采用模块化设计。

## 主要特性

- 🔐 **多重身份验证**：支持RFID卡片、密码、蓝牙、人脸识别四种验证方式
- 📱 **手机APP控制**：通过蓝牙与手机APP通信
- 👤 **人脸识别**：基于ESP32-CAM和OpenCV的实时人脸识别
- 🖥️ **中文显示界面**：OLED屏幕显示中文操作界面
- 🔧 **硬件控制**：舵机开门、LED状态指示、蜂鸣器提示
- 💾 **数据存储**：Flash存储密码和配置信息

## 系统架构

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   手机APP       │    │   ESP32-CAM     │    │   Python服务    │
│   (蓝牙控制)    │    │   (图像采集)    │    │   (人脸识别)    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         │ 蓝牙                  │ WiFi/UDP              │ 串口
         │                       │                       │
┌─────────────────────────────────────────────────────────────────┐
│                    STM32F103C8 主控制器                        │
├─────────────────────────────────────────────────────────────────┤
│  RFID(RC522) │ 指纹(AS608) │ 矩阵按键 │ OLED显示 │ 舵机控制  │
└─────────────────────────────────────────────────────────────────┘
```

## 硬件清单

### 主控制器
- STM32F103C8T6 微控制器

### 传感器模块
- RC522 RFID读卡器模块
- AS608 指纹识别模块
- ESP32-CAM 摄像头模块

### 显示与交互
- 0.96寸 OLED显示屏 (I2C接口)
- 4x4 矩阵按键

### 执行器件
- SG90 舵机 (门锁控制)
- LED指示灯 (红/绿)
- 无源蜂鸣器

### 通信模块
- HC-05/HC-06 蓝牙模块
- USB转串口模块 (调试用)


### 依赖库
#### STM32端
- STM32F10x标准外设库
- 自定义硬件驱动库

#### Python端
```bash
pip install opencv-python
pip install numpy
pip install pyserial
pip install pillow
```

## 项目结构

```
Face-door/
├── README.md          # 📖 项目主要说明文档
├── HARDWARE_SETUP.md  # 🔌 硬件连接详细指南
├── CHANGELOG.md       # 📝 版本更新日志
├── LICENSE            # ⚖️ MIT开源许可证
├── requirements.txt   # 📦 Python依赖包列表
├── Hardware/          # 硬件驱动模块
│   ├── rc522.c/h      # RFID读卡器驱动
│   ├── as608.c/h      # 指纹模块驱动
│   ├── oled_iic.c/h   # OLED显示驱动
│   ├── SERVO.c/h      # 舵机控制
│   ├── LED.c/h        # LED控制
│   ├── Beep.c/h       # 蜂鸣器控制
│   ├── Key1.c/h       # 矩阵按键
│   ├── keyCode.c/h    # 密码处理
│   ├── Serial.c/h     # 串口通信
│   └── flash.c/h      # Flash存储
├── User/              # 用户应用代码
│   ├── main.c         # 主程序
│   ├── stm32f10x_it.c # 中断处理
│   └── stm32f10x_conf.h # 配置文件
├── System/            # 系统功能
│   ├── Delay.c/h      # 延时函数
│   ├── sys.c/h        # 系统初始化
│   └── timer.c/h      # 定时器
├── Library/           # STM32标准库
├── Start/             # 启动文件
├── face/              # 人脸识别系统
│   ├── README.md      # 🎯 人脸识别模块说明
│   ├── esp32-cam/     # ESP32-CAM代码
│   │   ├── README.md  # 📷 ESP32-CAM使用说明
│   │   └── camera.py  # 摄像头控制
│   └── python/        # Python识别程序
│       ├── README.md  # 🐍 Python程序使用指南
│       ├── ESP32-cam摄像头识别.py
│       ├── ESP32摄像头人脸录入.py
│       ├── 数据训练.py
│       ├── img/       # 训练图片
│       └── trainer/   # 训练模型
├── Objects/           # 编译输出
├── Listings/          # 编译列表
└── Project.uvprojx    # Keil工程文件
```

## 📚 文档说明

本项目包含多个详细的说明文档，帮助您快速理解和使用系统：

### 主要文档
- **[README.md](README.md)** - 项目主要说明文档（当前文档）
  - 项目概述和功能介绍
  - 快速开始指南
  - 基本配置说明

### 硬件相关
- **[HARDWARE_SETUP.md](HARDWARE_SETUP.md)** - 🔌 **硬件连接详细指南**
  - 完整的引脚分配表
  - 详细的连接示意图
  - 电源要求和供电方案

### 软件依赖
- **[requirements.txt](requirements.txt)** - 📦 Python依赖包列表
  - 所有必需的Python包及版本
  - 安装命令和注意事项

### 人脸识别模块
- **[face/README.md](face/README.md)** - 🎯 **人脸识别系统总体说明**
  - 完整的系统架构图和工作流程
  - ESP32-CAM与Python服务通信协议
  - 技术规格和性能指标
  - 配置参数和部署指南

- **[face/esp32-cam/README.md](face/esp32-cam/README.md)** - 📷 **ESP32-CAM详细使用说明**
  - 硬件规格和连接方式
  - MicroPython固件烧录步骤
  - WiFi网络配置和摄像头参数调节
  - 分辨率选项和性能优化

- **[face/python/README.md](face/python/README.md)** - 🐍 **Python程序完整指南**
  - 详细的环境配置和依赖安装
  - 三个核心程序的使用方法
  - 人脸数据采集和模型训练流程
  - 识别参数调优和性能优化


### 📖 阅读建议

1. 📖 README.md（当前文档）- 了解项目概况
2. 🔌 HARDWARE_SETUP.md - 学习硬件连接
3. 🎯 face/README.md - 理解人脸识别系统
4. 📷 face/esp32-cam/README.md - 配置ESP32-CAM
5. 🐍 face/python/README.md - 设置Python环境


## 快速开始

### 1. 硬件连接

#### STM32F103C8引脚连接
```
OLED显示屏:
- VCC -> 3.3V
- GND -> GND  
- SCL -> PB6
- SDA -> PB7

RC522 RFID:
- VCC -> 3.3V
- GND -> GND
- SCK -> PA5
- MOSI -> PA7
- MISO -> PA6
- NSS -> PA4
- RST -> PA3

舵机:
- VCC -> 5V
- GND -> GND
- PWM -> PA1

LED指示灯:
- LED1(红) -> PA8
- LED2(绿) -> PA9

蜂鸣器:
- 正极 -> PA2
- 负极 -> GND
```

### 2. 软件配置

#### STM32程序烧录
1. 打开Keil uVision 5
2. 加载 `Project.uvprojx` 工程文件
3. 编译项目 (F7)
4. 连接ST-Link调试器
5. 下载程序到STM32 (F8)

#### ESP32-CAM配置
1. 修改 `face/esp32-cam/camera.py` 中的WiFi配置
```python
wlan.connect('你的WiFi名称', '你的WiFi密码')
```
2. 修改目标IP地址为运行Python程序的电脑IP
```python
s.sendto(buf,("你的电脑IP",9090))
```

#### Python环境配置
1. 安装依赖库
```bash
pip install -r requirements.txt
```
2. 修改 `ESP32-cam摄像头识别.py` 中的串口配置
```python
port = "COM7"  # 修改为实际串口号
```

### 3. 系统使用

#### 操作界面
系统启动后OLED显示主菜单：
- **K1 蓝牙** - 手机APP控制模式
- **K3 刷卡** - RFID卡片识别模式  
- **K6 密码** - 数字密码输入模式
- ***键** - 返回主菜单

#### 验证流程
1. **RFID模式**：将卡片靠近读卡器，系统自动识别
2. **密码模式**：按C键进入密码输入，输入6位数字密码，按B键确认
3. **蓝牙模式**：手机APP发送验证码 `0xAB 0xCD`
4. **人脸识别**：启动Python程序，ESP32-CAM自动采集并识别

#### 验证成功
- 绿色LED亮起
- 舵机转动180°开门
- 5秒后自动关门复位

#### 验证失败  
- 红色LED亮起
- 蜂鸣器报警
- 1.5秒后复位

## 配置说明

### RFID卡片配置
在 `main.c` 中修改预设卡片UID：
```c
unsigned char Card_0[4]= {0x97,0x2D,0xC0,0xA7}; // 白卡
unsigned char Card_1[4]= {0x19,0x18,0x91,0x98}; // 蓝卡
```

### 默认密码配置
在 `keyCode.c` 中修改默认密码：
```c
unsigned char Password2[7] = {1, 1, 1, 1, 1, 1, 0}; // 默认密码111111
```

### 蓝牙验证码配置
在 `main.c` 中修改蓝牙验证码：
```c
unsigned char Blu[2]= {0xab,0xcd}; // 蓝牙验证码
```

## 人脸识别训练

### 1. 人脸数据采集
```bash
python ESP32摄像头人脸录入.py
```
- 程序会自动采集人脸图片
- 图片保存在 `img/` 目录下
- 建议每人采集50-100张不同角度的照片

### 2. 模型训练
```bash
python 数据训练.py
```
- 训练完成后生成 `trainer/trainer.yml` 模型文件

### 3. 实时识别
```bash
python ESP32-cam摄像头识别.py
```

## 故障排除

### 常见问题

**Q: OLED显示屏无显示**
A: 检查I2C连接线，确认SCL/SDA引脚连接正确

**Q: RFID读卡无响应**  
A: 检查SPI连接，确认RC522模块供电正常

**Q: 舵机不转动**
A: 检查PWM信号线连接，确认舵机供电充足(5V)

**Q: 人脸识别无法连接**
A: 检查ESP32-CAM的WiFi连接和IP地址配置

**Q: 串口通信失败**
A: 检查波特率设置(9600)，确认串口号正确

### 调试方法

1. **串口调试**：通过串口1输出调试信息
2. **LED指示**：观察LED状态判断系统运行情况  
3. **OLED显示**：查看屏幕显示的状态信息

## 扩展功能

### 可扩展模块
- [ ] WiFi联网功能
- [ ] 远程监控
- [ ] 访问记录存储
- [ ] 多用户管理
- [ ] 语音提示
- [ ] 门磁检测

### 二次开发
项目采用模块化设计，便于添加新功能：
1. 在 `Hardware/` 目录添加新的硬件驱动
2. 在 `main.c` 中集成新功能
3. 更新菜单系统和用户界面

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 贡献

欢迎提交Issue和Pull Request来改进项目！

## 📖 完整文档索引

为了帮助您更好地使用本项目，我们提供了详细的分类文档：

1. 📖 [README.md](README.md) - 了解项目概况
2. 🔌 [HARDWARE_SETUP.md](HARDWARE_SETUP.md) - 学习硬件连接
3. 🎯 [face/README.md](face/README.md) - 理解人脸识别系统
4. 📷 [face/esp32-cam/README.md](face/esp32-cam/README.md) - 配置摄像头
5. 🐍 [face/python/README.md](face/python/README.md) - 运行识别程序


