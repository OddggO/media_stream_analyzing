import cv2
import numpy as np
from Manager import Manager
import logging
import time
# 配置日志格式和级别
logging.basicConfig(
level=logging.DEBUG,
format="%(asctime)s - %(name)s - %(levelname)s - %(message)s"
)
if __name__ == "__main__":
    logger = logging.getLogger(__name__)
    manager: Manager = Manager(logger=logger, config_file="./config.json")
    time.sleep(1000)






# from TrtModel import TrtModel
# import requests
# from concurrent.futures import ThreadPoolExecutor
# # 飞机、汽车、鸟类、猫、鹿、狗、蛙、马、船和卡车
# class_names = [
#     "airplane", "automobile", "bird", "cat", "deer",
#     "dog", "frog", "horse", "ship", "truck"
# ]

# if __name__ == "__main__":
#     cap = cv2.VideoCapture("C:\\myfiles\\projects\\cpp\\audio_video_learning\\BXC_RtspServer_study-master-original\\BXC_RtspServer_study-master\\data\\test.mp4")
#     trtModule = TrtModule("C:\\myfiles\\projects\\cpp\\cpp_learning\\nn\inference\\trt\\trt_test\\lenet.engine")
    
#     while cap.isOpened():
#         frame = cap.read()
#         # TODO 判断FRAME是不是RGB格式
#         frame_resize = TrtModule.preprocess(frame)
#         output = trtModule.inference(frame_resize)
#         print(f"输出向量: {output}")
#         predict_cls = np.argmax(output)
#         predict_cls_name = class_names[predict_cls]
#         print(f"预测类别: {predict_cls}, 类别名称: {predict_cls_name}")
#         if predict_cls == 5:
#             response_data = {
#                 "predict_cls": predict_cls, 
#                 "predict_cls_name": predict_cls_name
#             }
#             response = requests.post("http://xxxx", data=response_data)
#             # 打印响应内容
#             print(response.text)
        
        