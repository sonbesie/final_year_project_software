#include "controller.h"
#include "ui_controller.h"

Controller::Controller(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Controller)
{
    // create the ui
    ui->setupUi(this);

    // initialise transmission state
    tran = TRAN_IDLE;
    race = RACE_IDLE;

    // initialise universal settings
    universalSettings = NULL;

    // create athlete variables
    athleteLayout = new QHBoxLayout*[MAXIMUM_ATH];
    athleteTime = new QLCDNumber*[MAXIMUM_ATH];
    athleteName = new QLabel*[MAXIMUM_ATH];
    athleteSurname = new QLabel*[MAXIMUM_ATH];
    laneNumber = new QLabel*[MAXIMUM_ATH];
    athleteSettings = new QPushButton*[MAXIMUM_ATH];
    athleteDelete = new QPushButton*[MAXIMUM_ATH];
    athleteSettingsData = new QString*[MAXIMUM_ATH];

    // initialise athlete variables
    for (int index = 0; index < MAXIMUM_ATH; ++index)
    {
        athInitialised[index] = 0;
        athleteLayout[index] = NULL;
        athleteTime[index] = NULL;
        athleteName[index] = NULL;
        athleteSurname[index] = NULL;
        laneNumber[index] = NULL;
        athleteSettings[index] = NULL;
        athleteDelete[index] = NULL;
        athleteSettingsData[index] = NULL;
    }

    // create device variables
    deviceLayout = new QHBoxLayout*[MAXIMUM_DEV];
    deviceName = new QLabel*[MAXIMUM_DEV];
    deviceAddress = new QLabel*[MAXIMUM_DEV];
    deviceStatus = new QLabel*[MAXIMUM_DEV];
    deviceSettingsStatus = new QLabel*[MAXIMUM_DEV];
    deviceSettings = new QPushButton*[MAXIMUM_DEV];
    deviceDelete = new QPushButton*[MAXIMUM_DEV];
    deviceSettingsData = new QString*[MAXIMUM_DEV];
    statusPix = new QPixmap*[MAXIMUM_DEV];
    settingsPix = new QPixmap*[MAXIMUM_DEV];

    // initialise devive variables
    for (int index = 0; index < MAXIMUM_DEV; ++index)
    {
        devInitialised[index] = 0;
        deviceLayout[index] = NULL;
        deviceName[index] = NULL;
        deviceAddress[index] = NULL;
        deviceStatus[index] = NULL;
        deviceSettingsStatus[index] = NULL;
        deviceSettings[index] = NULL;
        deviceDelete[index] = NULL;
        deviceSettingsData[index] = NULL;
        statusPix[index] = NULL;
        settingsPix[index] = NULL;
    }

    // create statusbar variables
    statusPix1 = new QPixmap;
    statusPix2 = new QPixmap;
    statuslabel1 = new QLabel("Network:");
    statuspic1 = new QLabel();
    statuslabel2 = new QLabel("|");
    statuslabel3 = new QLabel("Armed:");
    statuspic2 = new QLabel();
    statusLayout = new QHBoxLayout();
    container = new QWidget;

    // initialise statusbar variables
    statuslabel1->setMinimumWidth(50);
    statuslabel2->setMinimumWidth(0);
    statuslabel3->setMinimumWidth(50);
    statusLayout->addWidget(statuslabel1);
    statusLayout->addWidget(statuspic1);
    statusLayout->addWidget(statuslabel2);
    statusLayout->addWidget(statuslabel3);
    statusLayout->addWidget(statuspic2);
    container->setLayout(statusLayout);
    ui->statusbar->insertPermanentWidget(0, container);

    // create icon
    statusPix1->load(":/pic/red.png");
    statusPix1->setDevicePixelRatio(5);
    statuspic1->setPixmap(*statusPix1);

    // create icon
    statusPix2->load(":/pic/red.png");
    statusPix2->setDevicePixelRatio(5);
    statuspic2->setPixmap(*statusPix2);

    // create and connect the create alive timer
    timer = NULL;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(updAlvHandler()));

    // create and connect the main and elapsed timer
    maint = NULL;
    maint = new QTimer(this);
    elaps = new QElapsedTimer();
    connect(maint, SIGNAL(timeout()), SLOT(updTimHandler()));

    // initialise time
    year = 0;
    month = 0;
    day = 0;
    hour = 0;
    minute = 0;
    second = 0;
    split = 0;

    // initialise image buffer counters
    for (int index = 0; index < MAXIMUM_DEV; ++index)
    {
        for (int item = 0; item < IMAGE_BUFFER_SIZE; item++)
        {
            pictureBufferLength1[index][item] = 0;
            pictureBufferLength2[index][item] = 0;

            pictureBuffer1[index][item] = NULL;
            pictureBuffer2[index][item] = NULL;
        }
        imageCounter1[index] = 0;
        imageCounter2[index] = 0;
        currentImage1[index] = 0;
        currentImage2[index] = 0;
    }

    for (int index = 0; index < MAXIMUM_ATH; ++index)
    {
        pictureBufferLength3[index] = 0;
        pictureBuffer3[index] = NULL;
    }

    // initialise progress pointer
    progressPointer = NULL;

    // read the global configuration file
    settingsReadWrite(SETT_READ);

    // activate the network receive routine
    receive();
}

Controller::~Controller()
{
    delete ui;
}

// settings
// -------------------------------------------------------------------------------
void Controller::settingsReadWrite(int mode)
{
    stpKeepAlives();

    // avaiable options in the settings file
    std::string line, key, value, options[5];
    options[0] = "resolution";
    options[1] = "baud";
    options[2] = "quality";
    options[3] = "sensitivity";
    options[4] = "automatic";

    // open the file
    std::fstream saved;
    saved.open("settings.ini", std::ios::in | std::ios::out);

    // if the file does not exist create and open a new one
    if (saved.is_open() == 0) saved.open("settings.ini", std::ios::in | std::ios::out | std::ios::trunc);

    switch (mode)
    {
    case SETT_READ:
        if (saved.is_open())
        {
            universalSettings = new QString[11];
            while (std::getline(saved, line))
            {
                if ((line[0] != '#') && (line.length() != 0))
                {
                    key = line.substr(0, line.find_first_of(" "));
                    value = line.substr(line.find_first_of(" ")+1, line.find_last_of('\n'));
                    for (int index = 0; index < 5; index++)
                    {
                        if (options[index] == key)
                        {
                            universalSettings[index] = QString::fromStdString(value);
                            break;
                        }
                    }
                }
            }
            if (universalSettings[4] == "1")
            {
                // time
                universalSettings[5 ] = "0";
                universalSettings[6 ] = "0";
                universalSettings[7 ] = "0";
                // date
                universalSettings[8 ] = "0";
                universalSettings[9 ] = "0";
                universalSettings[10] = "0";
            }
        }
        break;
    case SETT_WRIT:
        if (saved.is_open())
        {
            saved << "# This is the configuration file for the Athletics Information System" << "\n";
            for (int index = 0; index < 5; index++)
            {
                switch (index)
                {
                case 0:
                    saved << "\n";
                    saved << "# Camera" << "\n";
                    break;
                case 4:
                    saved << "\n";
                    saved << "# Time" << "\n";
                    break;
                }
                saved << options[index] << " " << universalSettings[index].toStdString() << "\n";
            }
        }
        break;
    }

    // close the file
    saved.close();

    strKeepAlives();
}
// -------------------------------------------------------------------------------

