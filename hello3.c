#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>

MODULE_AUTHOR("Nazar Bychkar");
MODULE_DESCRIPTION("Lab6 Module hello.c");
MODULE_LICENSE("Dual BSD/GPL");

static uint paramVal = 1;

module_param(paramVal, uint, 0444);
MODULE_PARM_DESC(paramVal, "Module parameter");

struct timeStruct {
    struct list_head list;
    ktime_t timestamp;
};

static LIST_HEAD(timeList);
int index;

static void releaseMemory(int outputType) {
    struct timeStruct *node, *nextNode;
    list_for_each_entry_safe(node, nextNode, &timeList, list) {
        if (outputType == 1) {
            pr_emerg("Time in nanoseconds: %lld\n", ktime_to_ns(node->timestamp));
        }
        list_del(&node->list);
        kfree(node);
    }
}

static int __init hello_init(void)
{
    BUG_ON(paramVal > 10);

    if (paramVal == 0 || (paramVal <= 10 && paramVal >= 5)) {
        pr_emerg("Warning: %u\n", paramVal);
    }

    index = 0;
    while (index < paramVal) {
        struct timeStruct *node = kmalloc(sizeof(*node), GFP_KERNEL);
        if (index == 9) {
            node = NULL; 
        }
        if (!node) {
            pr_err("kmalloc() failed\n");
            releaseMemory(0);
            BUG();
        }
        node->timestamp = ktime_get();
        list_add_tail(&node->list, &timeList);
        pr_emerg("Hello world!\n");
        index += 1;
    }

    return 0;
}

static void __exit hello_exit(void)
{
    releaseMemory(1);
}

module_init(hello_init);
module_exit(hello_exit);

