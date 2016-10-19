#-------------------------------------------------
#
# Project created by QtCreator 2016-10-04T01:29:55
#
#-------------------------------------------------

QT       += core gui
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HW1_ImageViewer
TEMPLATE = app


SOURCES += main.cpp\
           mainwindow.cpp\
           aboutdialog.cpp


HEADERS  += mainwindow.h \
            aboutdialog.h
            aboutdialog.h

FORMS    += mainwindow.ui \
            aboutdialog.ui
            aboutdialog.ui

INCLUDEPATH += C:/opencv_310_mingw32/include \
                    C:/opencv_310_mingw32/include/opencv \
                    C:/opencv_310_mingw32/include/opencv2

LIBS += -LC:/opencv_310_mingw32/x86/mingw/lib \
            -lopencv_calib3d310 \
            -lopencv_flann310 \
            -lopencv_imgproc310 \
            -lopencv_photo310 \
            -lopencv_superres310 \
            -lopencv_videostab310 \
            -lopencv_core310 \
            -lopencv_highgui310 \
            -lopencv_ml310 \
            -lopencv_shape310 \
            -lopencv_videoio310 \
            -lopencv_features2d310 \
            -lopencv_imgcodecs310 \
            -lopencv_objdetect310 \
            -lopencv_stitching310 \
            -lopencv_video310

