#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/moduleparam.h>
#include <linux/bug.h>

MODULE_AUTHOR("Artem Protsenko");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

#define BUG() do { \
    printk("BUG: failure at %s:%d/%s()!\n", __FILE__, __LINE__, __func__); \
    panic("BUG!"); \
} while (0)

#define BUG_ON(condition) do { \
    if (unlikely(condition)) \
        BUG(); \
} while (0)

static uint repeat_count = 1;
module_param(repeat_count, uint, S_IRUGO);
MODULE_PARM_DESC(repeat_count, "Number of times 'Hello, world!' is printed.");

struct time_entry {
    struct list_head list;
    ktime_t time;
};

static LIST_HEAD(time_list);

static int __init hello_init(void)
{
    struct time_entry *entry;
    int i;

    BUG_ON(repeat_count > 10);

    if (repeat_count == 0 || (repeat_count >= 5 && repeat_count <= 10)) {
        printk(KERN_WARNING "Warning: repeat_count is 0 or between 5 and 10.\n");
    } 

    for (i = 0; i < repeat_count; i++) {
        if (i == repeat_count - 1) {
            entry = NULL;
        } else {
            entry = kmalloc(sizeof(*entry), GFP_KERNEL);
        }
        if (!entry) {
            printk(KERN_ERR "Error: kmalloc() returned 0.\n");
            continue;
        }

        entry->time = ktime_get();
        list_add_tail(&entry->list, &time_list);

        printk(KERN_ALERT "Hello, world!\n");
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct time_entry *entry, *tmp;

    list_for_each_entry_safe(entry, tmp, &time_list, list) {
        printk(KERN_INFO "Time: %lld ns\n", ktime_to_ns(entry->time));
        list_del(&entry->list);
        kfree(entry);
    }

    printk(KERN_ALERT "Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);
