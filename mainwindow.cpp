//**************************************************************************
//
// open *.jpg *.png *.bmp image
// RGB to gray and show its histogram
// basic image progressing
//
// Date: 2016/11/02
//
// Version: 3.0
//
//**************************************************************************

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include <time.h>
#include <algorithm>
#include <math.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    on_comboBox_currentIndexChanged(0);

//    memset(image,25,sizeof(image));

//    clock = new QTimer();
//    connect(clock,SIGNAL(timeout()),this,SLOT(on_pushButton_test_clicked()));
//    clock->setInterval(1000);
//    clock->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//***** Close the window and exit the program *****
void MainWindow::on_actionExit_triggered()
{
    close();
}


// *****  Message to show the basic information of this program *****
void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutbox;
    aboutbox.setModal(true);
    aboutbox.exec();
}


//***** Open a image file *****
void MainWindow::on_actionOpen_triggered()
{
    clock_t t_read_show_start, t_read_show_end;
    t_read_show_start= clock();

    QString filepath = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".",
        tr("Image Files (*.jpg *.png *.bmp)"));

    QStringList parts = filepath.split("/");
    QString filename = parts.at(parts.size()-1);

    QStringList file_form_parts = filename.split(".");

    QString  file_form = file_form_parts.at(file_form_parts.size()-1);

    if(filepath != NULL)
    {

        if(file_form == "jpg" || file_form == "JPG" || file_form == "png"
                ||file_form == "PNG"|| file_form == "bmp" || file_form == "BMP")
        {
            src = cv::imread(filepath.toStdString().data());
            cvtColor(src, src, 4 );  // 4 for original CV_BGR2RGB
            src_row = src.rows ;
            src_column = src.cols ;
// ************test zone***************
//            qDebug() << src_row << " " << src_column ;
//            for(int i=0; i<src.rows; i++)
//            {
//                cv::Vec3b intensity = src.at<cv::Vec3b>(i, 0);
//                uchar blue = intensity.val[0];
//                uchar green = intensity.val[1];
//                uchar red = intensity.val[2];
//                qDebug() <<"b="<< blue <<" g="<<green<<" "<<" r="<<red<<" intensity="<< src.at<uchar>(i, 0);
//            }

//            for( int x = 0; x < src.rows; x++ ) {
//              for( int y = 0; y < src.cols; y++ ) {
//                  if ( src.at<cv::Vec3b>(x, y) == cv::Vec3b(0,0,0) ) {
//                    src.at<cv::Vec3b>(x, y)[0] = 255;
//                    src.at<cv::Vec3b>(x, y)[1] = 255;
//                    src.at<cv::Vec3b>(x, y)[2] = 255;
//                  }
//                }
//            }
//*************************************

            RGB2GRAY_2(src);
            show_image(gray_src);
//            cv::imshow("source",src);
//            cv::imshow("source2gray",gray_src);
            plot_histogram(gray_src);
            mode = 0;
            grayscale_shrink_factor =  1;
            t_read_show_end = clock();
            filename += ",  load time = ";
            filename += QString::number(t_read_show_end-t_read_show_start) +=" (ms),  ";
            filename += QString::number(src.rows)+="x";
            filename += QString::number(src.cols);
            ui->label_image_title->setText(filename);
        }

    }
}

//***** show the loaded image file on the screen *****

void MainWindow::show_image(cv::Mat input)
{
    QImage img;
    if(input.channels()==3)  // RGB image
    {
        img = QImage((const unsigned char*) (input.data),
                        input.cols, input.rows, input.step, QImage::Format_RGB888);
    }
    else if(input.channels()==1)  // gray scale image
    {
        img = QImage((const unsigned char*) (input.data),
                        input.cols, input.rows, input.step, QImage::Format_Grayscale8);
    }
    // Set the background color of label
    ui->label->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");

    // Display on label
    ui->label->setPixmap(QPixmap::fromImage(img));

//    Resize the label to fit the image
//    ui->label->resize(ui->label->pixmap()->size());

    // Resize the QImage to fit the label display
    QImage imgResize = img.scaled(ui->label->width(),ui->label->height(),Qt::KeepAspectRatio);
    ui->label->setPixmap(QPixmap::fromImage(imgResize));
}

//***** creat histogram from loaded image *****
void MainWindow::plot_histogram(cv::Mat input)
{
    for(int i=0; i<256; i++ ) histogram[i] = 0;	// Initialize the array

    if(input.channels()==3)
        for(int i=0; i<input.rows; i++)
        {
            for(int j=0; j<input.cols; j++)
            {
                cv::Vec3b intensity = input.at<cv::Vec3b>(i, j);
                uchar avg = (intensity.val[0] + intensity.val[1] + intensity.val[2])/3.0;
                histogram[ int(avg) ]++;
            }
        }
    else if (input.channels()==1)
    {
        for(int i=0; i<input.rows; i++)
        {
            for(int j=0; j<input.cols; j++)
            {
                histogram[ int(input.at<uchar>(i,j)) ]++;
            }
        }
    }

    // Use QChart to display the image histogram
    QBarSet *set0 = new QBarSet("Histogram");

    for(int i=0;i<256;i++)
        *set0 << histogram[i];

    QBarSeries *series = new QBarSeries();
    series->append(set0);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);  //whether to show the legend
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    while(!ui->horizontalLayout->isEmpty())
    {
        // Clear the horizontal layout content if there is any
        // important!! Or the program will fail on showing new one
        ui->horizontalLayout->removeItem(ui->horizontalLayout->itemAt(0));
    }
    ui->horizontalLayout->addWidget(chartView);
}

