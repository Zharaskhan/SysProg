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



int calc_disk(void){
	printk(KERN_INFO "BEGIN");

	struct gendisk *sgp;
	struct disk_part_iter piter;
	struct hd_struct *part;
	char buf[BDEVNAME_SIZE];

	/* Don't show non-partitionable removeable devices or empty devices */
	if (!get_capacity(sgp) || (!disk_max_parts(sgp) &&
				   (sgp->flags & GENHD_FL_REMOVABLE)))
		return 0;
	if (sgp->flags & GENHD_FL_SUPPRESS_PARTITION_INFO)
		return 0;

	/* show the full disk and all non-0 size partitions of it */
	disk_part_iter_init(&piter, sgp, DISK_PITER_INCL_PART0);
	while ((part = disk_part_iter_next(&piter)))
		printk(KERN_INFO, "%4d  %7d %10llu %s\n",
			   MAJOR(part_devt(part)), MINOR(part_devt(part)),
			   (unsigned long long)part_nr_sects_read(part) >> 1,
			   disk_name(sgp, part->partno, buf));
	disk_part_iter_exit(&piter);


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
	calc_disk();
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

