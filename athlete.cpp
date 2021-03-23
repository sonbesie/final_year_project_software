#include "athlete.h"
#include "ui_athlete.h"

Athlete::Athlete(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Athlete)
{
    // create the ui
    ui->setupUi(this);

    // initialise
    imageBuffer = NULL;
    imageBufferLength = 0;

    // time
    year_s = 0;
    month_s = 0;
    day_s = 0;
    hour_s = 0;
    minute_s = 0;
    second_s = 0;
    split_s = 0;

    // connect
    connect(ui->btnSave, SIGNAL(clicked()), SLOT(saveHandlers()));

    // set active to false
    updateActive(false);
}

Athlete::~Athlete()
{
    delete ui;
}

// dialog data
// -------------------------------------------------------------------------------
QString* Athlete::getDialogData(void)
{
    QString* returnData = new QString[4];

    returnData[0] = ui->edtName->text();                     // athlete name
    returnData[1] = ui->edtSurname->text();                  // athlete surname
    returnData[2] = QString::number(ui->spnAge->value());    // athlete age
    returnData[3] = ui->cmbLane->currentText();              // athlete lane

    return returnData;
}

void Athlete::setDialogData(QString** receivedData)
{
    for (int item = 0; item < 8; ++item)
    {
        if (receivedData[item] == NULL)
        {
            ui->cmbLane->addItem(QString::number(item+1), QVariant::String);
        }
    }
}

void Athlete::setDialogData(QString*  receivedData)
{
    ui->edtName->setText(receivedData[0]);
    ui->edtSurname->setText(receivedData[1]);
    ui->spnAge->setValue(receivedData[2].toInt());
    ui->cmbLane->addItem(receivedData[3], QVariant::String);

    updateActive(true);
}
// -------------------------------------------------------------------------------

// images
// -------------------------------------------------------------------------------
void Athlete::setRacStartTime(int year, int month, int day, int hour, int minute, int second, int split)
{
    year_s = year;
    month_s = month;
    day_s = day;
    hour_s = hour;
    minute_s = minute;
    second_s = second;
    split_s = split;
}

void Athlete::setFinishImage1(uint8_t* data, int length, int year, int month, int day, int hour, int minute, int second, int split, int row[2], int col[2])
{
    if (length > 0)
    {
        ui->btnSave->setEnabled(true );

        QImage image;
        image.loadFromData(QByteArray::fromRawData((const char*)data, length), "JPG");
        cv::Mat m_image(image.height(), image.width(), CV_8UC4, (uchar*)image.bits(), image.bytesPerLine());

        cv::Point pt1(col[0], row[0] - 20);
        cv::Point pt2(col[0], row[1] + 20);
        cv::line(m_image, pt1, pt2, cv::Scalar(0, 0, 255), 2);

        cv::Point pt3(10, 20);
        cv::putText(m_image, "FINISHED", pt3, cv::FONT_HERSHEY_SIMPLEX, 0.2, cv::Scalar(0, 255, 0), 1);

        ui->lblDateTaken->setText(QString::number(year  ) +"-"+ QString::number(month   ) +"-"+ QString::number(day     ));
        ui->lblTimeTaken->setText(QString::number(hour  ) +"-"+ QString::number(minute  ) +"-"+ QString::number(second  ) +"-"+ QString::number(split  ));
        ui->lblTimeStart->setText(QString::number(hour_s) +"-"+ QString::number(minute_s) +"-"+ QString::number(second_s) +"-"+ QString::number(split_s));
        ui->lblImage->setPixmap(QPixmap::fromImage(image));
        ui->lblLength->setText (QString::number  (length));

        imageBuffer = data;
        imageBufferLength = length;
    }
    else
    {
        ui->btnSave->setEnabled(false);
    }
}

void Athlete::setFinishImage2(uint8_t* data, int length, int year, int month, int day, int hour, int minute, int second, int split, int row[2], int col[2])
{
    if (length > 0)
    {
        ui->btnSave->setEnabled(true );

        QImage image;
        image.loadFromData(QByteArray::fromRawData((const char*)data, length), "JPG");
        cv::Mat m_image(image.height(), image.width(), CV_8UC4, (uchar*)image.bits(), image.bytesPerLine());

        cv::Point pt1(col[1], row[0] - 20);
        cv::Point pt2(col[1], row[1] + 20);
        cv::line(m_image, pt1, pt2, cv::Scalar(0, 0, 255), 2);

        cv::Point pt3(10, 20);
        cv::putText(m_image, "FINISHED", pt3, cv::FONT_HERSHEY_SIMPLEX, 0.2, cv::Scalar(0, 255, 0), 1);

        ui->lblDateTaken->setText(QString::number(year  ) +"-"+ QString::number(month   ) +"-"+ QString::number(day     ));
        ui->lblTimeTaken->setText(QString::number(hour  ) +"-"+ QString::number(minute  ) +"-"+ QString::number(second  ) +"-"+ QString::number(split  ));
        ui->lblTimeStart->setText(QString::number(hour_s) +"-"+ QString::number(minute_s) +"-"+ QString::number(second_s) +"-"+ QString::number(split_s));
        ui->lblImage->setPixmap(QPixmap::fromImage(image));
        ui->lblLength->setText (QString::number  (length));

        imageBuffer = data;
        imageBufferLength = length;
    }
    else
    {
        ui->btnSave->setEnabled(false);
    }
}
// -------------------------------------------------------------------------------

// general event handlers
// -------------------------------------------------------------------------------
void Athlete::updateActive(int active)
{
    switch (active)
    {
    case 0:
        ui->tab->widget(1)->setEnabled(0);
        ui->tab->setCurrentIndex(0);
        break;
    case 1:
        ui->tab->widget(1)->setEnabled(1);
        ui->tab->setCurrentIndex(0);
    }
}

void Athlete::saveHandlers(void)
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
        break;
    case 1:
        std::fstream outFile;
        outFile = std::fstream(fileDialog.selectedFiles()[0].toStdString(), std::ios::out | std::ios::binary);
        outFile.write((char*)imageBuffer, imageBufferLength*sizeof(uint8_t));
        outFile.close();
        break;
    }
}
// -------------------------------------------------------------------------------
