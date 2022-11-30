#ifndef DATAVISIONCORRECTGAIT_H
#define DATAVISIONCORRECTGAIT_H

#include <QMainWindow>
#include <QChartView>
QT_CHARTS_USE_NAMESPACE

namespace Ui {
class DataVisionCorrectGait;
}

class DataVisionCorrectGait : public QMainWindow
{
    Q_OBJECT

public:
    explicit DataVisionCorrectGait(QWidget *parent = nullptr);
    ~DataVisionCorrectGait();

private:
    Ui::DataVisionCorrectGait *ui;
};

#endif // DATAVISIONCORRECTGAIT_H
