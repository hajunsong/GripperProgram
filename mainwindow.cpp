#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1354, 970);

    init();

    connect(ui->btnConnect, SIGNAL(clicked()), this, SLOT(btnConnectClicked()));
    connect(ui->btnPing, SIGNAL(clicked()), this, SLOT(btnPingClicked()));
    connect(ui->btnInit, SIGNAL(clicked()), this, SLOT(btnInitClicked()));
    connect(ui->btnFeedbackOn, SIGNAL(clicked()), this, SLOT(btnFeedbackOnClicked()));
    connect(ui->btnFeedbackOff, SIGNAL(clicked()), this, SLOT(btnFeedbackOffClicked()));
    connect(ui->btnPneumaticOn, SIGNAL(clicked()), this, SLOT(btnPneumaticOnClicked()));
    connect(ui->btnPneumaticOff, SIGNAL(clicked()), this, SLOT(btnPneumaticOffClicked()));
    connect(ui->btnFingerOn, SIGNAL(clicked()), this, SLOT(btnFingerOnClicked()));
    connect(ui->btnFingerOff, SIGNAL(clicked()), this, SLOT(btnFingerOffClicked()));
    connect(ui->btnLoggingStart, SIGNAL(clicked()), this, SLOT(btnLoggingStartClicked()));

    connect(ui->cbOpPositionMode, SIGNAL(clicked(bool)), this, SLOT(cbOpPositionModeClicked(bool)));
    connect(ui->cbOpForceMode, SIGNAL(clicked(bool)), this, SLOT(cbOpForceModeClicked(bool)));
    connect(ui->cbFingerSensor, SIGNAL(clicked(bool)), this, SLOT(cbForceOptionFingerSensorClicked(bool)));
    connect(ui->cbCurrentSensor, SIGNAL(clicked(bool)), this, SLOT(cbForceOptionCurrentSensorClicked(bool)));

    connect(ui->hsliderVelocity1, SIGNAL(valueChanged(int)), this, SLOT(hsliderVelocityValueChanged(int)));
    connect(ui->hsliderPosition1, SIGNAL(valueChanged(int)), this, SLOT(hsliderPositionValueChanged(int)));
    connect(ui->hsliderForce1, SIGNAL(valueChanged(int)), this, SLOT(hsliderForceValueChanged(int)));

    connect(ui->txtVelocityLimit1, SIGNAL(editingFinished()), this, SLOT(txtVelLimitEditingFinished()));
    connect(ui->txtPositionLimit1, SIGNAL(editingFinished()), this, SLOT(txtPosLimitEditingFinished()));
    connect(ui->txtForceLimit1, SIGNAL(editingFinished()), this, SLOT(txtForceLimitEditingFinished()));

    connect(ui->hsliderVelocity2, SIGNAL(valueChanged(int)), this, SLOT(hsliderVelocityValueChanged(int)));
    connect(ui->hsliderPosition2, SIGNAL(valueChanged(int)), this, SLOT(hsliderPositionValueChanged(int)));
    connect(ui->hsliderForce2, SIGNAL(valueChanged(int)), this, SLOT(hsliderForceValueChanged(int)));

    connect(ui->txtVelocityLimit2, SIGNAL(editingFinished()), this, SLOT(txtVelLimitEditingFinished()));
    connect(ui->txtPositionLimit2, SIGNAL(editingFinished()), this, SLOT(txtPosLimitEditingFinished()));
    connect(ui->txtForceLimit2, SIGNAL(editingFinished()), this, SLOT(txtForceLimitEditingFinished()));

    connect(ui->btnOrg, SIGNAL(clicked()), this, SLOT(btnOrgClicked()));
    connect(ui->btnTransmit3, SIGNAL(clicked()), this, SLOT(btnRepeatOperatingClicked()));
    connect(ui->btnDIOSet, SIGNAL(clicked()), this, SLOT(btnDIOSetClicked()));

    ui->comboCommSpeed->setEnabled(false);
    ui->txtCommSpeed->setEnabled(false);
    ui->txtCommSpeed->hide();

    connect(ui->rbRS485, SIGNAL(clicked(bool)), this, SLOT(rbRS485Clicked(bool)));
    connect(ui->rbCAN, SIGNAL(clicked(bool)), this, SLOT(rbCANClicked(bool)));
    connect(ui->rbECAT, SIGNAL(clicked(bool)), this, SLOT(rbECATClicked(bool)));

    plotTimer = new QTimer(this);
    connect(plotTimer, SIGNAL(timeout()), this, SLOT(plotUpdateTimeout()));
    plotTimer->setInterval(50);

    customSettings = new CustomSettings(ui);
    customSettings->loadConfigFile();

    ui->hsliderVelocity1->setMaximum(ui->txtVelocityLimit1->text().toInt());
    ui->hsliderPosition1->setMaximum(ui->txtPositionLimit1->text().toInt());
    ui->hsliderForce1->setMaximum(ui->txtForceLimit1->text().toInt());

    ui->hsliderVelocity2->setMaximum(ui->txtVelocityLimit2->text().toInt());
    ui->hsliderPosition2->setMaximum(ui->txtPositionLimit2->text().toInt());
    ui->hsliderForce2->setMaximum(ui->txtForceLimit2->text().toInt());

    ui->comboPlotData->addItems(plotItems);
    connect(ui->comboPlotData, SIGNAL(currentIndexChanged(int)), this, SLOT(comboPlotCurrentIndexChanged(int)));

    ui->plot->addGraph();
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsLine);

    QPen blueDotPen;
    blueDotPen.setColor(QColor(0, 0, 255, 255));
    blueDotPen.setStyle(Qt::SolidLine);
    blueDotPen.setWidthF(4);
    //    ui->plot->graph(0)->setPen(blueDotPen);
    ui->plot->xAxis->setRange(-1, 1);
    ui->plot->yAxis->setRange(-1, 1);
    ui->plot->replot();
    ui->plot->xAxis->setLabel("Interval");

    connect(ui->btnTransmit1, SIGNAL(clicked()), this, SLOT(btnTransmitClicked()));
    connect(ui->btnTransmit2, SIGNAL(clicked()), this, SLOT(btnTransmitClicked()));

    initGUI();

    send_check = false;
    print_flag = false;

    string dir_name = "./logging";
    string file_name = dir_name + "/";
    mkdir(dir_name.c_str(), 0777);

    time_t now = time(NULL);
    struct tm tstruct;
    tstruct = *localtime(&now);
    char buf[80];
    sprintf(buf, "%04d-%02d-%02d-%02d-%02d-%02d",
            tstruct.tm_year + 1900, tstruct.tm_mon + 1, tstruct.tm_mday, tstruct.tm_hour, tstruct.tm_min, tstruct.tm_sec);

    file_name.append(buf);
    file_name.append(".txt");
    logger = new Logger(file_name);
    logger->setShowDateTime(false);

    ui->gbMessage->hide();

    appendMessage("start program");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(){
    comm_485 = false;
    comm_can = false;
    comm_ecat = false;
    comm_indx = 0;

    update_thread_run = false;
    comm_485_rx_run = false;
    comm_485_tx_run = false;
    comm_can_rx_run = false;
    comm_can_tx_run = false;
    comm_ecat_run = false;
    connectState = false;

    txString = "";
    rxString = "";

    rtsercan_fd = -1;
    rs485_fd = -1;

    cmd_vel1 = 0;
    cmd_pos1 = 0;
    cmd_force1 = 0;
    cmd_vel2 = 0;
    cmd_pos2 = 0;
    cmd_force2 = 0;

    memset(&gripperStatus, 0, sizeof(gripper_status));

    send_check = false;
    print_flag = false;
    send_flag = false;

    send_data_size = 0;
    recv_data_size = 0;

    logging_flag = false;
}