//***** add or subtract a value to eack pixel *****
void MainWindow::pixel_value_add(cv::Mat input, int value)
{
    if(input.channels() == 3)
    {
        for(int i=0; i<input.rows; i++ )
        {
            for(int j=0; j<input.cols; j++ )
            {
                for(int k=0; k<3; k++)
                {
                    if((input.at<cv::Vec3b>(i, j)[k] + value)>255) input.at<cv::Vec3b>(i, j)[k]=255 ;
                    else if ((input.at<cv::Vec3b>(i, j)[k] + value)<0)  input.at<cv::Vec3b>(i, j)[k]=0 ;
                    else input.at<cv::Vec3b>(i, j)[k] += value;
                }
            }
        }
        show_image(input);
        plot_histogram(input);
    }
    else if(input.channels() == 1)
    {
        for(int i=0; i<input.rows; i++ )
        {
            for(int j=0; j<input.cols; j++ )
            {
                if((input.at<uchar>(i, j) + value)>255) input.at<uchar>(i, j) = 255;
                else if ((input.at<uchar>(i, j) + value)<0) input.at<uchar>(i, j) = 0;
                else  input.at<uchar>(i, j) += value;
            }
        }
        show_image(input);
        plot_histogram(input);
    }
}

//***** multiply or devide a value to eack pixel *****
void MainWindow::pixel_value_product(cv::Mat input, double value)
{
    if(input.channels() == 3)
    {
        for(int i=0; i<input.rows; i++ )
        {
            for(int j=0; j<input.cols; j++ )
            {
                for(int k=0; k<3; k++)
                {
                    if((input.at<cv::Vec3b>(i, j)[k] * value)>255) input.at<cv::Vec3b>(i, j)[k]=255 ;
                    else input.at<cv::Vec3b>(i, j)[k] *= value;
                }
            }
        }
    }
    else if(input.channels() == 1)
    {
        for(int i=0; i<input.rows; i++ )
        {
            for(int j=0; j<input.cols; j++ )
            {
                if((input.at<uchar>(i, j) * value)>255) input.at<uchar>(i, j) = 255;
                else  input.at<uchar>(i, j) *= value;
            }
        }
    }
    show_image(input);
    plot_histogram(input);
}

void MainWindow::save_image(cv::Mat input , QString filename )
{
    QImage img;
    if(input.channels()==3)  // RGB image
    {
        img = QImage((const unsigned char*) (input.data),
                        input.cols, input.rows, QImage::Format_RGB888);
    }
    else if(input.channels()==1)  // gray scale image
    {
        img = QImage((const unsigned char*) (input.data),
                        input.cols, input.rows, QImage::Format_Grayscale8);
    }
    QFileDialog filedialog(this);
    QString save_fileName = filedialog.getSaveFileName(this,
                                                         tr("Save Image"), filename,
                                                    tr("Image Files (*.jpg *.png *.bmp)"));
    img.save(save_fileName);
}

void MainWindow::RGB2GRAY_1(cv::Mat input)
{
    gray_src.create(input.rows,input.cols,CV_8UC1);
    for(int i=0; i<input.rows; i++ )
    {
        for(int j=0; j<input.cols; j++ )
        {
            cv::Vec3b intensity = input.at<cv::Vec3b>(i, j);
            uchar red = intensity.val[0];
            uchar green = intensity.val[1];
            uchar blue = intensity.val[2];

            gray_src.at<uchar>(i,j) = (red+green+blue)/ 3.0 ;
//        qDebug() <<gray_src.at<uchar>(i,j) <<" "<<gray_src.at<cv::Vec3b>(i,j)[0]<<","<< gray_src.at<cv::Vec3b>(i,j)[1]<<gray_src.at<cv::Vec3b>(i,j)[0];
        }
    }
    mode = 1;
//    qDebug() << src.channels() << " " << gray_src.channels();
}

void MainWindow::RGB2GRAY_2(cv::Mat input)
{
    gray_src.create(input.rows,input.cols,CV_8UC1);
    for(int i=0; i<input.rows; i++ )
    {
        for(int j=0; j<input.cols; j++ )
        {
            cv::Vec3b intensity = input.at<cv::Vec3b>(i, j);
            uchar red = intensity.val[0];
            uchar green = intensity.val[1];
            uchar blue = intensity.val[2];
            gray_src.at<uchar>(i,j) = red*0.299 + green*0.587 + blue*0.114 ;
        }
    }
    mode = 1;
}

void MainWindow::GRAY2binary(cv::Mat input)
{
    for(int i=0; i<input.rows; i++)
    {
        for(int j=0; j<input.cols; j++)
        {
            if(input.at<uchar>(i,j)<= threshold_value) input.at<uchar>(i,j) = 0;
            else input.at<uchar>(i,j) = 255;
        }
    }

}

cv::Mat MainWindow::Histogram_Equalization(cv::Mat input)
{
    cv::Mat temp_mat;
    temp_mat.create(input.rows,input.cols,CV_8UC1);
    double sum_histogram [256] = {0};
    int new_slope [256] = {0};

    sum_histogram[0] = histogram[0];
    for(int i=1; i<256; i++)
    {
        sum_histogram[i] = sum_histogram[i-1] + histogram[i] ;
    }

    for(int i=1; i<256; i++)
    {
        new_slope[i] = cvRound(255.0*(sum_histogram[i]/(input.rows * input.cols)));
    }
    for(int i=0; i<input.rows; i++ )
    {
        for(int j=0; j<input.cols; j++ )
        {
           temp_mat.at<uchar>(i,j) = new_slope[ input.at<uchar>(i,j) ];
        }
    }
    return temp_mat;

}

void MainWindow::zoom_in(cv::Mat input, double value)  //zoom in
{
    cv::Mat temp_mat;
    temp_mat.create(input.rows,input.cols,CV_8UC1);

    for (int i=0;i<input.rows;i++)
    {
        for(int j=0;j<input.cols;j++)
        {
            temp_mat.at<uchar>(i,j)=gray_src.at<uchar>(int(i/value),int(j/value));
        }
    }
    for (int i=0;i<input.rows;i++)
    {
        for(int j=0;j<input.cols;j++)
        {
            gray_src.at<uchar>(i,j) = temp_mat.at<uchar>(i,j);
        }
    }

}

