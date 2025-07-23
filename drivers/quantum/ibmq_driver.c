#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <linux/quantum.h>

static struct sock *nl_sock;

// Callback para recibir datos desde userspace via Netlink
static void quantum_nl_recv_msg(struct sk_buff *skb) {
    struct nlmsghdr *nlh = nlmsg_hdr(skb);
    printk(KERN_INFO "Quantum: Mensaje recibido desde userspace: %s\n", (char *)NLMSG_DATA(nlh));
}

// Enviar circuito a userspace
static void quantum_send_to_userspace(const char *circuit) {
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    int size = strlen(circuit) + 1;

    skb = nlmsg_new(size, GFP_KERNEL);
    nlh = nlmsg_put(skb, 0, 0, NLMSG_DONE, size, 0);
    NETLINK_CB(skb).dst_group = 0;
    strncpy(nlmsg_data(nlh), circuit, size);

    nlmsg_unicast(nl_sock, skb, 0); // PID 0 = userspace daemon
}

static ssize_t quantum_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos) {
    char *data = kmalloc(len + 1, GFP_KERNEL);
    copy_from_user(data, buf, len);
    data[len] = '\0';
    
    quantum_send_to_userspace(data); // Enviar a userspace
    kfree(data);
    return len;
}

static int __init quantum_init(void) {
    struct netlink_kernel_cfg cfg = {
        .input = quantum_nl_recv_msg,
    };

    nl_sock = netlink_kernel_create(&init_net, QUANTUM_NETLINK_TYPE, &cfg);
    register_chrdev(QUANTUM_MAJOR, "quantum", &quantum_fops);
    printk(KERN_INFO "Quantum: Driver listo (Netlink type %d)\n", QUANTUM_NETLINK_TYPE);
    return 0;
}