// transmt
// -------------------------------------------------------------------------------
void Controller::transmt(char* address, char* data, int size)
{
    int times = std::ceil((float)size/(float)1500);
    int count = 0, prev = 0, t_size;

    while (count < times)
    {
        prev = 1500*count;
        if (count == (times-1))
            t_size = size-((times-1)*1500);
        else
            t_size = 1500;

        int sock;
        char sendbuf[(t_size+14)];
        struct ifreq if_idx;
        struct ifreq if_mac;
        char ifName[IFNAMSIZ] = "eno1";
        struct ether_header *eh = (struct ether_header *) sendbuf;
        struct sockaddr_ll socket_address;

        sock = socket(AF_PACKET, SOCK_RAW, htons(0xEEFA));

        if(sock < 0)
        {
            // update status indicators
            statusPix1->load(":/pic/red.png");
            statusPix1->setDevicePixelRatio(5);
            statuspic1->setPixmap(*statusPix1);
        }
        else
        {
            // update status indicators
            statusPix1->load(":/pic/green.png");
            statusPix1->setDevicePixelRatio(5);
            statuspic1->setPixmap(*statusPix1);
        }

        memset(&if_idx, 0, sizeof(struct ifreq));
        strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
        ioctl(sock, SIOCGIFINDEX, &if_idx);

        memset(&if_mac, 0, sizeof(struct ifreq));
        strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
        ioctl(sock, SIOCGIFHWADDR, &if_mac);

        memset(sendbuf, 0, (t_size+14));

        eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
        eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
        eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
        eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
        eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
        eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
        eh->ether_dhost[0] = address[0];
        eh->ether_dhost[1] = address[1];
        eh->ether_dhost[2] = address[2];
        eh->ether_dhost[3] = address[3];
        eh->ether_dhost[4] = address[4];
        eh->ether_dhost[5] = address[5];

        eh->ether_type = htons(0xEEFA);
        socket_address.sll_ifindex = if_idx.ifr_ifindex;
        socket_address.sll_halen = ETH_ALEN;

        for (int index = 0; index < t_size; index++)
        {
            sendbuf[index+14] = data[index+prev];
        }

        if (sendto(sock, sendbuf, (t_size+14), 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
        {
            // update status indicators
            statusPix1->load(":/pic/red.png");
            statusPix1->setDevicePixelRatio(5);
            statuspic1->setPixmap(*statusPix1);
        }

        ++count;
    }
}

void Controller::transmtCommand(int mode, int index)
{
    char address[6];
    QPixmap *indicator;
    QLabel *status;

    // create the device address
    std::string ret = deviceSettingsData[index][1].toStdString();
    address[0] = stoi(ret.substr(0 , 2), 0, 16);
    address[1] = stoi(ret.substr(3 , 2), 0, 16);
    address[2] = stoi(ret.substr(6 , 2), 0, 16);
    address[3] = stoi(ret.substr(9 , 2), 0, 16);
    address[4] = stoi(ret.substr(12, 2), 0, 16);
    address[5] = stoi(ret.substr(15, 2), 0, 16);

    // set the indicator based in the message
    if (mode == COMM_SETT)
    {
        // the settings indicator
        indicator = settingsPix[index];
        status = deviceSettingsStatus[index];
    }
    else
    {
        // the status indicator
        indicator = statusPix[index];
        status = deviceStatus[index];
    }

    // switch state to unknown
    indicator->load(":/pic/red.png");
    indicator->setDevicePixelRatio(4);
    status->setPixmap(*settingsPix[index]);

    switch (mode)
    {
    case COMM_SETT:
    {
        // display message
        ui->statusbar->showMessage("COMM_SETT");
        qDebug() << "----------------------------------";
        qDebug() << "COMM_SETT" << index;
        qDebug() << "----------------------------------";

        // declare the data array
        char data[11];
        // fill data array
        data[0 ] = 0x42;
        data[1 ] = universalSettings[0 ].toInt()+1;   // combo box 1
        data[2 ] = universalSettings[1 ].toInt()+1;   // combo box 2
        data[3 ] = universalSettings[2 ].toInt();
        data[4 ] = universalSettings[3 ].toInt()+1;   // combo box 3

        data[5 ] = universalSettings[5 ].toInt();
        data[6 ] = universalSettings[6 ].toInt();
        data[7 ] = universalSettings[7 ].toInt();

        data[8 ] = universalSettings[8 ].toInt()-2000;
        data[9 ] = universalSettings[9 ].toInt();
        data[10] = universalSettings[10].toInt();

        // transmit the data to the address
        transmt(address, data, strlen(data));
        std::chrono::milliseconds timespan(TIME_COMM);
        std::this_thread::sleep_for(timespan);

        break;
    }
    case COMM_PAIR:
    {
        // display message
        ui->statusbar->showMessage("COMM_PAIR");
        qDebug() << "----------------------------------";
        qDebug() << "COMM_PAIR" << index;
        qDebug() << "----------------------------------";

        // declare the data array
        char data[11];
        // fill the data array
        data[0 ] = 0x48;

        // transmit the data to the address
        transmt(address, data, strlen(data));
        std::chrono::milliseconds timespan(TIME_COMM);
        std::this_thread::sleep_for(timespan);

        break;
    }
    case COMM_PING:
    {
        // display message
        ui->statusbar->showMessage("COMM_PING");
        qDebug() << "----------------------------------";
        qDebug() << "COMM_PING" << index;
        qDebug() << "----------------------------------";

        // declare the data array
        char data[11];
        // fill the data array
        data[0 ] = 0x47;

        // transmit the data to the address
        transmt(address, data, strlen(data));
        std::chrono::milliseconds timespan(TIME_COMM);
        std::this_thread::sleep_for(timespan);

        break;
    }
    case COMM_ARME:
    {
        statusPix2->load(":/pic/green.png");
        statusPix2->setDevicePixelRatio(5);
        statuspic2->setPixmap(*statusPix2);

        // display message
        ui->statusbar->showMessage("COMM_ARME");
        qDebug() << "----------------------------------";
        qDebug() << "COMM_ARME" << index;
        qDebug() << "----------------------------------";

        // declare the data array
        char data[11];
        // fill the data array
        data[0 ] = 0x40;

        // transmit the data to the address
        transmt(address, data, strlen(data));
        std::chrono::milliseconds timespan(TIME_COMM);
        std::this_thread::sleep_for(timespan);

        break;
    }
    case COMM_ARMD:
    {
        statusPix2->load(":/pic/red.png");
        statusPix2->setDevicePixelRatio(5);
        statuspic2->setPixmap(*statusPix2);

        // display message
        ui->statusbar->showMessage("COMM_ARMD");
        qDebug() << "----------------------------------";
        qDebug() << "COMM_ARMD" << index;
        qDebug() << "----------------------------------";

        // declare the data array
        char data[11];
        // fill the data array
        data[0 ] = 0x41;

        // transmit the data to the address
        transmt(address, data, strlen(data));
        std::chrono::milliseconds timespan(TIME_COMM);
        std::this_thread::sleep_for(timespan);

        break;
    }
    case COMM_TOGG:
    {
        // display message
        ui->statusbar->showMessage("COMM_TOGG");
        qDebug() << "----------------------------------";
        qDebug() << "COMM_TOGG" << index;
        qDebug() << "----------------------------------";

        // declare the data array
        char data[11];
        // fill the data array
        data[0 ] = 0x49;

        // transmit the data to the address
        transmt(address, data, strlen(data));
        std::chrono::milliseconds timespan(TIME_COMM);
        std::this_thread::sleep_for(timespan);

        break;
    }
    case COMM_TAKE:
    {
        // display message
        ui->statusbar->showMessage("COMM_TAKE");
        qDebug() << "----------------------------------";
        qDebug() << "COMM_TAKE" << index;
        qDebug() << "----------------------------------";

        // declare the data array
        char data[11];
        // fill the data array
        data[0 ] = 0x43;

        // transmit the data to the address
        transmt(address, data, strlen(data));
        std::chrono::milliseconds timespan(TIME_COMM);
        std::this_thread::sleep_for(timespan);

        break;
    }
    case COMM_RATE:
    {
        // display message
        ui->statusbar->showMessage("COMM_RATE");
        qDebug() << "----------------------------------";
        qDebug() << "COMM_RATE" << index;
        qDebug() << "----------------------------------";

        // declare the data array
        char data[11];
        // fill the data array
        data[0 ] = 0x44;

        // transmit the data to the address
        transmt(address, data, strlen(data));
        std::chrono::milliseconds timespan(TIME_COMM);
        std::this_thread::sleep_for(timespan);

        break;
    }
    case COMM_DVAL:
    {
        // display message
        ui->statusbar->showMessage("COMM_DVAL");
        qDebug() << "----------------------------------";
        qDebug() << "COMM_DVAL" << index;
        qDebug() << "----------------------------------";

        // declare the data array
        char data[11];
        // fill the data array
        data[0 ] = 0x45;

        // transmit the data to the address
        transmt(address, data, strlen(data));
        std::chrono::milliseconds timespan(TIME_COMM);
        std::this_thread::sleep_for(timespan);

        break;
    }
    case COMM_READ:
    {
        imageCounter1[index] = 0;
        imageCounter2[index] = 0;

        // display message
        ui->statusbar->showMessage("COMM_READ");
        qDebug() << "----------------------------------";
        qDebug() << "COMM_READ" << index;
        qDebug() << "----------------------------------";

        // declare the data array
        char data[11];
        // fill the data array
        data[0 ] = 0x46;

        // transmit the data to the address
        transmt(address, data, strlen(data));
        std::chrono::milliseconds timespan(TIME_COMM);
        std::this_thread::sleep_for(timespan);

        break;
    }
    case COMM_GETD:
    {
        // display message
        ui->statusbar->showMessage("COMM_GETD");
        qDebug() << "----------------------------------";
        qDebug() << "COMM_GETD" << index;
        qDebug() << "----------------------------------";

        // declare the data array
        char data[11];
        // fill the data array
        data[0 ] = 0x50;

        // transmit the data to the address
        transmt(address, data, strlen(data));
        std::chrono::milliseconds timespan(TIME_COMM);
        std::this_thread::sleep_for(timespan);

        break;
    }
    }
}

void Controller::transmtHandler(int mode, float time, int index)
{
    // retry
    do
    {
        tran = TRAN_BUSY;
        Controller::setEnabled(false);
        transmtCommand(mode, index);

        // open progress
        Progress *progress = new Progress(this);
        progressPointer = progress;
        progress->startProgress(time);
        progress->exec();
        delete progress;

        if (tran == TRAN_BUSY)
        {
            // reset
            tran = TRAN_IDLE;
            Controller::setEnabled(true);
            // error handler
            transmtErrorHandler();
            // exit
            break;
        }
        else
        {
            // reset
            tran = TRAN_IDLE;
            Controller::setEnabled(true);
            // exit
            break;
        }
    }
    while (1);
}

void Controller::transmtErrorHandler(void)
{
    //
}
// -------------------------------------------------------------------------------

// receive
// -------------------------------------------------------------------------------
void Controller::receive(void)
{
    int sock;
    sock = socket(AF_PACKET, SOCK_RAW, htons(0xEEFA));

    if(sock == -1)
    {
        // update status indicators
        statusPix1->load(":/pic/red.png");
        statusPix1->setDevicePixelRatio(5);
        statuspic1->setPixmap(*statusPix1);
    }
    else
    {
        // update status indicators
        statusPix1->load(":/pic/green.png");
        statusPix1->setDevicePixelRatio(5);
        statuspic1->setPixmap(*statusPix1);
    }

    fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
    QSocketNotifier *notifier = new QSocketNotifier(sock, QSocketNotifier::Read);
    connect(notifier, SIGNAL(activated(int)), SLOT(receiveHandler(int)));
}

void Controller::receiveHandler(int socket)
{
    // do something with the received data
    uint8_t buff[1514];
    int length;
    while ((length = recv(socket, buff, 1514, 0)) >= 0)
    {
        qDebug() << "----------------------------------";
        qDebug() << "Received" << length << "bytes";
        qDebug() << buff[0] << buff[1] << buff[2] << buff[3] << buff[4] << buff[5] << buff[6] << buff[7] << buff[8] << buff[9] << buff[10] << buff[11] << buff[12] << buff[13];
        qDebug() << buff[14] << buff[15] << buff[16] << buff[17] << buff[18];
        qDebug() << buff[19] << buff[20] << buff[21] << buff[22] << buff[23] << buff[24] << buff[25] << buff[26] << buff[27] << buff[28];
        qDebug() << buff[29] << buff[30] << buff[31] << buff[32] << buff[33] << buff[34] << buff[35] << buff[36] << buff[37] << buff[38];

        // Data Transfer 1
        // CAM1
        if (((buff[14] & 0b11110000)       ) == 0xF0)
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        qDebug() << "Camera 1";

                        // first part of image
                        if (buff[17] == 0x00)
                        {
                            int counter = imageCounter1[index];

                            year1  [index][counter] = buff[19];
                            month1 [index][counter] = buff[20];
                            day1   [index][counter] = buff[21];

                            hour1  [index][counter] = buff[22];
                            minute1[index][counter] = buff[23];
                            second1[index][counter] = buff[24];

                            split1 [index][counter] = ((buff[25]) << 24)|((buff[26]) << 16)|((buff[27]) << 8)|((buff[28]) << 0);

                            if (pictureBuffer1[index] != NULL)
                            {
                                free(pictureBuffer1[index][counter]);
                                pictureBuffer1[index][counter] = NULL;
                            }

                            int len = length - 34;
                            pictureBuffer1[index][counter] = (uint8_t*)malloc(len*sizeof(uint8_t));
                            memcpy(pictureBuffer1[index][counter], &buff[34], len);
                            pictureBufferLength1[index][counter] = len;
                        }
                        // last part of image
                        else if (buff[17] == buff[18])
                        {
                            int counter = imageCounter1[index];

                            int len = length - 34;
                            pictureBuffer1[index][counter] = (uint8_t*)realloc(pictureBuffer1[index][counter], (pictureBufferLength1[index][counter]+len)*sizeof(uint8_t));
                            memcpy(&pictureBuffer1[index][counter][pictureBufferLength1[index][counter]], &buff[29], len);
                            pictureBufferLength1[index][counter] = pictureBufferLength1[index][counter]+len;

                            // increment the image counter
                            ++imageCounter1[index];
                        }
                        // middle part of an image
                        else
                        {
                            int counter = imageCounter1[index];

                            int len = length - 29;
                            pictureBuffer1[index][counter] = (uint8_t*)realloc(pictureBuffer1[index][counter], (pictureBufferLength1[index][counter]+len)*sizeof(uint8_t));
                            memcpy(&pictureBuffer1[index][counter][pictureBufferLength1[index][counter]], &buff[29], len);
                            pictureBufferLength1[index][counter] = pictureBufferLength1[index][counter]+len;
                        }
                    }
                }
            }
        }

        // Data Transfer 2
        // CAM2
        if (((buff[14] & 0b11110000)       ) == 0xE0)
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        qDebug() << "Camera 2";

                        // first part of image
                        if (buff[17] == 0x00)
                        {
                            int counter = imageCounter2[index];

                            year2  [index][counter] = buff[19];
                            month2 [index][counter] = buff[20];
                            day2   [index][counter] = buff[21];

                            hour2  [index][counter] = buff[22];
                            minute2[index][counter] = buff[23];
                            second2[index][counter] = buff[24];

                            split2 [index][counter] = ((buff[25]) << 24)|((buff[26]) << 16)|((buff[27]) << 8)|((buff[28]) << 0);

                            if (pictureBuffer2[index] != NULL)
                            {
                                free(pictureBuffer2[index][counter]);
                                pictureBuffer2[index][counter] = NULL;
                            }

                            int len = length - 34;
                            pictureBuffer2[index][counter] = (uint8_t*)malloc(len*sizeof(uint8_t));
                            memcpy(pictureBuffer2[index][counter], &buff[34], len);
                            pictureBufferLength2[index][counter] = len;
                        }
                        // last part of image
                        else if (buff[17] == buff[18])
                        {
                            int counter = imageCounter2[index];

                            int len = length - 34;
                            pictureBuffer2[index][counter] = (uint8_t*)realloc(pictureBuffer2[index][counter], (pictureBufferLength2[index][counter]+len)*sizeof(uint8_t));
                            memcpy(&pictureBuffer2[index][counter][pictureBufferLength2[index][counter]], &buff[29], len);
                            pictureBufferLength2[index][counter] = pictureBufferLength2[index][counter]+len;

                            // increment the image counter
                            ++imageCounter2[index];
                        }
                        // middle part of an image
                        else
                        {
                            int counter = imageCounter2[index];

                            int len = length - 29;
                            pictureBuffer2[index][counter] = (uint8_t*)realloc(pictureBuffer2[index][counter], (pictureBufferLength2[index][counter]+len)*sizeof(uint8_t));
                            memcpy(&pictureBuffer2[index][counter][pictureBufferLength2[index][counter]], &buff[29], len);
                            pictureBufferLength2[index][counter] = pictureBufferLength2[index][counter]+len;
                        }
                    }
                }
            }
        }

        // Data Transfer 3
        // FIN1
        if (((buff[14] & 0b11110000)       ) == 0xA0)
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        qDebug() << "Finish";

                        // first part of image
                        if (buff[17] == 0x00)
                        {
                            int counter = buff[14] & 0b00001111;

                            year3  [counter] = buff[19];
                            month3 [counter] = buff[20];
                            day3   [counter] = buff[21];

                            hour3  [counter] = buff[22];
                            minute3[counter] = buff[23];
                            second3[counter] = buff[24];

                            split3 [counter] = ((buff[25]) << 24)|((buff[26]) << 16)|((buff[27]) << 8)|((buff[28]) << 0);

                            camFlag[counter] = 1;

                            // time
                            int h_diff = hour3  [counter] - hour  ;
                            int m_diff = minute3[counter] - minute;
                            int s_diff = second3[counter] - second;
                            int n_diff = split3 [counter] - split ;

                            if (m_diff < 0) {h_diff = h_diff - 1; m_diff = m_diff + 60  ;}
                            if (s_diff < 0) {m_diff = m_diff - 1; s_diff = s_diff + 60  ;}
                            if (n_diff < 0) {s_diff = s_diff - 1; n_diff = n_diff + 1000;}

                            int sec = h_diff*60*60 + m_diff*60 + s_diff;
                            int mil = n_diff;
                            athleteTime[counter]->display(QString::number(sec)+":"+QString::number(mil));

                            qDebug() << year << month << day << hour << minute << second << split;
                            qDebug() << year3[counter] << month3[counter] << day3[counter] << hour3[counter] << minute3[counter] << second3[counter] << split3[counter];
                            qDebug() << sec;
                            qDebug() << mil;
                            qDebug() << camFlag[counter];

                            if (pictureBuffer3[index] != NULL)
                            {
                                free(pictureBuffer3[counter]);
                                pictureBuffer3[counter] = NULL;
                            }

                            int len = length - 34;
                            pictureBuffer3[counter] = (uint8_t*)malloc(len*sizeof(uint8_t));
                            memcpy(pictureBuffer3[counter], &buff[34], len);
                            pictureBufferLength3[counter] = len;
                        }
                        // last part of image
                        else if (buff[17] == buff[18])
                        {
                            int counter = buff[14] & 0b00001111;

                            int len = length - 34;
                            pictureBuffer3[counter] = (uint8_t*)realloc(pictureBuffer3[counter], (pictureBufferLength3[counter]+len)*sizeof(uint8_t));
                            memcpy(&pictureBuffer3[counter][pictureBufferLength3[counter]], &buff[29], len);
                            pictureBufferLength3[counter] = pictureBufferLength3[counter]+len;
                        }
                        // middle part of an image
                        else
                        {
                            int counter = buff[14] & 0b00001111;

                            int len = length - 29;
                            pictureBuffer3[counter] = (uint8_t*)realloc(pictureBuffer3[counter], (pictureBufferLength3[counter]+len)*sizeof(uint8_t));
                            memcpy(&pictureBuffer3[counter][pictureBufferLength3[counter]], &buff[29], len);
                            pictureBufferLength3[counter] = pictureBufferLength3[counter]+len;
                        }
                    }
                }
            }
        }

        // Data Transfer 4
        // FIN2
        if (((buff[14] & 0b11110000)       ) == 0xB0)
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        qDebug() << "Finish";

                        // first part of image
                        if (buff[17] == 0x00)
                        {
                            int counter = buff[14] & 0b00001111;

                            year3  [counter] = buff[19];
                            month3 [counter] = buff[20];
                            day3   [counter] = buff[21];

                            hour3  [counter] = buff[22];
                            minute3[counter] = buff[23];
                            second3[counter] = buff[24];

                            split3 [counter] = ((buff[25]) << 24)|((buff[26]) << 16)|((buff[27]) << 8)|((buff[28]) << 0);

                            camFlag[counter] = 2;

                            // time
                            int h_diff = hour3  [counter] - hour  ;
                            int m_diff = minute3[counter] - minute;
                            int s_diff = second3[counter] - second;
                            int n_diff = split3 [counter] - split ;

                            if (m_diff < 0) {h_diff = h_diff - 1; m_diff = m_diff + 60  ;}
                            if (s_diff < 0) {m_diff = m_diff - 1; s_diff = s_diff + 60  ;}
                            if (n_diff < 0) {s_diff = s_diff - 1; n_diff = n_diff + 1000;}

                            int sec = h_diff*60*60 + m_diff*60 + s_diff;
                            int mil = n_diff;
                            athleteTime[counter]->display(QString::number(sec)+":"+QString::number(mil));

                            qDebug() << year << month << day << hour << minute << second << split;
                            qDebug() << year3[counter] << month3[counter] << day3[counter] << hour3[counter] << minute3[counter] << second3[counter] << split3[counter];
                            qDebug() << sec;
                            qDebug() << mil;
                            qDebug() << camFlag[counter];

                            if (pictureBuffer3[index] != NULL)
                            {
                                free(pictureBuffer3[counter]);
                                pictureBuffer3[counter] = NULL;
                            }

                            int len = length - 34;
                            pictureBuffer3[counter] = (uint8_t*)malloc(len*sizeof(uint8_t));
                            memcpy(pictureBuffer3[counter], &buff[34], len);
                            pictureBufferLength3[counter] = len;
                        }
                        // last part of image
                        else if (buff[17] == buff[18])
                        {
                            int counter = buff[14] & 0b00001111;

                            int len = length - 34;
                            pictureBuffer3[counter] = (uint8_t*)realloc(pictureBuffer3[counter], (pictureBufferLength3[counter]+len)*sizeof(uint8_t));
                            memcpy(&pictureBuffer3[counter][pictureBufferLength3[counter]], &buff[29], len);
                            pictureBufferLength3[counter] = pictureBufferLength3[counter]+len;
                        }
                        // middle part of an image
                        else
                        {
                            int counter = buff[14] & 0b00001111;

                            int len = length - 29;
                            pictureBuffer3[counter] = (uint8_t*)realloc(pictureBuffer3[counter], (pictureBufferLength3[counter]+len)*sizeof(uint8_t));
                            memcpy(&pictureBuffer3[counter][pictureBufferLength3[counter]], &buff[29], len);
                            pictureBufferLength3[counter] = pictureBufferLength3[counter]+len;
                        }
                    }
                }
            }
        }

        // case 0
        // ARME
        if ((buff[14] == 0x5A) && (buff[15] == 0x40))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "ARME");
                        progressPointer->stopProgress();
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        // case 1
        // ARMD
        if ((buff[14] == 0x5A) && (buff[15] == 0x41))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "ARMD");
                        progressPointer->stopProgress();
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        // case 2
        // SETT
        if ((buff[14] == 0x5A) && (buff[15] == 0x42))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "SETT");
                        progressPointer->stopProgress();
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        // case 3
        // TAKE
        if ((buff[14] == 0x5A) && (buff[15] == 0x43))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "TAKE");
                        progressPointer->stopProgress();
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        // case 4
        // RATE
        if ((buff[14] == 0x5A) && (buff[15] == 0x44))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "RATE");
                        deviceSettingsData[index][3] = QString::number(buff[19]);
                        progressPointer->stopProgress();
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        // case 5
        // DVAL
        if ((buff[14] == 0x5A) && (buff[15] == 0x45))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "DVAL");
                        progressPointer->stopProgress();

                        // camera 1
                        rows1[index][0] = buff[19];
                        rows1[index][1] = buff[20];
                        cols1[index][0] = buff[21];
                        cols1[index][1] = buff[22];

                        // camera 2
                        rows2[index][0] = buff[23];
                        rows2[index][1] = buff[24];
                        cols2[index][0] = buff[25];
                        cols2[index][1] = buff[26];
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        // case 6
        // READ
        if ((buff[14] == 0x5A) && (buff[15] == 0x46))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "READ");
                        progressPointer->stopProgress();
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        // case 7
        // PING
        if ((buff[14] == 0x5A) && (buff[15] == 0x47))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "PING");
                        progressPointer->stopProgress();
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        // case 8
        // PAIR
        if ((buff[14] == 0x5A) && (buff[15] == 0x48))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "PAIR");
                        progressPointer->stopProgress();
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        // case 9
        // TOGG
        if ((buff[14] == 0x5A) && (buff[15] == 0x49))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "TOGG");
                        progressPointer->stopProgress();
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        // case 10
        // GETD
        if ((buff[14] == 0x5A) && (buff[15] == 0x50))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "GETD");
                        progressPointer->stopProgress();

                        // camera 1
                        rows1[index][0] = buff[19];
                        rows1[index][1] = buff[20];
                        cols1[index][0] = buff[21];
                        cols1[index][1] = buff[22];

                        // camera 2
                        rows2[index][0] = buff[23];
                        rows2[index][1] = buff[24];
                        cols2[index][0] = buff[25];
                        cols2[index][1] = buff[26];
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        // Interrupt 1
        // ISTR
        if ((buff[14] == 0x5B) && (buff[15] == 0x40))
        {
            for (int index = 0; index < MAXIMUM_DEV; index++)
            {
                if (devInitialised[index] != 0)
                {
                    std::string ret = deviceSettingsData[index][1].toStdString();
                    if ((stoi(ret.substr(0 , 2), 0, 16) == buff[6 ]) &&
                        (stoi(ret.substr(3 , 2), 0, 16) == buff[7 ]) &&
                        (stoi(ret.substr(6 , 2), 0, 16) == buff[8 ]) &&
                        (stoi(ret.substr(9 , 2), 0, 16) == buff[9 ]) &&
                        (stoi(ret.substr(12, 2), 0, 16) == buff[10]) &&
                        (stoi(ret.substr(15, 2), 0, 16) == buff[11])  )
                    {
                        receiveErrorHandler(buff[16], index, "STRT");

                        year   = buff[19];
                        month  = buff[20];
                        day    = buff[21];

                        hour   = buff[22];
                        minute = buff[23];
                        second = buff[24];

                        split  = ((buff[25]) << 24)|((buff[26]) << 16)|((buff[27]) << 8)|((buff[28]) << 0);

                        qDebug() << year << month << day << hour << minute << second << split;

                        maint->start(100);  // start the main timer
                        elaps->start(   );  // start the elapsed timer

                        for (int index = 0; index < MAXIMUM_DEV; index++)
                        {
                            if (devInitialised[index] != 0)
                            {
                                transmtHandler(COMM_TOGG, TIME_TOGG, index);
                            }
                        }
                    }
                }
            }

            // re-enable the user interface
            tran = TRAN_IDLE;
        }

        qDebug() << "----------------------------------";
    }
}

