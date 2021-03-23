#ifndef ATHLETE_H
#define ATHLETE_H

// qt
#include <QDialog>
#include <QLabel>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDebug>

// standard
#include <fstream>

// opencv
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace Ui {
class Athlete;
}

class Athlete : public QDialog
{
    Q_OBJECT

public:

    // constructor and destructor
    explicit Athlete(QWidget *parent = nullptr);
    ~Athlete();

    // dialog data
    QString* getDialogData(void );
    void setDialogData(QString**);
    void setDialogData(QString* );

    // images
    void setRacStartTime(int, int, int, int, int, int, int);
    void setFinishImage1(uint8_t*, int, int, int, int, int, int, int, int, int[2], int[2]);
    void setFinishImage2(uint8_t*, int, int, int, int, int, int, int, int, int[2], int[2]);

    // general event handlers
    void updateActive(int );

private slots:

    // general event handlers
    void saveHandlers(void);

private:

    // ui
    Ui::Athlete *ui;

    // image containers
    uint8_t* imageBuffer;
    int imageBufferLength;

    // time
    int year_s;
    int month_s;
    int day_s;
    int hour_s;
    int minute_s;
    int second_s;
    int split_s;
};

#endif // ATHLETE_H
