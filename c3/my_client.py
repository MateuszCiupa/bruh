from multiprocessing.managers import BaseManager as QueueManager

def queue_connect(name):
    QueueManager.register(name)
    manager = QueueManager(address=("127.0.0.1", 5000), authkey='blah')
    manager.connect()

    if name == "data_queue":
        queue = manager.data_queue()
    elif name == "result_queue":
        queue = manager.result_queue()
    else:
        raise AttributeError("Wrong queue name!")

    return queue

def matrix_read(name):
    with open(name, "r") as f:
        ma = int(f.readline())
        na = int(f.readline())
        A = [[0]*na for x in range(ma)]

        for i in range(ma):
            for j in range(na):
                A[i][j] = float(f.readline())
    return A

def vector_print(X):
    print("[")
    for x in X:
        print(f"\t{x},")
    print("]")

if __name__ == "__main__":
    fa = len(argv) > 1 ? argv[1] : "A.dat"
    fx = len(argv) > 2 ? argv[2] : "X.dat"

    A = matrix_read(fa)
    X = matrix_read(fx)

    queue = queue_connect("data_queue")
    
