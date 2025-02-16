import tensorflow as tf
import numpy as np
import cv2
from yolov3_tf2.models import YoloV3
from yolov3_tf2.dataset import transform_images

# Load YOLOv3 model
yolo = YoloV3(classes=80)  # COCO has 80 classes
yolo.load_weights("./yolo/yolov3.tf")  # Load converted TensorFlow model
print("YOLOv3 Model Loaded Successfully!")

# Load class names
def load_class_names(filepath):
    with open(filepath, "r") as f:
        return [line.strip() for line in f.readlines()]

class_names = load_class_names("yolo/coco.names")

# Load and preprocess image
def load_image(img_path):
    img_raw = tf.image.decode_image(open(img_path, "rb").read(), channels=3)
    img = tf.image.resize(img_raw, (416, 416)) / 255.0  # Resize and normalize
    return np.expand_dims(img, 0), img_raw

# Detect objects
def detect_objects(img_path):
    img, img_raw = load_image(img_path)
    boxes, scores, classes, nums = yolo(img)
    
    for i in range(nums[0]):
        x1, y1, x2, y2 = boxes[0][i].numpy()
        class_id = int(classes[0][i].numpy())
        confidence = scores[0][i].numpy()

        print(f"Detected {class_names[class_id]} with {confidence:.2f} confidence")

        # Draw bounding box
        cv2.rectangle(img_raw.numpy(), (int(x1), int(y1)), (int(x2), int(y2)), (255, 0, 0), 2)
        cv2.putText(img_raw.numpy(), f"{class_names[class_id]} {confidence:.2f}", 
                    (int(x1), int(y1) - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 0), 2)
    
    cv2.imshow("YOLOv3 Detection", img_raw.numpy())
    cv2.waitKey(0)
    cv2.destroyAllWindows()

# Run detection
detect_objects("test.jpg")  # Change to your image file