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
    int histogram[256];            // Array to store the histogram of the 64x64 image (32 gray scales)
    cv::Mat src;
    cv::Mat gray_src;
    int src_row, src_column;
    int threshold_value = 128;
    int mode = 0;  // 0 represent rgb888 ; 1 represent gray scale
    int grayscale_shrink_factor =  1;
    bool resize_image_to_label = false ;

    unsigned int brightness_factor = 10;  // assign initial brightness-adjusting-factor ,ranging 1~5
    double contrast_factor = 1.1;  // assign initial contrast-adjusting-factor ,ranging 1.1~1.5

    QTimer *clock;

private slots:
    void on_actionExit_triggered();

    void on_actionOpen_triggered();

    void on_actionAbout_triggered();

    void show_image(char image[64][64]);
    void show_image(cv::Mat input);

    void plot_histogram(char image[64][64]);
    void plot_histogram(cv::Mat src);

    void pixel_value_add(char image[64][64], int value);
    void pixel_value_add(cv::Mat input, int value);

    void pixel_value_product(char image[64][64], double value);
    void pixel_value_product(cv::Mat input, double value);

    void add_image(char image[64][64]);

    void image_reconstruct(char image[64][64]);

    void save_image(char image[64][64]);
    void save_image(cv::Mat input);

    void RGB2GRAY_1(cv::Mat src);
    void RGB2GRAY_2(cv::Mat src);

    void GRAY2binary(cv::Mat input);

    void Histogram_Equalization(cv::Mat input);

    void zoom_in(cv::Mat input, double value);
    cv::Mat spacial_resolution_enlarge(cv::Mat input);
    cv::Mat spacial_resolution_shrink(cv::Mat input);

    cv::Mat grayscale_levels_enlarge(cv::Mat input);
    cv::Mat grayscale_levels_shrink(cv::Mat input);

    void on_pushButton_add_constant_clicked();

    void on_pushButton_subtract_constant_clicked();

    void on_brightness_factor_valueChanged(int arg1);

    void on_pushButton_multiply_constant_clicked();

    void on_pushButton_devide_constant__clicked();

    void on_contrast_factor_valueChanged(double arg1);

    void on_pushButton_histogram_equ_clicked();

    void on_pushButton_save_file_clicked();

    void on_pushButton_RGBtoGRAY_1_clicked();

    void on_pushButton_RGBtoGRAY_2_clicked();

    void on_pushButton_original_clicked();

    void on_pushButton_GRAY2binary_clicked();

    void on_threshold_value_valueChanged(int arg1);

    void on_pushButton_zoom_in_clicked();

    void on_pushButton_zoom_out_clicked();

    void on_pushButton_grayscale_up_clicked();

    void on_pushButton_grayscale_down_clicked();

    void on_checkBox_resize_clicked(bool checked);

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
