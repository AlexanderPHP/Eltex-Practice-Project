#include "proc_fs.h"

static char msg[MAX_MSG_SIZE];

static int __init
proc_init(void)
{
	int ret;
	ret = create_new_proc_entry();
	/* msg = kmalloc(MAX_MSG_SIZE * sizeof(char), GFP_KERNEL); */
	if (ret < 0)
		return -1;
	return 0;
}

static void __exit
proc_exit(void)
{
	remove_proc_entry(HELLO_PROC_NAME, NULL);
}

module_init(proc_init);
module_exit(proc_exit);

static int
create_new_proc_entry(void)
{
	hello_proc_dir = proc_create(HELLO_PROC_NAME, S_IFREG | S_IRUSR | S_IWUSR, NULL, &proc_fops);
	if (!hello_proc_dir)
	{
		remove_proc_entry(HELLO_PROC_NAME, NULL);
		return -1;
	}
	return 0;
}

static ssize_t
proc_read(struct file *flip, char *buf, size_t buf_len, loff_t *offp)
{
	static int readed = 0; 
	if (readed)
	{
		readed = 0;
		return 0;
	}
	readed = 1;
	copy_to_user(buf, msg, buf_len);
	printk(KERN_INFO "buf: %s msg: %s buf_len: %ld bufstrlen: %ld msgstrlen %ld", buf, msg, buf_len, strlen(buf), strlen(msg));
	return strlen(buf);
}

static ssize_t proc_write(struct file *filp, const char *buf, size_t buf_len, loff_t *offp)
{
	static int writed = 0;
	int cop = 0;
	if (writed)
	{
		writed = 0;
		return strlen(buf);
	}	
	writed = 1;
	memset(msg, 0, MAX_MSG_SIZE);
	cop = strncpy_from_user(msg, buf, MAX_MSG_SIZE - 1);
	printk(KERN_INFO "writed: %d strlen: %ld msg: %s buf: %s\n", cop, strlen(msg), msg, buf);
	return cop;
}