void Controller::receiveErrorHandler(uint8_t error, int index, QString message)
{
    // set the indicator based in the message
    QPixmap *indicator;
    QLabel *status;
    if (message == "SETT")
    {
        // the settings indicator
        indicator = settingsPix[index];
        status = deviceSettingsStatus[index];
    }
    else
    {
        // the status indicator
        indicator = statusPix[index];
        status = deviceStatus[index];
    }

    if (error > 0)
    {
        // decode the returned device error
        bool a = (error & 0b00000001) >> 0;
        bool b = (error & 0b00000010) >> 1;
        bool c = (error & 0b00000100) >> 2;
        bool d = (error & 0b00001000) >> 3;
        bool e = (error & 0b00010000) >> 4;
        bool f = (error & 0b00100000) >> 5;
        bool g = (error & 0b01000000) >> 6;
        bool h = (error & 0b10000000) >> 7;

        // statusbar message
        ui->statusbar->clearMessage();
        ui->statusbar->showMessage(message + " -> ERROR: "+QString::number(error));
        // update status pictogram
        indicator->load(":/pic/orange.png");
        indicator->setDevicePixelRatio(4);
        status->setPixmap(*indicator);
        // debug output
        qDebug() << message + " -> ERROR: " << a << b << c << d << e << f << g << h;
    }
    else
    {
        // statusbar message
        ui->statusbar->clearMessage();
        ui->statusbar->showMessage(message + " -> SUCCESS");
        // update status pictogram
        indicator->load(":/pic/green.png");
        indicator->setDevicePixelRatio(4);
        status->setPixmap(*indicator);
        // debug output
        qDebug() << message + " -> SUCCESS";
    }
}
// -------------------------------------------------------------------------------

