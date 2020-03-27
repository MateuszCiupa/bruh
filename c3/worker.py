from collections import namedtuple
from multiprocessing.managers import BaseManager
from multiprocessing import Pool, Process
from sys import argv
from util import queue_connect

class QueueManager(BaseManager): pass

TaskResult = namedtuple('TaskResult', 'id row')
TaskData = namedtuple('TaskData', 'id row vector')

def mnoz(data):
    index, row, vector = data
    result = 0
    v = [x[0] for x in vector]
    for i, element in enumerate(row): result += element * v[i]
    task_result = TaskResult(index, result)
    result_queue.put(task_result)

ip = argv[1] if len(argv) > 1 else '127.0.0.1'
port = int(argv[2]) if len(argv) > 2 else 5000

result_queue = queue_connect('result_queue', ip, port)
data_queue = queue_connect('data_queue', ip, port)

n = int(argv[3]) if len(argv) > 3 else 5
tasks = []

while not data_queue.empty():
    task = data_queue.get()
    tasks.append(task)

with Pool(n) as p:
    args = [task for task in tasks]
    p.map(mnoz, args)
