// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/quantum.h>

#define DEVICE_NAME "quantum"
static int major;
static char *shared_buffer;
static struct proc_dir_entry *proc_log;

static int quantum_open(struct inode *inode, struct file *file) {
    return 0;
}

static int quantum_mmap(struct file *filp, struct vm_area_struct *vma) {
    return remap_pfn_range(vma, vma->vm_start,
                          virt_to_phys(shared_buffer) >> PAGE_SHIFT,
                          vma->vm_end - vma->vm_start,
                          vma->vm_page_prot);
}

static ssize_t quantum_write(struct file *file, const char __user *buf,
                            size_t len, loff_t *ppos) {
    if (len > 4096) return -EFBIG;
    if (copy_from_user(shared_buffer, buf, len)) return -EFAULT;
    shared_buffer[len] = '\0';
    printk(KERN_INFO "Quantum: Circuito recibido (%zu bytes)\n", len);
    return len;
}

static struct file_operations quantum_fops = {
    .owner = THIS_MODULE,
    .open = quantum_open,
    .write = quantum_write,
    .mmap = quantum_mmap,
};

static int __init quantum_init(void) {
    shared_buffer = (char *)get_zeroed_page(GFP_KERNEL);
    if (!shared_buffer) return -ENOMEM;

    major = register_chrdev(0, DEVICE_NAME, &quantum_fops);
    if (major < 0) {
        free_page((unsigned long)shared_buffer);
        return major;
    }

    proc_log = proc_create("quantum_log", 0444, NULL, NULL);
    printk(KERN_INFO "Quantum: Driver cargado (major %d)\n", major);
    return 0;
}

static void __exit quantum_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    free_page((unsigned long)shared_buffer);
    remove_proc_entry("quantum_log", NULL);
    printk(KERN_INFO "Quantum: Driver descargado\n");
}

module_init(quantum_init);
module_exit(quantum_exit);
MODULE_LICENSE("GPL");