// static menu
// -------------------------------------------------------------------------------
void Controller::on_actionAbtDialg_triggered(void)
{
    stpKeepAlives();

    // create and open the window
    About *about = new About(this);
    about->exec();
    delete about;

    strKeepAlives();
}

void Controller::on_actionSettings_triggered(void)
{
    stpKeepAlives();

    // read the global configuration file
    settingsReadWrite(SETT_READ);

    // create and open the window
    Settings *settings = new Settings(this);
    if (universalSettings != NULL)
    {
        settings->setDialogData(universalSettings);
        delete[] universalSettings;
        universalSettings = NULL;
    }
    else
    if (universalSettings == NULL)
    {
        // do nothing
    }

    int ret = settings->exec();
    switch (ret)
    {
    case 0:
        break;
    case 1:

        universalSettings = settings->getDialogData();
        settingsReadWrite(SETT_WRIT);

        for (int index = 0; index < MAXIMUM_DEV; index++)
        {
            if (devInitialised[index] != 0)
            {
                transmtHandler(COMM_SETT, TIME_SETT, index);
            }
        }
        break;
    }

    // delete settings
    delete settings;

    strKeepAlives();
}

void Controller::on_actionExtDialg_triggered(void)
{
    stpKeepAlives();

    // create a message dialog
    QMessageBox exitConfirmation(this);
    exitConfirmation.setModal(1);
    exitConfirmation.setWindowTitle("Quit");
    exitConfirmation.setIcon(QMessageBox::Question);
    exitConfirmation.setText("Are you sure you want to exit?");
    exitConfirmation.setInformativeText("<small>All unsaved times will be lost</small>");
    exitConfirmation.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    exitConfirmation.setDefaultButton(QMessageBox::Yes);

    // wait for the response
    int ret = exitConfirmation.exec();
    switch (ret)
    {
        case QMessageBox::Yes:
            QCoreApplication::quit();
            break;
        case QMessageBox::No:
            break;
    }

    strKeepAlives();
}

