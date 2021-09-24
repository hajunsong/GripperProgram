/* NRMKFoundation, Copyright 2016- Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>

#include "NRMKsercan_tp.h"
#include "NRMKhw_tp.h"

int rtsercan_fd  = -1;

RT_TASK write_task;
RT_TASK read_task;
int working=1;


void cleanup_all(void)
{
    working=0;
    usleep(1000000);
    rt_task_delete(&write_task);
    rt_task_delete(&read_task);

}

void catch_signal(int sig)
{
    cleanup_all();
    return;
}


void write_thread(void* arg)
{
    int i;
    CAN_FRAME cfame;
    cfame.can_id=0;
    cfame.can_dlc=8;
    for (i=0; i<8; ++i){
        cfame.data[i]=i;
    }
    cfame.can_id=0x080;
//    cfame.can_dlc=0;
    rt_task_set_periodic(NULL, TM_NOW, 1e9);
    while (working)
    {
        RTSERCAN_write(rtsercan_fd, cfame);
        rt_print_CANFrame(cfame);
        rt_task_wait_period(NULL);
    }
}

void read_task_proc(void *arg)
{
    int res;
    CAN_FRAME RxFrame;
    rt_task_set_periodic(NULL, TM_NOW, 1e6);
    while (working)
    {
        res=RTSERCAN_read(rtsercan_fd, &RxFrame);
        if (res==SERCAN_ERR_FREE)
        {
            rt_print_CANFrame(RxFrame);
        }

        rt_task_wait_period(NULL);

    }
}


int main(int argc, char* argv[])
{

    signal(SIGTERM, catch_signal);
    signal(SIGINT, catch_signal);

    /* no memory-swapping for this programm */
    mlockall(MCL_CURRENT | MCL_FUTURE);

    // Perform auto-init of rt_print buffers if the task doesn't do so
    rt_print_auto_init(1);

    // open rtsercan*******************
    rtsercan_fd=RTSERCAN_open();
    if (rtsercan_fd < 0){
        return 1;
    }
    if(SERCAN_SetBitRate(rtsercan_fd, "1M") < 0){
        return 1;
    }
    rt_printf("\nrt can scan finish\n");
    //********************************

    //-------------------------------------------------------
    rt_task_create(&read_task, "read_task", 0, 99, 0);
    rt_task_start(&read_task, &read_task_proc, NULL);


    rt_task_create(&write_task, "write_task", 0, 99, 0);
    rt_task_start(&write_task, &write_thread, NULL);

    pause();
    return 0;

}

