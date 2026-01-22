import threading

class SafeQueue_single_condition:
    def __init__(self, max_size = 0):
        self.queue = []
        self.max_size = max_size
        self.condition = threading.Condition()
        
    def put(self, item):
        with self.condition:
            while len(self.queue) >= self.max_size:
                self.condition.wait()
            self.queue.append(item)
            self.condition.notify_all()
            
    def get(self):
        with self.condition:
            while len(self.queue) == 0:
                self.condition.wait()
            # item = self.queue[-1]
            # self.queue.pop()
            item = self.queue.pop(0)
            self.condition.notify_all()
            return item


class SafeQueue:
    def __init__(self, max_size=5):
        self.queue = []
        self.max_size = max_size
        self.lock = threading.Lock()
        self.not_full = threading.Condition(self.lock)
        self.not_empty = threading.Condition(self.lock)

    def put(self, item):
        with self.not_full:
            while len(self.queue) >= self.max_size:
                self.not_full.wait()
            self.queue.append(item)
            self.not_empty.notify()  # 只唤醒一个消费者

    def get(self):
        with self.not_empty:
            while len(self.queue) == 0:
                self.not_empty.wait()
            item = self.queue.pop(0)
            self.not_full.notify()  # 只唤醒一个生产者
            return item