cv::Mat MainWindow::spacial_resolution_enlarge(cv::Mat input)
{
    int scale = 2;
    cv::Mat scaled_gray;
    scaled_gray.create(input.rows*scale,input.cols*scale,CV_8UC1);

    for (int i=0;i<scaled_gray.rows;i++)
    {
        for(int j=0;j<scaled_gray.cols;j++)
        {
            scaled_gray.at<uchar>(i,j) = input.at<uchar>(int(i/scale),int(j/scale));
        }
    }

    QImage img;

    img = QImage((const unsigned char*) (scaled_gray.data),
                        scaled_gray.cols, scaled_gray.rows, QImage::Format_Grayscale8);

    ui->label->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");

    ui->label->setPixmap(QPixmap::fromImage(img));
//    // Resize the label to fit the image
//    ui->label->resize(ui->label->pixmap()->size());
    //**************************************************************
    //*** in previous show, the image was resized to fit the label
    //*** but here the function was cancelled(in order to visualize the true spatial resolution)
    //*** so first click to zoom in/out will make the image seem to be non-linear-size-scaled
    //******************************************************************
    if(resize_image_to_label == true)
    {
        QImage imgResize = img.scaled(ui->label->width(),ui->label->height(),Qt::KeepAspectRatio);
        ui->label->setPixmap(QPixmap::fromImage(imgResize));
    }

    return scaled_gray;

}

cv::Mat MainWindow::spacial_resolution_shrink(cv::Mat input)
{
    int scale = 2;
    cv::Mat scaled_gray;
    scaled_gray.create(int(input.rows/scale),int(input.cols/scale),CV_8UC1);

//    qDebug() << input.rows <<" "<< input.cols<<" vs "<< scaled_gray.rows <<" "<< scaled_gray.cols;
    for (int i=0;i<scaled_gray.rows;i++)
    {
        for(int j=0;j<scaled_gray.cols;j++)
        {
            scaled_gray.at<uchar>(i,j) = input.at<uchar>(int(i*scale),int(j*scale));
        }
    }

    QImage img;

    img = QImage((const unsigned char*) (scaled_gray.data),
                        scaled_gray.cols, scaled_gray.rows, QImage::Format_Grayscale8);

    ui->label->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");

    ui->label->setPixmap(QPixmap::fromImage(img));
//    Resize the label to fit the image
//    ui->label->resize(ui->label->pixmap()->size());

    //**************************************************************
    //*** in previous show, the image was resized to fit the label
    //*** but here the resize function was cancelled(in order to visualize the true spatial resolution)
    //*** so first click to zoom in/out will make the image seem to be non-linear-size-scaled
    //******************************************************************
    if(resize_image_to_label == true)
    {
        QImage imgResize = img.scaled(ui->label->width(),ui->label->height(),Qt::KeepAspectRatio);
        ui->label->setPixmap(QPixmap::fromImage(imgResize));
    }

    return scaled_gray;

}

cv::Mat MainWindow::grayscale_levels_enlarge(cv::Mat input)
{
    cv::Mat scaled_gray;
    scaled_gray.create(input.rows,input.cols,CV_8UC1);

    for (int i=0;i<scaled_gray.rows;i++)
    {
        for(int j=0;j<scaled_gray.cols;j++)
        {
            scaled_gray.at<uchar>(i,j) = int(2*(input.at<uchar>(i,j)))/2;
        }
    }
    return scaled_gray;
}

cv::Mat MainWindow::grayscale_levels_shrink(cv::Mat input)
{
    cv::Mat scaled_gray;
    int grayscale_shrink_level = 1 ;
    for(int i=0; i<grayscale_shrink_factor; i++)
    {
        grayscale_shrink_level = grayscale_shrink_level * 2;
    }

    scaled_gray.create(input.rows,input.cols,CV_8UC1);

    for (int i=0;i<scaled_gray.rows;i++)
    {
        for(int j=0;j<scaled_gray.cols;j++)
        {
            scaled_gray.at<uchar>(i,j) =
                    grayscale_shrink_level*int(input.at<uchar>(i,j)/grayscale_shrink_level);
        }
    }
    grayscale_shrink_factor++;
    return scaled_gray;
}


