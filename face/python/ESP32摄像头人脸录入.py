
import cv2 as cv
import socket
import io
import numpy as np
import os
from PIL import Image

s = socket.socket(socket.AF_INET,socket.SOCK_DGRAM,0)
s.bind(("0.0.0.0",9090))

flag = 1
num = 7
while True:
    data, IP = s.recvfrom(100000)
    bytes_stream = io.BytesIO(data)
    image = Image.open(bytes_stream)
    Vshow = np.asarray(image)
    cv.imshow("Capture_test",Vshow)
    k = cv.waitKey(1) & 0xFF
    if k == ord('s'):
        cv.imwrite("img/"+str(num)+".tc"+".jpg",Vshow)
        print("success to save"+str(num)+".jpg")
        print("--------")
        num +=1
    elif k == ord(' '):
        break
cv.destroyAllWindows()