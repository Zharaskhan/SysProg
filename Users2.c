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


#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/segment.h>
#include <linux/buffer_head.h>



#define PROCFS_MAX_SIZE 100
#define PROCFS_NAME "helper"
MODULE_LICENSE("GPL");

struct proc_dir_entry *Our_Proc_File;
char proc_buf[PROCFS_MAX_SIZE];


// from related post
struct file* file_open(const char* path, int flags, int rights) 
{
    struct file* filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());
    int USERNAME_SIZE = 50;

    file->f_pos=0;    

    int i;
    for (i=0;i<100; i++) {
        char tempuname[USERNAME_SIZE];
        int a;
        for (a=0;a <USERNAME_SIZE-1; a++) tempuname[a] = '!'; //dummy
            tempuname[USERNAME_SIZE-1] = '\0';
        
         char buff[1];
                buff[0] = 10;


        a = 0;
        int ret = vfs_read(file,buff,1,&file->f_pos); //reads one character
        
        if (!ret) {
            break;
        }
        printk(KERN_ALERT "RET %d", ret);

        while(buff[0] != ':') { //parse the username
            tempuname[a++] = buff[0];

            vfs_read(file,buff,1,&file->f_pos);

        }

        tempuname[a] = '\0';
        while(buff[0] != 10) { 
            vfs_read(file,buff,1,&file->f_pos);

        }

        printk(KERN_ALERT "AHTUNG: %s", tempuname);
    }

    //sprintf(data, "%s", tempuname);

    set_fs(oldfs);
    return 0;
}  




void get_users(char *mybuf) {
     struct file* file = file_open("/etc/passwd", O_RDONLY, 0);
    char* data = vmalloc(sizeof(char) * 100);
    int ret = file_read(file, 0, data, 100); // crashes here
    sprintf(mybuf, "init %d %s\n", ret, data);


}

static ssize_t procfile_read(struct file *fp, char *buf, size_t len, loff_t *off){
    static int finished = 0;
    if(finished){
        finished = 0;
        return 0;
    }
    finished = 1;
    char users[300] = "";
    get_users(users);
    
    sprintf(buf, "Users:\n%s\n", users);
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