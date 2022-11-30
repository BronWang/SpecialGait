#ifndef DATAVISIONDYNAMIXEL_H
#define DATAVISIONDYNAMIXEL_H

#include <QMainWindow>
#include <QChartView>
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class DataVisionDynamixel;
}

class DataVisionDynamixel : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataVisionDynamixel(QWidget *parent = nullptr);
    ~DataVisionDynamixel();

private:
    Ui::DataVisionDynamixel *ui;
};

#endif // DATAVISIONDYNAMIXEL_H
