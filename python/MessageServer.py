from TrtModel import TrtModel
import requests
from typing import List
import logging
import threading
import json

class MessageServer:
    def __init__(self, logger: logging.Logger, ip: str, port: int, thread_num: int, trtModels: List[TrtModel]):
        if logger is None:
            exit(-1)
        self._logger = logger
        if ip is None or port is None or trtModels is None:
            self._logger.error("invalid params")
            exit(-1)
        self.__alive = False
        if thread_num is None or thread_num <= 0:
            self._logger.warning(f"invalid thread_num {thread_num}, set to 1")
            self.thread_num = 1
        else:
            self.thread_num = thread_num
        self.ip = ip
        self.port = port
        self.trtModels = trtModels
        self.threads = [threading.Thread(target=self._loop, daemon=True) for _ in range(self.thread_num)]

    def _loop(self):
        if len(self.trtModels) == 0:
            self.__alive = False
            return
        while self.__alive:
            for model in self.trtModels:
                result = model.get_result()
                try:
                    response = requests.post(
                        result['destination_url'], json=result
                        # result['destination_url'], data=json.dumps(result)
                    )
                    if response.status_code == 200:
                        print(f"发送成功，发送内容: {result}")
                        print(f"响应内容: \n{response.json()}")
                    else:
                        print(f"请求失败，状态码: {response.status_code}")
                        if not model.put_result(result=result, timeout=1):
                            self._logger.warning(f"too many results, throw the post failed result")
                except Exception as e:
                    self._logger.warning(f"网络错误: {e}")

    def start(self):
        self.__alive = True
        for thread in self.threads:
            thread.start()
        
    def stop(self):
        self.__alive = False