cv::Mat MainWindow::mask_operation(cv::Mat input)
{
    cv::Mat temp = cv::Mat(input.rows,input.cols,CV_8UC1,double(0.0));

    switch (mask_mode)
    {
        case 0:   // 3*3 smoothing linear mask
            for(int i=1;i<input.rows-1; i++)
            {
                for(int j=1;j<input.cols-1; j++)
                {
                    double new_scale = 0;
                    for(int p=0;p<3;p++)
                    {
                        for(int q=0;q<3;q++)
                        {
                            new_scale += mask_3[p][q]*input.at<uchar>(i-1+p,j-1+q);
                        }
                    }
                    if(new_scale>255) new_scale = 255;
                    else if(new_scale<0) new_scale =0;
                    temp.at<uchar>(i,j) = cvRound(new_scale);
                }
            }
            break;

        case 1:   // 5*5 smoothing linear mask
        case 3:   // 5*5 guassian filter
            for(int i=2;i<input.rows-2; i++)
            {
                for(int j=2;j<input.cols-2; j++)
                {
                    double new_scale = 0;
                    for(int p=0;p<5;p++)
                    {
                        for(int q=0;q<5;q++)
                        {
                            new_scale += mask_5[p][q]*input.at<uchar>(i-2+p,j-2+q);
                        }
                    }
                    if(new_scale>255) new_scale = 255;
                    else if(new_scale<0) new_scale =0;
                    temp.at<uchar>(i,j) = cvRound(new_scale);
                }
            }
            break;

        case 2: // 3*3 Order-statistics Filters
            int mask_array[9];
            for(int i=1;i<input.rows-1; i++)
            {
                for(int j=1;j<input.cols-1; j++)
                {
                    int array_index = 0;
                    for(int p=0;p<3;p++)
                    {
                        for(int q=0;q<3;q++)
                        {
                            mask_array[array_index] = input.at<uchar>(i-1+p,j-1+q);
                            array_index ++;
                        }
                    }
                    std::sort(mask_array,mask_array+9);
//                    *****test function*****
//                    if(i=10){
//                        QString test;
//                        for (int temp = 0; temp < 9; temp++) test += QString::number(mask_array[temp]) +=" ";
//                        qDebug()<< test;
//                        qDebug()<< QString::number(mask_array[4]);
//                    }
                    temp.at<uchar>(i,j) = cvRound(mask_array[4]);
                }
            }
            break;

        case 4:   // 3*3 laplacian
            for(int i=1;i<input.rows-1; i++)
            {
                for(int j=1;j<input.cols-1; j++)
                {
                    double new_scale = 0;
                    for(int p=0;p<3;p++)
                    {
                        for(int q=0;q<3;q++)
                        {
                            new_scale += mask_3[p][q]*input.at<uchar>(i-1+p,j-1+q);
                        }
                    }
                    if(new_scale>255) new_scale = 255;
                    else if(new_scale<0) new_scale =0;
                    temp.at<uchar>(i,j) = cvRound(new_scale);
                }
            }
            break;

        case 5:   // case4: 3*3 Sobel operation ; case5: 3*3 Scharr function
        case 6:
        {
            int Gx[3][3];
            int Gy[3][3];
            if(mask_mode==5) //Sobel mask
            {
                Gx[0][0]=-1; Gx[0][1]=0; Gx[0][2]=1;
                Gx[1][0]=-2; Gx[1][1]=0; Gx[1][2]=2;
                Gx[2][0]=-1; Gx[2][1]=0; Gx[2][2]=1;

                Gy[0][0]=-1; Gy[0][1]=-2; Gy[0][2]=-1;
                Gy[1][0]=0; Gy[1][1]=0; Gy[1][2]=0;
                Gy[2][0]=1; Gy[2][1]=2; Gy[2][2]=1;
            }
            else if(mask_mode==6) //Scharr mask
            {
                Gx[0][0]=-3; Gx[0][1]=0; Gx[0][2]=3;
                Gx[1][0]=-10; Gx[1][1]=0; Gx[1][2]=10;
                Gx[2][0]=-3; Gx[2][1]=0; Gx[2][2]=3;

                Gy[0][0]=-3; Gy[0][1]=-10; Gy[0][2]=-3;
                Gy[1][0]=0; Gy[1][1]=0; Gy[1][2]=0;
                Gy[2][0]=3; Gy[2][1]=10; Gy[2][2]=3;
            }

            for(int i=1;i<input.rows-1; i++)
            {
                for(int j=1;j<input.cols-1; j++)
                {
                    double new_scale_x = 0;
                    double new_scale_y = 0;
                    double new_scale = 0;

                    for(int p=0;p<3;p++)
                    {
                        for(int q=0;q<3;q++)
                        {
                            new_scale_x += Gx[p][q]*input.at<uchar>(i-1+p,j-1+q);
                            new_scale_y += Gy[p][q]*input.at<uchar>(i-1+p,j-1+q);
                        }
                    }
                    new_scale = sqrt(new_scale_x*new_scale_x + new_scale_y*new_scale_y);

                    if(new_scale>255) new_scale = 255;
                    else if(new_scale<0) new_scale =0;
                    temp.at<uchar>(i,j) = cvRound(new_scale);
                }
            }
            break;
        }

        case 7:  // Laplace of guassian
        case 8:  // Marr-Hildreth edge detection
        {
            cv::Mat temp_Gf = cv::Mat(input.rows,input.cols,CV_64FC1,double(0.0));  // after guassian filter
            cv::Mat temp_LoG = cv::Mat(input.rows,input.cols,CV_64FC1,double(0.0));
//            calculate_mean_and_sigma_square(input);
//            qDebug()<< "mean = "<< QString::number(mean_and_sigma_square[0]);
//            qDebug()<< "sigma^2 = "<<QString::number(mean_and_sigma_square[1]);

            //***step 1: Guassian filter  G(x,y)*f(x,y)***

            int guassian_filter[5][5] = {
                {1,  4,  7,  4, 1,},
                {4, 16, 26, 16, 4,},
                {7, 26, 41, 26, 7,},
                {4, 16, 26, 16, 4,},
                {1,  4,  7,  4, 1,}
            };

            for(int i=2;i<input.rows-2; i++)
            {
                for(int j=2;j<input.cols-2; j++)
                {                   
                    double new_scale = 0.0;
                    for(int p=0;p<5;p++)
                    {
                        for(int q=0;q<5;q++)
                        {
                            new_scale += guassian_filter[p][q]*input.at<uchar>(i-2+p,j-2+q);
                        }
                    }
//                     2D Guassian function
//                    double G = exp(-(i*i+j*j)/(2*mean_and_sigma_square[1]));
                    new_scale = new_scale/273.0;
                    if(new_scale>255) new_scale = 255.0;
                    else if(new_scale<0) new_scale =0.0;
                    temp_Gf.at<double>(i,j) = new_scale;
                }
            }
            // if use cv::imshow , double value should be scaled to 0~1

            // ***step 2: 3*3 laplacian***

            int laplacian_mask[3][3]={{0,-1,0},{-1,4,-1},{0,-1,0}};

            for(int i=1;i<temp_Gf.rows-1; i++)
            {
                for(int j=1;j<temp_Gf.cols-1; j++)
                {
                    double new_scale = 0;
                    for(int p=0;p<3;p++)
                    {
                        for(int q=0;q<3;q++)
                        {
                            new_scale += laplacian_mask[p][q]*temp_Gf.at<double>(i-1+p,j-1+q);
                        }
                    }
                    if(mask_mode==7)
                    {
                        new_scale *= 10;
                        if(new_scale>255) new_scale = 255.0;
                        else if(new_scale<0) new_scale =0.0;
                        temp.at<uchar>(i,j) = cvRound(new_scale);
                    }
                    else if(mask_mode==8)
                    {
                        temp_LoG.at<double>(i,j) = new_scale/255.0;  // scale to 0~1
                    }
                }
            }
            //cv::imshow("LoG",temp_LoG);

            // ***step 3: zero crossing and thershold***

            if(mask_mode==8)
            {
                for(int i=1;i<temp_LoG.rows-1; i++)
                {
                    for(int j=1;j<temp_LoG.cols-1; j++)
                    {
                        if((temp_LoG.at<double>(i,j) * temp_LoG.at<double>(i-1,j) < -MH_diff_rate  ||  //up
                            temp_LoG.at<double>(i,j) * temp_LoG.at<double>(i+1,j) < -MH_diff_rate  ||  //down
                            temp_LoG.at<double>(i,j) * temp_LoG.at<double>(i,j-1) < -MH_diff_rate  ||  //left
                            temp_LoG.at<double>(i,j) * temp_LoG.at<double>(i,j+1) < -MH_diff_rate      //right
                            ) &&  (temp_LoG.at<double>(i,j) > MH_rate || temp_LoG.at<double>(i,j) < -MH_rate)  //set threshold
                           )
                            temp.at<uchar>(i,j) = 255;
                        else
                            temp.at<uchar>(i,j) = 0;
                    }
                }
            }
//            qDebug() << "MH_diff_rate = " <<QString::number(MH_diff_rate)<<", MH_rate = "<<QString::number(MH_rate);
            break;
        }

        default:


            break;
    }
    return temp;
}

