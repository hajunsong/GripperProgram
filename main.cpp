#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}


/* NRMKFoundation, Copyright 2016- Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */

/* NRMKFoundation, Copyright 2016- Neuromeka. All rights reserved.
 *
 * This library is commercial and cannot be redistributed, and/or modified
 * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
 */

//#include <stdio.h>
//#include <signal.h>
//#include <unistd.h>
//#include <sys/mman.h>

//#include <native/task.h>
//#include <native/timer.h>
//#include <rtdk.h>

//#include "NRMKsercan_tp.h"
//#include "NRMKhw_tp.h"

////TP_PORT test_port=1;

//int rtsercan_fd  = -1;

//RT_TASK write_task;
//RT_TASK read_task;
//int working=1;

//unsigned char txHeader = 0xFA;
//unsigned char txDataFiled[] = {0x58, 0x0a, 0x34, 0x00, 0x6a, 0x08, 0x4e,
//                                 0x00, 0x36, 0xb4, 0xea, 0x11, 0x4e, 0x0e, 0xc8,
//                                 0x00, 0x65, 0xf2, 0x57, 0x00, 0xa1, 0x45, 0x83,
//                                 0x00, 0x1f, 0x56, 0x8e, 0x00, 0x1f, 0x56, 0x8e,
//                                 0x00, 0xae, 0xe9, 0xf2, 0xe9, 0xa6, 0x3e, 0x05,
//                               0xfe, 0x01, 0xef, 0x80};
//unsigned char txCheckSum = 0;
//unsigned char txTail = 0xFE;

//void cleanup_all(void)
//{
//    working=0;
//    rt_task_delete(&write_task);
//    rt_task_delete(&read_task);

//}

//void catch_signal(int sig)
//{
//    cleanup_all();
//    return;
//}


//void write_thread(void* arg)
//{
//    int i;
//    CAN_FRAME cfame;
//    cfame.can_id=0;
//    cfame.can_dlc=8;
//    for (i=0; i<8; ++i) cfame.data[i]=i+1;
//    cfame.can_id=0x10;
//    rt_task_set_periodic(NULL, TM_NOW, 1e9);
//    int ret = 0;
//    while (working)
//    {
//        ret = RTSERCAN_write(rtsercan_fd, cfame);
//        //        rt_printf("%d\n", ret);
//        rt_print_CANFrame(cfame);
//        rt_task_wait_period(NULL);
//    }

////    int i;
////    CAN_FRAME cfame;
////    cfame.can_id=0;
////    cfame.can_dlc=8;

////    int ret = 0;
////    int send_cnt = 0;
////    int data_size = sizeof(txDataFiled)/sizeof(txDataFiled[0]);
////    unsigned char *txData;

////    rt_task_set_periodic(NULL, TM_NOW, 1e9);

////    while (working)
////    {
////        rt_task_wait_period(NULL);

////        txCheckSum = 0;
////        for(int i = 0; i < data_size; i++){
////            txCheckSum += txDataFiled[i];
////        }
////        txCheckSum = ~txCheckSum;

////        txData = new unsigned char[data_size + 3];

////        txData[0] = txHeader;
////        for(int i = 0; i < data_size; i++){
////            txData[i+1] = txDataFiled[i];
////        }
////        txData[data_size + 1] = txCheckSum;
////        txData[data_size + 2] = txTail;

////        send_cnt = data_size/cfame.can_dlc + 1;

////        for(int i = 0; i < send_cnt; i++){
////            memset(cfame.data, 0, sizeof(unsigned char)*cfame.can_dlc);
////            for(int j = 0; j < cfame.can_dlc; j++){
////                cfame.data[j] = txData[i*cfame.can_dlc + j];
////            }
////            RTSERCAN_write(rtsercan_fd, cfame);
////            rt_print_CANFrame(cfame);
////            usleep(200);
////        }

////        delete[] txData;
////        rt_printf("\n");
////    }

//}

//void read_task_proc(void *arg)
//{
//    int res;
//    CAN_FRAME RxFrame;
//    rt_task_set_periodic(NULL, TM_NOW, 1e6);
//    while (working)
//    {
//        res=RTSERCAN_read(rtsercan_fd, &RxFrame);
//        if (res==SERCAN_ERR_FREE)
//        {
//            rt_print_CANFrame(RxFrame);
//        }

//        rt_task_wait_period(NULL);

//    }
//}


//int main(int argc, char* argv[])
//{

//    signal(SIGTERM, catch_signal);
//    signal(SIGINT, catch_signal);

//    /* no memory-swapping for this programm */
//    mlockall(MCL_CURRENT | MCL_FUTURE);

//    // Perform auto-init of rt_print buffers if the task doesn't do so
//    rt_print_auto_init(1);

//    // open rtsercan*******************
//    rtsercan_fd=RTSERCAN_open();
//    if (rtsercan_fd < 0) return 1;
//    //********************************
////    SERCAN_SetBitRate(rtsercan_fd, "1M");

//    //-------------------------------------------------------
//    rt_task_create(&read_task, "read_task", 0, 99, 0);
//    rt_task_start(&read_task, &read_task_proc, NULL);


//    rt_task_create(&write_task, "write_task", 0, 99, 0);
//    rt_task_start(&write_task, &write_thread, NULL);

//    pause();
//    return 0;

//}


///* NRMKFoundation, Copyright 2016- Neuromeka. All rights reserved.
// *
// * This library is commercial and cannot be redistributed, and/or modified
// * WITHOUT ANY ALLOWANCE OR PERMISSION OF Neuromeka
// */
//#include <stdio.h>
//#include <stdlib.h>
//#include <pthread.h>
//#include "NRMKhw_tp.h"

//int fd=-1;
//void * read_thread(void* arg);
//void * write_thread(void* arg);

////reading thread
//void * read_thread(void* arg)
//{
//    int nbytes;
//    unsigned char chr[255];
//    while(1)
//    {
//        nbytes=read(fd,chr,255);
//        if (nbytes>0){
//            printf("read data size : %d\t", nbytes);
//            for(int i = 0; i < nbytes; i++){
//                printf("%d\t", chr[i]);
//            }
//            printf("\n");
//        }
//        usleep(1e4);
//    }
//}

////writing thread
//void * write_thread(void* arg)
//{
//    int kchr;
//    while(1)
//    {
//        if (NRMKkbhit())
//        {
//            kchr = getchar();
//            write(fd,&kchr,1);
//            printf("\n");
//        }
//        usleep(1e4);
//    }
//}
///****************************************************************************
// * Signal handler
// ***************************************************************************/
//void catch_signal(int sig)
//{
//    if (fd>0) close(fd);
//    exit(1);
//}


//int main(int argc, char* argv[])
//{
//    signal(SIGTERM, catch_signal);
//    signal(SIGINT, catch_signal);

//    int portNum=1;			//COM1 as default
//    char portName[100];
//    unsigned int baud = 115200;
//    strcpy(portName, "/dev/ttyUSB0");

//    printf("Serial example: port=#%s, baud=%i\n", portName, baud);

//    fd=tp_open_serial_port(portName, baud);
//    if(fd<0)
//         return 1;

//    usleep(1e5);
//    pthread_t tid1,tid2;
//    pthread_create(&tid1, NULL, read_thread, NULL);
//    pthread_create(&tid2, NULL, write_thread, NULL);
//    pthread_join(tid1,NULL);
//    pthread_join(tid2,NULL);

//    return 0;
//}