void Controller::on_actionUpdtAths_triggered(void)
{
    stpKeepAlives();

    // get the data
    Athlete *athlete = new Athlete(this);
    athlete->setDialogData(athleteSettingsData);
    int ret = athlete->exec();

    switch (ret)
    {
    case 0:
        break;
    case 1:
        int index = athlete->getDialogData()[3].toInt()-1;
        if ((athInitialised[index] == 0) && (index >= 0) && (index <= 7))
        {
            deleteAthlete(index);
            createAthlete(index, athlete->getDialogData());
        }
        else
        {
            QMessageBox informationBox(this);
            informationBox.setModal(1);
            informationBox.setWindowTitle("Critical");
            informationBox.setIcon(QMessageBox::Critical);
            informationBox.setText("Invalid athlete");
            informationBox.setInformativeText("<small>The maximum number of athletes has already been added. Remove an athlete to continue.</small>");
            informationBox.exec();
        }
        break;
    }
    delete athlete;

    strKeepAlives();
}

void Controller::on_actionLoadAths_triggered(void)
{
    stpKeepAlives();

    // setup the file dialog
    QFileDialog fileDialog(this);
    fileDialog.setModal(1);
    fileDialog.setWindowTitle("Please select an athlete data file");
    fileDialog.setNameFilter(tr("Data Files (*.ath *.ini)"));
    fileDialog.setDirectory("/home/pieter/Documents/");
    fileDialog.setMinimumHeight(650);
    fileDialog.setMaximumWidth(800);

    std::string line, key, value, options[4];
    int index = 0;
    options[0] = "name";
    options[1] = "surname";
    options[2] = "age";
    options[3] = "lane";

    int ret = fileDialog.exec();
    switch (ret)
    {
    case 0:
        break;
    case 1:
        // get the file name and path
        QStringList fileName;
        fileName = fileDialog.selectedFiles();

        // temp variables
        QString *temp = new QString[4];
        int flag[4] = {0};

        // open the file
        std::fstream saved;
        saved.open(fileName[0].toStdString(), std::fstream::in);
        if (saved.is_open())
        {
            while (std::getline(saved, line))
            {
                if ((line[0] != '#') && (line.length() != 0))
                {
                    key = line.substr(0, line.find_first_of(" "));
                    value = line.substr(line.find_first_of(" ")+1, line.find_last_of('\n'));

                    if (key == "athlete")
                    {
                        index = stoi(value, 0, 10);

                        // initialise
                        temp[0] = '0';
                        temp[1] = '0';
                        temp[2] = '0';
                        temp[3] = '0';
                    }
                    else
                    {
                        for (int sub = 0; sub < 4; sub++)
                        {
                            if(options[sub] == key)
                            {
                                temp[sub] = QString::fromStdString(value);
                                flag[sub] = 1;
                            }
                        }

                        if ((flag[0] == 1) &&
                            (flag[1] == 1) &&
                            (flag[2] == 1) &&
                            (flag[3] == 1)
                           )
                        {
                            deleteAthlete(index);
                            createAthlete(index, temp);

                            // clear temp variables
                            memset(flag, 0, 4*sizeof(int));
                            temp = new QString[4];
                        }
                    }
                }
            }
            saved.close();
        }
        break;
    }

    strKeepAlives();
}

