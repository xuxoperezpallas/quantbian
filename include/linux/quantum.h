#ifndef _LINUX_QUANTUM_H
#define _LINUX_QUANTUM_H

#define QUANTUM_MAJOR 250  // Número mágico para el dispositivo
#define QUANTUM_NETLINK_TYPE 30  // Tipo de socket Netlink

struct quantum_cmd {
    char circuit[4096];
    char result[4096];
};

#endif /* _LINUX_QUANTUM_H */