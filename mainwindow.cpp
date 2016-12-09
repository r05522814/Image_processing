//**************************************************************************
//
// open *.jpg *.png *.bmp image
// RGB to gray and show its histogram
// basic image progressing
//
// Date: 2016/12/07
//
// Version: 5.0
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
    on_comboBox_spatial_mask_category_currentIndexChanged(0);
    on_comboBox_pseudo_color_type_currentIndexChanged(0);

//    memset(image,25,sizeof(image));

//    clock = new QTimer();
//    connect(clock,SIGNAL(timeout()),this,SLOT(on_pushButton_test_clicked()));
//    clock->setInterval(1000);
//    clock->start();

    colormap_gray = cv::imread("bone.png");
    cv::cvtColor(colormap_gray, colormap_gray, CV_BGR2RGB );
    QImage img_colormpap_gray;
    img_colormpap_gray = QImage((const unsigned char*) (colormap_gray.data),
                    colormap_gray.cols, colormap_gray.rows, colormap_gray.step, QImage::Format_RGB888);
    ui->label_colormap_gray->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
    ui->label_colormap_gray->setPixmap(QPixmap::fromImage(img_colormpap_gray));
    QImage imgResize = img_colormpap_gray.scaled(ui->label_colormap_gray->width(),ui->label_colormap_gray->height(),Qt::KeepAspectRatio);
    ui->label_colormap_gray->setPixmap(QPixmap::fromImage(imgResize));

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
        tr("Image Files (*.jpg *.png *.bmp *.tif)"));

    QStringList parts = filepath.split("/");
    QString filename = parts.at(parts.size()-1);

    QStringList file_form_parts = filename.split(".");

    QString  file_form = file_form_parts.at(file_form_parts.size()-1);

    if(filepath != NULL)
    {

        if(file_form == "jpg" || file_form == "JPG" || file_form == "png"
                ||file_form == "PNG"|| file_form == "bmp" || file_form == "BMP" || file_form == "tif")
        {
            src = cv::imread(filepath.toStdString().data());
            cv::cvtColor(src, src, CV_BGR2RGB );  // 4 for original CV_BGR2RGB
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
            //***normalize src to 0~1***
//            src_normalized.create(src.rows,src.cols,CV_32FC3);
//            for( int i = 0; i < src.rows; i++ )
//            {
//                for( int j = 0; j < src.cols; j++ )
//                {
//                    src_normalized.at<cv::Vec3f>(i,j)[0] = (float)src.at<cv::Vec3b>(i,j)[0]/255;
//                    src_normalized.at<cv::Vec3f>(i,j)[1] = (float)src.at<cv::Vec3b>(i,j)[1]/255;
//                    src_normalized.at<cv::Vec3f>(i,j)[2] = (float)src.at<cv::Vec3b>(i,j)[2]/255;
//                }
//            }
            //***showing the image by imshow***
//            cv::Mat test;
//            test.create(src.rows,src.cols,CV_32FC3);
//            cv::cvtColor(src_normalized, test, CV_RGB2BGR );
//            cv::imshow("src", test);


//            for(int i=0;i<src.cols;i++){qDebug()<<src.at<cv::Vec3b>(src.rows/2,i)[0]<<", "<<src_normalized.at<cv::Vec3d>(src_normalized.rows/2,i)[0];}

            RGB2GRAY_2(src);
            show_image(src);
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

cv::Mat MainWindow::RGB2GRAY_3(cv::Mat input)
{
    cv::Mat output;
    output.create(input.rows,input.cols,CV_8UC1);
    for(int i=0; i<input.rows; i++ )
    {
        for(int j=0; j<input.cols; j++ )
        {
            cv::Vec3b intensity = input.at<cv::Vec3b>(i, j);

            uchar red = intensity.val[0];
            uchar green = intensity.val[1];
            uchar blue = intensity.val[2];
            output.at<uchar>(i,j) = red*0.299 + green*0.587 + blue*0.114 ;
//            if(i == input.rows/2){qDebug()<<red<<", "<<green<<", "<<blue<<"   "<<gray_src.at<uchar>(i,j);}
        }
    }
    return output;
}

void MainWindow::GRAY2binary(cv::Mat input)
{
    cv::normalize(input, input, 0, 255, CV_MINMAX);
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

cv::Mat MainWindow::spatial_mask_operation(cv::Mat input)
{
    cv::Mat temp = cv::Mat(input.rows,input.cols,CV_8UC1,double(0.0));

    switch (spatial_mask_mode)
    {
        case 0:   // 3*3 Identical smoothing linear mask
        case 4:   // 3*3 Laplacian
        case 10:  // sharpening laplacian + origin
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

        case 1:   // 5*5 Identical smoothing linear mask
        case 3:   // 5*5 gaussian filter
        {
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
        }
        case 2: // 3*3 Order-statistics Filters
        {
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
        }

        case 5:   // case 5: 3*3 Sobel operation ; case 6: 3*3 Scharr function
        case 6:
        {
            int Gx[3][3];
            int Gy[3][3];
            if(spatial_mask_mode==5) //Sobel mask
            {
                Gx[0][0]=-1; Gx[0][1]=0; Gx[0][2]=1;
                Gx[1][0]=-2; Gx[1][1]=0; Gx[1][2]=2;
                Gx[2][0]=-1; Gx[2][1]=0; Gx[2][2]=1;

                Gy[0][0]=-1; Gy[0][1]=-2; Gy[0][2]=-1;
                Gy[1][0]=0; Gy[1][1]=0; Gy[1][2]=0;
                Gy[2][0]=1; Gy[2][1]=2; Gy[2][2]=1;
            }
            else if(spatial_mask_mode==6) //Scharr mask
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

        case 7:  // Laplace of gaussian
        case 8:  // Marr-Hildreth edge detection
        {
            cv::Mat temp_Gf = cv::Mat(input.rows,input.cols,CV_64FC1,double(0.0));  // after gaussian filter
            cv::Mat temp_LoG = cv::Mat(input.rows,input.cols,CV_64FC1,double(0.0));
//            calculate_mean_and_sigma_square(input);
//            qDebug()<< "mean = "<< QString::number(mean_and_sigma_square[0]);
//            qDebug()<< "sigma^2 = "<<QString::number(mean_and_sigma_square[1]);

            //***step 1: Gaussian filter  G(x,y)*f(x,y)***

            int gaussian_filter[5][5] = {
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
                            new_scale += gaussian_filter[p][q]*input.at<uchar>(i-2+p,j-2+q);
                        }
                    }
//                     2D Gaussian function
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
                    if(spatial_mask_mode==7)
                    {
                        new_scale *= 10;
                        if(new_scale>255) new_scale = 255.0;
                        else if(new_scale<0) new_scale =0.0;
                        temp.at<uchar>(i,j) = cvRound(new_scale);
                    }
                    else if(spatial_mask_mode==8)
                    {
                        temp_LoG.at<double>(i,j) = new_scale/255.0;  // scale to 0~1
                    }
                }
            }
            //cv::imshow("LoG",temp_LoG);

            // ***step 3: zero crossing and thershold***

            if(spatial_mask_mode==8)
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

double MainWindow::SShape(double z,int a,int b,int c)
{
    double val;
    if (z<a)
        {val=0;}
    else if (z>=a && z<=b)
        {val=2*pow(((z-a)/(c-a)),2);}
    else if (z>b && z<=c)
        {val=1-2*pow(((z-c)/(c-a)),2);}
    else
        {val=1;}
    return val;
}
double MainWindow::BellShape(double z)
{
    int b=10;   //width of bell
    int c=0;    //center of bell
    double val;
    if (z<=c)
       { val=SShape(z,c-b,c-b/2,c);}
    else if (z>c)
       { val=1-SShape(z,c,c+b/2,c+b);}
    return val;
}
double MainWindow::SigmaWhite(double z)
{
    double val;
    int a=255;   //end of slope
    int b=200;   //width of slope
    if (z<=a && z>=a-b)
        {val=1-(a-z)/b;}
    else if (z>a)
        {val=1;}
    else
        {val=0;}
    return val;
}
double MainWindow::SigmaBlack(double z)
{
    double val;
    int a=0;    //end of slope
    int b=200;  //width of slope

    if (z<=a && z>=a-b)
        {val=(a-z)/b;}
    else if (z>a)
        {val=0;}
    else
        {val=1;}
    return val;
}
// use fuzzy set for spatial filtering
cv::Mat MainWindow::Fuzzy_set_operation(cv::Mat input)
{
    cv::Mat temp = cv::Mat(input.rows,input.cols,CV_8UC1,double(0.0));
    for(int i=1; i<input.rows-1; i++)
    {
        for(int j=1; j<input.cols-1; j++)
        {
            int src[9];
            double diff[9];
            int index = 0;
            double degree_of_membership[8];
            double membership_output[8];
            double output_value;
            double weighted_sum = 0;
            double sum = 0;
            double new_scale = 0;

            for(int p=0;p<3;p++)
            {
                for(int q=0;q<3;q++)
                {
                    src[index] = input.at<uchar>(i-1+p,j-1+q);
                    index ++;
                }
            }
            for(int i=0; i<9; i++)
            {
                diff[i] = src[i] - src[4];  //calculate difference value of each pixel to the center pixel
            }

            // according to the rule at p.189, calculate degree of membership of "zero_BellShape" & "White_Sigma"
            // denote degree of membership of "zero_BellShape" as ZE(d) with respect to input d
            // denote degree of membership of "White_Sigma" as WH(v) with respect to input v
            // denote degree of membership of "Black_Sigma" as BL(v) with respect to input v
            // 1. every judgement include two ZE(d) => for white part ZE(d), choose the smaller one; for black part 1-ZE(d), choose the bigger one;
            // line => constant value (can use one parameter to store instead of a 255 component array)

            degree_of_membership[0] = min(BellShape(diff[1]), BellShape(diff[5]));
            degree_of_membership[1] = min(BellShape(diff[5]), BellShape(diff[7]));
            degree_of_membership[2] = min(BellShape(diff[7]), BellShape(diff[3]));
            degree_of_membership[3] = min(BellShape(diff[3]), BellShape(diff[1]));
            degree_of_membership[4] = max(1-BellShape(diff[1]),1-BellShape(diff[5]));
            degree_of_membership[5] = max(1-BellShape(diff[5]),1-BellShape(diff[7]));
            degree_of_membership[6] = max(1-BellShape(diff[7]),1-BellShape(diff[3]));
            degree_of_membership[7] = max(1-BellShape(diff[3]),1-BellShape(diff[1]));

            // 2. ZE(d) compare to WH(v) => choose the smaller one ; 1-ZE(d) compare to BL(v) = choose the smaller one
            // use "and"(smaller) to choose value of ZE(d)&WH(v);
            // obtain 8 degree of membership_output;

            // 3. use "or"(biggerer) to choose biggest degree of membership from 8 one;

            for(int i=0; i<256; i++)
            {
                membership_output[0] = min(degree_of_membership[0], SigmaWhite(i));
                membership_output[1] = min(degree_of_membership[1], SigmaWhite(i));
                membership_output[2] = min(degree_of_membership[2], SigmaWhite(i));
                membership_output[3] = min(degree_of_membership[3], SigmaWhite(i));
                membership_output[4] = min(degree_of_membership[4], SigmaBlack(i));
                membership_output[5] = min(degree_of_membership[5], SigmaBlack(i));
                membership_output[6] = min(degree_of_membership[6], SigmaBlack(i));
                membership_output[7] = min(degree_of_membership[7], SigmaBlack(i));

                std::sort(membership_output, membership_output+8);
                output_value = membership_output[7];   // biggest one
                // final output value = sum(f(i)*i) / sum(f(i)), where f(i) is output_value
                weighted_sum = weighted_sum + output_value * i;
                sum = sum + output_value ;
            }
            if(sum == 0) new_scale = 0.0;
            else
            {
                new_scale =  weighted_sum / sum ;
                if(new_scale>255) new_scale = 255.0;
                else if(new_scale<0) new_scale =0.0;
            }

            temp.at<uchar>(i,j) = cvRound(new_scale);
        }
    }
    return temp;
}

cv::Mat MainWindow::DFT(cv::Mat input)  //input image for fourier transform to frequency domain
{
    //***** padding image *****
    // get best size for DFT (to be poewr of 2)
//    int M = cv::getOptimalDFTSize( input.rows );
//    int N = cv::getOptimalDFTSize( input.cols );
    int M = input.rows*2;
    int N = input.cols*2;
//    qDebug() <<"row = "<<input.rows<<"=>"<< M <<", cols = "<<input.cols <<"=>"<< N;

    cv::Mat padded, temp, complexImg;

    cv::copyMakeBorder(input, padded, 0, M - input.rows, 0, N - input.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

//    qDebug() <<"padded_row = "<<padded.rows<<", padded_cols = "<<padded.cols ;
//    cv::imshow("padded",padded);

    // DFT needs two channel ,use merge function to combine two 2D matrices into one 2-channel 3D matrix
    cv::Mat planes[] = {cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F)};
    cv::merge(planes, 2, complexImg); // merge into one Mat

    //*****discrete fourier transform*****
    cv::dft(complexImg, complexImg);

    //*****rearrange the quadrants of Fourier image so that the origin is at the image center*****
//    qDebug() <<"complexImg_row_origin = "<<complexImg.rows<<", complexImg_cols_origin = "<<complexImg.cols ;
//    complexImg = complexImg(cv::Rect(0, 0, complexImg.cols & -2, complexImg.rows & -2));
//    qDebug() <<"complexImg_row = "<<complexImg.rows<<", complexImg_cols = "<<complexImg.cols ;

    int center_x = complexImg.cols/2;
    int center_y = complexImg.rows/2;

    //seperate complexImg into 4 quadrants
    cv::Mat q0(complexImg, cv::Rect(0, 0, center_x, center_y));  //left-up part
    cv::Mat q1(complexImg, cv::Rect(center_x, 0, center_x, center_y));  //right-up part
    cv::Mat q2(complexImg, cv::Rect(0, center_y, center_x, center_y));  //left-down part
    cv::Mat q3(complexImg, cv::Rect(center_x, center_y, center_x, center_y));  //right-down part

    // q0 <-> q3; q1 <-> q2
    q0.copyTo(temp);
    q3.copyTo(q0);
    temp.copyTo(q3);

    q1.copyTo(temp);
    q2.copyTo(q1);
    temp.copyTo(q2);

    return complexImg;
}

cv::Mat MainWindow::enhancement_for_showing_complexImg(cv::Mat input) //input complexImg
{

    cv::Mat output, mag;
    output.create(input.rows,input.cols,CV_32F);

    cv::Mat planes[] =  {cv::Mat_<float>(input), cv::Mat_<float>(input)};
    cv::split(input, planes);  // planes[0] is RE part, planes[1] is IM part
    //calculate sqrt(Re(DFT(img))^2 + Im(DFT(img))^2); output to planes[0]
    cv::magnitude(planes[0], planes[1], planes[0]);
    mag = planes[0];
    // all elements are positive

    float temp = 0.0, F_MIN, F_MAX;
    F_MIN = mag.at<float>(0,0);
    F_MAX = mag.at<float>(0,0);
    //max and min
    for (int i=0;i<mag.rows;i++)
    {
        for (int j=0;j<mag.cols;j++)
        {
            temp = mag.at<float>(i,j);
            if (temp < F_MIN) F_MIN = temp;
            else if (temp > F_MAX) F_MAX = temp;
        }
    }
    F_MIN = log(1 + abs(F_MIN));
    F_MAX = log(1 + abs(F_MAX));
//    qDebug()<<"F_MIN = "<<F_MIN<<", FMAX = "<<F_MAX;
    for (int i=0;i<mag.rows;i++)
    {
         for (int j=0;j<mag.cols;j++)
         {
             output.at<float>(i,j) = 255*(log(1+abs(mag.at<float>(i,j)))-F_MIN)/(F_MAX-F_MIN);
         }
    }
//    cv::normalize(output, output, 0, 1, CV_MINMAX);
//    cv::imshow("with enhancement",output);
    return output;


    //*****alternative method*****
    //***** calculate log(1 + sqrt(Re(DFT(img))^2 + Im(DFT(img))^2)) to enhance visualization *****
/*
    cv::Mat planes[] =  {cv::Mat_<float>(input), cv::Mat_<float>(input)};
    cv::split(input, planes);  // planes[0] is RE part, planes[1] is IM part
    //cv::normalize(planes[0], planes[0], 0, 1, CV_MINMAX);
    //cv::imshow("RE_without enhancement",planes[0]);
    //qDebug()<<QString::number(planes[0].at<float>(center_x,center_y-10));
    cv::magnitude(planes[0], planes[1], planes[0]);  //calculate sqrt(Re(DFT(img))^2 + Im(DFT(img))^2); output to planes[0]
    cv::Mat mag = planes[0];
    mag += cv::Scalar::all(1);
    cv::log(mag, mag);
    cv::normalize(mag, mag, 0, 1, CV_MINMAX);
//    cv::imshow("mag",mag);

    //test zone
//    cv::Mat test_planes[] =  {cv::Mat_<float>(input), cv::Mat_<float>(input)};
//    cv::split(input, test_planes);
//    cv::normalize(test_planes[0], test_planes[0], 0, 1, CV_MINMAX);
//    cv::imshow("input_plane[0]_without enhancement",test_planes[0]);

    return mag;
*/
}

cv::Mat MainWindow::IDFT(cv::Mat input)  //input complexImg for inverse fourier transform
{
    cv::Mat temp, inverse_img;
    //inverse_img.create(input.rows, input.cols, CV_32F);

    int center_x = input.cols/2;
    int center_y = input.rows/2;

    // rearrange the quadrants of Fourier image to the origin form
    cv::Mat q0(input, cv::Rect(0, 0, center_x, center_y));
    cv::Mat q1(input, cv::Rect(center_x, 0, center_x, center_y));
    cv::Mat q2(input, cv::Rect(0, center_y, center_x, center_y));
    cv::Mat q3(input, cv::Rect(center_x, center_y, center_x, center_y));

    q0.copyTo(temp);
    q3.copyTo(q0);
    temp.copyTo(q3);

    q1.copyTo(temp);
    q2.copyTo(q1);
    temp.copyTo(q2);

    cv::idft(input,inverse_img,cv::DFT_SCALE);  //Inverse Fourier Transform

    cv::Mat planes[] = {cv::Mat_<float>(input), cv::Mat_<float>(input)};
    cv::split(inverse_img, planes);
    cv::magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    cv::Mat mag(planes[0], cv::Rect(0, 0, center_x, center_y));  // trim the padding part

//    qDebug()<<"rows = "<<mag.rows<<", cols = "<<mag.cols;

//    normalize(mag, output, 0, 1, CV_MINMAX,CV_32F);
//    cv::imshow("IDFT",output);
    return mag;
}

void MainWindow::show_image_on_output_labels(cv::Mat input, QString output_title)
{
    cv::Mat output;
    QImage img;
    if(input.channels()==3)
    {
        cv::cvtColor(input,input,CV_BGR2RGB);
        img = QImage((const unsigned char*) (input.data),
                        input.cols, input.rows, input.step, QImage::Format_RGB888);
    }
    else if(input.channels()==1)
    {
        cv::normalize(input,output, 0, 255, CV_MINMAX,CV_8UC1);
        plot_histogram(output);  // plot histogram

        img = QImage((const unsigned char*) (input.data),
                        input.cols, input.rows, input.step, QImage::Format_Grayscale8);
    }

    if(label_output_index == 0)
    {
        output_1_image = output;
        ui->label_output_1_image_title->setText(output_title);
        ui->label_output_1->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
        ui->label_output_1->setPixmap(QPixmap::fromImage(img));
        QImage imgResize = img.scaled(ui->label_output_1->width(),ui->label_output_1->height(),Qt::KeepAspectRatio);
        ui->label_output_1->setPixmap(QPixmap::fromImage(imgResize));
    }
    else if (label_output_index == 1)
    {
        output_2_image = output;
        ui->label_output_2_image_title->setText(output_title);
        ui->label_output_2->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
        ui->label_output_2->setPixmap(QPixmap::fromImage(img));
        QImage imgResize = img.scaled(ui->label_output_2->width(),ui->label_output_2->height(),Qt::KeepAspectRatio);
        ui->label_output_2->setPixmap(QPixmap::fromImage(imgResize));
    }
    else if (label_output_index == 2)
    {
        output_3_image = output;
        ui->label_output_3_image_title->setText(output_title);
        ui->label_output_3->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
        ui->label_output_3->setPixmap(QPixmap::fromImage(img));
        QImage imgResize = img.scaled(ui->label_output_3->width(),ui->label_output_3->height(),Qt::KeepAspectRatio);
        ui->label_output_3->setPixmap(QPixmap::fromImage(imgResize));
    }
}

cv::Mat MainWindow::frequency_mask_operation(cv::Mat input)  // input complexImg
{
    cv::Mat output, frequency_mask;
    frequency_mask.create(input.rows, input.cols, CV_32F);
    int center_x = input.rows/2;
    int center_y = input.cols/2;

    double distance;
    double cutoff_frequency_radius = cutoff_frequency * max(center_x,center_y) / 100.0;  //D_0
    // cutoff_frequency is percentage
    // the biggest diameter is set to be the max(cols,rows)

    //calculate mask
    switch(frequency_mask_mode)
    {
        case 0:  //Ideal lowpass filter
        case 1:  //Ideal highpass filter
        {
            for(int i=0; i<input.rows; i++)
            {
                for(int j=0;j<input.cols; j++)
                {
                    distance = sqrt((i-center_x)*(i-center_x) + (j-center_y)*(j-center_y)); //D(u,v)
                    if(frequency_mask_mode == 0)  //Ideal lowpass filter
                    {
                        if(distance < cutoff_frequency_radius) frequency_mask.at<float>(i,j) = 1;
                        else frequency_mask.at<float>(i,j) = 0;
                    }
                    else  //Ideal highpass filter
                    {
                        if(distance < cutoff_frequency_radius) frequency_mask.at<float>(i,j) = 0;
                        else frequency_mask.at<float>(i,j) = 1;
                    }
                }
            }
//            cv::imshow("Ideal filter",frequency_mask);
            break;
        }

        case 2:  // Butterworth lowpass filter
        case 3:  // Butterworth highpass filter
        {
            for(int i=0; i<input.rows; i++)
            {
                for(int j=0;j<input.cols; j++)
                {
                    distance = sqrt((i-center_x)*(i-center_x) + (j-center_y)*(j-center_y)); //D(u,v)
                    if(frequency_mask_mode == 2)  //Butterworth lowpass filter
                    {
                        frequency_mask.at<float>(i,j)=(1/(1+pow( (distance/cutoff_frequency_radius) , 2*butterworth_order) ));
                    }
                    else  //Butterworth highpass filter
                    {
                        frequency_mask.at<float>(i,j)=1-(1/(1+pow( (distance/cutoff_frequency_radius) , 2*butterworth_order)));
                    }
                }
            }
//            cv::imshow("Butterworth filter",frequency_mask);
            break;
        }

        case 4:  // Gaussian lowpass filter
        case 5:  // Gaussian highpass filter
        {
            for(int i=0; i<input.rows; i++)
            {
                for(int j=0;j<input.cols; j++)
                {
                    distance = sqrt((i-center_x)*(i-center_x) + (j-center_y)*(j-center_y)); //D(u,v)
                    if(frequency_mask_mode == 4)  //Butterworth lowpass filter
                    {
                        frequency_mask.at<float>(i,j) = exp(-pow(distance,2)/(2*pow(cutoff_frequency_radius,2)));
                    }
                    else  //Butterworth highpass filter
                    {
                        frequency_mask.at<float>(i,j) = 1-exp(-pow(distance,2)/(2*pow(cutoff_frequency_radius,2)));
                    }
                }
            }
//            cv::imshow("Gaussian filter",frequency_mask);
            break;
        }

        case 6:  // Homomorphic filter
        {
            double rH = homomorphic_Gamma_H;
            double rL = homomorphic_Gamma_L;

            // calculating mask H(u,v)
            for(int i=0; i<input.rows; i++)
            {
                for(int j=0;j<input.cols; j++)
                {
                    distance = sqrt((i-center_x)*(i-center_x) + (j-center_y)*(j-center_y)); //D(u,v)
                    frequency_mask.at<float>(i,j) = (rH-rL)*(1-exp(-pow(distance,2)/pow(cutoff_frequency_radius,2))) + rL;
//                    frequency_mask.at<float>(i,j) = 1;
//                    if(i==center_x)qDebug()<<frequency_mask.at<float>(i,j);
                }
            }
//            cv::normalize(frequency_mask, frequency_mask, 0, 1, CV_MINMAX);
//            cv::imshow("Homomorphic filter",frequency_mask);
            break;
        }

        default:
            break;
    }

    //H(u,v)*F(u,v) mask operation
    cv::Mat planes[] = {cv::Mat_<float>(input), cv::Mat_<float>(input)};
    if(frequency_mask_mode == 6)
    {
        // do not use the original DFT complexImage(input), pre-processing for complexImg_homomorphic
        cv::Mat ln_gray_src, complexImg_homomorphic;
        ln_gray_src.create(gray_src.rows,gray_src.cols,CV_32FC1);

        // 0->1
        for(int i = 0; i < ln_gray_src.rows; i++)
        {
            for(int j = 0 ; j < ln_gray_src.cols; j++)
            {
                ln_gray_src.at<float>(i,j) = gray_src.at<uchar>(i,j) + 1.0;
            }
        }

        cv::log(ln_gray_src, ln_gray_src);   //cv::log input & output both need to be CV_32F or CV_64F

        complexImg_homomorphic = DFT(ln_gray_src);
        cv::split(complexImg_homomorphic, planes);
    }
    else  //for case 0~5, use the original DFT complexImage(input)
    {
        cv::split(input, planes);
    }
    planes[0] = frequency_mask.mul(planes[0]);  // .*
    planes[1] = frequency_mask.mul(planes[1]);  // .*
    cv::merge(planes, 2, output);

    return output;
}

cv::Mat MainWindow::color_mode_conversion(cv::Mat input)
{
    // both input and output are CV_8UC3 (0~255, 3 channels)
    cv::Mat output;

    output.create(input.rows, input.cols, CV_8UC3);

    double R, G, B;

    switch(color_mode[1]) //next color mode
    {
        case 0:  // to RGB
        {
            switch(color_mode[0]) //previous color mode
            {
                case 0:  //RGB to RGB
                {
                    for(int i=0;i<input.rows;i++)
                    {
                        for(int j=0;j<input.cols;j++)
                        {
                            output.at<cv::Vec3b>(i,j)[0] =  input.at<cv::Vec3b>(i,j)[0];
                            output.at<cv::Vec3b>(i,j)[1] =  input.at<cv::Vec3b>(i,j)[1];
                            output.at<cv::Vec3b>(i,j)[2] =  input.at<cv::Vec3b>(i,j)[2];
                        }
                    }
                    break;
                }
                case 1:  //CMY to RGB
                    for(int i=0;i<input.rows;i++)
                    {
                        for(int j=0;j<input.cols;j++)
                        {
                            output.at<cv::Vec3b>(i,j)[0] = 255 - input.at<cv::Vec3b>(i,j)[0];
                            output.at<cv::Vec3b>(i,j)[1] = 255 - input.at<cv::Vec3b>(i,j)[1];
                            output.at<cv::Vec3b>(i,j)[2] = 255 - input.at<cv::Vec3b>(i,j)[2];
                        }
                    }
                    break;
                case 2: //HSI to RGB
                {
                    for(int i=0;i<input.rows;i++)
                    {
                        for(int j=0;j<input.cols;j++)
                        {
                            double H, S, I;
                            //normalize H to 0~360, S & I to 0~1
                            H = input.at<cv::Vec3b>(i,j)[0]/255.0 * 360;
                            S = input.at<cv::Vec3b>(i,j)[1]/255.0;
                            I = input.at<cv::Vec3b>(i,j)[2]/255.0;

//                            qDebug()<<"H="<<H<<" ,S="<<S<<" ,I="<<I;

                            double R, G ,B;
                            if(0<=H && H<120)
                            {
                                B = I*(1-S);
                                R = I*(1 + (S*cos(H/180*M_PI))/(cos((60-H)/180*M_PI)));
                                G = 3*I - (R + B);
                            }
                            else if(120<=H && H<240)
                            {
                                H = H - 120;
                                R = I*(1-S);
                                G = I*(1 + (S*cos(H/180*M_PI))/(cos((60-H)/180*M_PI)));
                                B = 3*I - (R + G);
                            }
                            else // 240<=H && H<360
                            {
                                H = H - 240;
                                G = I*(1-S);
                                B = I*(1 + (S*cos(H/180*M_PI))/(cos((60-H)/180*M_PI)));
                                R = 3*I - (G + B);
                            }
                            output.at<cv::Vec3b>(i,j)[0] = (R * 255);
                            output.at<cv::Vec3b>(i,j)[1] = (G * 255);
                            output.at<cv::Vec3b>(i,j)[2] = (B * 255);
                        }
                    }
                    break;
                }
                case 3:  // XYZ to RGB
                {
                    cv::cvtColor(input, output, CV_XYZ2RGB );
                    break;
                }
                case 4:  // L*a*b* to RGB
                {
                    cv::cvtColor(input, output, CV_Lab2RGB );
                    break;
                }
                case 5:  // YUV to RGB
                {
                    cv::cvtColor(input, output, CV_YUV2RGB );
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case 1:  // to CMY
        {
            switch(color_mode[0]) //previous color mode
            {
                case 0: // from RGB to CMY
                {
                    for(int i=0;i<input.rows;i++)
                    {
                        for(int j=0;j<input.cols;j++)
                        {
//                            output.at<cv::Vec3b>(i,j) = cv::Vec3b(255,255,255) - input.at<cv::Vec3b>(i,j);
                            output.at<cv::Vec3b>(i,j)[0] = 255 - input.at<cv::Vec3b>(i,j)[0];
                            output.at<cv::Vec3b>(i,j)[1] = 255 - input.at<cv::Vec3b>(i,j)[1];
                            output.at<cv::Vec3b>(i,j)[2] = 255 - input.at<cv::Vec3b>(i,j)[2];
                        }
                    }
                    break;
                }
                default:
                    break;

            }
            break;
        }
        case 2:  // to HSI
        {
            switch(color_mode[0]) //previous color mode
            {
                case 0:  //from RGB to HSI
                {
                    double min;
                    for (int i=0;i<input.rows;i++)
                    {
                         for (int j=0;j<input.cols;j++)
                         {
                            R = input.at<cv::Vec3b>(i,j)[0]/255.0;
                            G = input.at<cv::Vec3b>(i,j)[1]/255.0;
                            B = input.at<cv::Vec3b>(i,j)[2]/255.0;
                            min=0;
                            //*** I ***  0~1 normalize to 0~255
                            output.at<cv::Vec3b>(i,j)[2]=(R+G+B)/3 *255;

                            //*** S ***  0~1 normalize to 0~255
                            // find the minimun value among RGB
                            if(R<G) {min=R;}
                            else    {min=G;}

                            if(B<min){min=B;}

                            if ((R+G+B)>0)
                            {output.at<cv::Vec3b>(i,j)[1]=(1-3*min/(R+G+B)) * 255;}
                            else if((R+G+B)==0)
                            {output.at<cv::Vec3b>(i,j)[1] = 0.0;}

                            //*** H ***  0~360 normalize to 0~255
                            if(G>=B) // H = theta
                            {output.at<cv::Vec3b>(i,j)[0] =
                                        (acos((R-G*0.5-B*0.5)/sqrt(pow(R,2)+pow(G,2)+pow(B,2)-R*G-R*B-B*G))/(2*M_PI)*360)/360*255;}
                            else //H = 360 - theta
                            {output.at<cv::Vec3b>(i,j)[0] =
                                        (360-(acos((R-G*0.5-B*0.5)/sqrt(pow(R,2)+pow(G,2)+pow(B,2)-R*G-R*B-B*G))/(2*M_PI)*360))/360*255;}
        //                    output.at<cv::Vec3b>(i,j)[0] = output.at<cv::Vec3b>(i,j)[0]/360.0*255;

                            //***test***
//                            if(sqrt(pow(R,2)+pow(G,2)+pow(B,2)-R*G-R*B-B*G) ==0){qDebug()<<(R-G*0.5-B*0.5)<<", "<<output.at<cv::Vec3b>(i,j)[0];}
                         }
                    }
                    break;
                }
                default:
                    break;
            }

            break;
        }
        case 3:  // RGB to XYZ
        {
            cv::cvtColor(input, output, CV_RGB2XYZ );
//            for (int i=0;i<input.rows;i++)
//            {
//                 for (int j=0;j<input.cols;j++)
//                 {
//                    R = input.at<cv::Vec3b>(i,j)[0];
//                    G = input.at<cv::Vec3b>(i,j)[1];
//                    B = input.at<cv::Vec3b>(i,j)[2];
//                    output.at<cv::Vec3b>(i,j)[0] = 0.412453*R + 0.357580*G + 0.180423*B;
//                    output.at<cv::Vec3b>(i,j)[1] = 0.212671*R + 0.715160*G + 0.072169*B;
//                    output.at<cv::Vec3b>(i,j)[2] = 0.019334*R + 0.119193*G + 0.950227*B;
//                 }
//            }
            break;
        }
        case 4:  // RGB to L*a*b*
        {
            cv::cvtColor(input, output, CV_RGB2Lab );
            break;
        }
        case 5:  // RGB to YUV
        {
            cv::cvtColor(input, output, CV_RGB2YUV );
            break;
        }

        default:
            break;
    }
    // color_mode[0]:current color mode; color_mode[1]:next color mode;
    // color_mode[0] = color_mode[1];

    return output;
}

void MainWindow::show_image_with_splitted_channels(cv::Mat input, QString output_title)
{
    //input matrix must have 3 channel and be normalized to 0~1
    cv::Mat planes[3];
//    cv::Mat planes[] = {cv::Mat_<uchar>(input), cv::Mat_<uchar>(input),cv::Mat_<uchar>(input)};
    cv::split(input,planes);

//    cv::Mat test;
//    test.create(input.rows,input.cols,CV_8UC1);
//    cv::imshow("plane", planes[1]);
    //*****test zone*****
//    if(color_mode[1]==5)
//    {
//        cv::Mat test;
//        cv::cvtColor(input,test, CV_YUV2BGR);
//        cv::imshow("test",test);
//    }
    //***test*****
    cv::Mat output;
    output.create(input.rows,input.cols,CV_8UC1);



//    qDebug()<<input.rows<<","<<input.cols<<"  "<<planes[1].rows<<","<<planes[1].cols;

    for(int i=0;i<3;i++)
    {
        output = planes[i];
//        for( int j = 0; j < input.rows; j++ )
//        {
//            for( int k = 0; k < input.cols; k++ )
//            {
//                output.at<uchar>(j,k) = cvRound(planes[i].at<float>(j,k)*255);
//            }
//        }

//        cv::normalize(planes[i],output,0,255,CV_MINMAX);

        QImage img;
        img = QImage((const unsigned char*) (output.data),
                        output.cols, output.rows, output.step, QImage::Format_Grayscale8);

        QString str;
        if(i == 0)
        {
            output_1_image = planes[i];
            str = ", plane_1";
            ui->label_output_1_image_title->setText(output_title + str);
            ui->label_output_1->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
            ui->label_output_1->setPixmap(QPixmap::fromImage(img));
            QImage imgResize = img.scaled(ui->label_output_1->width(),ui->label_output_1->height(),Qt::KeepAspectRatio);
            ui->label_output_1->setPixmap(QPixmap::fromImage(imgResize));
        }
        else if (i == 1)
        {
//            cv::imshow("nor_plane", planes[1]);
            output_2_image = planes[i];
            str = ", plane_2";
            ui->label_output_2_image_title->setText(output_title + str);
            ui->label_output_2->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
            ui->label_output_2->setPixmap(QPixmap::fromImage(img));
            QImage imgResize = img.scaled(ui->label_output_2->width(),ui->label_output_2->height(),Qt::KeepAspectRatio);
            ui->label_output_2->setPixmap(QPixmap::fromImage(imgResize));
        }
        else if (i == 2)
        {
            output_3_image = planes[i];
            str = ", plane_3";
            ui->label_output_3_image_title->setText(output_title + str);
            ui->label_output_3->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
            ui->label_output_3->setPixmap(QPixmap::fromImage(img));
            QImage imgResize = img.scaled(ui->label_output_3->width(),ui->label_output_3->height(),Qt::KeepAspectRatio);
            ui->label_output_3->setPixmap(QPixmap::fromImage(imgResize));
        }
    }
}

cv::Mat MainWindow::gray_to_pseudo_color(cv::Mat input)
{
    cv::Mat normalized_input, output;

    // use upper and lower bound to choose what color range should be present
    // project 0~255 to the assigned color range
    cv::normalize(input, normalized_input, pseudo_color_low, pseudo_color_high, CV_MINMAX);

    switch(colormap_index)
    {
        case 0: cv::applyColorMap(normalized_input, output, cv::COLORMAP_JET);break;
        case 1: cv::applyColorMap(normalized_input, output, cv::COLORMAP_RAINBOW);break;
        case 2: cv::applyColorMap(normalized_input, output, cv::COLORMAP_HOT);break;
        case 3: cv::applyColorMap(normalized_input, output, cv::COLORMAP_PARULA);break;
        case 4:  // user defined
        {
            output.create(input.rows, input.cols, CV_8UC3);
            for (int i=0;i<input.rows;i++)
            {
                 for (int j=0;j<input.cols;j++)
                 {
                      uchar Value = input.at<uchar>(i,j);
                      if(Value <= pseudo_color_seperate[0]){
                      output.at<cv::Vec3b>(i,j)[0]=0;
                      output.at<cv::Vec3b>(i,j)[1]=0;
                      output.at<cv::Vec3b>(i,j)[2]=0;
                      }
                      else if(Value <= pseudo_color_seperate[1]){
                      output.at<cv::Vec3b>(i,j)[0]=0;
                      output.at<cv::Vec3b>(i,j)[1]=0;
                      output.at<cv::Vec3b>(i,j)[2]=255;
                      }
//                      else if(Value <= 90){
//                      output.at<cv::Vec3b>(i,j)[0]=0;
//                      output.at<cv::Vec3b>(i,j)[1]=255;
//                      output.at<cv::Vec3b>(i,j)[2]=255;
//                      }
                      else if(Value <= pseudo_color_seperate[2]){
                      output.at<cv::Vec3b>(i,j)[0]=0;
                      output.at<cv::Vec3b>(i,j)[1]=255;
                      output.at<cv::Vec3b>(i,j)[2]=0;
                      }
//                      else if(Value <= 160){
//                      output.at<cv::Vec3b>(i,j)[0]=255;
//                      output.at<cv::Vec3b>(i,j)[1]=255;
//                      output.at<cv::Vec3b>(i,j)[2]=0;
//                      }
                      else if(Value <= pseudo_color_seperate[3]){
                      output.at<cv::Vec3b>(i,j)[0]=255;
                      output.at<cv::Vec3b>(i,j)[1]=0;
                      output.at<cv::Vec3b>(i,j)[2]=0;
                      }
//                      else if(Value <= 240){
//                      output.at<cv::Vec3b>(i,j)[0]=255;
//                      output.at<cv::Vec3b>(i,j)[1]=0;
//                      output.at<cv::Vec3b>(i,j)[2]=255;
//                      }
                      else{
                      output.at<cv::Vec3b>(i,j)[0]=255;
                      output.at<cv::Vec3b>(i,j)[1]=255;
                      output.at<cv::Vec3b>(i,j)[2]=255;
                      }
                 }
            }
            break;
        }
        default:
            break;
    }
//    for(int i=0;i<input.cols;i++){qDebug()<<input.at<cv::Vec3b>(output.rows/2,i)[0]<<","<< output.at<cv::Vec3b>(input.rows/2,i)[1];}
    return output;
}

cv::Mat MainWindow::Kmeans_color_segmentation(cv::Mat input, int cluster_count)
{
    int attempts = 6;
    cv::Mat labels, centers ;

    //***step 1 : map 3 channel matrix into vectos, where each vector is a voxel (ri,gi,bi)***
    cv::Mat samples(input.rows * input.cols, 3, CV_32F);
    for( int y = 0; y < input.rows; y++ ){
        for( int x = 0; x < input.cols; x++ ){
            for( int z = 0; z < 3; z++){
                samples.at<float>(y + x*input.rows, z) = input.at<cv::Vec3b>(y,x)[z];
            }
        }
    }

    //***step 2 : find the centers and labels using Kmeanns ***
    cv::kmeans(samples, cluster_count, labels,
               cv::TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.01),
                   attempts, cv::KMEANS_PP_CENTERS, centers);

    //***step 3 : process the output ***
    cv::Mat new_image( input.size(), input.type() );
    for( int y = 0; y < input.rows; y++ )
      for( int x = 0; x < input.cols; x++ )
      {
        int cluster_idx = labels.at<int>(y + x*src.rows,0);
        new_image.at<cv::Vec3b>(y,x)[0] = centers.at<float>(cluster_idx, 0);
        new_image.at<cv::Vec3b>(y,x)[1] = centers.at<float>(cluster_idx, 1);
        new_image.at<cv::Vec3b>(y,x)[2] = centers.at<float>(cluster_idx, 2);
      }
    cv::cvtColor(new_image,new_image,CV_BGR2RGB);
    return new_image;
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

void MainWindow::on_comboBox_spatial_mask_category_currentIndexChanged(int index)
{
    switch (index)
    {
        case 0:  // 3*3 identical smoothing mask
            spatial_mask_mode = 0;
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

        case 1:  // 5*5 identical smoothing mask
            spatial_mask_mode = 1;
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
            spatial_mask_mode = 2;
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

        case 3:  // 5*5 gaussian filter
            spatial_mask_mode = 3;
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
            spatial_mask_mode = 4;
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
            spatial_mask_mode = 5;
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
            spatial_mask_mode = 6;
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

        case 7:  // laplacian of Gaussian
            spatial_mask_mode = 7;
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
            spatial_mask_mode = 8;
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

        case 9:  // Fuzzy set
            spatial_mask_mode = 9;
            ui->N11->setText("Fuzzy set");
            ui->N12->setText("Fuzzy set");
            ui->N13->setText("Fuzzy set");
            ui->N14->hide();
            ui->N15->hide();
            ui->N21->setText("Fuzzy set");
            ui->N22->setText("Fuzzy set");
            ui->N23->setText("Fuzzy set");
            ui->N24->hide();
            ui->N25->hide();
            ui->N31->setText("Fuzzy set");
            ui->N32->setText("Fuzzy set");
            ui->N33->setText("Fuzzy set");
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

        case 10:  // sharpen (laplacian + origin)
            spatial_mask_mode = 10;
            ui->N11->setText("-1");
            ui->N12->setText("-1");
            ui->N13->setText("-1");
            ui->N14->hide();
            ui->N15->hide();
            ui->N21->setText("-1");
            ui->N22->setText("9");
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

        default:
            break;

    }
}

void MainWindow::on_pushButton_spatial_mask_operation_clicked()
{
    QString output_title;
    clock_t t_start, t_end;
    cv::Mat spatial_mask_operation_output;

    switch(spatial_mask_mode)
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
            spatial_mask_operation_output = spatial_mask_operation(gray_src);
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
            spatial_mask_operation_output = spatial_mask_operation(gray_src);
            output_title += "5x5 Identical smoothing mask,   ";
            t_end = clock();
            break;

        case 2:
            t_start = clock();
            spatial_mask_operation_output = spatial_mask_operation(gray_src);
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
            spatial_mask_operation_output = spatial_mask_operation(gray_src);
            output_title += "5x5 Gaussian filter,   ";
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
            spatial_mask_operation_output = spatial_mask_operation(gray_src);
            output_title += "3x3 Laplacian,   ";
            t_end = clock();
            break;

        case 5:
            t_start = clock();
            spatial_mask_operation_output = spatial_mask_operation(gray_src);
            output_title += "3x3 Sobel operation,   ";
            t_end = clock();
            break;

        case 6:
            t_start = clock();
            spatial_mask_operation_output = spatial_mask_operation(gray_src);
            output_title += "3x3 Scharr function,   ";
            t_end = clock();
            break;

        case 7:
            t_start = clock();
            spatial_mask_operation_output = spatial_mask_operation(gray_src);
            output_title += "Laplacian of gaussian,   ";
            t_end = clock();
            break;

        case 8:
            t_start = clock();
            spatial_mask_operation_output = spatial_mask_operation(gray_src);
            output_title += "Marr-Hildreth edge detection,   ";
            t_end = clock();
            break;

        case 9:
            t_start = clock();
            spatial_mask_operation_output = Fuzzy_set_operation(gray_src);
            output_title += "Fuzzy set for spatial filtering,   ";
            t_end = clock();
            break;

        case 10:
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
            spatial_mask_operation_output = spatial_mask_operation(gray_src);
            output_title += "sharpening with laplacian,   ";
            t_end = clock();
            break;

        default:
            break;
    }

    output_title += "run time = ";
    output_title += QString::number(t_end-t_start) +=" (ms)";

    //show output image on assigned label and plot histogram
    show_image_on_output_labels(spatial_mask_operation_output,output_title);
}

void MainWindow::on_MH_diff_rate_valueChanged(double arg1)
{
    MH_diff_rate = arg1;
    on_comboBox_spatial_mask_category_currentIndexChanged(8);
    on_pushButton_spatial_mask_operation_clicked();
}

void MainWindow::on_MH_rate_valueChanged(double arg1)
{
    MH_rate = arg1;
    on_comboBox_spatial_mask_category_currentIndexChanged(8);
    on_pushButton_spatial_mask_operation_clicked();
}

void MainWindow::on_comboBox_layers_currentIndexChanged(int index)
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

void MainWindow::on_pushButton_DFT_clicked()
{
//    qDebug()<<"dim = "<<QString::number(processed_complexImage.dims);
    //dims = 0, not initialized; dims = 2, initialized

    cv::Mat DFT_output;
    QString output_title;
    clock_t t_start, t_end;

    t_start = clock();

//    qDebug()<<"before DFT";
//    for(int i=0;i<gray_src.cols;i++)
//    { qDebug()<<gray_src.at<uchar>(gray_src.rows/2,i);}

    complexImage = DFT(gray_src);
    DFT_output = enhancement_for_showing_complexImg(complexImage);

    output_title += "Discrete Fourier Transform, ";
    t_end = clock();
    output_title += "run time = ";
    output_title += QString::number(t_end-t_start) +=" (ms)";

    //show output image on assigned label and plot histogram
    show_image_on_output_labels(DFT_output,output_title);
}

void MainWindow::on_pushButton_IDFT_clicked()
{
    cv::Mat IDFT_output;
    QString output_title;
    clock_t t_start, t_end;

    t_start = clock();

    if(processed_complexImage.dims != 2)  // dims = 2 means processed_complexImage being initialized
    {
        IDFT_output = IDFT(complexImage);
        // Does not apply mask opreation , so IDFT will turn the freq_image to the origin image
    }
    else
    {
        IDFT_output = IDFT(processed_complexImage);
    }

//    qDebug()<<"after IDFT";
//    for(int i=0;i<IDFT_output.cols;i++)
//    { qDebug()<<IDFT_output.at<float>(IDFT_output.rows/2,i);}

    if(frequency_mask_mode == 6)  //need to do exp after IDFT to get processed image
    {

        cv::exp(IDFT_output, IDFT_output);
    }

    switch(frequency_mask_mode)
    {
        case 0: output_title += "IDFT_Ideal lowpass filter, "; break;
        case 1: output_title += "IDFT_Ideal highpass filter, "; break;
        case 2: output_title += "IDFT_Butterworth lowpass filter, "; break;
        case 3: output_title += "IDFT_Butterworth highpass filter, "; break;
        case 4: output_title += "IDFT_Gaussian lowpass filter, "; break;
        case 5: output_title += "IDFT_Gaussian highpass filter, "; break;
        case 6: output_title += "IDFT_Homomorphic filter, "; break;
        default:
            break;
    }
    t_end = clock();
    output_title += "run time = ";
    output_title += QString::number(t_end-t_start) +=" (ms)";

    //show output image on assigned label and plot histogram
    show_image_on_output_labels(IDFT_output,output_title);
}

void MainWindow::on_horizontalSlider_cutoff_frequency_valueChanged(int position)
{
    if( cutoff_frequency != position)
    {
        cutoff_frequency = position;
        ui->spinBox_cutoff_frequency->setValue(position);
    }
    //on_pushButton_frequency_mask_operation_clicked();
}

void MainWindow::on_spinBox_cutoff_frequency_valueChanged(int arg1)
{
    if( cutoff_frequency != arg1)
    {
        cutoff_frequency = arg1;
        ui->horizontalSlider_cutoff_frequency->setValue(arg1);
    }
    //on_pushButton_frequency_mask_operation_clicked();
}

void MainWindow::on_comboBox_frequency_mask_category_currentIndexChanged(int index)
{
    frequency_mask_mode = index;
}

void MainWindow::on_pushButton_frequency_mask_operation_clicked()
{
    if(complexImage.dims != 2)
    {
        on_pushButton_DFT_clicked(); //if DFT wasn't carried out, then do it
        //qDebug()<<"Do DFT";
    }

    cv::Mat frequency_mask_operated_output;
    QString output_title;
    clock_t t_start, t_end;

    t_start = clock();
    processed_complexImage = frequency_mask_operation(complexImage);

    frequency_mask_operated_output = enhancement_for_showing_complexImg(processed_complexImage);

    switch(frequency_mask_mode)
    {
        case 0: output_title += "Freq_Ideal lowpass mask, "; break;
        case 1: output_title += "Freq_Ideal highpass mask, "; break;
        case 2: output_title += "Freq_Butterworth lowpass mask, "; break;
        case 3: output_title += "Freq_Butterworth highpass mask, "; break;
        case 4: output_title += "Freq_Gaussian lowpass mask, "; break;
        case 5: output_title += "Freq_Gaussian highpass mask, "; break;
        case 6: output_title += "Freq_Homomorphic mask, "; break;
        default:
            break;
    }
    t_end = clock();
    output_title += "run time = ";
    output_title += QString::number(t_end-t_start) +=" (ms)";

    //show output image on assigned label and plot histogram
    show_image_on_output_labels(frequency_mask_operated_output,output_title);
}

void MainWindow::on_spinBox_butterworth_order_valueChanged(int arg1)
{
    butterworth_order = arg1;
    on_pushButton_frequency_mask_operation_clicked();
}

void MainWindow::on_pushButton_color_model_conversion_clicked()
{
    QString output_title;
    clock_t t_start, t_end;

    t_start = clock();

    color_mode_conversion_output = color_mode_conversion(src);

//    cv::imshow("color_mode_conversion_output",color_mode_conversion_output);

    switch(color_mode[1])
    {
        case 0: output_title += "RGB_mode, "; break;
        case 1: output_title += "CMY_mode, "; break;
        case 2: output_title += "HSI_mode, "; break;
        case 3: output_title += "XYZ_mode, "; break;
        case 4: output_title += "L*a*b*_mode, "; break;
        case 5: output_title += "YUV_mode, "; break;
        default:
            break;
    }
    t_end = clock();
    output_title += "run time = ";
    output_title += QString::number(t_end-t_start) +=" (ms)";

    if(color_mode_conversion_index == 0)
    {
        //show output image on three labels
        show_image_with_splitted_channels(color_mode_conversion_output,output_title);
    }
    else if(color_mode_conversion_index == 1)
    {
        cvtColor(color_mode_conversion_output, color_mode_conversion_output, CV_BGR2RGB);
        show_image_on_output_labels(color_mode_conversion_output,output_title);
    }

}

void MainWindow::on_comboBox_color_model_currentIndexChanged(int index)
{
    color_mode[1] = index;
}

void MainWindow::on_comboBox_pseudo_color_type_currentIndexChanged(int index)
{
    colormap_index = index;
    cv::Mat colormap;
    if( 0 <= colormap_index && colormap_index <= 3)
    {
        ui->PC21->hide();
        ui->PC22->hide();
        ui->PC23->hide();
        ui->PC24->hide();
        ui->horizontalSlider_pseudo_color_low->show();
        ui->horizontalSlider_pseudo_color_high->show();
        ui->spinBox_pseudo_color_low->show();
        ui->spinBox_pseudo_color_high->show();

        switch(colormap_index)
        {
            case 0: colormap = cv::imread("jet.png");break;
            case 1: colormap = cv::imread("rainbow.png");break;
            case 2: colormap = cv::imread("hot.png");break;
            case 3: colormap = cv::imread("parula.png");break;
            default:
                break;
        }
        cv::cvtColor(colormap, colormap, CV_BGR2RGB );
    }
    else
    {
        ui->horizontalSlider_pseudo_color_low->hide();
        ui->horizontalSlider_pseudo_color_high->hide();
        ui->spinBox_pseudo_color_low->hide();
        ui->spinBox_pseudo_color_high->hide();
        ui->PC21->show(); ui->PC21->setText("50");
        ui->PC22->show(); ui->PC22->setText("100");
        ui->PC23->show(); ui->PC23->setText("150");
        ui->PC24->show(); ui->PC24->setText("200");

        cv::Mat test;
        test.create(src.rows, src.cols, CV_8UC1);
        test = RGB2GRAY_3(colormap_gray);
        colormap = gray_to_pseudo_color(test);
        cv::cvtColor(colormap, colormap, CV_BGR2RGB );
    }
        // use colormap_gray to calculate corresponding color map
        QImage img_colormpap;
        img_colormpap = QImage((const unsigned char*) (colormap.data),
                        colormap.cols, colormap.rows, colormap.step, QImage::Format_RGB888);
        ui->label_colormap->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
        ui->label_colormap->setPixmap(QPixmap::fromImage(img_colormpap));
        QImage imgResize = img_colormpap.scaled(ui->label_colormap->width(),ui->label_colormap->height(),Qt::KeepAspectRatio);
        ui->label_colormap->setPixmap(QPixmap::fromImage(imgResize));

//    on_pushButton_Pseudo_color_conversion_clicked();
}

void MainWindow::on_pushButton_Pseudo_color_conversion_clicked()
{
    cv::Mat output;
    QString output_title;
    clock_t t_start, t_end;

    t_start = clock();

//    cv::imshow("pseudo color",output);

    switch(colormap_index)
    {
        case 0: output_title += "Pseudo Color_jet, "; break;
        case 1: output_title += "Pseudo Color_rainbow, "; break;
        case 2: output_title += "Pseudo Color_hot, "; break;
        case 3: output_title += "Pseudo Color_parula, "; break;
        case 4:
        {
            output_title += "Pseudo Color_user defined, ";
            pseudo_color_seperate[0] = ui->PC21->text().toInt();
            pseudo_color_seperate[1] = ui->PC22->text().toInt();
            pseudo_color_seperate[2] = ui->PC23->text().toInt();
            pseudo_color_seperate[3] = ui->PC24->text().toInt();
            break;
        }
        default:
            break;
    }

    output = gray_to_pseudo_color(gray_src);

    // use colormap_gray to calculate corresponding color map
    cv::Mat colormap, test;
    test.create(src.rows, src.cols, CV_8UC1);
    test = RGB2GRAY_3(colormap_gray);
    colormap = gray_to_pseudo_color(test);
    cv::cvtColor(colormap, colormap, CV_BGR2RGB );
    QImage img_colormpap;
    img_colormpap = QImage((const unsigned char*) (colormap.data),
                    colormap.cols, colormap.rows, colormap.step, QImage::Format_RGB888);
    ui->label_colormap->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");
    ui->label_colormap->setPixmap(QPixmap::fromImage(img_colormpap));
    QImage imgResize = img_colormpap.scaled(ui->label_colormap->width(),ui->label_colormap->height(),Qt::KeepAspectRatio);
    ui->label_colormap->setPixmap(QPixmap::fromImage(imgResize));

    t_end = clock();
    output_title += "run time = ";
    output_title += QString::number(t_end-t_start) +=" (ms)";
    show_image_on_output_labels(output, output_title);
}

void MainWindow::on_horizontalSlider_pseudo_color_low_valueChanged(int value)
{
    if( pseudo_color_low != value)
    {
        pseudo_color_low = value;
        ui->spinBox_pseudo_color_low->setValue(value);
    }
    on_pushButton_Pseudo_color_conversion_clicked();
}

void MainWindow::on_horizontalSlider_pseudo_color_high_valueChanged(int value)
{
    if( pseudo_color_high != value)
    {
        pseudo_color_high = value;
        ui->spinBox_pseudo_color_high->setValue(value);
    }
    on_pushButton_Pseudo_color_conversion_clicked();
}

void MainWindow::on_spinBox_pseudo_color_low_valueChanged(int arg1)
{
    if( pseudo_color_low != arg1)
    {
        pseudo_color_low = arg1;
        ui->horizontalSlider_pseudo_color_low->setValue(arg1);
    }
    on_pushButton_Pseudo_color_conversion_clicked();
}

void MainWindow::on_spinBox_pseudo_color_high_valueChanged(int arg1)
{
    if( pseudo_color_high != arg1)
    {
        pseudo_color_high = arg1;
        ui->horizontalSlider_pseudo_color_high->setValue(arg1);
    }
    on_pushButton_Pseudo_color_conversion_clicked();
}

void MainWindow::on_pushButton_color_segmentation_clicked()
{
//    cv::Mat output;
    QString output_title;
    clock_t t_start, t_end;

    t_start = clock();

    color_mode_conversion_output = color_mode_conversion(src);
    color_mode_conversion_output = Kmeans_color_segmentation(color_mode_conversion_output, k_kmeans_cluster_count);

    switch(color_mode[1])
    {
        case 0: output_title += "RGB_k-means, "; break;
        case 1: output_title += "CMY_k-means, "; break;
        case 2: output_title += "HSI_k-means, "; break;
        case 3: output_title += "XYZ_k-means, "; break;
        case 4: output_title += "L*a*b*_k-means, "; break;
        case 5: output_title += "YUV_k-means, "; break;
        default:
            break;
    }
    output_title += "k = "; output_title += QString::number(k_kmeans_cluster_count);
    t_end = clock();
    output_title += ", run time = ";
    output_title += QString::number(t_end-t_start) +=" (ms)";

    //show output image on assigned label
    show_image_on_output_labels(color_mode_conversion_output, output_title);

}

void MainWindow::on_spinBox_k_for_kmeans_valueChanged(int arg1)
{
    k_kmeans_cluster_count = arg1;
}

void MainWindow::on_pushButton_convert_to_RGB_clicked()
{
    QString output_title;
    clock_t t_start, t_end;
    t_start = clock();

    int temp = color_mode[1];
    color_mode[1] = 0;
    color_mode[0] = temp;

    cv::Mat output_RGB;

    switch(color_mode[0])
    {
        case 0: output_title += "RGB to RGB, "; break;
        case 1: output_title += "CMY to RGB, "; break;
        case 2: output_title += "HSI to RGB, "; break;
        case 3: output_title += "XYZ to RGB, "; break;
        case 4: output_title += "L*a*b* to RGB, "; break;
        case 5: output_title += "YUV to RGB, "; break;
        default:
            break;
    }

    t_end = clock();
    output_title += ", run time = ";
    output_title += QString::number(t_end-t_start) +=" (ms)";

    output_RGB = color_mode_conversion(color_mode_conversion_output);
    cv::cvtColor(output_RGB,output_RGB,CV_RGB2BGR);
    color_mode[0] = 0;
    ui->comboBox_color_model->setCurrentIndex(0);
    show_image_on_output_labels(output_RGB, output_title);
}

void MainWindow::on_comboBox_color_mode_conversion_index_currentIndexChanged(int index)
{
    color_mode_conversion_index = index;
}
