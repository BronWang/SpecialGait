#ifndef DATAVISIONNINEAXISSENSOR_H
#define DATAVISIONNINEAXISSENSOR_H

#include <QMainWindow>
#include <QChartView>
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class DataVisionNineAxisSensor;
}

class DataVisionNineAxisSensor : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataVisionNineAxisSensor(QWidget *parent = nullptr);
    ~DataVisionNineAxisSensor();

private:
    Ui::DataVisionNineAxisSensor *ui;
};

#endif // DATAVISIONNINEAXISSENSOR_H
