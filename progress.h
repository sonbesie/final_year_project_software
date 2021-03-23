#ifndef PROGRESS_H
#define PROGRESS_H

// qt
#include <QDialog>
#include <QTimer>
#include <QDebug>

namespace Ui {
class Progress;
}

class Progress : public QDialog
{
    Q_OBJECT

public:

    // constructor and destructor
    explicit Progress(QWidget *parent = nullptr);
    ~Progress();

    // progress
    void startProgress(float);
    void stopProgress (void);

private:

    // ui
    Ui::Progress *ui;

    // progress
    void updateProgress(void);
    void checkProgress(void);

    QTimer *barTimer;
    int barMaximum;
};

#endif // PROGRESS_barH
