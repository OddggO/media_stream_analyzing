import cv2
import numpy as np
import tensorrt as trt
# import pycuda.driver as cuda # pip install pycuda
# import pycuda.autoinit # 自动初始化CUDA， 不import的话需要手动初始化，否则会报错
import queue
import threading
import logging
from MediaStream import MediaStream
from typing import List

TRT_LOGGER = trt.Logger(trt.Logger.INFO)

class TrtModel:
    DEFAULT_MAX_RESULT_SIZE = 100
    
    def __init__(self, logger: logging.Logger, engine_path: str, media_stream: MediaStream, 
                 class_names: List[str], risk_cls_idx: List[int], analyze_func, 
                 max_result_size: int = DEFAULT_MAX_RESULT_SIZE):
        if logger is None:
            exit(-1)
        self._logger = logger
        if media_stream is None:
            self._logger.error("media_stream is None")
            return
        self.media_stream = media_stream
        self.__alive = False
        self.engine_path = engine_path
        self.risk_cls_idx = risk_cls_idx
        # self.__initialize_trt()
        # if trt.volume(self.output_shape) != len(class_names):
        #     self._logger.error("input_shape and class_names not match")
        #     return
        self.__class_names = class_names
        if analyze_func is None:
            self._logger.error("analyze_func is None")
            return
        self.analyze_func = analyze_func
        if max_result_size < 0:
            self._logger.warning("invalid max_result_size={max_result_size}, set default value={DEFAULT_MAX_RESULT_SIZE}")
            self.max_result_size = TrtModel.DEFAULT_MAX_RESULT_SIZE
        else:
            self.max_result_size = max_result_size
        self.result_queue = queue.Queue(maxsize=self.max_result_size)
        self.thread = threading.Thread(target=self._loop, daemon=True)
        
    def __initialize_trt(self):
        with open(self.engine_path, "rb") as f:
            # runtime, 创建Trt运行时
            runtime = trt.Runtime(TRT_LOGGER)
            # engine, 推理引擎，从优化过的可执行模型文件中加载engine，不能跨显卡，如3060导出的engine在2060上运行通常会报错
            self.engine = runtime.deserialize_cuda_engine(f.read()) # 加载 engine
        # context, 执行模型的上下文
        self.context = self.engine.create_execution_context()     

        # bingding, 输入和输出的slot
        # binding是trt的重要概念，每个bingding对应一个tensor输入或输出，binding的顺序和模型定义时的顺序一致
        self.input_shape = self.engine.get_binding_shape(0)
        self.output_shape = self.engine.get_binding_shape(1)

        # allocate cuda memory
        self.d_input = self.cuda.mem_alloc(trt.volume(self.input_shape) * np.float32().nbytes) #计算输入tensor的大小并分配显存
        self.d_output = self.cuda.mem_alloc(trt.volume(self.output_shape) * np.float32().nbytes)
        self.bindings = [int(self.d_input), int(self.d_output)]

    def inference(self, img):
        # copy input
        self.cuda.memcpy_htod(self.d_input, img)
        self.context.execute_v2(self.bindings)
        # self.context.execute_async_v2(self.bindings) # 异步执行，需要stream.synchronize()
        # self.context.executte() # 旧API，7.X以前的API
        # self.context.executte_async() # 旧API，7.X以前的API
        
        # copy output
        output = np.empty(self.output_shape, dtype=np.float32)
        self.cuda.memcpy_dtoh(output, self.d_output)
        return output
    
    def _loop(self):
        import pycuda.driver as cuda
        cuda.init()
        device = cuda.Device(0)
        self.cuda = cuda
        self.cuda_ctx = device.make_context()   # 关键：为当前线程创建 context
        self.__initialize_trt()
        if self.analyze_func is None:
            self.__alive = False
            return
        while self.__alive:
            frame = self.media_stream.get_frame()
            if frame is None:
                self._logger.error("get none from media_stream")
                continue
            frame_resize = TrtModel.preprocess(frame)
            output = self.inference(frame_resize)
            # self._logger.debug(f"output: {output}")
            result = self.analyze_func(output, self.__class_names, self.risk_cls_idx)
            self.media_stream.add_stream_info(result=result) # 添加媒体流相关信息
            self.result_queue.put(result)
            
    def start(self):
        self.__alive = True
        self.thread.start()
        
    def stop(self):
        self.__alive = False
        
    def get_result(self, timeout = None):
        result = None
        try:
            result = self.result_queue.get(timeout=timeout)
        except queue.Empty:
            self.logger.warning(f"timeout with {timeout}, return None")
        return result

    def put_result(self, result, timeout = None) -> bool:
        try:
            self.result_queue.put(result, timeout=timeout)
            return True
        except queue.Full:
            self.logger.warning(f"timeout with {timeout}, throw the result")
            return False

    @staticmethod
    def preprocess(img):
        img = cv2.resize(img, (32, 32))
        img = img.astype(np.float32) / 255.0
        img = img.transpose(2, 0, 1)  # HWC → CHW
        img = np.expand_dims(img, 0)
        return img.copy()  
    
    