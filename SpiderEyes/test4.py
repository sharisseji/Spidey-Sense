import cv2
import numpy as np
import urllib.request
import requests  # Added for sending data to ESP32
import time

# Camera URL
ESP_IP = "http://172.20.10.4"  # ESP32-CAM IP
url = f"{ESP_IP}/cam-hi.jpg"

# YOLO Parameters
whT = 320
confThreshold = 0.5
nmsThreshold = 0.3

# Load COCO Class Names
classesfile = 'coco.names'
with open(classesfile, 'rt') as f:
    classNames = f.read().rstrip('\n').split('\n')

# Load YOLO Model
modelConfig = 'yolov3.cfg'
modelWeights = 'yolov3.weights'
net = cv2.dnn.readNetFromDarknet(modelConfig, modelWeights)
net.setPreferableBackend(cv2.dnn.DNN_BACKEND_OPENCV)
net.setPreferableTarget(cv2.dnn.DNN_TARGET_CPU)

# Function to send AI result to ESP32-CAM
def send_data(command):
    try:
        response = requests.get(f"{ESP_IP}/send", params={"data": command}, timeout=1)
        print("ESP Response:", response.text)
    except requests.exceptions.RequestException as e:
        print("Failed to send data to ESP32:", e)

# Object Detection Function
def findObject(outputs, im):
    hT, wT, cT = im.shape
    bbox = []
    classIds = []
    confs = []
    found_vehicle = False
    found_person = False
    found_animal = False
    found_obstruction = False

    for output in outputs:
        for det in output:
            scores = det[5:]
            classId = np.argmax(scores)
            confidence = scores[classId]
            if confidence > confThreshold:
                w, h = int(det[2] * wT), int(det[3] * hT)
                x, y = int((det[0] * wT) - w / 2), int((det[1] * hT) - h / 2)
                bbox.append([x, y, w, h])
                classIds.append(classId)
                confs.append(float(confidence))

    indices = cv2.dnn.NMSBoxes(bbox, confs, confThreshold, nmsThreshold)

    if len(indices) > 0:
        for i in indices.flatten():  # Flatten indices for safe looping
            box = bbox[i]
            x, y, w, h = box[0], box[1], box[2], box[3]

            if classNames[classIds[i]] == 'person':
                found_person = True
            elif classNames[classIds[i]] in {'car', 'bus', 'train', 'motorbike', 'bicycle'}:
                found_vehicle = True
            elif classNames[classIds[i]] in {'bird', 'cat', 'dog', 'horse', 'sheep', 'cow', 'elephant', 'bear', 'zebra', 'giraffe'}:
                found_animal = True
            elif classNames[classIds[i]] in {'traffic light', 'fire hydrant', 'stop sign', 'parking meter', 'bench', 'chair', 'sofa', 'pottedplant', 'bed', 'diningtable', 'toilet'}:
                found_obstruction = True


            cv2.rectangle(im, (x, y), (x + w, y + h), (255, 0, 255), 2)
            cv2.putText(im, f'{classNames[classIds[i]].upper()} {int(confs[i] * 100)}%',
                        (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 0, 255), 2)

    # Decide what to send based on detections
    if found_person:
        send_data("PERSON")
    elif found_vehicle:
        send_data("VEHICLE")
    elif found_animal:
        send_data("ANIMAL")
    elif found_obstruction:
        send_data("OBSTRUCTION")
    else:
        send_data("CLEAR")  # Default if nothing detected

totaltime = 0
count = 0

# Main Loop
while True:
    try:
        start_time = time.time()
        img_resp = urllib.request.urlopen(url)
        imgnp = np.array(bytearray(img_resp.read()), dtype=np.uint8)
        im = cv2.imdecode(imgnp, -1)

        if im is None:
            print("Failed to decode image")
            continue

        # Prepare Image for YOLO
        blob = cv2.dnn.blobFromImage(im, 1 / 255.0, (whT, whT), [0, 0, 0], swapRB=True, crop=False)
        net.setInput(blob)

        # Get Output Layer Names
        unconnected_layers = net.getUnconnectedOutLayers()
        if isinstance(unconnected_layers, int):
            unconnected_layers = [unconnected_layers]
        outputNames = [net.getLayerNames()[i - 1] for i in unconnected_layers]

        # Forward Pass
        outputs = net.forward(outputNames)
        findObject(outputs, im)

        cv2.imshow('Image', im)

        end_time = time.time()
        process_time = end_time - start_time
        totaltime += process_time
        count += 1

        if cv2.waitKey(1) & 0xFF == ord('q'):
            print("Average Processing Time:", totaltime / count)
            break

    except Exception as e:
        print(f"Error fetching or processing frame: {e}")

cv2.destroyAllWindows()