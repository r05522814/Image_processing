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

    int histogram[256];            // Array to store the histogram of the image (32 gray scales)
    cv::Mat src;
    cv::Mat src_normalized;
    cv::Mat gray_src;
    cv::Mat output_1_image;
    cv::Mat output_2_image;
    cv::Mat output_3_image;

    cv::Mat complexImage;
    cv::Mat processed_complexImage;

    cv::Mat colormap_gray;

    cv::Mat color_mode_conversion_output;

    double mask_3[3][3];
    double mask_5[5][5];
    int src_row, src_column;
    int mode = 0;  // 0 represent rgb888 ; 1 represent gray scale
    int spatial_mask_mode = 0 ; // select spatial doamin mask category
    int label_output_index = 0;
    bool resize_image_to_label = false ;
    double mean_and_sigma_square[2]={0};
    int frequency_mask_mode = 0 ;
    int color_mode[2] = {0,0};  // color_mode[0]:current color mode; color_mode[1]:next color mode;
    int colormap_index = 0;  //select type of colormap for grayscale transformation
    int pseudo_color_low = 0;
    int pseudo_color_high = 255;  // 0~255 normalize to pseudo_color_low~pseudo_color_high
    int color_mode_conversion_index = 0; //0 means showing 3 splitted color planes; 1 means showing merged image

    //***parameters***
    int threshold_value = 128;  //binary threshold
    unsigned int brightness_factor = 10;  // assign initial brightness-adjusting-factor ,ranging 1~5
    int grayscale_shrink_factor =  1;
    double contrast_factor = 1.1;  // assign initial contrast-adjusting-factor ,ranging 1.1~1.5
    double MH_diff_rate = 0.00012;   // when judging difference of two sides of zero crossing
    double MH_rate = 0.001;   // percentage threshold for the zero crossing pixel
    int cutoff_frequency = 10; // percentage of the radius in frequency domain
    int butterworth_order = 1; // frequency domian butterworth_order(n)
    double homomorphic_Gamma_H = 2.0;
    double homomorphic_Gamma_L = 0.25;
    int pseudo_color_seperate[4] = {50,100,150,200};
    int k_kmeans_cluster_count = 2;

    //QTimer *clock;

private slots:
    void on_actionExit_triggered();

    void on_actionOpen_triggered();

    void on_actionAbout_triggered();

    //***** functions *****
    void show_image(cv::Mat input);

    void plot_histogram(cv::Mat src);

    void pixel_value_add(cv::Mat input, int value);

    void pixel_value_product(cv::Mat input, double value);

    void save_image(cv::Mat input, QString filename);

    void RGB2GRAY_1(cv::Mat src);
    void RGB2GRAY_2(cv::Mat src);
    cv::Mat RGB2GRAY_3(cv::Mat input);

    void GRAY2binary(cv::Mat input);

    cv::Mat Histogram_Equalization(cv::Mat input);

    void zoom_in(cv::Mat input, double value);
    cv::Mat spacial_resolution_enlarge(cv::Mat input);
    cv::Mat spacial_resolution_shrink(cv::Mat input);

    cv::Mat grayscale_levels_enlarge(cv::Mat input);
    cv::Mat grayscale_levels_shrink(cv::Mat input);

    cv::Mat spatial_mask_operation(cv::Mat input);

    void calculate_mean_and_sigma_square(cv::Mat input);

    double SShape(double z,int a,int b,int c);
    double BellShape(double z);
    double SigmaWhite(double z);
    double SigmaBlack(double z);
    cv::Mat Fuzzy_set_operation(cv::Mat input);

    cv::Mat DFT(cv::Mat input); //discrete fourier transform

    cv::Mat enhancement_for_showing_complexImg(cv::Mat input);  // use log(1+F(u,v)) to enhance

    cv::Mat IDFT(cv::Mat input); //discrete fourier transform

    void show_image_on_output_labels(cv::Mat output, QString output_title);

    cv::Mat frequency_mask_operation(cv::Mat input);

    cv::Mat color_mode_conversion(cv::Mat input);

    void show_image_with_splitted_channels(cv::Mat input, QString output_title);

    cv::Mat gray_to_pseudo_color(cv::Mat input);

    cv::Mat Kmeans_color_segmentation(cv::Mat input, int cluster_count);

    //***** user interface *****
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

    void on_comboBox_spatial_mask_category_currentIndexChanged(int index);

    void on_pushButton_spatial_mask_operation_clicked();

    void on_MH_diff_rate_valueChanged(double arg1);
    void on_MH_rate_valueChanged(double arg1);

    void on_comboBox_layers_currentIndexChanged(int index);

    void on_pushButton_save_output_1_clicked();
    void on_pushButton_save_output_2_clicked();
    void on_pushButton_save_output_3_clicked();

    void on_pushButton_DFT_clicked();
    void on_pushButton_IDFT_clicked();

    void on_horizontalSlider_cutoff_frequency_valueChanged(int position);
    void on_spinBox_cutoff_frequency_valueChanged(int arg1);

    void on_comboBox_frequency_mask_category_currentIndexChanged(int index);

    void on_pushButton_frequency_mask_operation_clicked();

    void on_spinBox_butterworth_order_valueChanged(int arg1);

    void on_pushButton_color_model_conversion_clicked();

    void on_comboBox_color_model_currentIndexChanged(int index);

    void on_comboBox_pseudo_color_type_currentIndexChanged(int index);

    void on_pushButton_Pseudo_color_conversion_clicked();

    void on_horizontalSlider_pseudo_color_low_valueChanged(int value);

    void on_horizontalSlider_pseudo_color_high_valueChanged(int value);

    void on_spinBox_pseudo_color_low_valueChanged(int arg1);

    void on_spinBox_pseudo_color_high_valueChanged(int arg1);

    void on_pushButton_color_segmentation_clicked();

    void on_spinBox_k_for_kmeans_valueChanged(int arg1);

    void on_pushButton_convert_to_RGB_clicked();

    void on_comboBox_color_mode_conversion_index_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
