#ifndef DEVICE_H
#define DEVICE_H

// qt
#include <QDialog>
#include <QLabel>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDebug>

// standard
#include <fstream>
#include <string>

// opencv
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace Ui {
class Device;
}

class Device : public QDialog
{
    Q_OBJECT

public:

    // constructor and destructor
    explicit Device(QWidget *parent = nullptr);
    ~Device();

    // dialog data
    QString* getDialogData(void);
    void setDialogData(QString*);

    // images
    void setTakenImage1(uint8_t*[3000], int[3000], int[3000], int[3000], int[3000], int[3000], int[3000], int[3000], int[3000], int);
    void setTakenImage2(uint8_t*[3000], int[3000], int[3000], int[3000], int[3000], int[3000], int[3000], int[3000], int[3000], int);
    void setWiningImage(uint8_t*, int, int, int, int, int, int, int, int);
    void setDetections1(int*, int*);
    void setDetections2(int*, int*);
    void setDisplayImg1(uint8_t*, int);
    void setDisplayImg2(uint8_t*, int);

    // general event handlers
    void updateActive(int );

private slots:

    // test event handlers
    void on_btnDev_clicked(void);
    void on_btnTak_clicked(void);
    void on_btnCal_clicked(void);
    void on_btnGet_clicked(void);
    void on_btnMem_clicked(void);

    // general event handlers
    void saveHandlers(void);

private:

    // ui
    Ui::Device *ui;

    // device help image
    QPixmap *pix;

    // image containers
    QImage *imagePixmpCam1, *imagePixmpCam2;
    QLabel *imageLabelCam1, *imageLabelCam2;
    QHBoxLayout *layoutCam1, *layoutCam2;
    QPushButton *saveButtonCam1, *saveButtonCam2;

    // image pointers
    uint8_t **pictureBuffer1, **pictureBuffer2;
    int *pictureBufferLength1, *pictureBufferLength2;
    int imageCounter1, imageCounter2;
    int *rows1, *cols1;
    int *rows2, *cols2;

    // test requests
    int testRequest[5];
};

#endif // DEVICE_H
