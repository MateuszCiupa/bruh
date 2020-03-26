from multiprocessing.managers import BaseManager as QueueManager
from collections import namedtuple
from time import sleep

TaskData = namedtuple('TaskData', 'id row vector')

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
    fa = sys.argv[1] if len(argv) > 1 else "A.dat"
    fx = sys.argv[2] if len(argv) > 2 else "X.dat"

    A = matrix_read(fa)
    X = matrix_read(fx)

    queue = queue_connect("data_queue")
    results = list()

    for task_number, row in enumerate(A):
        task_data = TaskData(task_number, row, X)

        # wrzucamy zadanie do kolejki
        queue.put(task_data)

        # uzupełniamy listę zerami aby później wpisać wyniki w dobre miejsca
        results.append(0)

    print("wrzucono dane do kolejki")

    result_queue = queue_connect("result_queue")

    print("połączono do resultqueue")

    while True:
        try:
            if not result_queue.empty():
                result = result_queue.get()
                print(f"pobrano z kolejki: {result}")
            else:
                print("wait...", end="")
                sleep(1)
                continue
        except:
            print("wait...", end="")
            sleep(1)
            continue
        else:
            print("wstawiamy")
            results[result.id] = result.row  # wstawiamy wynik w odpowiednie miejsce

            if result_queue.empty():
                break

    print()
    print("WYNIK: ")
    print_result(results)
    
