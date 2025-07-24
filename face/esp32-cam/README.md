# ESP32-CAM 摄像头模块使用说明

## 📷 模块概述

ESP32-CAM是本项目的图像采集核心，负责实时采集摄像头图像并通过WiFi网络传输到Python人脸识别服务。

## 🔧 硬件规格

### ESP32-CAM开发板
- **主控芯片**: ESP32-S (双核32位LX6处理器)
- **运行频率**: 240MHz
- **内存**: 520KB SRAM + 4MB PSRAM
- **存储**: 4MB Flash
- **WiFi**: 802.11 b/g/n (2.4GHz)
- **蓝牙**: 支持BLE 4.2

### OV2640摄像头
- **分辨率**: 最大2MP (1600x1200)
- **输出格式**: JPEG, RGB565, YUV422
- **帧率**: 最大60fps (QVGA), 15fps (UXGA)
- **视角**: 66°
- **焦距**: 固定焦距，约1米最佳

## 📁 文件说明

### camera.py
ESP32-CAM的MicroPython控制程序，主要功能：
- WiFi网络连接
- 摄像头初始化和参数配置
- 图像采集和UDP传输
- 网络状态监控

## 🚀 快速开始

### 1. 硬件准备
```
所需硬件：
├── ESP32-CAM开发板 x1
├── USB转串口模块 x1 (CH340/CP2102)
├── 杜邦线若干
├── 5V电源适配器
└── MicroUSB数据线
```

### 2. 连接方式

#### 烧录模式连接
```
ESP32-CAM    USB转串口
VCC    ──────  5V
GND    ──────  GND
U0R    ──────  TX
U0T    ──────  RX
IO0    ──────  GND (烧录时接地)
```

#### 运行模式连接
```
ESP32-CAM    电源
VCC    ──────  5V
GND    ──────  GND
注意：IO0悬空或接高电平
```

### 3. 固件烧录

#### 方法一：使用Arduino IDE
1. 安装ESP32开发板支持包
2. 选择开发板："AI Thinker ESP32-CAM"
3. 上传MicroPython固件

#### 方法二：使用esptool
```bash
# 安装esptool
pip install esptool

# 擦除Flash
esptool.py --chip esp32 --port COM3 erase_flash

# 烧录MicroPython固件
esptool.py --chip esp32 --port COM3 --baud 460800 write_flash -z 0x1000 esp32-20210902-v1.17.bin
```

### 4. 程序上传
```bash
# 使用ampy工具上传文件
pip install adafruit-ampy
ampy --port COM3 put camera.py main.py
```

## ⚙️ 配置说明

### WiFi配置
修改`camera.py`中的WiFi连接参数：
```python
# WiFi配置
WIFI_SSID = 'YOUR_WIFI_NAME'        # 替换为你的WiFi名称
WIFI_PASSWORD = 'YOUR_WIFI_PASSWORD' # 替换为你的WiFi密码

# 连接WiFi
wlan.connect(WIFI_SSID, WIFI_PASSWORD)
```

### 网络配置
修改目标服务器IP地址：
```python
# 目标服务器配置
SERVER_IP = "192.168.0.107"  # Python服务运行的电脑IP
UDP_PORT = 9090              # UDP通信端口

# 发送图像数据
s.sendto(buf, (SERVER_IP, UDP_PORT))
```

### 摄像头参数配置
```python
# 摄像头基本设置
camera.flip(0)              # 垂直翻转: 0=不翻转, 1=翻转
camera.mirror(1)            # 水平镜像: 0=不镜像, 1=镜像

# 图像质量设置
camera.framesize(camera.FRAME_VGA)    # 分辨率设置
camera.quality(10)                    # 图像质量: 0-63 (数值越小质量越高)

# 图像效果设置
camera.brightness(0)        # 亮度: -2 到 +2
camera.contrast(0)          # 对比度: -2 到 +2
camera.saturation(0)        # 饱和度: -2 到 +2

# 白平衡设置
camera.whitebalance(camera.WB_NONE)   # 白平衡模式

# 特效设置
camera.speffect(camera.EFFECT_NONE)   # 特效: NONE, NEG, BW, RED, GREEN, BLUE, RETRO
```

## 📊 分辨率选项