void MainWindow::calculate_mean_and_sigma_square(cv::Mat input)
{
    double temp_mean = 0.0;
    double temp_sigma_sqare = 0.0;

    for(int i=0;i<input.rows; i++)
    {
        for(int j=0;j<input.cols; j++)
        {
            temp_mean +=  input.at<uchar>(i,j);
        }
    }
    temp_mean = temp_mean / (input.rows * input.cols);

    for(int i=0;i<input.rows; i++)
    {
        for(int j=0;j<input.cols; j++)
        {
            temp_sigma_sqare += (input.at<uchar>(i,j) - temp_mean)*(input.at<uchar>(i,j) - temp_mean);
        }
    }
    temp_sigma_sqare = temp_sigma_sqare / (input.rows * input.cols);

    mean_and_sigma_square[0] = temp_mean;
    mean_and_sigma_square[1] = temp_sigma_sqare;

}



//********** Buttons ************
void MainWindow::on_pushButton_add_constant_clicked()
{
    //pixel_value_add(image, pixel_added);
    if(mode == 0) pixel_value_add(src, brightness_factor);
    else if(mode == 1)  pixel_value_add(gray_src, brightness_factor);
}

void MainWindow::on_pushButton_subtract_constant_clicked()
{
//    pixel_value_add(image, pixel_added);
    if(mode == 0) pixel_value_add(src,-(brightness_factor));
    else if(mode == 1)  pixel_value_add(gray_src,-(brightness_factor));
}

void MainWindow::on_pushButton_multiply_constant_clicked()
{
    //pixel_value_product(image, multiple_constant);
    if(mode == 0)  pixel_value_product(src, contrast_factor);
    else if(mode == 1)  pixel_value_product(gray_src, contrast_factor);
}

void MainWindow::on_pushButton_devide_constant__clicked()
{
    //pixel_value_product(image, multiple_constant);
    if(mode == 0)  pixel_value_product(src, 1.0/contrast_factor);
    else if(mode == 1)  pixel_value_product(gray_src, 1.0/contrast_factor);
}

void MainWindow::on_pushButton_histogram_equ_clicked()
{
    gray_src = Histogram_Equalization(gray_src);
    show_image(gray_src);
    plot_histogram(gray_src);
}

void MainWindow::on_pushButton_save_file_clicked()
{
//    save_image(image,"image.jpg");
    save_image(gray_src,QString::fromStdString("gray_src.jpg"));
}

void MainWindow::on_brightness_factor_valueChanged(int arg1)
{
    brightness_factor = arg1;
}

void MainWindow::on_contrast_factor_valueChanged(double arg1)
{
    contrast_factor = arg1;
}

void MainWindow::on_pushButton_RGBtoGRAY_1_clicked()
{
    RGB2GRAY_1(src);
    show_image(gray_src);
    plot_histogram(gray_src);
}

void MainWindow::on_pushButton_RGBtoGRAY_2_clicked()
{
    RGB2GRAY_2(src);
    show_image(gray_src);
    plot_histogram(gray_src);
}

void MainWindow::on_pushButton_original_clicked()
{
    show_image(src);
    plot_histogram(src);
    mode = 0;
    grayscale_shrink_factor =  1;
}

void MainWindow::on_pushButton_GRAY2binary_clicked()
{
    GRAY2binary(gray_src);
    show_image(gray_src);
    plot_histogram(gray_src);
}

void MainWindow::on_threshold_value_valueChanged(int arg1)
{
    threshold_value = arg1;
}

void MainWindow::on_pushButton_zoom_in_clicked()
{
//    zoom_in(gray_src, 2.0);
    gray_src = spacial_resolution_enlarge(gray_src);
    plot_histogram(gray_src);
}

void MainWindow::on_pushButton_zoom_out_clicked()
{
    gray_src = spacial_resolution_shrink(gray_src);
    plot_histogram(gray_src);
}

void MainWindow::on_pushButton_grayscale_up_clicked()
{
    gray_src = grayscale_levels_enlarge(gray_src);
    show_image(gray_src);
    plot_histogram(gray_src);
}

void MainWindow::on_pushButton_grayscale_down_clicked()
{
    gray_src = grayscale_levels_shrink(gray_src);
    show_image(gray_src);
    plot_histogram(gray_src);
}

