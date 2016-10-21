//**************************************************************************
//
// open *.jpg *.png *.bmp image
// RGB to gray and show its histogram
// basic image progressing
//
// Date: 2016/10/20
//
// Version: 2.0
//
//**************************************************************************

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    memset(image,25,sizeof(image));

    show_image(image);

    plot_histogram(image);

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

    QString filepath = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".",
        tr("Image Files (*.64 *.jpg *.png *.bmp)"));

    QStringList parts = filepath.split("/");
    QString filename = parts.at(parts.size()-1);
    ui->label_image_title->setText(filename);

    QStringList file_form_parts = filename.split(".");
    QString  file_form = file_form_parts.at(file_form_parts.size()-1);

    // Read the .64 text file and convert the characters into image array
    if(filepath != NULL)
    {
        if(file_form == "64")
        {
            ifstream imagefile;
            imagefile.open(filepath.toStdString());
//            qDebug() << "imagefile.is_open" ;
            if (imagefile.is_open())
            {
                for(int i=0; i<64; i++)
                {
                    for(int j=0; j<64; j++)
                    {
                        // Get the character and convert it to integer ranging from 0 to 31
                        image[i][j] = imagefile.get();
                        // cout <<image[i][j];    // Use this line to check if the input is correct
                        // use ASCII code
                        if( (image[i][j] >= '0') && (image[i][j] <= '9') )
                        {
                            image[i][j] = (image[i][j]-'0');
                        }
                        else
                        {
                            if (image[i][j] > 86) image[i][j] = 86;  // switch to V
                            else if (image[i][j] < 41) image[i][j] = 41;  // switch to A
                            else
                            {
                                image[i][j] = ((image[i][j]-'A') + 10);
                            }
                        }
                        //cout << int(image[i][j]) << ",";  // Use this line to check the data
                    }
                    char a = imagefile.get();  // Discard the end of line character
                }
            }
            imagefile.close();
            show_image(image);
            plot_histogram(image);
        }
        else if(file_form == "jpg" || file_form == "JPG" || file_form == "png"
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
            show_image(src);
            plot_histogram(src);
            RGB2GRAY_2(src);
            mode = 0;
            grayscale_shrink_factor =  1;
        }

    }
}

//***** show the loaded image file on the screen *****
void MainWindow::show_image(char image[64][64])
{
    // Set up QImage for displaying it in the QLabel label

    QImage img(64, 64, QImage::Format_RGB32);
    char temp_pixel ;
    for(int i=0;i<64;i++)
    {
        for(int j=0;j<64;j++)
        {
            // Set the pixel value of the QImage
            // set resolution of each pixel from 32 to 256

            if (image[i][j] < 0) temp_pixel = 0;
            else if (image[i][j] > 31) temp_pixel = 31;
            else temp_pixel = image[i][j];
            img.setPixel(j,i,qRgb(temp_pixel*8,temp_pixel*8,temp_pixel*8));
        }
    }

    // Display QImage on the label
    ui->label->setPixmap(QPixmap::fromImage(img));

    // Resize the QImage to fit the label display
    QImage imgResize = img.scaled(ui->label->width(),ui->label->height(),Qt::KeepAspectRatio);
    ui->label->setPixmap(QPixmap::fromImage(imgResize));
}

void MainWindow::show_image(cv::Mat input)
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

void MainWindow::plot_histogram(char image[64][64])
{
    // Calculate the histogram of the 64x64 image;
    // resolution of each pixel shown is ranging from 0 to 31

    for(int i=0; i<32; i++ ) histogram[i] = 0;	// Initialize the array

    // to ensure the histogram is synchronous with the image shown
    for(int i=0; i<64; i++)
    {
        for(int j=0; j<64; j++)
        {
            if (image[i][j] > 31) histogram[31]++;
            else if (image[i][j] < 0) histogram[0]++;
            else histogram[ image[i][j] ]++;
        }
    }


    // Use QChart to display the image histogram

    QBarSet *set0 = new QBarSet("Histogram");

    for(int i=0;i<32;i++)
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
        // Clear the horizontal layout content if there is any (important!! Or the program will fail on showing new one)
        ui->horizontalLayout->removeItem(ui->horizontalLayout->itemAt(0));
    }
    ui->horizontalLayout->addWidget(chartView);
}