void MainWindow::finish(){
    if(comm_485_rx_run){
        comm_485_rx_run = false;
        rt_task_join(&comm_485_rx_task);
        usleep(10000);
        rt_task_delete(&comm_485_rx_task);
        usleep(10000);
    }

    if(comm_485_tx_run){
        comm_485_tx_run = false;
        rt_task_join(&comm_485_tx_task);
        usleep(10000);
        rt_task_delete(&comm_485_tx_task);
        usleep(10000);
    }

    if(comm_can_rx_run){
        comm_can_rx_run = false;
        rt_task_join(&comm_can_rx_task);
        usleep(10000);
        rt_task_delete(&comm_can_rx_task);
        usleep(10000);
    }

    if(comm_can_tx_run){
        comm_can_tx_run = false;
        rt_task_join(&comm_can_tx_task);
        usleep(10000);
        rt_task_delete(&comm_can_tx_task);
        usleep(10000);
    }
    connectState = false;

    if(update_thread_run){
        update_thread_run = false;
        pthread_join(update_thread, NULL);
        usleep(10000);
        pthread_cancel(update_thread);
        usleep(10000);
    }

    printf("Finish !!!\n");
}

void MainWindow::closeEvent(QCloseEvent* event){
    cout << "Closed window" << endl;
    event->ignore();

    finish();

    if(logging_thread_run){
        logging_thread_run = false;
        pthread_join(logging_thread, NULL);
        usleep(10000);
        pthread_cancel(logging_thread);
        usleep(10000);
    }

    delete logger;

    customSettings->saveConfigFile();

    event->accept();
}

void MainWindow::rbRS485Clicked(bool arg){
    ui->txtCommSpeed->show();
    ui->txtCommSpeed->setText("");
    ui->comboCommSpeed->show();
    ui->comboCommSpeed->clear();

    if(arg){
        ui->rbCAN->setChecked(false);
        ui->rbECAT->setChecked(false);

        ui->txtCommSpeed->setEnabled(true);
        ui->txtCommSpeed->setText("Buad Rate : ");
        ui->comboCommSpeed->setEnabled(true);
        ui->comboCommSpeed->addItem("");
        ui->comboCommSpeed->addItem("9600");
        ui->comboCommSpeed->addItem("38400");
        ui->comboCommSpeed->addItem("57600");
        ui->comboCommSpeed->addItem("115200");

        comm_indx = 1;
    }
    else{
        ui->txtCommSpeed->setEnabled(false);
        ui->comboCommSpeed->setEnabled(false);

        comm_indx = 0;
    }
}

void MainWindow::rbCANClicked(bool arg){
    ui->txtCommSpeed->show();
    ui->txtCommSpeed->setText("");
    ui->comboCommSpeed->show();
    ui->comboCommSpeed->clear();

    if(arg){
        ui->rbRS485->setChecked(false);
        ui->rbECAT->setChecked(false);

        ui->txtCommSpeed->setEnabled(true);
        ui->txtCommSpeed->setText("Bit Rate : ");
        ui->comboCommSpeed->setEnabled(true);
        ui->comboCommSpeed->addItem("");
        ui->comboCommSpeed->addItem("5K");
        ui->comboCommSpeed->addItem("10K");
        ui->comboCommSpeed->addItem("20K");
        ui->comboCommSpeed->addItem("50K");
        ui->comboCommSpeed->addItem("125K");
        ui->comboCommSpeed->addItem("250K");
        ui->comboCommSpeed->addItem("500K");
        ui->comboCommSpeed->addItem("1M");

        comm_indx = 2;
    }
    else{
        ui->txtCommSpeed->setEnabled(false);
        ui->comboCommSpeed->setEnabled(false);

        comm_indx = 0;
    }
}

void MainWindow::rbECATClicked(bool arg){
    ui->txtCommSpeed->hide();
    ui->txtCommSpeed->setText("");
    ui->comboCommSpeed->clear();
    ui->comboCommSpeed->hide();
}

void *MainWindow::update_func(void *arg)
{
    MainWindow *pThis = static_cast<MainWindow*>(arg);

    pThis->update_thread_run = true;

    while(pThis->update_thread_run){
//        printf("send check : %s\n", pThis->send_check ? "true" : "false");

        if(pThis->send_check){
            if(pThis->gripperStatus.transfer_ready){
                int indx = 0;
                memset(pThis->txData, 0, sizeof(unsigned char)*MAX_RECV_BUFFER_SIZE);
                pThis->txData[indx++] = TX_SOP;

                for(uint i = 0; i < pThis->data_field.size(); i++){
                    pThis->txData[indx++] = pThis->data_field[i];
                }
                pThis->txData[indx++] = pThis->checksum(pThis->data_field);
                pThis->txData[indx++] = EOP;

                pThis->send_data_size = indx;

                pThis->send_flag = true;
                pThis->send_check = false;

                pThis->msgString.clear();
                pThis->msgString.append("TX Data : ");
                for(int i = 0; i < indx; i++){
                    pThis->msgString.push_back(pThis->txData[i]);
                }
                pThis->appendMessage(QString::fromStdString(pThis->msgString));

                break;
            }
            else{
                if(pThis->print_flag){
                    pThis->appendMessage("Device not ready");
                    pThis->print_flag = false;
                }
            }
        }
    }

    printf("finished update thread\n");

    return NULL;
}

void *MainWindow::logging_func(void *arg)
{
    MainWindow *pThis = static_cast<MainWindow*>(arg);

    pThis->logging_thread_run = true;

    while(pThis->logging_thread_run){
        if(pThis->logging_flag){
            pThis->logger->write(pThis->loggingString);

            pThis->logging_flag = false;
        }
        usleep(1000);
    }

    printf("finished logging thread\n");

    return NULL;
}