void MainWindow::on_checkBox_resize_clicked(bool checked)
{
    resize_image_to_label = checked;
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:  // 3*3 smoothing linear mask
            mask_mode = 0;
            ui->N11->setText("0.111");
            ui->N12->setText("0.111");
            ui->N13->setText("0.111");
            ui->N14->hide();
            ui->N15->hide();
            ui->N21->setText("0.111");
            ui->N22->setText("0.111");
            ui->N23->setText("0.111");
            ui->N24->hide();
            ui->N25->hide();
            ui->N31->setText("0.111");
            ui->N32->setText("0.111");
            ui->N33->setText("0.111");
            ui->N34->hide();
            ui->N35->hide();
            ui->N41->hide();
            ui->N42->hide();
            ui->N43->hide();
            ui->N44->hide();
            ui->N45->hide();
            ui->N51->hide();
            ui->N52->hide();
            ui->N53->hide();
            ui->N54->hide();
            ui->N55->hide();
            break;

        case 1:  // 5*5 smoothing linear mask
            mask_mode = 1;
            ui->N11->setText("0.04");
            ui->N12->setText("0.04");
            ui->N13->setText("0.04");
            ui->N14->show(); ui->N14->setText("0.04");
            ui->N15->show(); ui->N15->setText("0.04");
            ui->N21->setText("0.04");
            ui->N22->setText("0.04");
            ui->N23->setText("0.04");
            ui->N24->show(); ui->N24->setText("0.04");
            ui->N25->show(); ui->N25->setText("0.04");
            ui->N31->setText("0.04");
            ui->N32->setText("0.04");
            ui->N33->setText("0.04");
            ui->N34->show(); ui->N34->setText("0.04");
            ui->N35->show(); ui->N35->setText("0.04");
            ui->N41->show(); ui->N41->setText("0.04");
            ui->N42->show(); ui->N42->setText("0.04");
            ui->N43->show(); ui->N43->setText("0.04");
            ui->N44->show(); ui->N44->setText("0.04");
            ui->N45->show(); ui->N45->setText("0.04");
            ui->N51->show(); ui->N51->setText("0.04");
            ui->N52->show(); ui->N52->setText("0.04");
            ui->N53->show(); ui->N53->setText("0.04");
            ui->N54->show(); ui->N54->setText("0.04");
            ui->N55->show(); ui->N55->setText("0.04");
            break;

        case 2:  // 3*3 Order-statistics Filters
            mask_mode = 2;
            ui->N11->setText("order");
            ui->N12->setText("order");
            ui->N13->setText("order");
            ui->N14->hide();
            ui->N15->hide();
            ui->N21->setText("order");
            ui->N22->setText("order");
            ui->N23->setText("order");
            ui->N24->hide();
            ui->N25->hide();
            ui->N31->setText("order");
            ui->N32->setText("order");
            ui->N33->setText("order");
            ui->N34->hide();
            ui->N35->hide();
            ui->N41->hide();
            ui->N42->hide();
            ui->N43->hide();
            ui->N44->hide();
            ui->N45->hide();
            ui->N51->hide();
            ui->N52->hide();
            ui->N53->hide();
            ui->N54->hide();
            ui->N55->hide();
            break;

        case 3:  // 5*5 guassian filter
            mask_mode = 3;
            ui->N11->setText("0.003663");
            ui->N12->setText("0.014652");
            ui->N13->setText("0.025641");
            ui->N14->show(); ui->N14->setText("0.014652");
            ui->N15->show(); ui->N15->setText("0.003663");
            ui->N21->setText("0.014652");
            ui->N22->setText("0.058608");
            ui->N23->setText("0.095238");
            ui->N24->show(); ui->N24->setText("0.058608");
            ui->N25->show(); ui->N25->setText("0.014652");
            ui->N31->setText("0.025641");
            ui->N32->setText("0.095238");
            ui->N33->setText("0.150183");
            ui->N34->show(); ui->N34->setText("0.095238");
            ui->N35->show(); ui->N35->setText("0.025641");
            ui->N41->show(); ui->N41->setText("0.014652");
            ui->N42->show(); ui->N42->setText("0.058608");
            ui->N43->show(); ui->N43->setText("0.095238");
            ui->N44->show(); ui->N44->setText("0.058608");
            ui->N45->show(); ui->N45->setText("0.014652");
            ui->N51->show(); ui->N51->setText("0.003663");
            ui->N52->show(); ui->N52->setText("0.014652");
            ui->N53->show(); ui->N53->setText("0.025641");
            ui->N54->show(); ui->N54->setText("0.014652");
            ui->N55->show(); ui->N55->setText("0.003663");
            break;

        case 4:  // 3*3 laplacian
            mask_mode = 4;
            ui->N11->setText("-1");
            ui->N12->setText("-1");
            ui->N13->setText("-1");
            ui->N14->hide();
            ui->N15->hide();
            ui->N21->setText("-1");
            ui->N22->setText("8");   // to sharpen the image, change "8" to "9"
            ui->N23->setText("-1");
            ui->N24->hide();
            ui->N25->hide();
            ui->N31->setText("-1");
            ui->N32->setText("-1");
            ui->N33->setText("-1");
            ui->N34->hide();
            ui->N35->hide();
            ui->N41->hide();
            ui->N42->hide();
            ui->N43->hide();
            ui->N44->hide();
            ui->N45->hide();
            ui->N51->hide();
            ui->N52->hide();
            ui->N53->hide();
            ui->N54->hide();
            ui->N55->hide();
            break;

        case 5:  // 3*3 sobel
            mask_mode = 5;
            ui->N11->setText("sobel");
            ui->N12->setText("sobel");
            ui->N13->setText("sobel");
            ui->N14->hide();
            ui->N15->hide();
            ui->N21->setText("sobel");
            ui->N22->setText("sobel");
            ui->N23->setText("sobel");
            ui->N24->hide();
            ui->N25->hide();
            ui->N31->setText("sobel");
            ui->N32->setText("sobel");
            ui->N33->setText("sobel");
            ui->N34->hide();
            ui->N35->hide();
            ui->N41->hide();
            ui->N42->hide();
            ui->N43->hide();
            ui->N44->hide();
            ui->N45->hide();
            ui->N51->hide();
            ui->N52->hide();
            ui->N53->hide();
            ui->N54->hide();
            ui->N55->hide();
            break;

        case 6:  // 3*3 Scharr function
            mask_mode = 6;
            ui->N11->setText("Scharr");
            ui->N12->setText("Scharr");
            ui->N13->setText("Scharr");
            ui->N14->hide();
            ui->N15->hide();
            ui->N21->setText("Scharr");
            ui->N22->setText("Scharr");
            ui->N23->setText("Scharr");
            ui->N24->hide();
            ui->N25->hide();
            ui->N31->setText("Scharr");
            ui->N32->setText("Scharr");
            ui->N33->setText("Scharr");
            ui->N34->hide();
            ui->N35->hide();
            ui->N41->hide();
            ui->N42->hide();
            ui->N43->hide();
            ui->N44->hide();
            ui->N45->hide();
            ui->N51->hide();
            ui->N52->hide();
            ui->N53->hide();
            ui->N54->hide();
            ui->N55->hide();
            break;

        case 7:  // laplacian of Guassian
            mask_mode = 7;
            ui->N11->setText("LoG");
            ui->N12->setText("LoG");
            ui->N13->setText("LoG");
            ui->N14->hide();
            ui->N15->hide();
            ui->N21->setText("LoG");
            ui->N22->setText("LoG");
            ui->N23->setText("LoG");
            ui->N24->hide();
            ui->N25->hide();
            ui->N31->setText("LoG");
            ui->N32->setText("LoG");
            ui->N33->setText("LoG");
            ui->N34->hide();
            ui->N35->hide();
            ui->N41->hide();
            ui->N42->hide();
            ui->N43->hide();
            ui->N44->hide();
            ui->N45->hide();
            ui->N51->hide();
            ui->N52->hide();
            ui->N53->hide();
            ui->N54->hide();
            ui->N55->hide();
            break;

        case 8:  // Marr-Hildreth
            mask_mode = 8;
            ui->N11->setText("Marr-Hildreth");
            ui->N12->setText("Marr-Hildreth");
            ui->N13->setText("Marr-Hildreth");
            ui->N14->hide();
            ui->N15->hide();
            ui->N21->setText("Marr-Hildreth");
            ui->N22->setText("Marr-Hildreth");
            ui->N23->setText("Marr-Hildreth");
            ui->N24->hide();
            ui->N25->hide();
            ui->N31->setText("Marr-Hildreth");
            ui->N32->setText("Marr-Hildreth");
            ui->N33->setText("Marr-Hildreth");
            ui->N34->hide();
            ui->N35->hide();
            ui->N41->hide();
            ui->N42->hide();
            ui->N43->hide();
            ui->N44->hide();
            ui->N45->hide();
            ui->N51->hide();
            ui->N52->hide();
            ui->N53->hide();
            ui->N54->hide();
            ui->N55->hide();
            break;

        default:
            break;

    }
}

