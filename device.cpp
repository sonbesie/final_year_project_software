#include "device.h"
#include "ui_device.h"

Device::Device(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Device)
{
    // create the ui
    ui->setupUi(this);

    // set the input mask for the mac address input
    ui->edtDeviceAddress->setInputMask(">HH:HH:HH:HH:HH:HH;F");

    // create and load help image
    pix = new QPixmap;
    pix->load(":/pic/assembly_device_b.jpg");
    ui->lblDeviceModel->setText("2020P01");
    ui->lblDeviceVoltage->setText("5V");
    ui->lblDevicePicture->setPixmap(*pix);

    // initialise the test request array
    testRequest[0] = 0;
    testRequest[1] = 0;
    testRequest[2] = 0;
    testRequest[3] = 0;

    // initialise declared pointers
    imagePixmpCam1 = NULL;
    imagePixmpCam2 = NULL;
    imageLabelCam1 = NULL;
    imageLabelCam2 = NULL;
    layoutCam1 = NULL;
    layoutCam2 = NULL;
    saveButtonCam1 = NULL;
    saveButtonCam2 = NULL;
    pictureBuffer1 = NULL;
    pictureBuffer2 = NULL;
    pictureBufferLength1 = NULL;
    pictureBufferLength2 = NULL;

    // detection
    rows1 = NULL;
    cols1 = NULL;
    rows2 = NULL;
    cols2 = NULL;

    // image counters
    imageCounter1 = 0;
    imageCounter2 = 0;

    // set active to false
    updateActive(false);
}

Device::~Device()
{
    delete ui ;
    delete pix;

    if (imagePixmpCam1 != NULL) delete[] imagePixmpCam1;
    if (imagePixmpCam2 != NULL) delete[] imagePixmpCam2;

    if (imageLabelCam1  != NULL) delete[] imageLabelCam1;
    if (imageLabelCam2  != NULL) delete[] imageLabelCam2;

    if (layoutCam1 != NULL) delete[] layoutCam1;
    if (layoutCam2 != NULL) delete[] layoutCam2;

    if (saveButtonCam1 != NULL) delete[] saveButtonCam1;
    if (saveButtonCam2 != NULL) delete[] saveButtonCam2;
}

// dialog data
// -------------------------------------------------------------------------------
QString* Device::getDialogData(void)
{
    QString* returnData = new QString[9];

    returnData[0] = ui->edtDeviceName->text();
    returnData[1] = ui->edtDeviceAddress->displayText();
    returnData[2] = QString::number(ui->cmbDeviceType->currentIndex());
    returnData[3] = QString::number(ui->fpsDisplay->value());

    returnData[4] = QString::number(testRequest[0]);
    returnData[5] = QString::number(testRequest[1]);
    returnData[6] = QString::number(testRequest[2]);
    returnData[7] = QString::number(testRequest[3]);
    returnData[8] = QString::number(testRequest[4]);

    return returnData;
}

void Device::setDialogData(QString* receivedData)
{
    ui->edtDeviceName->setText(receivedData[0]);
    ui->edtDeviceAddress->setText(receivedData[1]);
    ui->cmbDeviceType->setCurrentIndex(receivedData[2].toInt());
    ui->fpsDisplay->display(receivedData[3].toInt());

    updateActive(true);
}
// -------------------------------------------------------------------------------

