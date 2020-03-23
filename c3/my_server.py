from multiprocessing.managers import BaseManager as QueueManager
from multiprocessing import Queue
import sys

def main(ip, port):
    result_queue = Queue()
    data_queue = Queue()
    QueueManager.register('result_queue', callable=lambda: result_queue)
    QueueManager.register('data_queue', callable=lambda: data_queue)
    manager = QueueManager(address=(ip, int(port)), authkey='blah')
    server = manager.get_server()
    server.serve_forever()

if __name__ == '__main__':
    main("", 5000)
