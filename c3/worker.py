from collections import namedtuple
import concurrent.futures
from multiprocessing.managers import BaseManager as QueueManager
from sys import argv
from my_client import queue_connect

TaskResult = namedtuple('TaskResult', 'id row')

def put(queue, task_data):
    queue.put(task_data)

def multiply_row(index: int, row: list, vector: list):
    result = 0
    vector1d = [element[0] for element in vector]

    for i, element in enumerate(row):
        result += element * vector1d[i]

    print("result=", result)


    task_result = TaskResult(index, result)

    put(result_queue, task_result)
    print("wrzuciłem ", task_result)

result_queue = queue_connect("result_queue")
data_queue = queue_connect("data_queue")

def main():
    if len(argv) > 1:
        N = int(argv[1])
    else:
        N = 5

    tasks = []
    while not data_queue.empty():
        print("pobieram z kolejki...", end="")
        task = data_queue.get()
        print(task)
        tasks.append(task)

    with concurrent.futures.ProcessPoolExecutor(max_workers=N) as executor:
        args = [task for task in tasks]
        res = executor.map(lambda p: multiply_row(*p), args)

if __name__ == "__main__":
    main()
