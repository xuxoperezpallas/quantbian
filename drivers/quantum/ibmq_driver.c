#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/quantum.h>

static int quantum_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Quantum: Dispositivo abierto\n");
    return 0;
}

static ssize_t quantum_write(struct file *file, const char __user *buf, 
                            size_t len, loff_t *ppos) {
    char *data = kmalloc(len + 1, GFP_KERNEL);
    if (!data) return -ENOMEM;

    if (copy_from_user(data, buf, len)) {
        kfree(data);
        return -EFAULT;
    }
    data[len] = '\0';
    printk(KERN_INFO "Quantum: Circuito recibido:\n%s\n", data);
    kfree(data);
    return len;
}

static struct file_operations quantum_fops = {
    .owner = THIS_MODULE,
    .open = quantum_open,
    .write = quantum_write,
};

static int __init quantum_init(void) {
    if (register_chrdev(QUANTUM_MAJOR, "quantum", &quantum_fops)) {
        printk(KERN_ERR "Quantum: Error al registrar dispositivo\n");
        return -1;
    }
    printk(KERN_INFO "Quantum: Driver registrado (major %d)\n", QUANTUM_MAJOR);
    return 0;
}

static void __exit quantum_exit(void) {
    unregister_chrdev(QUANTUM_MAJOR, "quantum");
    printk(KERN_INFO "Quantum: Driver eliminado\n");
}

module_init(quantum_init);
module_exit(quantum_exit);
MODULE_LICENSE("GPL");