void Controller::on_actionUpdtDevs_triggered(void)
{
    stpKeepAlives();

    int flag = 0;

    // get the data
    Device *device = new Device(this);
    int ret = device->exec();

    switch (ret)
    {
    case 0:
        break;
    case 1:

        for (int index = 0; index < MAXIMUM_DEV; index++)
        {
            if (devInitialised[index] == 0)
            {
                flag = 1;

                deleteDevices(index);
                createDevices(index, device->getDialogData());
                transmtHandler(COMM_PAIR, TIME_PAIR, index);
                break;
            }
        }

        if (flag == 0)
        {
            QMessageBox informationBox(this);
            informationBox.setModal(1);
            informationBox.setWindowTitle("Critical");
            informationBox.setIcon(QMessageBox::Critical);
            informationBox.setText("Invalid device");
            informationBox.setInformativeText("<small>The maximum number of devices has already been added. Remove a device to continue.</small>");
            informationBox.exec();
        }

        break;
    }
    delete device;

    strKeepAlives();
}

void Controller::on_actionLoadDevs_triggered(void)
{
    stpKeepAlives();

    QFileDialog fileDialog(this);
    fileDialog.setModal(1);
    fileDialog.setWindowTitle("Please select a device data file");
    fileDialog.setNameFilter(tr("Data Files (*.dev *.ini)"));
    fileDialog.setDirectory("/home/pieter/Documents/");
    fileDialog.setMinimumHeight(650);
    fileDialog.setMaximumWidth(800);

    std::string line, key, value, options[3];
    int index = 0;
    options[0] = "name";
    options[1] = "address";
    options[2] = "type";

    int ret = fileDialog.exec();
    switch (ret)
    {
    case 0:
        break;
    case 1:
        // get the file name and path
        QStringList fileName;
        fileName = fileDialog.selectedFiles();

        // temp variables
        QString *temp = new QString[9];
        int flag[3] = {0};

        // open the file
        std::fstream saved;
        saved.open(fileName[0].toStdString(), std::fstream::in);
        if (saved.is_open())
        {
            while (std::getline(saved, line))
            {
                if ((line[0] != '#') && (line.length() != 0))
                {
                    key = line.substr(0, line.find_first_of(" "));
                    value = line.substr(line.find_first_of(" ")+1, line.find_last_of('\n'));

                    if (key == "device")
                    {
                        index = stoi(value, 0, 10);

                        // initialise
                        temp[0] = "0";
                        temp[1] = "0";
                        temp[2] = "0";
                        temp[3] = "0";
                        temp[4] = "0";
                        temp[5] = "0";
                        temp[6] = "0";
                        temp[7] = "0";
                        temp[8] = "0";
                    }
                    else
                    {
                        for (int sub = 0; sub < 3; sub++)
                        {
                            if(options[sub] == key)
                            {
                                temp[sub] = QString::fromStdString(value);
                                flag[sub] = 1;
                            }
                        }

                        if ((flag[0] == 1) &&
                            (flag[1] == 1) &&
                            (flag[2] == 1)
                           )
                        {
                            deleteDevices(index);
                            createDevices(index, temp);

                            // clear temp variables
                            memset(flag, 0, 3*sizeof(int));
                            temp = new QString[9];

                            transmtHandler(COMM_PAIR, TIME_PAIR, index);
                        }
                    }
                }
            }
            saved.close();
        }
        break;
    }

    strKeepAlives();
}

