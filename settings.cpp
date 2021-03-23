#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    // create the uie
    ui->setupUi(this);

    // ensure settings menu always opens on the first tab
    ui->tab->setCurrentIndex(0);

    // save the date and update the date selector
    QDate date = ui->calSelector->selectedDate();
    ui->datSelector->setDate(date);

    // set the current date and time
    ui->timSelector->setTime(QTime::currentTime());
    ui->datSelector->setDate(QDate::currentDate());
}

Settings::~Settings()
{
    delete ui;
}

// general event handlers
// -------------------------------------------------------------------------------
void Settings::on_calSelector_clicked(const QDate& date)
{
    ui->datSelector->setDate(date);
}

void Settings::on_chkAutomatc_toggled(bool checked)
{
    if (checked)
    {
        ui->datSelector->setDisabled(1);
        ui->timSelector->setDisabled(1);
        ui->calSelector->setDisabled(1);
    }
    else
    {
        ui->datSelector->setDisabled(0);
        ui->timSelector->setDisabled(0);
        ui->calSelector->setDisabled(0);
    }
}
// -------------------------------------------------------------------------------

// dialog data
// -------------------------------------------------------------------------------
QString* Settings::getDialogData(void)
{
    QString *data = new QString[11];

    data[0] = QString::number(ui->cmbResolution->currentIndex());
    data[1] = QString::number(ui->cmbBaud->currentIndex());
    data[2] = QString::number(ui->spnQuality->value());
    data[3] = QString::number(ui->cmbSensitivity->currentIndex());

    // automatic time and date
    data[4] = QString::number(ui->chkAutomatc->isChecked());

    if (data[4].toInt() == 1)
    {
        // the current time will be saved and transmitted to all connected devices
        // save the time
        data[5 ] = QString::number(QTime::currentTime().hour());
        data[6 ] = QString::number(QTime::currentTime().minute());
        data[7 ] = QString::number(QTime::currentTime().second());

        //save the data
        data[8 ] = QString::number(QDate::currentDate().year());
        data[9 ] = QString::number(QDate::currentDate().month());
        data[10] = QString::number(QDate::currentDate().day());
    }
    else
    {
        // the custom time will be saved and transmitted to all connected devices
        // save the time
        data[5 ] = QString::number(ui->timSelector->time().hour());
        data[6 ] = QString::number(ui->timSelector->time().minute());
        data[7 ] = QString::number(ui->timSelector->time().second());

        //save the data
        data[8 ] = QString::number(ui->datSelector->date().year());
        data[9 ] = QString::number(ui->datSelector->date().month());
        data[10] = QString::number(ui->datSelector->date().day());
    }

    return data;
}

void Settings::setDialogData(QString* receivedData)
{
    ui->cmbResolution->setCurrentIndex(receivedData[0].toInt());
    ui->cmbBaud->setCurrentIndex(receivedData[1].toInt());
    ui->spnQuality->setValue(receivedData[2].toInt());
    ui->cmbSensitivity->setCurrentIndex(receivedData[3].toInt());
    ui->chkAutomatc->setChecked(receivedData[4].toInt());
}
// -------------------------------------------------------------------------------