// images
// -------------------------------------------------------------------------------
void Device::setTakenImage1(uint8_t* data[3000], int length[3000], int year[3000], int month[3000], int day[3000], int hour[3000], int minute[3000], int second[3000], int split[3000], int total)
{
    pictureBuffer1 = data;
    pictureBufferLength1 = length;
    imageCounter1 = total;

    imagePixmpCam1 = new QImage[imageCounter1];
    imageLabelCam1 = new QLabel[imageCounter1];
    layoutCam1 = new QHBoxLayout[imageCounter1];
    saveButtonCam1 = new QPushButton[imageCounter1];

    for (int index = 0; index < imageCounter1; index++)
    {
        imagePixmpCam1[index].loadFromData(QByteArray::fromRawData((const char*)data[index], length[index]), "JPG");

        cv::Mat m_image(imagePixmpCam1[index].height(), imagePixmpCam1[index].width(), CV_8UC4, (uchar*)imagePixmpCam1[index].bits(), imagePixmpCam1[index].bytesPerLine());
        cv::Point pt1(10, 20);
        cv::putText(m_image, (std::to_string(minute[index])+":"+std::to_string(second[index])+":"+std::to_string(split[index])), pt1, cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 0), 1);

        imageLabelCam1[index].setPixmap(QPixmap::fromImage(imagePixmpCam1[index]));
        imageLabelCam1[index].setToolTip("Image Length: " + QString::number(length[index]) +
                                           '\n' + "Date Taken: " + QString::number(year [index]) +"/"+ QString::number(month [index]) +"/"+ QString::number(day   [index]) +
                                           '\n' + "Time Taken: " + QString::number(hour [index]) +":"+ QString::number(minute[index]) +":"+ QString::number(second[index]) +
                                           '\n' + "Sub Second: " + QString::number(split[index]) +
                                           '\n' + "Image numb: " + QString::number(index)
                                           );
        saveButtonCam1[index].setText("Save");
        connect(&saveButtonCam1[index], SIGNAL(clicked()), SLOT(saveHandlers()));

        imageLabelCam1[index].setMinimumSize(160, 120);
        imageLabelCam1[index].setMaximumSize(160, 120);
        imageLabelCam1[index].setScaledContents(true);
        imageLabelCam1[index].setFrameShape(QFrame::StyledPanel);

        layoutCam1[index].addStretch();
        layoutCam1[index].addWidget(&imageLabelCam1[index]);
        layoutCam1[index].addStretch();
        layoutCam1[index].addWidget(&saveButtonCam1[index]);
        layoutCam1[index].addStretch();

        ui->saCam1Layout->addLayout(&layoutCam1[index]);
    }
}

void Device::setTakenImage2(uint8_t* data[3000], int length[3000], int year[3000], int month[3000], int day[3000], int hour[3000], int minute[3000], int second[3000], int split[3000], int total)
{
    pictureBuffer2 = data;
    pictureBufferLength2 = length;
    imageCounter2 = total;

    imagePixmpCam2 = new QImage[imageCounter2];
    imageLabelCam2 = new QLabel[imageCounter2];
    layoutCam2 = new QHBoxLayout[imageCounter2];
    saveButtonCam2 = new QPushButton[imageCounter2];

    for (int index = 0; index < imageCounter2; index++)
    {
        imagePixmpCam2[index].loadFromData(QByteArray::fromRawData((const char*)data[index], length[index]), "JPG");

        cv::Mat m_image(imagePixmpCam2[index].height(), imagePixmpCam2[index].width(), CV_8UC4, (uchar*)imagePixmpCam2[index].bits(), imagePixmpCam2[index].bytesPerLine());
        cv::Point pt1(10, 20);
        cv::putText(m_image, (std::to_string(minute[index])+":"+std::to_string(second[index])+":"+std::to_string(split[index])), pt1, cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 0), 1);

        imageLabelCam2[index].setPixmap(QPixmap::fromImage(imagePixmpCam2[index]));
        imageLabelCam2[index].setToolTip("Image Length: " + QString::number(length[index]) +
                                           '\n' + "Date Taken: " + QString::number(year [index]) +"/"+ QString::number(month [index]) +"/"+ QString::number(day   [index]) +
                                           '\n' + "Time Taken: " + QString::number(hour [index]) +":"+ QString::number(minute[index]) +":"+ QString::number(second[index]) +
                                           '\n' + "Sub Second: " + QString::number(split[index]) +
                                           '\n' + "Image numb: " + QString::number(index)
                                           );
        saveButtonCam2[index].setText("Save");
        connect(&saveButtonCam2[index], SIGNAL(clicked()), SLOT(saveHandlers()));

        imageLabelCam2[index].setMinimumSize(160, 120);
        imageLabelCam2[index].setMaximumSize(160, 120);
        imageLabelCam2[index].setScaledContents(true);
        imageLabelCam2[index].setFrameShape(QFrame::StyledPanel);

        layoutCam2[index].addStretch();
        layoutCam2[index].addWidget(&imageLabelCam2[index]);
        layoutCam2[index].addStretch();
        layoutCam2[index].addWidget(&saveButtonCam2[index]);
        layoutCam2[index].addStretch();

        ui->saCam2Layout->addLayout(&layoutCam2[index]);
    }
}

