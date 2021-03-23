#ifndef SETTINGS_H
#define SETTINGS_H

// qt
#include <QDialog>
#include <QLabel>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDebug>

// standard
#include <ctime>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:

    // constructor and destructor
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    // dialog data
    QString* getDialogData(void);
    void setDialogData(QString*);

private slots:

    // general event handlers
    void on_calSelector_clicked(const QDate&);
    void on_chkAutomatc_toggled(bool);

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
