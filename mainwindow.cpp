//**************************************************************************
//
// A demonstration program to open *.64 image and show its histogram
//
// Date: 2016/10/4
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

//    memset(image,0,sizeof(image));

//    show_image(image);

//    plot_histogram(image);

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



//***** Open a *.64 image file *****

void MainWindow::on_actionOpen_triggered()
{
    int i,j;

    // Open file dialog to get the file name
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".",
        tr("Image Files (*.64)"));

    // Read the .64 text file and convert the characters into image array
    if(fileName != NULL)
    {
        ifstream imagefile;
        imagefile.open(fileName.toStdString());

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
              //cout <<  endl;

          }
          imagefile.close();
        }

        show_image(image);

        plot_histogram(image);

   }
}


//***** show the loaded image file on the screen *****

void MainWindow::show_image(unsigned char image[64][64])
{
    // Set up QImage for displaying it in the QLabel label

    QImage img(64, 64, QImage::Format_RGB32);
    for(int i=0;i<64;i++)
    {
        for(int j=0;j<64;j++)
        {
            // Set the pixel value of the QImage

            // set resolution of each pixel from 32 to 256
            img.setPixel(j,i,qRgb(image[i][j]*8,image[i][j]*8,image[i][j]*8));
        }
    }

    // Display QImage on the label
    ui->label->setPixmap(QPixmap::fromImage(img));

    // Resize the QImage to fit the label display
    QImage imgResize = img.scaled(ui->label->width(),ui->label->height(),Qt::KeepAspectRatio);
    ui->label->setPixmap(QPixmap::fromImage(imgResize));
}

//***** creat histogram from loaded image *****

void MainWindow::plot_histogram(unsigned char image[64][64])
{
    // Calculate the histogram of the 64x64 image;
    // resolution of each pixel is ranging from 0 to 31

    for(int i=0; i<32; i++ ) histogram[i] = 0;	// Initialize the array
    for(int i=0; i<64; i++)
    {
        for(int j=0; j<64; j++)
        {
            histogram[ image[i][j] ]++;
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
        // Clear the horizontal layout content if there is any
//        ui->horizontalLayout->removeItem(ui->horizontalLayout->itemAt(0));
    }
    ui->horizontalLayout->addWidget(chartView);
}



//*****

void MainWindow::pixel_value_add(unsigned char image[64][64], int value)
{
    for(int i=0; i<64; i++)
    {
        for(int j=0; j<64; j++)
        {
            image[i][j] = image[i][j] + value;
            if (image[i][j] > 31) image[i][j] = 31;
            //else if (image[i][j] < 0) image[i][j] = 0;
        }
    }
    show_image(image);

    plot_histogram(image);
}

void MainWindow::pixel_value_product(unsigned char image[64][64], double value)
{
    for(int i=0; i<64; i++)
    {
        for(int j=0; j<64; j++)
        {
            image[i][j] = image[i][j] * value;
            if (image[i][j] > 31) image[i][j] = 31;
            //else if (image[i][j] < 0) image[i][j] = 0;
        }
    }
    show_image(image);

    plot_histogram(image);
}

void MainWindow::add_image(unsigned char image[64][64])
{
    unsigned char image_2[64][64];
    // Open file dialog to get the file name
    QString fileName_2 = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".",
        tr("Image Files (*.64)"));

    // Read the .64 text file and convert the characters into image array
    if(fileName_2 != NULL)
    {
        ifstream imagefile_2;
        imagefile_2.open(fileName_2.toStdString());

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
            image[i][j] = int(image[i][j] + image_2[i][j])/2;
            if (image[i][j] > 31) image[i][j] = 31;
            //else if (image[i][j] < 0) image[i][j] = 0;
        }
    }

    show_image(image);

    plot_histogram(image);
}

void MainWindow::image_reconstruct(unsigned char image[64][64])
{
    for(int i=0; i<64; i++)
    {
        for(int j=1; j<64; j++)
        {
            image[i][j] = image[i][j] - image[i][j-1];
            if (image[i][j] > 31) image[i][j] = 31;
            //else if (image[i][j] < 0) image[i][j] = 0;
        }
    }
}

void MainWindow::on_pushButton_add_constant_clicked()
{
    int pixel_added = 1;
    pixel_value_add(image, pixel_added);
}



/******test to open .64 through opencv*******
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

MainWindow::~MainWindow()
{
    delete ui;
}

*/



void MainWindow::on_pushButton_multiply_constant_clicked()
{
    double multiple_constant = 1.2;
    pixel_value_product(image, multiple_constant);
}

void MainWindow::on_pushButton_average_clicked()
{
    add_image(image);
}

void MainWindow::on_pushButton_reconstruct_clicked()
{
    image_reconstruct(image);
}