| 分辨率选项 | 像素尺寸 | 说明 | 推荐用途 |
|-----------|----------|------|----------|
| FRAME_96X96 | 96x96 | 最小分辨率 | 测试用 |
| FRAME_QQVGA | 160x120 | 极低分辨率 | 高速传输 |
| FRAME_QCIF | 176x144 | 低分辨率 | 节省带宽 |
| FRAME_HQVGA | 240x176 | 中低分辨率 | 平衡性能 |
| FRAME_240X240 | 240x240 | 方形分辨率 | 特殊应用 |
| FRAME_QVGA | 320x240 | 标准低分辨率 | 快速识别 |
| FRAME_CIF | 400x296 | 中等分辨率 | 一般应用 |
| FRAME_HVGA | 480x320 | 中等分辨率 | 平衡质量 |
| **FRAME_VGA** | **640x480** | **标准分辨率** | **推荐使用** |
| FRAME_SVGA | 800x600 | 高分辨率 | 高质量需求 |
| FRAME_XGA | 1024x768 | 超高分辨率 | 最高质量 |
| FRAME_HD | 1280x720 | 高清分辨率 | 专业应用 |
| FRAME_SXGA | 1280x1024 | 超高分辨率 | 特殊需求 |
| FRAME_UXGA | 1600x1200 | 最大分辨率 | 最高质量 |

**推荐设置**: `FRAME_VGA (640x480)` - 在图像质量和传输速度间取得最佳平衡

## 🔍 性能优化

### 传输速度优化
```python
# 降低图像质量提高传输速度
camera.quality(20)  # 增大数值降低质量

# 降低分辨率提高帧率
camera.framesize(camera.FRAME_QVGA)  # 320x240

# 调整传输间隔
time.sleep(0.05)  # 减小间隔提高帧率
```

### 图像质量优化
```python
# 提高图像质量
camera.quality(5)   # 降低数值提高质量

# 调整光照补偿
camera.brightness(1)   # 增加亮度
camera.contrast(1)     # 增加对比度

# 自动白平衡
camera.whitebalance(camera.WB_AUTO)
```

## 🐛 故障排除

### 常见问题及解决方案

#### 1. 无法连接WiFi
**症状**: 设备无法连接到WiFi网络
**解决方案**:
```python
# 检查WiFi配置
print('WiFi SSID:', WIFI_SSID)
print('WiFi Password:', WIFI_PASSWORD)

# 检查网络状态
if wlan.isconnected():
    print('网络已连接:', wlan.ifconfig())
else:
    print('网络连接失败')
```

#### 2. 摄像头初始化失败
**症状**: 摄像头无法正常工作
**解决方案**:
```python
# 重新初始化摄像头
try:
    camera.deinit()
    camera.init(0, format=camera.JPEG)
    print('摄像头初始化成功')
except Exception as e:
    print('摄像头初始化失败:', e)
```

#### 3. 图像传输中断
**症状**: Python端无法接收图像数据
**解决方案**:
- 检查网络连接稳定性
- 确认防火墙设置
- 验证IP地址和端口配置
- 检查UDP缓冲区大小

#### 4. 图像质量差
**症状**: 接收到的图像模糊或有噪点
**解决方案**:
```python
# 调整图像参数
camera.quality(5)      # 提高图像质量
camera.brightness(1)   # 调整亮度
camera.contrast(1)     # 调整对比度

# 改善环境光照
# 确保充足的环境光线
# 避免强光直射摄像头
```

### 调试方法

#### 串口调试
```python
# 添加调试信息
print('系统启动...')
print('WiFi连接状态:', wlan.isconnected())
print('IP地址:', wlan.ifconfig()[0])
print('摄像头状态: 正常')
```

#### 网络测试
```bash
# 在电脑端测试网络连通性
ping ESP32_IP_ADDRESS

# 检查端口是否开放
telnet ESP32_IP_ADDRESS 9090
```

## 📈 扩展功能

### 可添加功能
- [ ] 图像预处理 (降噪、增强)
- [ ] 多分辨率自适应传输
- [ ] 图像压缩算法优化
- [ ] 网络断线重连机制
- [ ] 远程参数配置
- [ ] 状态LED指示
- [ ] 低功耗模式

### 硬件扩展
- [ ] 添加PIR运动传感器
- [ ] 集成补光LED
- [ ] 外接存储卡
- [ ] 电池供电模块

## 🔗 相关链接

- [ESP32-CAM官方文档](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html)
- [MicroPython ESP32文档](https://docs.micropython.org/en/latest/esp32/quickref.html)
- [OV2640摄像头规格书](https://www.ov.com/products/ov2640/)

