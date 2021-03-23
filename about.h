#ifndef ABOUT_H
#define ABOUT_H

// qt
#include <QDialog>

namespace Ui {
class About;
}

class About : public QDialog
{
    Q_OBJECT

public:

    // constructor and destructor
    explicit About(QWidget *parent = nullptr);
    ~About();

private:

    // ui
    Ui::About *ui;
};

#endif // ABOUT_H