void MainWindow::on_pushButton_mask_operation_clicked()
{
    QString output_title;
    clock_t t_start, t_end;
    cv::Mat mask_operation_output;

    switch(mask_mode)
    {
        case 0:
            t_start = clock();
            mask_3[0][0] = ui->N11->text().toDouble();
            mask_3[0][1] = ui->N12->text().toDouble();
            mask_3[0][2] = ui->N13->text().toDouble();
            mask_3[1][0] = ui->N21->text().toDouble();
            mask_3[1][1] = ui->N22->text().toDouble();
            mask_3[1][2] = ui->N23->text().toDouble();
            mask_3[2][0] = ui->N31->text().toDouble();
            mask_3[2][1] = ui->N32->text().toDouble();
            mask_3[2][2] = ui->N33->text().toDouble();
            mask_operation_output = mask_operation(gray_src);
            output_title += "3x3 Identical smoothing mask,   ";
            t_end = clock();
            break;

        case 1:
            t_start = clock();
            mask_5[0][0] = ui->N11->text().toDouble();
            mask_5[0][1] = ui->N12->text().toDouble();
            mask_5[0][2] = ui->N13->text().toDouble();
            mask_5[0][3] = ui->N14->text().toDouble();
            mask_5[0][4] = ui->N15->text().toDouble();
            mask_5[1][0] = ui->N21->text().toDouble();
            mask_5[1][1] = ui->N22->text().toDouble();
            mask_5[1][2] = ui->N23->text().toDouble();
            mask_5[1][3] = ui->N24->text().toDouble();
            mask_5[1][4] = ui->N25->text().toDouble();
            mask_5[2][0] = ui->N31->text().toDouble();
            mask_5[2][1] = ui->N32->text().toDouble();
            mask_5[2][2] = ui->N33->text().toDouble();
            mask_5[2][3] = ui->N34->text().toDouble();
            mask_5[2][4] = ui->N35->text().toDouble();
            mask_5[3][0] = ui->N41->text().toDouble();
            mask_5[3][1] = ui->N42->text().toDouble();
            mask_5[3][2] = ui->N43->text().toDouble();
            mask_5[3][3] = ui->N44->text().toDouble();
            mask_5[3][4] = ui->N45->text().toDouble();
            mask_5[4][0] = ui->N51->text().toDouble();
            mask_5[4][1] = ui->N52->text().toDouble();
            mask_5[4][2] = ui->N53->text().toDouble();
            mask_5[4][3] = ui->N54->text().toDouble();
            mask_5[4][4] = ui->N55->text().toDouble();
            mask_operation_output = mask_operation(gray_src);
            output_title += "5x5 Identical smoothing mask,   ";
            t_end = clock();
            break;

        case 2:
            t_start = clock();
            mask_operation_output = mask_operation(gray_src);
            output_title += "3x3 Order-statistics Filters,   ";
            t_end = clock();
            break;

        case 3:
            t_start = clock();
            mask_5[0][0] = ui->N11->text().toDouble();
            mask_5[0][1] = ui->N12->text().toDouble();
            mask_5[0][2] = ui->N13->text().toDouble();
            mask_5[0][3] = ui->N14->text().toDouble();
            mask_5[0][4] = ui->N15->text().toDouble();
            mask_5[1][0] = ui->N21->text().toDouble();
            mask_5[1][1] = ui->N22->text().toDouble();
            mask_5[1][2] = ui->N23->text().toDouble();
            mask_5[1][3] = ui->N24->text().toDouble();
            mask_5[1][4] = ui->N25->text().toDouble();
            mask_5[2][0] = ui->N31->text().toDouble();
            mask_5[2][1] = ui->N32->text().toDouble();
            mask_5[2][2] = ui->N33->text().toDouble();
            mask_5[2][3] = ui->N34->text().toDouble();
            mask_5[2][4] = ui->N35->text().toDouble();
            mask_5[3][0] = ui->N41->text().toDouble();
            mask_5[3][1] = ui->N42->text().toDouble();
            mask_5[3][2] = ui->N43->text().toDouble();
            mask_5[3][3] = ui->N44->text().toDouble();
            mask_5[3][4] = ui->N45->text().toDouble();
            mask_5[4][0] = ui->N51->text().toDouble();
            mask_5[4][1] = ui->N52->text().toDouble();
            mask_5[4][2] = ui->N53->text().toDouble();
            mask_5[4][3] = ui->N54->text().toDouble();
            mask_5[4][4] = ui->N55->text().toDouble();
            mask_operation_output = mask_operation(gray_src);
            output_title += "5x5 Guassian filter,   ";
            t_end = clock();
            break;

        case 4:
            t_start = clock();
            mask_3[0][0] = ui->N11->text().toDouble();
            mask_3[0][1] = ui->N12->text().toDouble();
            mask_3[0][2] = ui->N13->text().toDouble();
            mask_3[1][0] = ui->N21->text().toDouble();
            mask_3[1][1] = ui->N22->text().toDouble();
            mask_3[1][2] = ui->N23->text().toDouble();
            mask_3[2][0] = ui->N31->text().toDouble();
            mask_3[2][1] = ui->N32->text().toDouble();
            mask_3[2][2] = ui->N33->text().toDouble();
            mask_operation_output = mask_operation(gray_src);
            output_title += "3x3 Laplacian,   ";
            t_end = clock();
            break;

        case 5:
            t_start = clock();
            mask_operation_output = mask_operation(gray_src);
            output_title += "3x3 Sobel operation,   ";
            t_end = clock();
            break;

        case 6:
            t_start = clock();
            mask_operation_output = mask_operation(gray_src);
            output_title += "3x3 Scharr function,   ";
            t_end = clock();
            break;

        case 7:
            t_start = clock();
            mask_operation_output = mask_operation(gray_src);
            output_title += "Laplacian of guassian,   ";
            t_end = clock();
            break;

        case 8:
            t_start = clock();
            mask_operation_output = mask_operation(gray_src);
            output_title += "Marr-Hildreth edge detection,   ";
            t_end = clock();
            break;

        default:
            break;
    }

    output_title += "run time = ";
    output_title += QString::number(t_end-t_start) +=" (ms)";
    QImage img;


    img = QImage((const unsigned char*) (mask_operation_output.data),
                        mask_operation_output.cols, mask_operation_output.rows, mask_operation_output.step, QImage::Format_Grayscale8);
    if(label_output_index == 0)
    {
        output_1_image = mask_operation_output;
        ui->label_output_1_image_title->setText(output_title);
        ui->label_output_1->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
        ui->label_output_1->setPixmap(QPixmap::fromImage(img));
        QImage imgResize = img.scaled(ui->label_output_1->width(),ui->label_output_1->height(),Qt::KeepAspectRatio);
        ui->label_output_1->setPixmap(QPixmap::fromImage(imgResize));
    }
    else if (label_output_index == 1)
    {
        output_2_image = mask_operation_output;
        ui->label_output_2_image_title->setText(output_title);
        ui->label_output_2->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
        ui->label_output_2->setPixmap(QPixmap::fromImage(img));
        QImage imgResize = img.scaled(ui->label_output_2->width(),ui->label_output_2->height(),Qt::KeepAspectRatio);
        ui->label_output_2->setPixmap(QPixmap::fromImage(imgResize));
    }
    else if (label_output_index == 2)
    {
        output_3_image = mask_operation_output;
        ui->label_output_3_image_title->setText(output_title);
        ui->label_output_3->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
        ui->label_output_3->setPixmap(QPixmap::fromImage(img));
        QImage imgResize = img.scaled(ui->label_output_3->width(),ui->label_output_3->height(),Qt::KeepAspectRatio);
        ui->label_output_3->setPixmap(QPixmap::fromImage(imgResize));
    }
}

