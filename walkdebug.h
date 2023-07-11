#ifndef WALKDEBUG_H
#define WALKDEBUG_H

#include <QMainWindow>

namespace Ui {
class WalkDebug;
}

class WalkDebug : public QMainWindow
{
    Q_OBJECT

public:
    explicit WalkDebug(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *);
    ~WalkDebug();
signals:
    void cmd_walk_start_walk();
    void cmd_walk_forward();
    void cmd_walk_left();
    void cmd_walk_right();
    void cmd_walk_stop();
    void cmd_walk_end_walk();
private slots:
    void on_pushButtonStartWalk_clicked();

    void on_pushButtonForward_clicked();

    void on_pushButtonLeft_clicked();

    void on_pushButtonRight_clicked();

    void on_pushButtonStop_clicked();

    void on_pushButtonEndWalk_clicked();

private:
    Ui::WalkDebug *ui;
};

#endif // WALKDEBUG_H
