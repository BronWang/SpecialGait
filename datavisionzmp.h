#ifndef DATAVISIONZMP_H
#define DATAVISIONZMP_H

#include <QMainWindow>
#include <QChartView>
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class DataVisionZmp;
}

class DataVisionZmp : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataVisionZmp(QWidget *parent = nullptr);
    ~DataVisionZmp();

private:
    Ui::DataVisionZmp *ui;
};

#endif // DATAVISIONZMP_H