void *MainWindow::repeat_func(void *arg)
{
    MainWindow *pThis = static_cast<MainWindow*>(arg);

    bool flag = true;

    for(int i = 0; i < pThis->repeat_count*2; i++)
    {
        pThis->data_field.clear();
        pThis->data_field.push_back(CMD_MOVE);
        pThis->data_field.push_back(12);

        uint8_t pos1[2], pos2[2], vel1[2], vel2[2], force1[2], force2[2];

        if(flag){
            pos1[0] = (uint8_t)(pThis->repeat_position1 >> 8);
            pos1[1] = (uint8_t)pThis->repeat_position1;
            vel1[0] = (uint8_t)(pThis->cmd_vel1 >> 8);
            vel1[1] = (uint8_t)pThis->cmd_vel1;
            force1[0] = (uint8_t)(pThis->cmd_force1 >> 8);
            force1[1] = (uint8_t)pThis->cmd_force1;

            pos2[0] = (uint8_t)(pThis->cmd_pos2 >> 8);
            pos2[1] = (uint8_t)pThis->cmd_pos2;
            vel2[0] = (uint8_t)(pThis->cmd_vel2 >> 8);
            vel2[1] = (uint8_t)pThis->cmd_vel2;
            force2[0] = (uint8_t)(pThis->cmd_force2 >> 8);
            force2[1] = (uint8_t)pThis->cmd_force2;
        }
        else{
            pos1[0] = (uint8_t)(pThis->repeat_position2 >> 8);
            pos1[1] = (uint8_t)pThis->repeat_position2;
            vel1[0] = (uint8_t)(pThis->cmd_vel1 >> 8);
            vel1[1] = (uint8_t)pThis->cmd_vel1;
            force1[0] = (uint8_t)(pThis->cmd_force1 >> 8);
            force1[1] = (uint8_t)pThis->cmd_force1;

            pos2[0] = (uint8_t)(pThis->cmd_pos2 >> 8);
            pos2[1] = (uint8_t)pThis->cmd_pos2;
            vel2[0] = (uint8_t)(pThis->cmd_vel2 >> 8);
            vel2[1] = (uint8_t)pThis->cmd_vel2;
            force2[0] = (uint8_t)(pThis->cmd_force2 >> 8);
            force2[1] = (uint8_t)pThis->cmd_force2;
        }

        pThis->data_field.push_back(pos1[0]);
        pThis->data_field.push_back(pos1[1]);
        pThis->data_field.push_back(vel1[0]);
        pThis->data_field.push_back(vel1[1]);
        pThis->data_field.push_back(force1[0]);
        pThis->data_field.push_back(force1[1]);

        pThis->data_field.push_back(pos2[0]);
        pThis->data_field.push_back(pos2[1]);
        pThis->data_field.push_back(vel2[0]);
        pThis->data_field.push_back(vel2[1]);
        pThis->data_field.push_back(force2[0]);
        pThis->data_field.push_back(force2[1]);

        pThis->sendPacket();

        usleep(100000);
        while(!pThis->gripperStatus.transfer_ready){
            usleep(1000);
        }

        flag ^= true;
    }
}

void MainWindow::btnConnectClicked(){
    if (connectState){
        finish();

        appendMessage("Disonnected");
        enableComponent(false);
        plotTimer->stop();
        initGUI();
        init();
    }
    else{
        switch(comm_indx){
            case 1:
            {
                try{
                    if(ui->comboCommSpeed->currentIndex() == 0){
                        throw -1;
                    }
                    rs485_fd = tp_open_serial_port("/dev/ttyUSB_RS485", ui->comboCommSpeed->currentText().toUInt());
                    if(rs485_fd < 0){
                        throw -2;
                    }

                    connectState = true;

                    mlockall(MCL_CURRENT|MCL_FUTURE);

                    rt_print_auto_init(1);

                    rt_task_create(&comm_485_rx_task, "comm_rs485_rx_rt", 0, 99, 0);
                    rt_task_start(&comm_485_rx_task, &comm_485_rx_func, this);

                    rt_task_create(&comm_485_tx_task, "comm_rs485_tx_rt", 0, 99, 0);
                    rt_task_start(&comm_485_tx_task, &comm_485_tx_func, this);

                } catch (int err) {
                    if(err == -1){
                        QMessageBox::warning(this, tr("Communication Error"), tr("Please check the baud rate is correctly selected"), QMessageBox::Ok);
                    }
                    else if(err == -2){
                        QMessageBox::warning(this, tr("Communication Error"), tr("Please check the USB cable is correctly connected or communition type is properly selected to RS485"), QMessageBox::Ok);
                    }
                    else{

                    }
                }

                break;
            }
            case 2:
            {
                try {
                    if(ui->comboCommSpeed->currentIndex() == 0){
                        throw -1;
                    }

                    rtsercan_fd=RTSERCAN_open();
                    if (rtsercan_fd < 0){
                        throw -2;
                    }

                    char bit_rate[100];
                    std::string bit_rate_str = ui->comboCommSpeed->currentText().toStdString();
                    for(unsigned int i = 0; i < bit_rate_str.length(); i++){
                        bit_rate[i] = bit_rate_str[i];
                    }
                    SERCAN_SetBitRate(rtsercan_fd, bit_rate);
                    printf("\n");

                    connectState = true;

                    mlockall(MCL_CURRENT|MCL_FUTURE);

                    rt_print_auto_init(1);

                    rt_task_create(&comm_can_rx_task, "comm_can_rx_rt", 0, 99, 0);
                    rt_task_start(&comm_can_rx_task, &comm_can_rx_func, this);

                    rt_task_create(&comm_can_tx_task, "comm_can_tx_rt", 0, 99, 0);
                    rt_task_start(&comm_can_tx_task, &comm_can_tx_func, this);

                } catch (int err) {
                    if(err == -1){
                        QMessageBox::warning(this, tr("Communication Error"), tr("Please check the bit rate is correctly selected"), QMessageBox::Ok);
                    }
                    else if(err == -2){
                        QMessageBox::warning(this, tr("Communication Error"), tr("Please check the cable is correctly connected or communition type is properly selected to CAN"), QMessageBox::Ok);
                    }
                    else{

                    }
                }
                break;
            }
            case 3:
            {
                try {
                    throw -1;
                } catch (...) {
                    QMessageBox::warning(this, tr("Communication Error"), tr("Please check the cable is correctly connected or communition type is properly selected to EtherCAT"), QMessageBox::Ok);
                }
                break;
            }
            default:
                QMessageBox::warning(this, tr("Communication Type Error"), tr("Please select communition type\n(RS485 or CAN or EtherCAT)"), QMessageBox::Ok);
                break;
        }
        appendMessage("Connected");
        enableComponent(true);
        plotTimer->start();
//        pthread_create(&update_thread, NULL, update_func, this);
        pthread_create(&logging_thread, NULL, logging_func, this);
    }
    ui->btnConnect->setText(connectState ? "Disconnect" : "Connect");
}

