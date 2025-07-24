import socket
import network
import camera
import time
import machine
 
#连接wifi
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
if not wlan.isconnected():
    print('connecting to network...')
    wlan.connect('DESKTOP', '01010101')
    
    while not wlan.isconnected():
        pass
print('网络配置:', wlan.ifconfig())

 
#摄像头初始化
try:
    camera.init(0, format=camera.JPEG)
except Exception as e:
    camera.deinit()
    camera.init(0, format=camera.JPEG)
 
 
#图像设置----------------------------  

camera.flip(0)
# left / right
camera.mirror(1)
 
# framesize
camera.framesize(camera.FRAME_VGA)

camera.speffect(camera.EFFECT_NONE)

camera.whitebalance(camera.WB_NONE)

camera.saturation(0)

camera.brightness(0)

camera.contrast(0)

camera.quality(10)

 
#socket UDP 的创建
s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM,0)

while True:
    buf = camera.capture() # 获取图像数据
  # 向服务器发送图像数据
    s.sendto(buf,("192.168.0.107",9090)) # 
    time.sleep(0.1)
 
 
 
