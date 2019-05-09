#include <linux/module.h>       
#include <linux/kernel.h>
#include <linux/cpumask.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/sched/stat.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/irqnr.h>
#include <linux/sched/cputime.h>
#include <linux/tick.h>
#include <linux/uaccess.h>
#include <asm/types.h>
#include <linux/vmstat.h>
#include <linux/swap.h>
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/mman.h>
#include <linux/mmzone.h>


#define PROCFS_MAX_SIZE	100
#define PROCFS_NAME "helper"

struct proc_dir_entry *Our_Proc_File;
char proc_buf[PROCFS_MAX_SIZE];


unsigned long get_uptime() {
	struct timespec64 uptime;
	u64 nsec;
	int i;

	nsec = 0;
	for_each_possible_cpu(i)
		nsec += (__force u64) kcpustat_cpu(i).cpustat[CPUTIME_IDLE];

	ktime_get_boottime_ts64(&uptime);

	return (unsigned long) uptime.tv_sec
}

int calc_uptime(void){
	printk(KERN_INFO "BEGIN");
	
	printk(KERN_INFO get_uptime());


	printk(KERN_INFO "END");
	return 0;
}

static ssize_t procfile_read(struct file *fp, char *buf, size_t len, loff_t *off){
	static int finished = 0;
	if(finished){
		finished = 0;
		return 0;
	}
	finished = 1;
	calc_uptime();
	//sprintf(buf, "MemTotal: %lld\n", memTotal);
	//sprintf(buf, "MemTotal: %lld\nMemFree: %lld\nBuffers: %lld\n", memTotal, memFree, buffers);
	return strlen(buf);
}

static struct file_operations proc_stat_operations = {
	.read = procfile_read,
};

static int start_module(void){
	Our_Proc_File = proc_create(PROCFS_NAME, 0644, NULL, &proc_stat_operations);
	return 0;
}

static void finish_module(void){
	remove_proc_entry(PROCFS_NAME, NULL);
}

module_init(start_module);
module_exit(finish_module);

