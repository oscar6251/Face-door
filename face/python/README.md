# Python 人脸识别程序使用指南

## 🐍 程序概述

Python人脸识别模块是Face-Door系统的核心算法部分，基于OpenCV实现实时人脸检测、识别和训练功能。

## 📁 文件结构

```
python/
├── README.md                    # 📖 本文档
├── ESP32-cam摄像头识别.py        # 🎯 主要识别程序
├── ESP32摄像头人脸录入.py        # 📸 人脸数据采集程序
├── 数据训练.py                  # 🧠 模型训练程序
├── img/                        # 📂 训练图片存储目录
│   ├── user.1.1.jpg           # 用户1的人脸图片
│   ├── user.1.2.jpg
│   ├── user.2.1.jpg           # 用户2的人脸图片
│   └── ...
└── trainer/                   # 🎓 训练模型存储目录
    └── trainer.yml            # 训练完成的LBPH模型文件
```

## 🛠️ 环境配置

### Python版本要求
- **Python 3.9+** (推荐Python 3.9-3.11)

### 依赖包安装
```bash
# 核心依赖
pip install opencv-python==4.8.1.78
pip install opencv-contrib-python==4.8.1.78
pip install numpy==1.24.3
pip install Pillow==10.0.0
pip install pyserial==3.5

# 或使用requirements.txt安装
pip install -r requirements.txt
```

### 系统要求
- **操作系统**: Windows 10/11, Ubuntu 18.04+, macOS 10.14+
- **内存**: 最低4GB RAM (推荐8GB+)
- **摄像头**: ESP32-CAM或USB摄像头
- **网络**: 局域网环境，支持UDP通信

## 📋 程序详解

### 1. ESP32摄像头人脸录入.py

#### 功能说明
- 接收ESP32-CAM传输的实时图像
- 检测图像中的人脸
- 手动保存人脸图片用于训练

#### 使用方法
```bash
python ESP32摄像头人脸录入.py
```

#### 操作说明
1. **启动程序**: 运行后会显示实时视频窗口
2. **人脸检测**: 程序自动检测并标记人脸区域
3. **保存图片**: 按 `s` 键保存当前人脸图片
4. **退出程序**: 按 `空格键` 或 `ESC` 退出

#### 关键参数
```python
# 图片保存设置
save_path = "img/"              # 图片保存目录
image_format = ".jpg"           # 图片格式
user_id = 1                     # 用户ID (手动设置)

# 人脸检测参数
face_detector = cv2.CascadeClassifier(
    'haarcascade_frontalface_alt2.xml'  # Haar分类器路径
)
detectMultiScale(
    gray,           # 灰度图像
    1.1,           # 缩放因子
    5,             # 最小邻居数
    cv2.CASCADE_SCALE_IMAGE,
    (100,100),     # 最小人脸尺寸
    (300,300)      # 最大人脸尺寸
)
```

### 2. 数据训练.py

#### 功能说明
- 读取img目录中的人脸图片
- 使用LBPH算法训练人脸识别模型
- 生成trainer.yml模型文件

#### 使用方法
```bash
python 数据训练.py
```

#### 训练流程
1. **图片读取**: 自动扫描img目录下的所有jpg图片
2. **人脸提取**: 从每张图片中提取人脸区域
3. **特征训练**: 使用LBPH算法训练模型
4. **模型保存**: 将训练结果保存为trainer.yml

#### 图片命名规则
```
img/
├── user.1.1.jpg    # 用户ID=1, 图片序号=1
├── user.1.2.jpg    # 用户ID=1, 图片序号=2
├── user.2.1.jpg    # 用户ID=2, 图片序号=1
└── user.2.2.jpg    # 用户ID=2, 图片序号=2
```

#### 训练参数
```python
# LBPH识别器参数
recognizer = cv2.face.LBPHFaceRecognizer_create(
    radius=1,          # LBP半径
    neighbors=8,       # 邻居数量
    grid_x=8,         # 网格X方向
    grid_y=8,         # 网格Y方向
    threshold=50.0     # 识别阈值
)
```

### 3. ESP32-cam摄像头识别.py

#### 功能说明
- 实时接收ESP32-CAM图像数据
- 加载训练好的人脸识别模型
- 进行人脸检测和身份识别
- 发送控制信号到STM32

#### 使用方法
```bash
python ESP32-cam摄像头识别.py
```

#### 工作流程
1. **模型加载**: 加载trainer/trainer.yml模型文件
2. **网络监听**: 监听UDP端口9090接收图像
3. **人脸检测**: 使用Haar分类器检测人脸
4. **身份识别**: 使用LBPH算法识别人脸身份
5. **结果处理**: 根据识别结果发送控制信号

#### 识别逻辑
```python
# 置信度判断
if confidence > 50:
    # 识别失败
    cv2.putText(img, 'unknown', (x+10, y-10), ...)
    print("识别失败")
else:
    # 识别成功
    cv2.putText(img, str(names[ids-1]), (x+10, y-10), ...)
    # 发送UDP信号
    s.sendto(b'3', ("192.168.43.228", 9090))
    # 发送串口信号
    uart.write(bytes.fromhex("ffabcdfe"))
    print("识别成功")
```

## ⚙️ 配置参数

### 网络配置
```python
# UDP通信设置
UDP_IP = "0.0.0.0"      # 监听所有网络接口
UDP_PORT = 9090         # 监听端口
BUFFER_SIZE = 65536     # 接收缓冲区大小

# 目标设备设置
TARGET_IP = "192.168.43.228"  # STM32设备IP
TARGET_PORT = 9090            # 目标端口
```