void MainWindow::comm_485_rx_func(void *arg)
{
    MainWindow *pThis = static_cast<MainWindow*>(arg);

    rt_task_set_periodic(NULL, TM_NOW, 10e6);

    rt_printf("Start Comm RS485 RX...\n");

    pThis->comm_485_rx_run = true;

    int nbytes = 0, re_nbytes = 0;
    unsigned char chr[100] = {0,}, re_chr[100] = {0,};

    while(pThis->comm_485_rx_run){
        rt_task_wait_period(NULL);

        if(nbytes > 0){
            memset(re_chr, 0, sizeof(unsigned char)*RECV_MAX_LEN);
            re_nbytes = read(pThis->rs485_fd, re_chr, RECV_MAX_LEN);
            if(re_nbytes > 0) rt_printf("r-read data size : %d\n", re_nbytes);
        }
        if(re_nbytes == 0){
            memset(chr, 0, sizeof(unsigned char)*RECV_MAX_LEN);
            nbytes = read(pThis->rs485_fd, chr, RECV_MAX_LEN);
            if(nbytes > 0) rt_printf("read data size : %d\n", nbytes);
        }

        if(nbytes > 0){
            if(nbytes >= RECV_MAX_LEN){
                rt_printf("read data size : %d\n", nbytes);
                for(int i = 0; i < nbytes; i++){
                    rt_printf("%d ", chr[i]);
                    pThis->rxData[i] = chr[i];
                }
                rt_printf("\n");

                pThis->recv_data_size = nbytes;

                nbytes = 0;
                re_nbytes = 0;

                pThis->readPacket();
            }
            else{
                if(re_nbytes > 0){
                    rt_printf("read data size : %d\n", nbytes + re_nbytes);
                    for(int i = 0; i < nbytes; i++){
                        rt_printf("%d ", chr[i]);
                        pThis->rxData[i] = chr[i];
                    }
                    for(int i = 0; i < re_nbytes; i++){
                        rt_printf("%d ", re_chr[i]);
                        pThis->rxData[i + nbytes] = chr[i];
                    }
                    rt_printf("\n");

                    pThis->recv_data_size = nbytes + re_nbytes;

                    nbytes = 0;
                    re_nbytes = 0;

                    pThis->readPacket();
                }
            }
        }
    }

    rt_printf("Exit Comm RS485 RX...\n");
}

void MainWindow::comm_485_tx_func(void *arg)
{
    MainWindow *pThis = static_cast<MainWindow*>(arg);

    rt_task_set_periodic(NULL, TM_NOW, 10e6);

    rt_printf("Start Comm RS485 TX...\n");

    pThis->comm_485_tx_run = true;

    while(pThis->comm_485_tx_run){
        rt_task_wait_period(NULL);
        if(pThis->send_flag){
            write(pThis->rs485_fd, pThis->txData, pThis->send_data_size);
            usleep(2);
            tcflush(pThis->rs485_fd, TCIFLUSH);
            rt_printf("send data size : %d\t", pThis->send_data_size);
            for(int i = 0; i < pThis->send_data_size; i++){
                rt_printf("%d ", pThis->txData[i]);
            }
            rt_printf("\n");

            pThis->send_check = false;
            pThis->send_flag = false;
        }
    }

    rt_printf("Exit Comm RS485 TX...\n");
}

void MainWindow::comm_can_rx_func(void *arg)
{
    int res;
    CAN_FRAME RxFrame;
    MainWindow *pThis = static_cast<MainWindow*>(arg);

    rt_task_set_periodic(NULL, TM_NOW, 1e6);

    pThis->comm_can_rx_run = true;

    rt_printf("Start Comm CAN RX...\n");

    int indx = 0, indx2 = 0;;

    while(pThis->comm_can_rx_run)
    {
        rt_task_wait_period(NULL);

        res=RTSERCAN_read(pThis->rtsercan_fd, &RxFrame);
        if (res==SERCAN_ERR_FREE)
        {
//            rt_print_CANFrame(RxFrame);
            if(indx == 0 && RxFrame.data[0] == RX_SOP){
                memset(pThis->rxData, 0, sizeof(unsigned char)*RECV_MAX_LEN);
                indx2 = 0;
            }

            for(int i = 0; i < RxFrame.can_dlc; i++){
                pThis->rxData[indx2] = RxFrame.data[i];
//                rt_printf("%d ", pThis->rxData[indx2]);
                indx2++;
                if(indx2 == RECV_MAX_LEN){
                    break;
                }
            }
//            rt_printf("\n");

            if(pThis->rxData[0] == RX_SOP && pThis->rxData[indx2-1] == EOP && indx2 == RECV_MAX_LEN){
                pThis->recv_data_size = indx2;

                rt_printf("Recv Packet : \n");
                for(int i = 0; i < pThis->recv_data_size; i++){
                    rt_printf("%02X ", pThis->rxData[i]);
                }
                rt_printf("\n\n");

                indx = 0;

                pThis->readPacket();
            }
            else{
//                rt_printf("%d\n", indx);
//                rt_printf("%d\n", indx2);
                indx++;
            }
        }
    }

    rt_printf("Exit Comm CAN RX...\n");
}

void MainWindow::comm_can_tx_func(void *arg)
{
    CAN_FRAME cfame;
    MainWindow *pThis = static_cast<MainWindow*>(arg);

    rt_task_set_periodic(NULL, TM_NOW, 10e6);

    pThis->comm_can_tx_run = true;

    rt_printf("Start Comm CAN TX...\n");

    cfame.can_id = 0;
    cfame.can_dlc = 8;

    int send_cnt = 0;
    int send_cnt_max = 0;

    while(pThis->comm_can_tx_run)
    {
        rt_task_wait_period(NULL);

        if(pThis->send_flag){
            if(send_cnt == 0){
                rt_printf("Send Packet : \n");
            }
            send_cnt_max = pThis->send_data_size/cfame.can_dlc + 1;

            memset(cfame.data, 0, sizeof(unsigned char)*cfame.can_dlc);
            for(int j = 0; j < cfame.can_dlc; j++){
                cfame.data[j] = pThis->txData[send_cnt*cfame.can_dlc + j];
            }
            RTSERCAN_write(pThis->rtsercan_fd, cfame);
            rt_print_CANFrame(cfame);

            send_cnt++;

            if(send_cnt >= send_cnt_max){
                send_cnt = 0;
                rt_printf("\n");

//                pThis->send_check = false;
                pThis->send_flag = false;

                for(int i = 0; i < pThis->send_data_size; i++){
                    rt_printf("%02X ", pThis->txData[i]);
                }
                rt_printf("\n\n");
            }
        }
    }

    rt_printf("Exit Comm CAN TX...\n");
}

void MainWindow::comm_ecat_func(void *arg)
{

}

void MainWindow::btnPingClicked()
{
    data_field.clear();
    data_field.push_back(CMD_PING);
    data_field.push_back(0);

    sendPacket();
}

void MainWindow::btnInitClicked()
{
    data_field.clear();
    data_field.push_back(CMD_INIT);
    data_field.push_back(0);

    sendPacket();
}

void MainWindow::btnOrgClicked()
{
    data_field.clear();
    data_field.push_back(CMD_ORG);
    data_field.push_back(0);

    sendPacket();
}

void MainWindow::btnFeedbackOnClicked()
{
    data_field.clear();
    data_field.push_back(CMD_FEEDBACK);
    data_field.push_back(1);
    data_field.push_back(1);

    sendPacket();
}

void MainWindow::btnFeedbackOffClicked()
{
    data_field.clear();
    data_field.push_back(CMD_FEEDBACK);
    data_field.push_back(1);
    data_field.push_back(0);

    sendPacket();
}