void Device::setDetections1(int* row, int* col)
{
    rows1 = row;
    cols1 = col;
}

void Device::setDetections2(int* row, int* col)
{
    rows2 = row;
    cols2 = col;
}

void Device::setDisplayImg1(uint8_t* data, int length)
{
    // get picture
    QImage image;

    if (length > 0)
        image.loadFromData(QByteArray::fromRawData((const char*)data, length), "JPG");
    else
        image.load(":/pic/no_image.jpg");

    cv::Mat m_image(image.height(), image.width(), CV_8UC4, (uchar*)image.bits(), image.bytesPerLine());

    // detection area 1
    if ((rows1[0] != 0) && (rows1[1] != 0) && (cols1[0] != 0) && (cols1[1] != 0))
    {
        // debug
        qDebug() << "----------------------------------";
        qDebug() << "detection area 1: loaded";
        qDebug() << rows1[0] << rows1[1] << cols1[0] << cols1[1];
        qDebug() << "----------------------------------";

        // convert image to mat
        cv::Point pt1(cols1[0] - 20, rows1[0]);
        cv::Point pt2(cols1[1] + 20, rows1[1]);
        cv::rectangle(m_image, pt1, pt2, cv::Scalar(0, 255, 0), 2);
        cv::Point pt3(cols1[0], rows1[0] - 20);
        cv::Point pt4(cols1[0], rows1[1] + 20);
        cv::line(m_image, pt3, pt4, cv::Scalar(0, 0, 255), 2);

        cv::Point pt5(10, 20);
        cv::putText(m_image, "LOADED", pt5, cv::FONT_HERSHEY_SIMPLEX, 0.2, cv::Scalar(0, 255, 0), 1);

        ui->image_1->setToolTip("Image Length: " + QString::number(length) + '\n' + "LOADED");
    }
    else
    {
        cv::Point pt1(10, 20);
        cv::putText(m_image, "NOT LOADED", pt1, cv::FONT_HERSHEY_SIMPLEX, 0.2, cv::Scalar(0, 255, 0), 1);

        ui->image_1->setToolTip("Image Length: " + QString::number(length) + '\n' + "NOT LOADED");

        qDebug() << "----------------------------------";
        qDebug() << "detection area 1: not loaded";
        qDebug() << "----------------------------------";
    }

    ui->image_1->setPixmap (QPixmap::fromImage(image));
    ui->lblLength1->setText(QString::number  (length));
}