void MainWindow::on_MH_diff_rate_valueChanged(double arg1)
{
    MH_diff_rate = arg1;
    on_comboBox_currentIndexChanged(8);
    on_pushButton_mask_operation_clicked();
}

void MainWindow::on_MH_rate_valueChanged(double arg1)
{
    MH_rate = arg1;
    on_comboBox_currentIndexChanged(8);
    on_pushButton_mask_operation_clicked();
}

void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{
    label_output_index = index;
}

void MainWindow::on_pushButton_save_output_1_clicked()
{
    QStringList file_name_parts = ui->label_image_title->text().split(".");
    QString save_filename = file_name_parts.at(0);
    save_filename += "_";
    QStringList mask_name_parts = ui->label_output_1_image_title->text().split(",");
    save_filename += mask_name_parts.at(0);
    save_filename += ".jpg";
//    QString test = QString::fromStdString("test output.jpg");
//    qDebug() << test;
    save_image(output_1_image, save_filename);
}

void MainWindow::on_pushButton_save_output_2_clicked()
{
    QStringList file_name_parts = ui->label_image_title->text().split(".");
    QString save_filename = file_name_parts.at(0);
    save_filename += "_";
    QStringList mask_name_parts = ui->label_output_2_image_title->text().split(",");
    save_filename += mask_name_parts.at(0);
    save_filename += ".jpg";
    save_image(output_2_image, save_filename);
}

void MainWindow::on_pushButton_save_output_3_clicked()
{
    QStringList file_name_parts = ui->label_image_title->text().split(".");
    QString save_filename = file_name_parts.at(0);
    save_filename += "_";
    QStringList mask_name_parts = ui->label_output_3_image_title->text().split(",");
    save_filename += mask_name_parts.at(0);
    save_filename += ".jpg";
    save_image(output_3_image, save_filename);
}