### 串口配置
```python
# 串口通信设置
SERIAL_PORT = "COM7"    # Windows串口号
# SERIAL_PORT = "/dev/ttyUSB0"  # Linux串口号
BAUD_RATE = 9600        # 波特率
TIMEOUT = 5             # 超时时间(秒)

# 控制信号
UNLOCK_SIGNAL = "ffabcdfe"  # 开门信号(十六进制)
```

### 识别参数
```python
# 置信度阈值
CONFIDENCE_THRESHOLD = 50   # 小于此值认为识别成功

# 人脸检测参数
SCALE_FACTOR = 1.1         # 图像缩放因子
MIN_NEIGHBORS = 5          # 最小邻居数
MIN_SIZE = (100, 100)      # 最小人脸尺寸
MAX_SIZE = (300, 300)      # 最大人脸尺寸

# 显示参数
FONT = cv2.FONT_HERSHEY_SIMPLEX
FONT_SCALE = 0.75
FONT_COLOR = (0, 255, 0)   # 绿色
FONT_THICKNESS = 1
```

## 🎯 使用流程

### 完整使用流程
```bash
# 1. 人脸数据采集
python ESP32摄像头人脸录入.py
# 操作：对着摄像头，按's'键保存多张人脸图片

# 2. 模型训练
python 数据训练.py
# 自动处理：读取图片，训练模型，保存trainer.yml

# 3. 实时识别
python ESP32-cam摄像头识别.py
# 自动运行：加载模型，识别人脸，发送控制信号
```

### 数据采集建议
- **图片数量**: 每人至少50张，推荐100-200张
- **拍摄角度**: 正面、左侧、右侧、上仰、下俯
- **光照条件**: 明亮、正常、较暗等不同光照
- **表情变化**: 正常、微笑、严肃等不同表情
- **距离变化**: 近距离、中距离、远距离

## 🔧 性能优化

### 识别准确率优化
```python
# 1. 增加训练样本
# 每人采集更多不同角度、光照的图片

# 2. 调整检测参数
face_detector.detectMultiScale(
    gray, 
    1.05,          # 减小缩放因子，提高检测精度
    8,             # 增加最小邻居数，减少误检
    cv2.CASCADE_SCALE_IMAGE,
    (80, 80),      # 减小最小尺寸，检测更小人脸
    (400, 400)     # 增大最大尺寸，检测更大人脸
)

# 3. 调整识别阈值
CONFIDENCE_THRESHOLD = 40  # 降低阈值，提高识别敏感度
```

### 处理速度优化
```python
# 1. 图像预处理
# 调整图像尺寸
img = cv2.resize(img, (320, 240))

# 2. 跳帧处理
frame_count = 0
if frame_count % 2 == 0:  # 每2帧处理一次
    # 进行人脸检测和识别
    pass
frame_count += 1

# 3. 多线程处理
import threading
# 将图像接收和处理分离到不同线程
```

## 🐛 故障排除

### 常见问题

#### 1. 无法接收图像数据
**症状**: Python程序启动后无法显示图像
**解决方案**:
```python
# 检查网络连接
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(("0.0.0.0", 9090))
print("等待数据...")

# 检查防火墙设置
# Windows: 允许Python程序通过防火墙
# Linux: sudo ufw allow 9090
```

#### 2. 人脸检测失败
**症状**: 无法检测到人脸或检测不准确
**解决方案**:
```python
# 检查Haar分类器路径
import cv2
cascade_path = cv2.data.haarcascades + 'haarcascade_frontalface_default.xml'
face_detector = cv2.CascadeClassifier(cascade_path)

# 调整检测参数
faces = face_detector.detectMultiScale(
    gray, 1.1, 3,  # 降低最小邻居数
    cv2.CASCADE_SCALE_IMAGE,
    (50, 50),      # 降低最小尺寸
    (500, 500)     # 增大最大尺寸
)
```

#### 3. 识别准确率低
**症状**: 经常识别错误或无法识别
**解决方案**:
- 增加训练样本数量
- 改善光照条件
- 调整置信度阈值
- 重新训练模型

#### 4. 串口通信失败
**症状**: 无法发送控制信号到STM32
**解决方案**:
```python
# 检查串口配置
import serial.tools.list_ports
ports = serial.tools.list_ports.comports()
for port in ports:
    print(f"可用串口: {port.device}")

# 测试串口连接
try:
    uart = serial.Serial("COM7", 9600, timeout=5)
    uart.write(b"test")
    print("串口连接正常")
except Exception as e:
    print(f"串口连接失败: {e}")
```

## 📊 性能指标

### 系统性能
- **识别准确率**: >95% (充足光照条件)
- **处理延迟**: <2秒 (从接收到识别完成)
- **支持用户数**: 理论无限制
- **图像处理速度**: 10-30 FPS

### 资源占用
- **内存使用**: 约500MB
- **CPU占用**: 20-50% (单核)
- **网络带宽**: 1-5 Mbps
- **存储空间**: 模型文件约1-10MB

## 🔗 扩展功能

### 可添加功能
- [ ] 多人脸同时识别
- [ ] 活体检测防伪
- [ ] 年龄性别识别
- [ ] 表情识别
- [ ] 口罩检测
- [ ] 人脸特征点定位
- [ ] 识别结果数据库存储
- [ ] Web界面管理

### 算法优化
- [ ] 使用深度学习模型 (如FaceNet)
- [ ] 实现人脸对齐预处理
- [ ] 添加光照补偿算法
- [ ] 集成多种识别算法

---

**提示**: 使用前请确保ESP32-CAM正常工作并已连接到同一网络！
