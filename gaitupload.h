#ifndef GAITUPLOAD_H
#define GAITUPLOAD_H

#include <QMainWindow>

namespace Ui {
class GaitUpload;
}

class GaitUpload : public QMainWindow
{
    Q_OBJECT

public:
    explicit GaitUpload(QWidget *parent = nullptr);
    ~GaitUpload();

private:
    Ui::GaitUpload *ui;

signals:
    void startFileUpload();
private slots:
    void displayProcessBar(int cur, int total);

    void on_pushButton_clicked();
    void clearProcessBar();
};

#endif // GAITUPLOAD_H