void MainWindow::btnPneumaticOnClicked()
{
    data_field.clear();
    data_field.push_back(CMD_PNEUMATIC);
    data_field.push_back(1);
    data_field.push_back(1);

    sendPacket();
}

void MainWindow::btnPneumaticOffClicked()
{
    data_field.clear();
    data_field.push_back(CMD_PNEUMATIC);
    data_field.push_back(1);
    data_field.push_back(0);

    sendPacket();
}

void MainWindow::btnFingerOnClicked()
{
    data_field.clear();
    data_field.push_back(CMD_FINGER);
    data_field.push_back(1);
    data_field.push_back(1);

    sendPacket();
}

void MainWindow::btnFingerOffClicked()
{
    data_field.clear();
    data_field.push_back(CMD_FINGER);
    data_field.push_back(1);
    data_field.push_back(0);

    sendPacket();
}

void MainWindow::btnLoggingStartClicked()
{
    uint8_t logging_time = (uint8_t)ui->txtLoggingTime->text().toUShort();
    if(logging_time > 0){
        data_field.clear();
        data_field.push_back(CMD_LOGGING);
        data_field.push_back(1);
        data_field.push_back(logging_time);

        sendPacket();
    }
}

void MainWindow::btnTransmitClicked()
{
    moveGripper();
}

void MainWindow::btnRepeatOperatingClicked()
{
    repeat_position1 = ui->txtRepeatPosition1->text().toUInt();
    repeat_position2 = ui->txtRepeatPosition2->text().toUInt();
    repeat_count = (char)ui->txtRepeatCount->text().toShort();

    pthread_create(&repeat_thread, NULL, repeat_func, this);
}

void MainWindow::btnDIOSetClicked()
{
    dio_number = (unsigned char)ui->txtDIONum->text().toUInt();
    dio_position = ui->txtDIOPosition->text().toUInt();
    dio_force = ui->txtDIOForce->text().toUInt();
    dio_velocity = ui->txtDIOVelocity->text().toUInt();

    data_field.clear();
    data_field.push_back(CMD_DIO);
    data_field.push_back(7);

    uint8_t pos[2], vel[2], force[2];

    pos[0] = (uint8_t)(dio_position >> 8);
    pos[1] = (uint8_t)dio_position;
    force[0] = (uint8_t)(dio_force >> 8);
    force[1] = (uint8_t)dio_force;
    vel[0] = (uint8_t)(dio_velocity >> 8);
    vel[1] = (uint8_t)dio_velocity;

    data_field.push_back(dio_number);
    data_field.push_back(pos[0]);
    data_field.push_back(pos[1]);
    data_field.push_back(force[0]);
    data_field.push_back(force[1]);
    data_field.push_back(vel[0]);
    data_field.push_back(vel[1]);

    sendPacket();
}

//void MainWindow::btnConnectFingerClicked()
//{

//}

//void MainWindow::btnGripFingerClicked()
//{

//}

//void MainWindow::btnReleaseFingerClicked()
//{

//}

//void MainWindow::btnStopFingerClicked()
//{

//}

void MainWindow::cbOpPositionModeClicked(bool flag)
{
    if(flag){
        ui->cbOpForceMode->setChecked(false);

        ui->cbCurrentSensor->setChecked(true);
        ui->cbFingerSensor->setChecked(false);
        ui->frameForceOption->hide();

        data_field.clear();
        data_field.push_back(CMD_OP_MODE);
        data_field.push_back(2);
        data_field.push_back(0);
        data_field.push_back(0);

        sendPacket();
    }
    else{
        ui->cbOpPositionMode->setChecked(!flag);
    }
}

void MainWindow::cbOpForceModeClicked(bool flag)
{
    if(flag){
        ui->cbOpPositionMode->setChecked(false);
        ui->frameForceOption->show();

        data_field.clear();
        data_field.push_back(CMD_OP_MODE);
        data_field.push_back(2);
        data_field.push_back(1);
        data_field.push_back(0);

        sendPacket();
    }
    else{
        ui->cbOpForceMode->setChecked(!flag);
    }
}

void MainWindow::cbForceOptionCurrentSensorClicked(bool flag)
{
    if(flag){
        ui->cbFingerSensor->setChecked(false);

        data_field.clear();
        data_field.push_back(CMD_OP_MODE);
        data_field.push_back(2);
        data_field.push_back(1);
        data_field.push_back(0);

        sendPacket();
    }
    else{
        ui->cbCurrentSensor->setChecked(!flag);
    }
}

void MainWindow::cbForceOptionFingerSensorClicked(bool flag)
{
    if(flag){
        ui->cbCurrentSensor->setChecked(false);

        data_field.clear();
        data_field.push_back(CMD_OP_MODE);
        data_field.push_back(2);
        data_field.push_back(1);
        data_field.push_back(1);

        sendPacket();
    }
    else{
        ui->cbFingerSensor->setChecked(!flag);
    }
}

void MainWindow::hsliderVelocityValueChanged(int arg)
{
    QString objName = sender()->objectName();
    if(objName.contains("1")){
        cmd_vel1 = arg;
    }
    else{
        cmd_vel2 = arg;
    }
    ui->txtVelocity1->setNum(cmd_vel1);
    ui->txtVelocity2->setNum(cmd_vel2);
}

void MainWindow::hsliderPositionValueChanged(int arg)
{
    QString objName = sender()->objectName();
    if(objName.contains("1")){
        cmd_pos1 = arg;
    }
    else{
        cmd_pos2 = arg;
    }
    ui->txtPosition1->setNum(cmd_pos1);
    ui->txtPosition2->setNum(cmd_pos2);
}

void MainWindow::hsliderForceValueChanged(int arg)
{
    QString objName = sender()->objectName();
    if(objName.contains("1")){
        cmd_force1 = arg;
    }
    else{
        cmd_force2 = arg;
    }
    ui->txtForce1->setNum(cmd_force1);
    ui->txtForce2->setNum(cmd_force2);
}

void MainWindow::txtVelLimitEditingFinished()
{
    QString objName = sender()->objectName();
    int value = 0;
    if(objName.contains("1")){
        if(ui->hsliderVelocity1->maximum() != ui->txtVelocityLimit1->text().toInt()){
            ui->hsliderVelocity1->setMaximum(ui->txtVelocityLimit1->text().toInt());
            appendMessage("Changed the value of velocity1 limit to " + ui->txtVelocityLimit1->text());

            data_field.clear();
            data_field.push_back(CMD_M1_VEL_LIMIT);
            value = ui->txtVelocityLimit1->text().toInt();
        }
    }
    else{
        if(ui->hsliderVelocity2->maximum() != ui->txtVelocityLimit2->text().toInt()){
            ui->hsliderVelocity2->setMaximum(ui->txtVelocityLimit2->text().toInt());
            appendMessage("Changed the value of velocity2 limit to " + ui->txtVelocityLimit2->text());

            data_field.clear();
            data_field.push_back(CMD_M2_VEL_LIMIT);
            value = ui->txtVelocityLimit2->text().toInt();
        }
    }

    data_field.push_back(2);
    uint8_t upper, lower;
    upper = (uint8_t)(value >> 8);
    lower = (uint8_t)value;
    data_field.push_back(upper);
    data_field.push_back(lower);

    sendPacket();
}