//***** add or subtract a value to eack pixel *****
void MainWindow::pixel_value_add(char image[64][64], int value)
{
    for(int i=0; i<64; i++)
    {
        for(int j=0; j<64; j++)
        {
            image[i][j] = image[i][j] + value;
        }
    }
    show_image(image);

    plot_histogram(image);
}

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
void MainWindow::pixel_value_product(char image[64][64], double value)
{
    for(int i=0; i<64; i++)
    {
        for(int j=0; j<64; j++)
        {
            image[i][j] = image[i][j] * value;
        }
    }
    show_image(image);

    plot_histogram(image);
}

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

void MainWindow::add_image(char image[64][64])
{
    char image_2[64][64];
    // Open file dialog to get the file name
    QString filepath_2 = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".",
        tr("Image Files (*.64)"));

    // Read the .64 text file and convert the characters into image array
    if(filepath_2 != NULL)
    {
        ifstream imagefile_2;
        imagefile_2.open(filepath_2.toStdString());

        if (imagefile_2.is_open())
        {
          for(int i=0; i<64; i++)
          {
              for(int j=0; j<64; j++)
              {
                  // Get the character and convert it to integer ranging from 0 to 255
                  image_2[i][j] = imagefile_2.get();
                  // cout <<image[i][j];    // Use this line to check if the input is correct
                  // use ASCII code
                  if( (image_2[i][j] >= '0') && (image_2[i][j] <= '9') )
                  {
                    image_2[i][j] = (image_2[i][j]-'0');
                  }
                  else
                  {
                    if (image_2[i][j] > 86) image_2[i][j] = 86;  // switch to V
                    else if (image_2[i][j] < 41) image_2[i][j] = 41;  // switch to A
                    else
                    {
                        image_2[i][j] = ((image_2[i][j]-'A') + 10);
                    }
                  }

                  //cout << int(image_2[i][j]) << ",";  // Use this line to check the data
              }

              char a = imagefile_2.get();  // Discard the end of line character
              //cout <<  endl;

          }
          imagefile_2.close();
        }
    }

    for(int i=0; i<64; i++)
    {
        for(int j=0; j<64; j++)
        {
            image[i][j] = (image[i][j] + image_2[i][j])/2.0 ;
        }
    }

    show_image(image);

    plot_histogram(image);

    QStringList parts = filepath_2.split("/");
    QString filename_2 = parts.at(parts.size()-1);
    QString temp = ui->label_image_title->text();
    temp.append(" + " + filename_2);
    ui->label_image_title->setText(temp);
}

void MainWindow::image_reconstruct(char image[64][64])
{
    char temp_image[64][64] ;

    for(int i=0; i<64; i++)
    {
        for(int j=0; j<64; j++)
        {
            temp_image[i][j] = image[i][j];
            // transfer the resolution of input image to normal value (0~31)
            if (temp_image[i][j] < 0) temp_image[i][j] = 0;
            else if (temp_image[i][j] > 31) temp_image[i][j] = 31;
        }
    }

    for(int i=0; i<64; i++)
    {
        for(int j=1; j<64; j++)
        {
            image[i][j] = temp_image[i][j] - temp_image[i][j-1];
            if(image[i][j] < 0) image [i][j] = 0;
        }
    }
    show_image(image);
    plot_histogram(image);

}

void MainWindow::save_image(char image[64][64])
{

//     use opencv as double-check

//    cv::imshow("img",src);

    // Set up QImage for saving file

    char temp_pixel;
    QImage img(64, 64, QImage::Format_RGB32);
    for(int i=0;i<64;i++)
    {
        for(int j=0;j<64;j++)
        {
            // Set the pixel value of the QImage
            // set resolution of each pixel from 32 to 256

            if (image[i][j] < 0) temp_pixel = 0;
            else if (image[i][j] > 31) temp_pixel = 31;
            else temp_pixel = image[i][j];
            img.setPixel(j,i,qRgb(temp_pixel*8,temp_pixel*8,temp_pixel*8));
        }
    }

    QString save_fileName = QFileDialog::getSaveFileName(this,tr("Open Image"), ".",
                                                    tr("Image Files (*.jpg *.bmp *.png)"));
    img.save(save_fileName);

}

void MainWindow::save_image(cv::Mat input)
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
    QString save_fileName = QFileDialog::getSaveFileName(this,tr("Open Image"), ".",
                                                    tr("Image Files (*.64 *.png)"));
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

void MainWindow::Histogram_Equalization(cv::Mat input)
{
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
           gray_src.at<uchar>(i,j) = new_slope[ gray_src.at<uchar>(i,j) ];
        }
    }

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
    Histogram_Equalization(gray_src);
    show_image(gray_src);
    plot_histogram(gray_src);
}

void MainWindow::on_pushButton_save_file_clicked()
{
//    save_image(image);
    save_image(gray_src);
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
