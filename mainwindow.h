#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QVector>
#include <QTimer>

#include "customsettings.h"
#include "logger.h"

#include <iostream>

using namespace std;

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <native/task.h>
#include <native/timer.h>
#include <rtdk.h>

#include "NRMKhw_tp.h"
#include "NRMKsercan_tp.h"

#include <pthread.h>

namespace Ui { class MainWindow; }

const uint8_t RX_SOP = 0xFA;
const uint8_t TX_SOP = 0xFB;
const uint8_t EOP = 0xFE;

const uint8_t CMD_PING = 0xC1;
const uint8_t CMD_INIT = 0xC2;
const uint8_t CMD_FEEDBACK = 0xC3;
const uint8_t CMD_OP_MODE = 0xC4;
const uint8_t CMD_PNEUMATIC = 0xC5;
const uint8_t CMD_FINGER = 0xC6;
const uint8_t CMD_M1_VEL_LIMIT = 0xC7;
const uint8_t CMD_M1_FORCE_LIMIT = 0xC8;
const uint8_t CMD_M2_VEL_LIMIT = 0xC9;
const uint8_t CMD_M2_FORCE_LIMIT = 0xCA;
const uint8_t CMD_MOVE = 0xCB;
const uint8_t CMD_LOGGING = 0xCC;
const uint8_t CMD_DATA_TRANSFER = 0xCD;

const uint16_t STATUS_MOTOR_ON = 0x8000;
const uint16_t STATUS_INIT_COMPLETE = 0x4000;
const uint16_t STATUS_IN_MOTION = 0x2000;
const uint16_t STATUS_MOVE_COMPLETE = 0x1000;
const uint16_t STATUS_FEEDBACK = 0x0800;
const uint16_t STATUS_OP_MODE = 0x0400;
const uint16_t STATUS_FORCE_OPTION = 0x0200;
const uint16_t STATUS_PNEUMATIC = 0x0100;
const uint16_t STATUS_FINGER_POWER = 0x0080;
const uint16_t STATUS_LOGGING = 0x0040;
const uint16_t STATUS_TRANSFER_READY = 0x0020;
const uint16_t STATUS_FINGER_TYPE = 0x0010;
const uint16_t STATUS_ERROR = 0x0008;

const uint8_t RECV_MAX_LEN = 46;

typedef struct gripper_status{
    uint16_t pos[2], vel[2], force[2], finger[3], RCC_sensor[6], status;
    uint8_t finger_counts;
    int16_t ft_sensor[6];
    bool motor_on, init_complete, in_motion, move_complete, feedback_status, pneumatic_status, finger_tip_power_status, logging, error, transfer_ready;
    uint8_t operating_mode, force_sensing_option, finger_type;
    uint8_t error_status;
    double fx, fy, fz, tx, ty, tz;
}gripper_status;

typedef struct plot_data{
    double data_indx;
    int indx;
    QVector<double> x;
    QVector<double> fx, fy, fz, tx, ty, tz;
    QVector<double> gripper_pos1, gripper_pos2, gripper_vel1, gripper_vel2, gripper_force1, gripper_force2;
    QVector<double> finger_sensor1, finger_sensor2;
    QVector<double> RCC_data1, RCC_data2, RCC_data3, RCC_data4, RCC_data5, RCC_data6;
}plot_data;

const QList<QString> plotItems = {"", "Gripper Position 1", "Gripper Velocity 1", "Gripper Force 1", "Gripper Position 2", "Gripper Velocity 2", "Gripper Force 2",
                                  "F/T Sensor FX", "F/T Sensor FY", "F/T Sensor FZ", "F/T Sensor TX", "F/T Sensor TY", "F/T Sensor TZ", "Finger Tip Sensor 1", "Finger Tip Sensor 2",
                                  "RCC Data 1", "RCC Data 2", "RCC Data 3", "RCC Data 4", "RCC Data 5", "RCC Data 6"};
const QList<QString> plotUnit = {"", "[mm]", "[mm/s]", "[N]", "[mm]", "[mm/s]", "[N]", "[N]", "[N]", "[N]", "[Nm]", "[Nm]", "[Nm]", "[Nm]", "[Nm]", "[]", "[]", "[]", "[]", "[]", "[]"};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // button event
    void btnConnectClicked();
    void btnPingClicked();
    void btnInitClicked();
    void btnFeedbackOnClicked();
    void btnFeedbackOffClicked();
    void btnPneumaticOnClicked();
    void btnPneumaticOffClicked();
    void btnFingerOnClicked();
    void btnFingerOffClicked();
    void btnLoggingStartClicked();
    void btnTransmitClicked();
//    void btnConnectFingerClicked();
//    void btnGripFingerClicked();
//    void btnReleaseFingerClicked();
//    void btnStopFingerClicked();

    // radio button event
    void rbRS485Clicked(bool arg);
    void rbCANClicked(bool arg);
    void rbECATClicked(bool arg);

    // check box event
    void cbOpPositionModeClicked(bool flag);
    void cbOpForceModeClicked(bool flag);
    void cbForceOptionCurrentSensorClicked(bool flag);
    void cbForceOptionFingerSensorClicked(bool flag);

    // horizontal slider bar event
    void hsliderVelocityValueChanged(int arg);
    void hsliderPositionValueChanged(int arg);
    void hsliderForceValueChanged(int arg);

    // text edit event
    void txtVelLimitEditingFinished();
    void txtPosLimitEditingFinished();
    void txtForceLimitEditingFinished();

    // combo box event
    void comboPlotCurrentIndexChanged(int index);

    // timer event
    void plotUpdateTimeout();
//    void updateTimeout();

private:
    Ui::MainWindow *ui;

    void closeEvent(QCloseEvent *event);

    bool comm_485, comm_can, comm_ecat;
    int comm_indx;
    pthread_t update_thread, logging_thread;
    RT_TASK comm_485_rx_task, comm_485_tx_task, comm_can_rx_task, comm_can_tx_task, comm_ecat_task;
    bool update_thread_run, logging_thread_run, comm_ecat_run, comm_485_tx_run, comm_485_rx_run, comm_can_tx_run, comm_can_rx_run;
    bool connectState;
    std::string txString, rxString;
    int rtsercan_fd, rs485_fd;
    CustomSettings *customSettings;
    vector<unsigned char> data_field;
    QString currentDateTime;
    int cmd_vel1, cmd_force1, cmd_pos1, cmd_vel2, cmd_force2, cmd_pos2;
    gripper_status gripperStatus;
    plot_data plotData;
    bool send_check, print_flag, send_flag;
    int send_data_size, recv_data_size;
    QTimer *plotTimer;
    unsigned char txData[MAX_RECV_BUFFER_SIZE], rxData[MAX_RECV_BUFFER_SIZE];
    std::string msgString, loggingString;
    bool logging_flag;
    Logger *logger;

    static void* update_func(void *arg);
    static void* logging_func(void *arg);
    static void comm_485_rx_func(void *arg);
    static void comm_485_tx_func(void *arg);
    static void comm_can_rx_func(void *arg);
    static void comm_can_tx_func(void *arg);
    static void comm_ecat_func(void *arg);

    void init();
    void finish();
    uchar checksum(vector<uchar> data);
    void appendMessage(QString msg);
    void enableComponent(bool enable);
    void initGUI();
    void dataTransferCheck();
    void sendPacket();
    void moveGripper();
    void updateGripperInformation();
    void statusParsing();
    void errorParsing();
    void plotDataUpdate();
    void readPacket();
};
#endif // MAINWINDOW_H