void MainWindow::txtPosLimitEditingFinished()
{
    QString objName = sender()->objectName();
    if(objName.contains("1")){
        if(ui->hsliderPosition1->maximum() != ui->txtPositionLimit1->text().toInt()){
            ui->hsliderPosition1->setMaximum(ui->txtPositionLimit1->text().toInt());
            appendMessage("Changed the value of position1 limit to " + ui->txtPositionLimit1->text());
        }
    }
    else{
        if(ui->hsliderPosition2->maximum() != ui->txtPositionLimit2->text().toInt()){
            ui->hsliderPosition2->setMaximum(ui->txtPositionLimit2->text().toInt());
            appendMessage("Changed the value of position2 limit to " + ui->txtPositionLimit2->text());
        }
    }
}

void MainWindow::txtForceLimitEditingFinished()
{
    QString objName = sender()->objectName();
    int value = 0;
    if(objName.contains("1")){
        if(ui->hsliderForce1->maximum() != ui->txtForceLimit1->text().toInt()){
            ui->hsliderForce1->setMaximum(ui->txtForceLimit1->text().toInt());
            appendMessage("Changed the value of force1 limit to " + ui->txtForceLimit1->text());

            data_field.clear();
            data_field.push_back(CMD_M1_FORCE_LIMIT);
            value = ui->txtForceLimit1->text().toInt();
        }
    }
    else{
        if(ui->hsliderForce2->maximum() != ui->txtForceLimit2->text().toInt()){
            ui->hsliderForce2->setMaximum(ui->txtForceLimit2->text().toInt());
            appendMessage("Changed the value of force2 limit to " + ui->txtForceLimit2->text());

            data_field.clear();
            data_field.push_back(CMD_M2_FORCE_LIMIT);
            value = ui->txtForceLimit2->text().toInt();
        }
    }

    data_field.push_back(2);
    uint8_t upper, lower;
    upper = (uint8_t)(value >> 8);
    lower = (uint8_t)value;
    data_field.push_back(upper);
    data_field.push_back(lower);

    sendPacket();
}

void MainWindow::comboPlotCurrentIndexChanged(int index)
{
    plotData.indx = index;
}

void MainWindow::readPacket(){
    msgString.clear();
    msgString.append("RX Data : ");
    for(int i = 0; i < recv_data_size; i++){
        msgString.push_back(rxData[i]);
    }
    appendMessage(QString::fromStdString(msgString));

    int size = recv_data_size;
    uint8_t checksum = 0;
    uint8_t* rx_packet = new uint8_t[size];
    for(int i = 0; i < size; i++){
        rx_packet[i] = rxData[i];
        if(i > 0 && i < size - 2){
            checksum += rx_packet[i];
        }
    }
    checksum = ~checksum;
    int indx = 1;
    if(rx_packet[0] == RX_SOP && rx_packet[size-1] == EOP){
//        if(checksum == rx_packet[size-2])
        {
            appendMessage("Confirmed validity of received packet");
            gripperStatus.pos[0] = rx_packet[indx]*256 + rx_packet[indx+1];
            indx += 2;
            gripperStatus.vel[0] = rx_packet[indx]*256 + rx_packet[indx+1];
            indx += 2;
            gripperStatus.force[0] = rx_packet[indx]*256 + rx_packet[indx+1];
            indx += 2;
            gripperStatus.pos[1] = rx_packet[indx]*256 + rx_packet[indx+1];
            indx += 2;
            gripperStatus.vel[1] = rx_packet[indx]*256 + rx_packet[indx+1];
            indx += 2;
            gripperStatus.force[1] = rx_packet[indx]*256 + rx_packet[indx+1];
            indx += 2;
            gripperStatus.finger[0] = rx_packet[indx]*256 + rx_packet[indx+1];
            indx += 2;
            gripperStatus.finger[1] = rx_packet[indx]*256 + rx_packet[indx+1];
            indx += 2;
            gripperStatus.finger[2] = rx_packet[indx]*256 + rx_packet[indx+1];
            indx += 2;

            for(int i = 0; i < 6; i++){
                gripperStatus.ft_sensor[i] = (int16_t)(rx_packet[indx]*256 + rx_packet[indx+1]);
                indx += 2;
            }

            for(int i = 0; i < 6; i++){
                gripperStatus.RCC_sensor[i] = rx_packet[indx++];
            }

            gripperStatus.status = rx_packet[indx]*256 + rx_packet[indx+1];
        }

        statusParsing();
        updateGripperInformation();
    }
    else{
        ui->txtError->setText("Occurred Error");
        ui->txtErrorMsg->setText("Checksum error");
    }
}

uchar MainWindow::checksum(vector<uchar> data)
{
    uint8_t checksum = 0;
    for(uint i = 0; i < data.size(); i++){
        checksum += data[i];
    }

    checksum = ~checksum;

    return checksum;
}

void MainWindow::appendMessage(QString msg)
{
    QString timeFormat = "yyyy-MM-dd HH:mm:ss";
    QDateTime currentDateTime = QDateTime::currentDateTime();
    loggingString = currentDateTime.toString(timeFormat).toStdString() + " " + msg.toStdString();
    logging_flag = true;
//    ui->txtMessage->append(currentDateTime.toString(timeFormat) + " " + msg);

//    QScrollBar *sb = ui->txtMessage->verticalScrollBar();
//    sb->setValue(sb->maximum());
}

void MainWindow::enableComponent(bool enable)
{
    ui->gbOption->setEnabled(enable);
    ui->gbMessage->setEnabled(enable);
    ui->gbOperating1->setEnabled(enable);
    ui->gbOperating2->setEnabled(enable);
    ui->gbStatus->setEnabled(enable);
    ui->gbDataVisualization->setEnabled(enable);
    ui->gbRepeatOperating->setEnabled(enable);
    ui->gbDIO->setEnabled(enable);
}

void MainWindow::initGUI()
{
    ui->gbOption->setEnabled(false);
    ui->gbMessage->setEnabled(false);
    ui->gbOperating1->setEnabled(false);
    ui->gbOperating2->setEnabled(false);
    ui->gbStatus->setEnabled(false);
    ui->gbDataVisualization->setEnabled(false);
    ui->gbRepeatOperating->setEnabled(false);
    ui->gbDIO->setEnabled(false);

    ui->frameForceOption->hide();
    ui->cbOpPositionMode->setChecked(true);
    ui->cbOpForceMode->setChecked(false);
    ui->cbCurrentSensor->setChecked(true);
    ui->cbFingerSensor->setChecked(false);

    memset(&gripperStatus, 0, sizeof(gripper_status));

    plotData.data_indx = 0;
    plotData.x.clear();
    //    plotData.y.clear();
    plotData.gripper_pos1.clear();
    plotData.gripper_pos2.clear();
    plotData.gripper_vel1.clear();
    plotData.gripper_vel2.clear();
    plotData.gripper_force1.clear();
    plotData.gripper_force2.clear();
    plotData.fx.clear();
    plotData.fy.clear();
    plotData.fz.clear();
    plotData.tx.clear();
    plotData.ty.clear();
    plotData.tz.clear();
    plotData.finger_sensor1.clear();
    plotData.finger_sensor2.clear();
    plotData.RCC_data1.clear();
    plotData.RCC_data2.clear();
    plotData.RCC_data3.clear();
    plotData.RCC_data4.clear();
    plotData.RCC_data5.clear();
    plotData.RCC_data6.clear();
}

