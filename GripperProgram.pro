QT += core gui widgets printsupport

CONFIG += c++11 console

include($$PWD/NRMKLib.pri)

target.path = /home/user/Project/Gripper
INSTALLS += target

SOURCES += \
    customsettings.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    logger.cpp \

HEADERS += \
    customsettings.h \
    mainwindow.h \
    qcustomplot.h \
    logger.h \

FORMS += \
    mainwindow.ui

LIBS += \
    -L$$NRMK_LIB_PATH/ -lNRMKHelperi686 \
    -L$$NRMK_POCO_LIB_PATH/i686/ -lPocoNet \
    -L$$NRMK_POCO_LIB_PATH/i686/ -lPocoFoundation \
    -L$$NRMK_XENO_PATH/lib/ -lnative \
#    -L$$NRMK_XENO_PATH/lib/ -lrtdm \
    -L$$NRMK_XENO_PATH/lib/ -lxenomai \
    -L$$NRMK_LIB_PATH/ -lNRMKhw_tp \

INCLUDEPATH += \
    $$NRMK_PATH \
    $$NRMK_POCO_LIB_PATH/i686 \
    $$NRMK_POCO_INC_PATH \
    $$NRMK_XENO_PATH/include \
    $$NRMK_ECAT_PATH/include \
    $$NRMK_CORE_INC_PATH \
    $$NRMK_HELPER_INC_PATH \
    $$NRMK_HELPER_INC_PATH/hw \
    $$NRMK_HELPER_INC_PATH/hw/nrmk_ecat \

DEPENDPATH += \
    $$NRMK_PATH \
    $$NRMK_POCO_LIB_PATH/i686 \
    $$NRMK_POCO_INC_PATH \
    $$NRMK_XENO_PATH/include \
    $$NRMK_ECAT_PATH/include \
    $$NRMK_CORE_INC_PATH \
    $$NRMK_HELPER_INC_PATH \
    $$NRMK_HELPER_INC_PATH/hw \
    $$NRMK_HELPER_INC_PATH/hw/nrmk_ecat \

PRE_TARGETDEPS += \
    $$NRMK_LIB_PATH/libNRMKHelperi686.a \

LIBS += -L$$PWD/../../../../opt/neuromeka/NRMKPlatformPC2/bin/i686/xenomai/lib/ -lrtdm

INCLUDEPATH += $$PWD/../../../../opt/neuromeka/NRMKPlatformPC2/bin/i686/xenomai/include
DEPENDPATH += $$PWD/../../../../opt/neuromeka/NRMKPlatformPC2/bin/i686/xenomai/include

PRE_TARGETDEPS += $$PWD/../../../../opt/neuromeka/NRMKPlatformPC2/bin/i686/xenomai/lib/librtdm.a