void Device::setDisplayImg2(uint8_t* data, int length)
{
    // get picture 2
    QImage image;

    if (length > 0)
        image.loadFromData(QByteArray::fromRawData((const char*)data, length), "JPG");
    else
        image.load(":/pic/no_image.jpg");

    cv::Mat m_image(image.height(), image.width(), CV_8UC4, (uchar*)image.bits(), image.bytesPerLine());

    // detection area 1
    if ((rows2[0] != 0) && (rows2[1] != 0) && (cols2[0] != 0) && (cols2[1] != 0))
    {
        qDebug() << "----------------------------------";
        qDebug() << "detection area 2: loaded";
        qDebug() << rows2[0] << rows2[1] << cols2[0] << cols2[1];
        qDebug() << "----------------------------------";

        // convert image to mat
        cv::Point pt1(cols2[0] - 20, rows2[0]);
        cv::Point pt2(cols2[1] + 20, rows2[1]);
        cv::rectangle(m_image, pt1, pt2, cv::Scalar(0, 255, 0), 2);
        cv::Point pt3(cols2[1], rows2[0] - 20);
        cv::Point pt4(cols2[1], rows2[1] + 20);
        cv::line(m_image, pt3, pt4, cv::Scalar(0, 0, 255), 2);

        cv::Point pt5(10, 20);
        cv::putText(m_image, "LOADED", pt5, cv::FONT_HERSHEY_SIMPLEX, 0.2, cv::Scalar(0, 255, 0), 1);

        ui->image_2->setToolTip("Image Length: " + QString::number(length) + '\n' + "LOADED");
    }
    else
    {
        cv::Point pt1(10, 20);
        cv::putText(m_image, "NOT LOADED", pt1, cv::FONT_HERSHEY_SIMPLEX, 0.2, cv::Scalar(0, 255, 0), 1);

        ui->image_2->setToolTip("Image Length: " + QString::number(length) + '\n' + "NOT LOADED");

        qDebug() << "----------------------------------";
        qDebug() << "detection area 2: not loaded";
        qDebug() << "----------------------------------";
    }

    ui->image_2->setPixmap (QPixmap::fromImage(image));
    ui->lblLength2->setText(QString::number  (length));
}
// -------------------------------------------------------------------------------

// test event handlers
// -------------------------------------------------------------------------------
void Device::on_btnDev_clicked(void)
{
    testRequest[0] = 1;
}

void Device::on_btnTak_clicked(void)
{
    testRequest[1] = 1;
}

void Device::on_btnCal_clicked(void)
{
    testRequest[2] = 1;
}

void Device::on_btnMem_clicked(void)
{
    testRequest[3] = 1;
}

void Device::on_btnGet_clicked(void)
{
    testRequest[4] = 1;
}
// -------------------------------------------------------------------------------

// general event handlers
// -------------------------------------------------------------------------------
void Device::updateActive(int active)
{
    switch (active)
    {
    case 0:
        ui->tab->widget(1)->setEnabled(0);
        ui->tab->widget(2)->setEnabled(0);
        ui->tab->setCurrentIndex(0);
        break;
    case 1:
        ui->tab->widget(1)->setEnabled(1);
        ui->tab->widget(2)->setEnabled(1);
        ui->tab->setCurrentIndex(1);
    }
}

void Device::saveHandlers(void)
{
    QFileDialog fileDialog(this);
    fileDialog.setModal(1);
    fileDialog.setWindowTitle("Please select a location and name for the image and click save");
    fileDialog.setNameFilter(tr("Image Files (*.jpg)"));
    fileDialog.setDirectory("/home/pieter/Documents/");
    fileDialog.setMinimumHeight(650);
    fileDialog.setMaximumWidth(800);
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);

    int ret = fileDialog.exec();
    switch (ret)
    {
        case 0:
        {
            break;
        }
        case 1:
        {
            std::fstream outFile;
            outFile = std::fstream(fileDialog.selectedFiles()[0].toStdString(), std::ios::out | std::ios::binary);
            for (int index = 0; index < imageCounter1; index++)
            {
                if (&saveButtonCam1[index] == sender())
                {
                    outFile.write((char*)pictureBuffer1[index], pictureBufferLength1[index]*sizeof(uint8_t));
                }
            }
            for (int index = 0; index < imageCounter2; index++)
            {
                if (&saveButtonCam2[index] == sender())
                {
                    outFile.write((char*)pictureBuffer2[index], pictureBufferLength2[index]*sizeof(uint8_t));
                }
            }
            outFile.close();
            break;
        }
    }
}
// -------------------------------------------------------------------------------
