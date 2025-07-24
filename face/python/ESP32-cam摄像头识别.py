import cv2
import socket
import io
import numpy as np
import os
from PIL import Image
import serial
# coding=utf-8
import urllib
import urllib.request
import hashlib

aa = 0

#加载训练数据集文件
recogizer=cv2.face.LBPHFaceRecognizer_create()
recogizer.read('trainer/trainer.yml')
names=[]

s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM,0)
s.bind(("0.0.0.0",9090))

def face_detect_demo(img):
    global aa
    img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
    gray=cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)#转换为灰度
    face_detector = cv2.CascadeClassifier('C:/Users/16275/AppData/Local/Programs/Python/Python39/Lib/site-packages/cv2/data/haarcascade_frontalface_alt2.xml')
    face=face_detector.detectMultiScale(gray,1.1,5,cv2.CASCADE_SCALE_IMAGE,(100,100),(300,300))
    for x,y,w,h in face:
        cv2.rectangle(img,(x,y),(x+w,y+h),color=(0,0,255),thickness=2)
        cv2.circle(img,center=(x+w//2,y+h//2),radius=w//2,color=(0,255,0),thickness=1)
        # 人脸识别
        ids, confidence = recogizer.predict(gray[y:y + h, x:x + w])
        #print('标签id:',ids,'置信评分：', confidence) 置信度评分用来衡量所识别人脸与原模型的差距，0 表示完全匹配。
        if confidence > 50:
            cv2.putText(img, 'unkonw', (x + 10, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 255, 0), 1)
            print("FFFFFF")
        else:
            cv2.putText(img,str(names[ids-1]), (x + 10, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.75, (0, 255, 0), 1)
            s.sendto(b'3', ("192.168.43.228", 9090))
            print("YYYYYYY")
            try:
                port = "COM7"
                bps = 9600
                timeout = 5
                # 打开串口，并返回串口对象
                uart = serial.Serial(port, bps, timeout=timeout)

                # 要发送的数据（十六进制）
                hex_data = "ffabcdfe"
                # 将十六进制数据转换为字节数据
                data = bytes.fromhex(hex_data)

                # 串口发送数据
                length = uart.write(data)
                print("发送数据长度：", length)

                # 关闭串口
                uart.close()

            except Exception as result:
                print("******错误******：", result)
    cv2.imshow('result',img)

    aa += 1

def name():
    path = './img/'
    imagePaths=[os.path.join(path,f) for f in os.listdir(path)]
    for imagePath in imagePaths:
       name = str(os.path.split(imagePath)[1].split('.',2)[1])
       names.append(name)

name()
while True:
    data, IP = s.recvfrom(100000)
    bytes_stream = io.BytesIO(data)
    image = Image.open(bytes_stream)
    img = np.asarray(image)
    face_detect_demo(img)
    if ord(' ') == cv2.waitKey(10):
        break
cv2.destroyAllWindows()