void Controller::on_actionSavDialg_triggered(void)
{
    stpKeepAlives();

    QFileDialog fileDialog(this);
    fileDialog.setModal(1);
    fileDialog.setWindowTitle("Please select a location and name for the time data and click save");
    fileDialog.setNameFilter(tr("Time Files (*.csv)"));
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

            outFile = std::fstream(fileDialog.selectedFiles()[0].toStdString(), std::ios::out);
            outFile << "times" << "," << QDate::currentDate().toString(Qt::ISODate).toStdString() << "," << QTime::currentTime().toString(Qt::ISODate).toStdString() << '\n';
            for (int index = 0; index < MAXIMUM_ATH; index++)
            {
                if (athInitialised[index] != 0)
                {
                    outFile << athleteSettingsData[index][0].toStdString() << "," << athleteSettingsData[index][1].toStdString() << "," << athleteSettingsData[index][2].toStdString() << "," << athleteTime[index]->value() << '\n';
                }
            }
            outFile.close();

            break;
        }
    }

    strKeepAlives();
}
// -------------------------------------------------------------------------------

// static buttons
// -------------------------------------------------------------------------------
void Controller::on_RaceStr_clicked(void)
{
    race = RACE_BUSY;
    stpKeepAlives();

    for (int index = 0; index < MAXIMUM_DEV; index++)
    {
        if (devInitialised[index] != 0)
        {
            transmtHandler(COMM_ARME, TIME_ARME, index);
        }
    }
}

void Controller::on_RaceStp_clicked(void)
{
    for (int index = 0; index < MAXIMUM_DEV; index++)
    {
        if (devInitialised[index] != 0)
        {
            transmtHandler(COMM_ARMD, TIME_ARMD, index);
        }
    }

    maint->stop();
    elaps->invalidate();

    race = RACE_IDLE;
    strKeepAlives();
}

void Controller::on_RaceRes_clicked(void)
{
    // stop
    on_RaceStp_clicked();

    // rest
    for (int index = 0; index < MAXIMUM_ATH; index++)
    {
        if (athInitialised[index] != 0)
        {
            athleteTime[index]->display(0);
        }
    }

    // rest
    ui->RaceTime->display(QString::number(0));
}
// -------------------------------------------------------------------------------

// runtime buttons
// -------------------------------------------------------------------------------
void Controller::athSetHandler(void)
{
    stpKeepAlives();

    for (int index = 0; index < MAXIMUM_ATH; ++index)
    {
        if (athleteSettings[index] == sender())
        {
            Athlete *athlete = new Athlete(this);

            athlete->setDialogData (athleteSettingsData[index]);
            athlete->setRacStartTime(year, month, day, hour, minute, second, split);
            if (camFlag[index] == 1)
                athlete->setFinishImage1(pictureBuffer3[index], pictureBufferLength3[index], year3[index], month3[index], day3[index], hour3[index], minute3[index], second3[index], split3[index], rows1[index], cols1[index]);
            else
            if (camFlag[index] == 2)
                athlete->setFinishImage2(pictureBuffer3[index], pictureBufferLength3[index], year3[index], month3[index], day3[index], hour3[index], minute3[index], second3[index], split3[index], rows2[index], cols2[index]);

            int ret = athlete->exec();

            switch (ret)
            {
            case 0:
                break;
            case 1:

                athleteSettingsData[index] = athlete->getDialogData();
                athleteName[index]->setText(athleteSettingsData[index][0]);
                athleteSurname[index]->setText(athleteSettingsData[index][1]);
                laneNumber[index]->setText(athleteSettingsData[index][3]);

                break;
            }

            delete athlete;
            break;
        }
    }

    strKeepAlives();
}

void Controller::athDelHandler(void)
{
    stpKeepAlives();

    for (int index = 0; index < MAXIMUM_ATH; ++index)
    {
        if (athleteDelete[index] == sender())
        {
            deleteAthlete(index);
            break;
        }
    }

    strKeepAlives();
}

void Controller::devSetHandler(void)
{
    stpKeepAlives();

    for (int index = 0; index < MAXIMUM_DEV; ++index)
    {
        if (deviceSettings[index] == sender())
        {
            Device *device = new Device(this);

            device->setDialogData (deviceSettingsData [index]);
            device->setDetections1(rows1[index], cols1[index]);
            device->setDetections2(rows2[index], cols2[index]);
            device->setDisplayImg1(pictureBuffer1[index][0], pictureBufferLength1[index][0]);
            device->setDisplayImg2(pictureBuffer2[index][0], pictureBufferLength2[index][0]);
            device->setTakenImage1(pictureBuffer1[index], pictureBufferLength1[index], year1[index], month1[index], day1[index], hour1[index], minute1[index], second1[index], split1[index], imageCounter1[index]);
            device->setTakenImage2(pictureBuffer2[index], pictureBufferLength2[index], year2[index], month2[index], day2[index], hour2[index], minute2[index], second2[index], split2[index], imageCounter2[index]);

            int ret = device->exec();

            switch (ret)
            {
            case 0:
                break;
            case 1:

                deviceSettingsData[index] = device->getDialogData();
                deviceName[index]->setText(deviceSettingsData[index][0]);

                // tests
                if (deviceSettingsData[index][4].toInt() == 1) transmtHandler(COMM_RATE, TIME_RATE, index);
                if (deviceSettingsData[index][5].toInt() == 1) transmtHandler(COMM_TAKE, TIME_TAKE, index);
                if (deviceSettingsData[index][6].toInt() == 1) transmtHandler(COMM_DVAL, TIME_DVAL, index);
                if (deviceSettingsData[index][7].toInt() == 1) transmtHandler(COMM_READ, TIME_READ, index);
                if (deviceSettingsData[index][8].toInt() == 1) transmtHandler(COMM_GETD, TIME_GETD, index);

                break;
            }

            delete device;
            break;
        }
    }

    strKeepAlives();
}

