#include "progress.h"
#include "ui_progress.h"

Progress::Progress(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Progress)
{
    // create the ui
    ui->setupUi(this);

    // connect progressbar to dialog
    connect(ui->bar, &QProgressBar::valueChanged, this, &Progress::checkProgress);

    // connect timer to progressbar
    barTimer = new QTimer(this);
    connect(barTimer, &QTimer::timeout, this, &Progress::updateProgress);
}

Progress::~Progress()
{
    // stop and delete the timer
    barTimer->stop();
    delete barTimer;

    // delete the ui
    delete ui;
}

// progress
// -------------------------------------------------------------------------------
void Progress::startProgress(float miliseconds)
{
    barMaximum = miliseconds*100;

    // set min max
    ui->bar->setMinimum(0);
    ui->bar->setMaximum(barMaximum);

    // set value
    ui->bar->setValue(0);

    // start an timer to update every 10ms
    barTimer->start(10);
}

void Progress::updateProgress(void)
{
    // update the progress bar
    ui->bar->setValue(ui->bar->value()+1);
}

void Progress::checkProgress(void)
{
    if (ui->bar->value() == barMaximum)
    {
        Progress::close();
    }
}

void Progress::stopProgress(void)
{
    // set value
    float updated = barMaximum-(0.05*barMaximum);
    if (updated > ui->bar->value())
    {
        ui->bar->setValue(updated);
    }
}
// -------------------------------------------------------------------------------
