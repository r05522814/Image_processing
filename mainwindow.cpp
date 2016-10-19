//**************************************************************************
//
// A demonstration program to open *.64 image and show its histogram
//
// Date: 2016/10/6
//
// Version: 1.2
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
    int i,j;

    QString filepath = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".",
        tr("Image Files (*.64 *.jpg *.png)"));

    QStringList parts = filepath.split("/");
    QString filename = parts.at(parts.size()-1);
    ui->label_image_title->setText(filename);

    QStringList file_form_parts = filename.split(".");
    QString  file_form = file_form_parts.at(file_form_parts.size()-1);
//    qDebug() << file_form ;

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
                for(i=0; i<64; i++)
                {
                    for(j=0; j<64; j++)
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
        else if(file_form == "jpg" || file_form == "png")
        {
            src = cv::imread(filepath.toStdString().data());
            cvtColor(src, src, 4 );  // 4 for original CV_BGR2RGB
            src_row = src.rows ;
            src_column = src.cols ;
            qDebug() << src_row << " " << src_column ;


// *****test zone*****
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
            show_image(src);
//            plot_histogram(src);
        }

    }
}

//============================================================================================
// Notice: In the program, image resolution of each pixel remains original after calaulations,
//         while being restricted within 0~255 on showing the image
//         and within 0~32 on ploting the histogram
//============================================================================================

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
    QImage img = QImage((const unsigned char*) (input.data),
                        input.cols, input.rows, QImage::Format_RGB888);

    // Set the background color of label
    ui->label->setStyleSheet("QLabel { background-color: rgb(215, 215, 215); border-color: rgb(8, 8, 8); }");

    // Display on label
    ui->label->setPixmap(QPixmap::fromImage(img));
//    // Resize the label to fit the image
//    ui->label->resize(ui->label->pixmap()->size());

    // Resize the QImage to fit the label display
    QImage imgResize = img.scaled(ui->label->width(),ui->label->height(),Qt::KeepAspectRatio);
    ui->label->setPixmap(QPixmap::fromImage(imgResize));
}

//***** creat histogram from loaded image *****

void MainWindow::plot_histogram(cv::Mat src)
{

    for(int i=0; i<256; i++ ) histogram[i] = 0;	// Initialize the array

    // to ensure the histogram is synchronous with the image shown
    for(int i=0; i<src_row; i++)
    {
        for(int j=0; j<src_column; j++)
        {
            if (int(src.at<cv::Vec3d>(i,j)[0]) > 255) histogram[255]++;
            else if (int(src.at<cv::Vec3d>(i,j)[0]) < 0) histogram[0]++;
            else histogram[ int(src.at<cv::Vec3d>(i,j)[0]) ]++;

        }
        qDebug() << src.at<cv::Vec3d>(i,0)[0];
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
        // Clear the horizontal layout content if there is any (important!! Or the program will fail on showing new one)
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
//            for debug
//            if(i==0&&j<5)
//            {
//                qDebug() << int(temp_image[i][j])<<"-"<<int(temp_image[i][j-1])<<"="<<temp_image[i][j] - temp_image[i][j-1]<<"  "<<int(image[i][j]);
//            }

        }
    }
//    qDebug("separate") ;
    show_image(image);

    plot_histogram(image);

}

void MainWindow::save_image(char image[64][64])
{

//     use opencv as double-check

//    cv::Mat src;
//    src.create(64,64,CV_8UC1);
//    for(int i=0; i<64;i++)
//    {
//        for(int j=0; i<64;i++)
//        {
//            src.at<uchar>(i,j) = image[i][j];
//        }
//    }
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
                                                    tr("Image Files (*.64 *.png)"));
//    QString save_path = "test_save.jpg";
//        qDebug() << save_fileName;
    img.save(save_fileName);

}

void MainWindow::on_pushButton_add_constant_clicked()
{
    int pixel_added = brightness_factor;
    pixel_value_add(image, pixel_added);
}

void MainWindow::on_pushButton_subtract_constant_clicked()
{
    int pixel_added = -brightness_factor;
    pixel_value_add(image, pixel_added);
}

void MainWindow::on_pushButton_multiply_constant_clicked()
{
    double multiple_constant = contract_factor;
    pixel_value_product(image, multiple_constant);
}

void MainWindow::on_pushButton_devide_constant__clicked()
{
    double multiple_constant = 1.0/contract_factor;
    pixel_value_product(image, multiple_constant);
}

void MainWindow::on_pushButton_average_clicked()
{
    add_image(image);
}

void MainWindow::on_pushButton_reconstruct_clicked()
{
    RGB2GRAY_A(src);

    show_image(gray_src);
}

void MainWindow::on_pushButton_save_file_clicked()
{
    save_image(image);
}

/****** test to open .64 through opencv *******
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //read file
        QString fileName = QFileDialog::getOpenFileName();
        QFile file;
        file.setFileName(fileName);

        cv::Mat src;
        src.create(64,64,CV_8UC1);

        if(file.exists())
        {
            file.open(QIODevice::ReadOnly);
            int rows = 0;
            while(!file.atEnd())
            {
                QString msg = file.readLine().trimmed();
                for(int i = 0; i< msg.size();i++)
                {
                    src.at<uchar>(rows,i) = msg[i].toLatin1();

                }
                rows++;

            }

            cv::imshow("img",src);
        }
}



*/


void MainWindow::on_brightness_factor_valueChanged(int arg1)
{
    brightness_factor = arg1;
}

void MainWindow::on_contract_factor_valueChanged(double arg1)
{
    contract_factor = arg1;
}

void MainWindow::RGB2GRAY_A(cv::Mat src)
{
    gray_src.create(src_row,src_column,CV_8UC1);

    qDebug() << src.rows << "," << src.cols <<" vs "<< gray_src.rows<< "," <<gray_src.cols;
    for(int i=0; i<src.rows; i++ )
    {
        for(int j=0; j<src.cols; j++ )
        {
            cv::Vec3b intensity = src.at<cv::Vec3b>(i, j);
            uchar red = intensity.val[0];
            uchar green = intensity.val[1];
            uchar blue = intensity.val[2];


            gray_src.at<uchar>(i,j) = (red + green + blue)/3 ;
//            qDebug() <<"r="<< red <<" g="<<green<<" "<<" b="<<blue<<" gray="<< gray_src.at<uchar>(i,j);
        }

    }

}
