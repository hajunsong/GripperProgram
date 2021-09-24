#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "customsettings.h"

CustomSettings::CustomSettings(void *_ui)
{
    ui = static_cast<Ui::MainWindow*>(_ui);
}

void CustomSettings::saveConfigFile()
{
    QSettings settings(configFile(), QSettings::IniFormat);

    bool comm_485 = static_cast<Ui::MainWindow*>(ui)->rbRS485->isChecked();
    bool comm_can = static_cast<Ui::MainWindow*>(ui)->rbCAN->isChecked();
    bool comm_ecat = static_cast<Ui::MainWindow*>(ui)->rbECAT->isChecked();
    int combo_comm_speed_indx = static_cast<Ui::MainWindow*>(ui)->comboCommSpeed->currentIndex();

    QStringList limit;
    limit.append(static_cast<Ui::MainWindow*>(ui)->txtVelocityLimit1->text());
    limit.append(static_cast<Ui::MainWindow*>(ui)->txtPositionLimit1->text());
    limit.append(static_cast<Ui::MainWindow*>(ui)->txtForceLimit1->text());
    limit.append(static_cast<Ui::MainWindow*>(ui)->txtVelocityLimit2->text());
    limit.append(static_cast<Ui::MainWindow*>(ui)->txtPositionLimit2->text());
    limit.append(static_cast<Ui::MainWindow*>(ui)->txtForceLimit2->text());

    settings.setValue("LIMIT", limit);
    settings.setValue("COMM_485_CHECKED", comm_485);
    settings.setValue("COMM_can_CHECKED", comm_can);
    settings.setValue("COMM_ecat_CHECKED", comm_ecat);
    settings.setValue("COMBO_COMM_SPEED_INDX", combo_comm_speed_indx);

    settings.sync();
}

void CustomSettings::loadConfigFile()
{
    if (!QFile::exists(configFile())) return;

    QSettings settings(configFile(), QSettings::IniFormat);

    QStringList limit = settings.value("LIMIT").toStringList();
    if(limit.length() == 6){
        static_cast<Ui::MainWindow*>(ui)->txtVelocityLimit1->setText(limit[0]);
        static_cast<Ui::MainWindow*>(ui)->txtPositionLimit1->setText(limit[1]);
        static_cast<Ui::MainWindow*>(ui)->txtForceLimit1->setText(limit[2]);
        static_cast<Ui::MainWindow*>(ui)->txtVelocityLimit2->setText(limit[3]);
        static_cast<Ui::MainWindow*>(ui)->txtPositionLimit2->setText(limit[4]);
        static_cast<Ui::MainWindow*>(ui)->txtForceLimit2->setText(limit[5]);
    }

    bool comm_485 = settings.value("COMM_485_CHECKED").toBool();
    bool comm_can = settings.value("COMM_can_CHECKED").toBool();
    bool comm_ecat = settings.value("COMM_ecat_CHECKED").toBool();
    int combo_comm_speed_indx = settings.value("COMBO_COMM_SPEED_INDX").toInt();

    static_cast<Ui::MainWindow*>(ui)->rbRS485->setChecked(comm_485);
    static_cast<Ui::MainWindow*>(ui)->rbCAN->setChecked(comm_can);
    static_cast<Ui::MainWindow*>(ui)->rbECAT->setChecked(comm_ecat);
    static_cast<Ui::MainWindow*>(ui)->comboCommSpeed->setCurrentIndex(combo_comm_speed_indx);

    if(comm_485) static_cast<Ui::MainWindow*>(ui)->rbRS485->animateClick();
    if(comm_can) static_cast<Ui::MainWindow*>(ui)->rbCAN->animateClick();
    if(comm_ecat) static_cast<Ui::MainWindow*>(ui)->rbECAT->animateClick();


}

QString CustomSettings::configFile()
{
    QString filePath = qApp->applicationDirPath() + "/config.ini";
    return filePath;
}
