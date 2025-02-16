import cv2

try:
    net = cv2.dnn.readNet("yolov4.cfg", "yolov4.weights")
    print("Model loaded successfully!")
except cv2.error as e:
    print("OpenCV Error:", e)