void Controller::devDelHandler(void)
{
    stpKeepAlives();

    for (int index = 0; index < MAXIMUM_DEV; ++index)
    {
        if (deviceDelete[index] == sender())
        {
            deleteDevices(index);
            break;
        }
    }
    strKeepAlives();
}

void Controller::createAthlete(int index, QString* settings)
{
    athleteSettingsData[index] = settings;

    athleteLayout[index] = new QHBoxLayout();
    athleteName[index] = new QLabel(athleteSettingsData[index][0]);
    athleteSurname[index] = new QLabel(athleteSettingsData[index][1]);
    laneNumber[index] = new QLabel(athleteSettingsData[index][3]);
    athleteSettings[index] = new QPushButton("Settings");
    athleteDelete[index] = new QPushButton("Delete");
    athleteTime[index] = new QLCDNumber();

    athleteTime[index]->setMaximumHeight(28);
    athleteTime[index]->setDigitCount(7);
    athleteSettings[index]->setMaximumWidth(75);
    athleteDelete[index]->setMaximumWidth(75);

    connect(athleteSettings[index], SIGNAL(clicked()), SLOT(athSetHandler()));
    connect(athleteDelete[index], SIGNAL(clicked()), SLOT(athDelHandler()));

    athleteLayout[index]->addWidget(laneNumber[index]);
    athleteLayout[index]->addWidget(athleteName[index]);
    athleteLayout[index]->addWidget(athleteSurname[index]);
    athleteLayout[index]->addWidget(athleteSettings[index]);
    athleteLayout[index]->addWidget(athleteDelete[index]);
    athleteLayout[index]->addWidget(athleteTime[index]);

    ui->raceBoxLayout->insertLayout(index, athleteLayout[index]);

    athInitialised[index] = 1;
}

void Controller::createDevices(int index, QString* settings)
{
    deviceSettingsData[index] = settings;

    deviceLayout[index] = new QHBoxLayout();
    deviceName[index] = new QLabel(deviceSettingsData[index][0]);
    deviceAddress[index] = new QLabel(deviceSettingsData[index][1]);
    deviceStatus[index] = new QLabel();
    deviceSettingsStatus[index] = new QLabel();
    deviceSettings[index] = new QPushButton("Settings");
    deviceDelete[index] = new QPushButton("Delete");
    statusPix[index] = new QPixmap();
    settingsPix[index] = new QPixmap();

    statusPix[index]->load(":/pic/red.png");
    statusPix[index]->setDevicePixelRatio(4);
    deviceStatus[index]->setPixmap(*statusPix[index]);

    settingsPix[index]->load(":/pic/red.png");
    settingsPix[index]->setDevicePixelRatio(4);
    deviceSettingsStatus[index]->setPixmap(*settingsPix[index]);

    deviceSettings[index]->setMaximumWidth(75);
    deviceDelete[index]->setMaximumWidth(75);

    connect(deviceSettings[index], SIGNAL(clicked()), SLOT(devSetHandler()));
    connect(deviceDelete[index], SIGNAL(clicked()), SLOT(devDelHandler()));

    deviceLayout[index]->addWidget(deviceName[index]);
    deviceLayout[index]->addWidget(deviceStatus[index]);
    deviceLayout[index]->addWidget(deviceSettingsStatus[index]);
    deviceLayout[index]->addWidget(deviceSettings[index]);
    deviceLayout[index]->addWidget(deviceDelete[index]);

    ui->deviceBoxLayout->insertLayout(index, deviceLayout[index]);

    devInitialised[index] = 1;
}

void Controller::deleteAthlete(int index)
{
    // test
    if (athInitialised[index] == 0)
    {
        return;
    }

    athInitialised[index] = 0;

    athleteLayout[index]->removeWidget(athleteTime[index]);
    athleteLayout[index]->removeWidget(laneNumber[index]);
    athleteLayout[index]->removeWidget(athleteName[index]);
    athleteLayout[index]->removeWidget(athleteSurname[index]);
    athleteLayout[index]->removeWidget(athleteSettings[index]);
    athleteLayout[index]->removeWidget(athleteDelete[index]);

    athleteSettings[index]->disconnect();
    athleteDelete[index]->disconnect();

    ui->raceBoxLayout->removeItem(athleteLayout[index]);

    delete laneNumber[index];
    delete athleteTime[index];
    delete athleteName[index];
    delete athleteSurname[index];
    delete athleteSettings[index];
    delete athleteDelete[index];
    delete athleteLayout[index];

    delete[] athleteSettingsData[index];
    athleteSettingsData[index] = NULL;
}

void Controller::deleteDevices(int index)
{
    // test
    if (devInitialised[index] == 0)
    {
        return;
    }

    devInitialised[index] = 0;

    deviceLayout[index]->removeWidget(deviceName[index]);
    deviceLayout[index]->removeWidget(deviceSettings[index]);
    deviceLayout[index]->removeWidget(deviceDelete[index]);
    deviceLayout[index]->removeWidget(deviceStatus[index]);
    deviceLayout[index]->removeWidget(deviceSettingsStatus[index]);

    deviceSettings[index]->disconnect();
    deviceDelete[index]->disconnect();

    ui->raceBoxLayout->removeItem(deviceLayout[index]);

    delete deviceName[index];
    delete deviceSettings[index];
    delete deviceDelete[index];
    delete deviceStatus[index];
    delete deviceSettingsStatus[index];
    delete statusPix[index];
    delete deviceLayout[index];

    delete[] deviceSettingsData[index];
    deviceSettingsData[index] = NULL;
}
// -------------------------------------------------------------------------------

// runtime timers
// -------------------------------------------------------------------------------
void Controller::updAlvHandler(void)
{
    stpKeepAlives();

    for (int index = 0; index < MAXIMUM_DEV; index++)
    {
        if (devInitialised[index] != 0)
        {
            transmtHandler(COMM_PING, TIME_PING, index);
        }
    }

    strKeepAlives();
}

void Controller::updTimHandler(void)
{
    ui->RaceTime->display(QString::number(elaps->elapsed()));
}

void Controller::strKeepAlives(void)
{
    int flag = 0;

    for (int index = 0; index < MAXIMUM_DEV; index++)
    {
        if (devInitialised[index] != 0)
        {
            flag = 1;
        }
    }

    if ((timer->isActive() == 0) && (flag == 1) && (race == RACE_IDLE))
    {
        timer->start(KEEP_ALIV);
        qDebug() << "----------------------------------";
        qDebug() << "keep alive timer started";
        qDebug() << "----------------------------------";
    }
}

void Controller::stpKeepAlives(void)
{
    if (timer->isActive() == 1)
    {
        timer->stop();
        qDebug() << "----------------------------------";
        qDebug() << "keep alive timer stopped";
        qDebug() << "----------------------------------";
    }
}
// -------------------------------------------------------------------------------
