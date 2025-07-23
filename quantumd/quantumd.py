import socket
import json
from qiskit_ibm_runtime import QiskitRuntimeService

class QuantumDaemon:
    def __init__(self):
        self.sk = socket.socket(socket.AF_NETLINK, socket.SOCK_RAW, 30)
        self.sk.bind((0, 0))
        self.backend = QiskitRuntimeService().backend("ibm_brisbane")

    def listen_kernel(self):
        while True:
            data = self.sk.recv(4096)
            circuit = json.loads(data.decode())
            job = self.backend.run(circuit)
            result = job.result()
            self.sk.send(json.dumps(result.get_counts()).encode())

if __name__ == "__main__":
    QuantumDaemon().listen_kernel()