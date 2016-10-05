#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "opencv.hpp"
#include <QString>
#include <QDebug>
#include <QFile>
#include <QTimer>

#include <QMainWindow>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>

using namespace QtCharts;
using namespace std;

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QDialog aboutbox;

    char image[64][64] ;  // Array to store a 64x64 image
    int histogram[32];            // Array to store the histogram of the 64x64 image (32 gray scales)

    unsigned int brightness_factor = 1;  // assign initial brightness-adjusting-factor ,ranging 1~5
    double contract_factor = 1.1;  // assign initial contract-adjusting-factor ,ranging 1.1~1.5

    QTimer *clock;

private slots:
    void on_actionExit_triggered();

    void on_actionOpen_triggered();

    void on_actionAbout_triggered();

    void show_image(char image[64][64]);

    void plot_histogram(char image[64][64]);

    void pixel_value_add(char image[64][64], int value);

    void pixel_value_product(char image[64][64], double value);

    void add_image(char image[64][64]);

    void image_reconstruct(char image[64][64]);

    void save_image(char image[64][64]);

    void on_pushButton_add_constant_clicked();

    void on_pushButton_subtract_constant_clicked();

    void on_brightness_factor_valueChanged(int arg1);

    void on_pushButton_multiply_constant_clicked();

    void on_pushButton_devide_constant__clicked();

    void on_contract_factor_valueChanged(double arg1);

    void on_pushButton_average_clicked();

    void on_pushButton_reconstruct_clicked();

    void on_pushButton_save_file_clicked();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
