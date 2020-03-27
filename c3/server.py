from multiprocessing.managers import BaseManager
from multiprocessing import Queue
from sys import argv
from collections import namedtuple

TaskData = namedtuple('TaskData', 'id row vector')
TaskResult = namedtuple('TaskResult', 'id row')

class QueueManager(BaseManager): pass

def main(ip, port):
    result_queue = Queue()
    data_queue = Queue()
    QueueManager.register('result_queue', callable=lambda: result_queue)
    QueueManager.register('data_queue', callable=lambda: data_queue)
    manager = QueueManager(address=(ip, int(port)), authkey=b'abracadabra')
    server = manager.get_server()
    server.serve_forever()

if __name__ == '__main__':
    ip = argv[1] if len(argv) > 1 else ''
    port = int(argv[2]) if len(argv) > 2 else 5000
    main(ip, port)
