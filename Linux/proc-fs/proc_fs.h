#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/types.h>
#include <asm/uaccess.h>

#define HELLO_PROC_NAME "hello"
#define MAX_MSG_SIZE 10

static int __init proc_init(void);
static void __exit proc_exit(void);

static int create_new_proc_entry(void);
static ssize_t proc_read(struct file *filp,
			 char *buf,
			 size_t buf_len,
			 loff_t *offp);
static ssize_t proc_write(struct file *filp,
			  const char *buf,
			  size_t buf_len,
			  loff_t *offp); 

static struct proc_dir_entry *hello_proc_dir;
static struct file_operations proc_fops = {
	.owner = THIS_MODULE,
	.read = proc_read,
	.write = proc_write
};

MODULE_LICENSE("GPL");
