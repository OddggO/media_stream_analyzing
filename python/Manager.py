from MediaStream import MediaStream
from TrtModel import TrtModel
from MessageServer import MessageServer
from enum import Enum
from typing import List
import numpy as np
import json
import logging

class AssignmentType(Enum):
    IMAGE_CLASSIFICATION = "IMAGE_CLASSIFICATION"
    IMAGE_MULTI_CLASSIFICATION = "MULTI_IMAGE_CLASSIFICATION"
    IMAGE_DETECTION = "IMAGE_DETECTION"

class AssignmentAnalyze:
    @staticmethod
    def IMAGE_CLASSIFICATION(output, class_name: List[str], risk_cls_idx: List[int]):
        predict_cls_idx = np.max(output)
        result = {}
        if predict_cls_idx in risk_cls_idx:
            result["predict_cls_idx"] = predict_cls_idx
            result["predict_cls_name"] = class_name[predict_cls_idx]
            
        return result
    
    @staticmethod
    def IMAGE_MULTI_CLASSIFICATION(output, class_name: List[str], risk_cls_idx: List[int]):
        pass
    
    @staticmethod
    def IMAGE_DETECTION(output, class_name: List[str], risk_cls_idx: List[int]):
        pass

class Manager:
    def __init__(self, logger: logging.Logger, config_file: str):
        print("init Manager")
        if logger is None:
            exit(-1)
        self._logger = logger
        self._logger.debug("init Manager")
        self._logger.log(1, "init Manager")
        if config_file is None:
            return
        stream_analyzers = []
        with open(config_file, 'r') as f:
            configs = json.load(f)
        for config in configs:
            if config is None or not self._check_config_legal(config):
                continue

            if config["assignment_type"] == AssignmentType.IMAGE_CLASSIFICATION.value:
                analyze_func = AssignmentAnalyze.IMAGE_CLASSIFICATION
            elif config["assignment_type"] == AssignmentType.IMAGE_MULTI_CLASSIFICATION.value:
                analyze_func = AssignmentAnalyze.IMAGE_MULTI_CLASSIFICATION
            elif config["assignment_type"] == AssignmentType.IMAGE_DETECTION.value:
                analyze_func = AssignmentAnalyze.IMAGE_DETECTION
            else:
                continue
            
            stream_analyzer = {}
            stream_analyzer["config_file"] = config_file
            stream_analyzer["media_stream"] = MediaStream(logger=self._logger, assignment_name=config['assignment_name'], source_type=config["source_type"], 
                                                          source=config['source'], destination_url=config['destination_url'], 
                                                          max_frame_size=config["max_frame_size"])
            stream_analyzer["trt_model"] = TrtModel(logger=self._logger, engine_path=config["engine_path"], 
                                                    media_stream=stream_analyzer["media_stream"], class_names=config["class_names"], risk_cls_idx=config["risk_cls_idx"], 
                                                    analyze_func=analyze_func, max_result_size=config["max_result_size"])
            stream_analyzer["media_stream"].start()
            stream_analyzer["trt_model"].start()
            stream_analyzers.append(stream_analyzer)

        trt_models = [stream_analyzer["trt_model"]  for stream_analyzer in stream_analyzers]
        message_server = MessageServer(self._logger, "0.0.0.0", 12020, 2, trtModels=trt_models)
        message_server.start()
        



                
    def _check_config_legal(self, config) -> bool:
        return True

