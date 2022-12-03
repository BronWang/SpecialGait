#ifndef IKIDROBOTIMAGE_H
#define IKIDROBOTIMAGE_H

#include <QMainWindow>

namespace Ui {
class IkidRobotImage;
}

class IkidRobotImage : public QMainWindow
{
    Q_OBJECT

public:
    explicit IkidRobotImage(QWidget *parent = nullptr);
    ~IkidRobotImage();

private:
    Ui::IkidRobotImage *ui;
};

#endif // IKIDROBOTIMAGE_H
