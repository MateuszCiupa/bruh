from multiprocessing.managers import BaseManager
from collections import namedtuple
from time import sleep, time
from sys import argv
from util import queue_connect

class QueueManager(BaseManager): pass

TaskData = namedtuple('TaskData', 'id row vector')
TaskResult = namedtuple('TaskResult', 'id row')

program_time = time()

def matrix_read(name):
    with open(name, 'r') as f:
        ma = int(f.readline())
        na = int(f.readline())
        A = [[0]*na for x in range(ma)]

        for i in range(ma):
            for j in range(na):
                A[i][j] = float(f.readline())
    return A

def vector_print(X):
    print('[')
    for x in X:
        print(f'\t{x},')
    print(']')

ip = argv[1] if len(argv) > 1 else '127.0.0.1'
port = int(argv[2]) if len(argv) > 2 else 5000
fa = argv[3] if len(argv) > 3 else 'A.dat'
fx = argv[4] if len(argv) > 4 else 'X.dat'

A = matrix_read(fa)
X = matrix_read(fx)

queue = queue_connect('data_queue', ip, port)
print('connected to data_queue')

results = list()

for task_id, row in enumerate(A):
    task_data = TaskData(task_id, row, X)
    queue.put(task_data)
    results.append(0)

result_queue = queue_connect('result_queue', ip, port)
print('connected to result_queue')

calc_time = time()

while result_queue.qsize() < len(A):
    continue

while True:
    try:
        if not result_queue.empty():
            result = result_queue.get()
        else:
            sleep(1)
            continue
    except:
        sleep(1)
        continue
    else:
        results[result.id] = result.row
        if result_queue.empty(): break

print()
print('Result vector: ')
vector_print(results)
print(f'Overall program time: {time()-program_time}')
print(f'Calculations time: {time()-calc_time}')
