#include <linux/ktime.h> //uptime
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
MODULE_LICENSE("GPL");

struct proc_dir_entry *Our_Proc_File;
char proc_buf[PROCFS_MAX_SIZE];


char* get_uptime(char *buf) {
	struct timespec  uptime;
	get_monotonic_boottime(&uptime);
	unsigned long uptime_in_seconds = uptime.tv_sec;

	int seconds = uptime_in_seconds % 60;
	int minutes = uptime_in_seconds / 60 % 60;
	int hours   = uptime_in_seconds / 3600 % 24;
	int days    = uptime_in_seconds / 86400;

	char daystring[32] = "";
   	if (days > 1) {
      sprintf(daystring, "%d days, ", days);
   	} else if (days == 1) {
      sprintf(daystring, "%d days, ", days);
   	}

 	sprintf(buf, "%s%02d:%02d:%02d", daystring, hours, minutes, seconds);
}

static ssize_t procfile_read(struct file *fp, char *buf, size_t len, loff_t *off){
	static int finished = 0;
	if(finished){
		finished = 0;
		return 0;
	}
	finished = 1;
	char uptime[30] = "";
	get_uptime(uptime);
	
	sprintf(buf, "Uptime: %s\n", uptime);
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