void MainWindow::dataTransferCheck()
{
    if(connectState){
        int indx = 0;
        memset(txData, 0, sizeof(unsigned char)*MAX_RECV_BUFFER_SIZE);
        txData[indx++] = TX_SOP;
        txData[indx++] = CMD_DATA_TRANSFER;
        uint8_t checksum = CMD_DATA_TRANSFER;
        checksum = ~checksum;
        txData[indx++] = checksum;
        txData[indx++] = EOP;
        send_data_size = indx;

        send_flag = true;
        send_check = true;

        gripperStatus.transfer_ready = false;
        pthread_create(&update_thread, NULL, update_func, this);

        msgString.clear();
        msgString.append("TX Data : ");
        for(int i = 0; i < indx; i++){
            msgString.push_back(txData[i]);
        }
        appendMessage(QString::fromStdString(msgString));
    }
}

void MainWindow::sendPacket()
{
    print_flag = true;
    dataTransferCheck();
}

void MainWindow::moveGripper()
{
    data_field.clear();
    data_field.push_back(CMD_MOVE);
    data_field.push_back(12);

    uint8_t pos1[2], pos2[2], vel1[2], vel2[2], force1[2], force2[2];

    pos1[0] = (uint8_t)(cmd_pos1 >> 8);
    pos1[1] = (uint8_t)cmd_pos1;
    vel1[0] = (uint8_t)(cmd_vel1 >> 8);
    vel1[1] = (uint8_t)cmd_vel1;
    force1[0] = (uint8_t)(cmd_force1 >> 8);
    force1[1] = (uint8_t)cmd_force1;

    pos2[0] = (uint8_t)(cmd_pos2 >> 8);
    pos2[1] = (uint8_t)cmd_pos2;
    vel2[0] = (uint8_t)(cmd_vel2 >> 8);
    vel2[1] = (uint8_t)cmd_vel2;
    force2[0] = (uint8_t)(cmd_force2 >> 8);
    force2[1] = (uint8_t)cmd_force2;

    data_field.push_back(pos1[0]);
    data_field.push_back(pos1[1]);
    data_field.push_back(vel1[0]);
    data_field.push_back(vel1[1]);
    data_field.push_back(force1[0]);
    data_field.push_back(force1[1]);

    data_field.push_back(pos2[0]);
    data_field.push_back(pos2[1]);
    data_field.push_back(vel2[0]);
    data_field.push_back(vel2[1]);
    data_field.push_back(force2[0]);
    data_field.push_back(force2[1]);

    sendPacket();
}

void MainWindow::updateGripperInformation()
{
    ui->txtMotorOn->setText(gripperStatus.motor_on ? "On" : "Off");
    ui->txtInitComplete->setText(gripperStatus.init_complete ? "Complete" : "No Initialization");
    ui->txtInMotion->setText(gripperStatus.in_motion ? "In Motion" : "No Motion");
    ui->txtMoveComplete->setText(gripperStatus.move_complete ? "Move Complete" : "Not Move");
    ui->txtFeedbackStatus->setText(gripperStatus.feedback_status ? "On" : "Off");
    ui->txtOpMode->setText(gripperStatus.operating_mode == 0 ? "Position Mode" : "Force Mode");
    if(gripperStatus.operating_mode == 0){
        ui->txtForceSensingOption->setText("Not use force operating mode");
    }
    else{
        ui->txtForceSensingOption->setText(gripperStatus.force_sensing_option == 0 ? "Current Sensing" : "Finger Tip Sensing");
    }
    ui->txtPneumaticStatus->setText(gripperStatus.pneumatic_status ? "On" : "Off");
    ui->txtFingerTipPowerStatus->setText(gripperStatus.finger_tip_power_status ? "On" : "Off");
    ui->txtLogging->setText(gripperStatus.logging ? "On" : "Off");
    ui->txtFingerType->setText(gripperStatus.finger_type == 1 ? "3 - Finger Gripper" : "2 - Finger Gripper");
    ui->txtError->setText(gripperStatus.error ? "Occurred Error" : "No Error");

    gripperStatus.error_status = ((uint8_t)gripperStatus.status)&0x08;
    errorParsing();

    gripperStatus.fx = ((double)gripperStatus.ft_sensor[0]) / 50;
    gripperStatus.fy = ((double)gripperStatus.ft_sensor[1]) / 50;
    gripperStatus.fz = ((double)gripperStatus.ft_sensor[2]) / 50;
    gripperStatus.tx = ((double)gripperStatus.ft_sensor[3]) / 2000;
    gripperStatus.ty = ((double)gripperStatus.ft_sensor[4]) / 2000;
    gripperStatus.tz = ((double)gripperStatus.ft_sensor[5]) / 2000;

    ui->txtFX->setText(QString::number(gripperStatus.fx));
    ui->txtFY->setText(QString::number(gripperStatus.fy));
    ui->txtFZ->setText(QString::number(gripperStatus.fz));
    ui->txtTX->setText(QString::number(gripperStatus.tx));
    ui->txtTY->setText(QString::number(gripperStatus.ty));
    ui->txtTZ->setText(QString::number(gripperStatus.tz));

    ui->txtRCC1->setText(QString::number(gripperStatus.RCC_sensor[0]));
    ui->txtRCC2->setText(QString::number(gripperStatus.RCC_sensor[1]));
    ui->txtRCC3->setText(QString::number(gripperStatus.RCC_sensor[2]));
    ui->txtRCC4->setText(QString::number(gripperStatus.RCC_sensor[3]));
    ui->txtRCC5->setText(QString::number(gripperStatus.RCC_sensor[4]));
    ui->txtRCC6->setText(QString::number(gripperStatus.RCC_sensor[5]));

    ui->txtFinger1->setText(QString::number(gripperStatus.finger[0]));
    ui->txtFinger2->setText(QString::number(gripperStatus.finger[1]));
    ui->txtFinger3->setText(QString::number(gripperStatus.finger[2]));

    if(gripperStatus.finger_type == 1){
        ui->txtFinger3->show();
    }
    else{
        ui->txtFinger3->hide();
    }
}

