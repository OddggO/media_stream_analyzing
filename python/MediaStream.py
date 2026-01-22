from enum import Enum
import cv2
import queue
import logging
import threading
import queue

class MediaStream:
    # 静态成员变量定义
    DEFATULT_MAX_FRAME_SIZE: int = 50 
    
    class SourceType(Enum):
        FILE = "FILE"
        RTSP = "RTSP"
        RTMP = "RTMP"
        
    # class AssignmentType(Enum):
    #     IMAGE_CLASSIFICATION = 1
    #     MULTI_IMAGE_CLASSIFICATION = 2
        
    def __init__(self, logger: logging.Logger, assignment_name: str, source_type: str, 
                        source: str, # assignment_type: AssignmentType
                        destination_url: str, 
                        max_frame_size: int = DEFATULT_MAX_FRAME_SIZE):
        # 参数检查
        if logger is None:
            print("logger is None")
            exit(-1)
        self.logger = logger
        self.__alive = False
        # if source_type == MediaStream.SourceType.FILE.value:
        #     self.source_type = MediaStream.SourceType.FILE
        # elif source_type == MediaStream.SourceType.RTSP.value:
        #     self.source_type = MediaStream.SourceType.RTSP
        # elif source_type == MediaStream.SourceType.RTMP.value:
        #     self.source_type = MediaStream.SourceType.RTMP
        # else:
        #     self.logger.error("invalid source_type")
        self.source_type = source_type
        # if assignment_type not in MediaStream.AssignmentType:
        #     self.logger.error("invalid assignment_type")
        #     return
        
        # 成员变量初始化
        self.assignment_name = assignment_name
        self.source = source
        self.destination_url = destination_url
        # self.assignment_type = assignment_type
        if max_frame_size < 0:
            self.logger.warning(f"invalid max_frame_size={max_frame_size}, set max_frame_size={MediaStream.DEFATULT_MAX_FRAME_SIZE}")
            self.max_frame_size = MediaStream.DEFATULT_MAX_FRAME_SIZE
        else:
            self.max_frame_size = max_frame_size
        self.frame_queue = queue.Queue(self.max_frame_size)
        
        # 打开媒体流
        self.cap = cv2.VideoCapture(source)
        if not self.cap.isOpened():
            logging.warning(f"opencv {source} failed")
            print(f"opencv {source} failed")
            return
        
        self.thread = threading.Thread(target=self._loop, daemon=True)
        
    def read_one_frame(self):
        if self.cap.isOpened():
            return self.cap.read()
        else:
            self.__alive = False
            return None
        
    def _loop(self):
        # 改为定时调用？
        while self.__alive:
            _, frame = self.read_one_frame()
            # print(f"read one frmae, {frame.size}")
            # self.logger.info(f"read one frmae, {frame.size}")
            self.frame_queue.put(frame) # 阻塞直到有空位

        self.stop()
            
    def start(self):
        self.__alive = True
        self.thread.start()
        
    def stop(self):
        self.__alive = False
        
    def get_frame(self, timeout = None):
        frame = None
        try:
            frame = self.frame_queue.get(timeout=timeout)
        except queue.Empty:
            self.logger.warning(f"timeout with {timeout}, return None")
        return frame
    
    def add_stream_info(self, result):
        result["assignment_name"] = self.assignment_name
        result["source_type"] = self.source_type
        result["source"] = self.source
        result["destination_url"] = self.destination_url
        
    
if __name__ == "__main__":
    tmp = MediaStream.SourceType.FILE
    print(tmp)