from multiprocessing.managers import BaseManager

class QueueManager(BaseManager): pass

def queue_connect(name, ip, port):
    QueueManager.register(name)
    manager = QueueManager(address=(ip, port), authkey=b'abracadabra')
    manager.connect()

    if name == 'data_queue':
        queue = manager.data_queue()
    elif name == 'result_queue':
        queue = manager.result_queue()
    else:
        raise AttributeError('Wrong queue name!')

    return queue