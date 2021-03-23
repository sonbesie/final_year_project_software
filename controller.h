#ifndef CONTROLLER_H
#define CONTROLLER_H

// qt
#include <QMainWindow>
#include <QtWidgets>
#include <QSocketNotifier>
#include <QTimer>

// forms
#include "about.h"
#include "settings.h"
#include "athlete.h"
#include "device.h"
#include "progress.h"

// standard
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <ctime>

// network
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <fcntl.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Controller; }
QT_END_NAMESPACE

//////////////////////////////////
// main states
#define RACE_BUSY 1
#define RACE_IDLE 0
//////////////////////////////////

//////////////////////////////////
// settings states
#define SETT_READ 0
#define SETT_WRIT 1
//////////////////////////////////

//////////////////////////////////
// network commands
#define COMM_ARME 3
#define COMM_ARMD 4
#define COMM_SETT 1
#define COMM_TAKE 6
#define COMM_DVAL 8
#define COMM_RATE 7
#define COMM_PING 2
#define COMM_PAIR 0
#define COMM_TOGG 5
#define COMM_READ 9
#define COMM_GETD 10

// network delays
#define TIME_ARME 00.20
#define TIME_ARMD 00.20
#define TIME_SETT 10.00
#define TIME_TAKE 15.00
#define TIME_RATE 15.00
#define TIME_DVAL 35.00
#define TIME_PING 00.20
#define TIME_PAIR 00.20
#define TIME_TOGG 05.00
#define TIME_READ 10.00
#define TIME_GETD 00.20
#define TIME_COMM 00005

// network states
#define TRAN_BUSY 1
#define TRAN_IDLE 0
//////////////////////////////////

//////////////////////////////////
// keep alive time
#define KEEP_ALIV 60000
//////////////////////////////////

//////////////////////////////////
// maximum devices and athletes
#define MAXIMUM_DEV 8
#define MAXIMUM_ATH 8
//////////////////////////////////


class Controller : public QMainWindow
{
    Q_OBJECT

public:

    // constructor and destructor
    Controller(QWidget *parent = nullptr);
    ~Controller();

private slots:

    // static menu
    void on_actionAbtDialg_triggered(void);
    void on_actionSettings_triggered(void);
    void on_actionExtDialg_triggered(void);
    void on_actionSavDialg_triggered(void);
    void on_actionUpdtAths_triggered(void);
    void on_actionLoadAths_triggered(void);
    void on_actionUpdtDevs_triggered(void);
    void on_actionLoadDevs_triggered(void);

    // static buttons
    void on_RaceStr_clicked(void);
    void on_RaceStp_clicked(void);
    void on_RaceRes_clicked(void);

    // runtime buttons
    void athSetHandler(void);
    void athDelHandler(void);
    void devSetHandler(void);
    void devDelHandler(void);
    void createDevices(int, QString*);
    void deleteDevices(int);
    void createAthlete(int, QString*);
    void deleteAthlete(int);

    // runtime timers
    void updTimHandler(void);
    void updAlvHandler(void);
    void strKeepAlives(void);
    void stpKeepAlives(void);

    // receive
    void receiveHandler(int);

private:

    // ui
    Ui::Controller *ui;

    // transmt
    void transmt(char*, char*, int);
    void transmtCommand(int, int);
    void transmtHandler(int, float, int);
    void transmtErrorHandler(void);

    // receive
    void receive(void);
    void receiveErrorHandler(uint8_t, int, QString);

    // settings
    void settingsReadWrite(int);

    // flags
    int tran, race;

    // athletes
    int athInitialised[MAXIMUM_ATH];
    QLCDNumber **athleteTime;
    QHBoxLayout **athleteLayout;
    QLabel **athleteName, **athleteSurname, **laneNumber;
    QPushButton **athleteSettings, **athleteDelete;
    QString **athleteSettingsData;

    // devices
    int devInitialised[MAXIMUM_DEV];
    QHBoxLayout **deviceLayout;
    QLabel **deviceName, **deviceAddress, **deviceStatus, **deviceSettingsStatus;
    QPushButton **deviceSettings, **deviceDelete;
    QString **deviceSettingsData;
    QPixmap **statusPix, **settingsPix;
    QString *universalSettings;

    // statusbar
    QPixmap *statusPix1;
    QPixmap *statusPix2;
    QLabel *statuslabel1;
    QLabel *statuspic1;
    QLabel *statuslabel2;
    QLabel *statuslabel3;
    QLabel *statuspic2;
    QHBoxLayout *statusLayout;
    QWidget *container;

    // timers
    QTimer *timer, *maint;
    QElapsedTimer *elaps ;

    // progress pointer
    Progress *progressPointer;

    //////////////////////////////////
    // image buffer
    #define IMAGE_BUFFER_SIZE 3000
    //////////////////////////////////

    // picture buffer 1 (camera 1)
    uint8_t *pictureBuffer1[MAXIMUM_DEV][IMAGE_BUFFER_SIZE];
    int pictureBufferLength1[MAXIMUM_DEV][IMAGE_BUFFER_SIZE];
    int year1[MAXIMUM_DEV][IMAGE_BUFFER_SIZE], month1[MAXIMUM_DEV][IMAGE_BUFFER_SIZE], day1[MAXIMUM_DEV][IMAGE_BUFFER_SIZE], hour1[MAXIMUM_DEV][IMAGE_BUFFER_SIZE],
        minute1[MAXIMUM_DEV][IMAGE_BUFFER_SIZE], second1[MAXIMUM_DEV][IMAGE_BUFFER_SIZE], split1[MAXIMUM_DEV][IMAGE_BUFFER_SIZE];
    int rows1[MAXIMUM_DEV][2], cols1[MAXIMUM_DEV][2];

    // picture buffer 2 (camera 2)
    uint8_t *pictureBuffer2[MAXIMUM_DEV][IMAGE_BUFFER_SIZE];
    int pictureBufferLength2[MAXIMUM_DEV][IMAGE_BUFFER_SIZE];
    int year2[MAXIMUM_DEV][IMAGE_BUFFER_SIZE], month2[MAXIMUM_DEV][IMAGE_BUFFER_SIZE], day2[MAXIMUM_DEV][IMAGE_BUFFER_SIZE], hour2[MAXIMUM_DEV][IMAGE_BUFFER_SIZE],
        minute2[MAXIMUM_DEV][IMAGE_BUFFER_SIZE], second2[MAXIMUM_DEV][IMAGE_BUFFER_SIZE], split2[MAXIMUM_DEV][IMAGE_BUFFER_SIZE];
    int rows2[MAXIMUM_DEV][2], cols2[MAXIMUM_DEV][2];

    // picture buffer 3 (finish 1 & 2)
    uint8_t *pictureBuffer3[MAXIMUM_ATH];
    int pictureBufferLength3[MAXIMUM_ATH], year3[MAXIMUM_ATH], month3[MAXIMUM_ATH], day3[MAXIMUM_ATH], hour3[MAXIMUM_ATH], minute3[MAXIMUM_ATH], second3[MAXIMUM_ATH], split3[MAXIMUM_ATH];

    // image counter
    int imageCounter1[MAXIMUM_DEV];
    int imageCounter2[MAXIMUM_DEV];

    // image to show in preview
    int currentImage1[MAXIMUM_DEV];
    int currentImage2[MAXIMUM_DEV];

    int camFlag[MAXIMUM_ATH];

    int year, month, day, hour, minute, second, split;
};
#endif // CONTROLLER_H