void MainWindow::statusParsing()
{
    gripperStatus.motor_on = (gripperStatus.status & STATUS_MOTOR_ON) == STATUS_MOTOR_ON ? true : false;
    gripperStatus.init_complete = (gripperStatus.status & STATUS_INIT_COMPLETE) == STATUS_INIT_COMPLETE ? true : false;
    gripperStatus.in_motion = (gripperStatus.status & STATUS_IN_MOTION) == STATUS_IN_MOTION ? true : false;
    gripperStatus.move_complete = (gripperStatus.status & STATUS_MOVE_COMPLETE) == STATUS_MOVE_COMPLETE ? true : false;
    gripperStatus.feedback_status = (gripperStatus.status & STATUS_FEEDBACK) == STATUS_FEEDBACK ? true : false;
    gripperStatus.operating_mode = (gripperStatus.status & STATUS_OP_MODE) == STATUS_OP_MODE ? 1 : 0;
    gripperStatus.force_sensing_option = (gripperStatus.status & STATUS_FORCE_OPTION) == STATUS_FORCE_OPTION ? 1 : 0;
    gripperStatus.pneumatic_status = (gripperStatus.status & STATUS_PNEUMATIC) == STATUS_PNEUMATIC ? true : false;
    gripperStatus.finger_tip_power_status = (gripperStatus.status & STATUS_FINGER_POWER) == STATUS_FINGER_POWER ? true : false;
    gripperStatus.logging = (gripperStatus.status & STATUS_LOGGING) == STATUS_LOGGING ? true : false;
    gripperStatus.transfer_ready = (gripperStatus.status & STATUS_TRANSFER_READY) == STATUS_TRANSFER_READY ? true : false;
    gripperStatus.finger_type = (gripperStatus.status & STATUS_FINGER_TYPE) == STATUS_FINGER_TYPE ? 1 : 0;
    gripperStatus.error = (gripperStatus.status & STATUS_ERROR) == STATUS_ERROR ? true : false;
}

void MainWindow::errorParsing()
{
    QString error_msg;
    switch(gripperStatus.error_status){
        case 0b000:
            error_msg = "No Error";
            break;
        case 0b001:
            error_msg = "Motor is switched off";
            break;
        case 0b010:
            error_msg = "Selected force control option finger tip sensor, but not connected finger tip sensor";
            break;
        case 0b011:
            error_msg = "Positions not available";
            break;
        case 0b100:
            error_msg = "Grip force not available";
            break;
        case 0b101:
            error_msg = "Move speed vlaue not available";
            break;
        case 0b110:
            error_msg = "Sensor disconnected(F/T sensor, RCC sensor, Finger tip sensor)";
            break;
        case 0b111:
            error_msg = "Internal system error";
            break;
        default:
            break;
    }
    ui->txtErrorMsg->setText(error_msg);
}

void MainWindow::plotDataUpdate()
{
    plotData.data_indx += 1.0;
    plotData.x.push_back(plotData.data_indx);

    plotData.gripper_pos1.push_back(gripperStatus.pos[0]);
    plotData.gripper_vel1.push_back(gripperStatus.vel[0]);
    plotData.gripper_force1.push_back(gripperStatus.force[0]);
    plotData.gripper_pos2.push_back(gripperStatus.pos[1]);
    plotData.gripper_vel2.push_back(gripperStatus.vel[1]);
    plotData.gripper_force2.push_back(gripperStatus.force[1]);
    plotData.fx.push_back(gripperStatus.fx);
    plotData.fy.push_back(gripperStatus.fy);
    plotData.fz.push_back(gripperStatus.fz);
    plotData.tx.push_back(gripperStatus.tx);
    plotData.ty.push_back(gripperStatus.ty);
    plotData.tz.push_back(gripperStatus.tz);
    plotData.finger_sensor1.push_back(gripperStatus.finger[0]);
    plotData.finger_sensor2.push_back(gripperStatus.finger[1]);
    plotData.RCC_data1.push_back(gripperStatus.RCC_sensor[0]);
    plotData.RCC_data2.push_back(gripperStatus.RCC_sensor[1]);
    plotData.RCC_data3.push_back(gripperStatus.RCC_sensor[2]);
    plotData.RCC_data4.push_back(gripperStatus.RCC_sensor[3]);
    plotData.RCC_data5.push_back(gripperStatus.RCC_sensor[4]);
    plotData.RCC_data6.push_back(gripperStatus.RCC_sensor[5]);
}

void MainWindow::plotUpdateTimeout(){
    QVector<double> y;
    plotDataUpdate();

    switch(plotData.indx){
        case 1:
            //            y.append(plotData.gripper_pos1);
            y = plotData.gripper_pos1;
            break;
        case 2:
            //            y.append(plotData.gripper_vel1);
            y = plotData.gripper_vel1;
            break;
        case 3:
            //            y.append(plotData.gripper_force1);
            y = plotData.gripper_force1;
            break;
        case 4:
            //            y.append(plotData.gripper_pos2);
            y = plotData.gripper_pos2;
            break;
        case 5:
            //            y.append(plotData.gripper_vel2);
            y = plotData.gripper_vel2;
            break;
        case 6:
            //            y.append(plotData.gripper_force2);
            y = plotData.gripper_force2;
            break;
        case 7:
            //            y.append(plotData.fx);
            y = plotData.fx;
            break;
        case 8:
            //            y.append(plotData.fy);
            y = plotData.fy;
            break;
        case 9:
            //            y.append(plotData.fz);
            y = plotData.fz;
            break;
        case 10:
            //            y.append(plotData.tx);
            y = plotData.tx;
            break;
        case 11:
            //            y.append(plotData.ty);
            y = plotData.ty;
            break;
        case 12:
            //            y.append(plotData.tz);
            y = plotData.tz;
            break;
        case 13:
            //            y.append(plotData.finger_sensor1);
            y = plotData.finger_sensor1;
            break;
        case 14:
            //            y.append(plotData.finger_sensor2);
            y = plotData.finger_sensor2;
            break;
        case 15:
            //            y.append(plotData.RCC_data1);
            y = plotData.RCC_data1;
            break;
        case 16:
            //            y.append(plotData.RCC_data2);
            y = plotData.RCC_data2;
            break;
        case 17:
            //            y.append(plotData.RCC_data3);
            y = plotData.RCC_data3;
            break;
        case 18:
            //            y.append(plotData.RCC_data4);
            y = plotData.RCC_data4;
            break;
        case 19:
            //            y.append(plotData.RCC_data5);
            y = plotData.RCC_data5;
            break;
        case 20:
            //            y.append(plotData.RCC_data6);
            y = plotData.RCC_data6;
            break;
    }

    if(y.size() > 0){
        //        ui->plot->graph(0)->setName(plotItems[plotData.indx]);
        ui->plot->graph(0)->setData(plotData.x, y);
        QVector<double> y_temp;
        //        y_temp.append(y);
        y_temp = y;
        sort(y_temp.begin(), y_temp.end());
        ui->plot->xAxis->setRange(plotData.x.front(), plotData.x.back());
        ui->plot->yAxis->setRange(y_temp.front(), y_temp.back());
        ui->plot->replot();
        ui->plot->yAxis->setLabel(plotItems[plotData.indx] + " " + plotUnit[plotData.indx]);
    }
}
