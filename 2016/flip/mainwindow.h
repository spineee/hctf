#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define BOARDWIDTH 3
#define BOARDHEIGHT 3
#define BOARDWIDTH_1 6
#define BOARDHEIGHT_1 6

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QWidget>
#include <QLabel>
#include <QMovie>
#include <QPixmap>
#include <QLineEdit>
class Button;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void foo();
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void check_if_clear();
    void _change(int x,int y);
    void new_world();
    void test();
    void check();
    void _check(char ch1, char ch2, char *resolve,int* pos);
public slots:
    void change_status(int x,int y);
    void help();
    void submit();
private:
    Ui::MainWindow *ui;
    Button* button_board[BOARDWIDTH_1][BOARDHEIGHT_1];
    QByteArray step;
    QGridLayout* stage;
    bool under_stage;
    int count;
};

#endif // MAINWINDOW_H

class Button : public QPushButton
{
    Q_OBJECT
public:
    Button(int p_x,int p_y){
        x = p_x;
        y = p_y;
    }

    void set_disable(){
        this->setEnabled(false);
        this->setStyleSheet("background-color: rgb(215,215,215)");
    }

    void set_enable(){
        this->setEnabled(true);
        this->setStyleSheet("background-color: rgb(255,255,255)");
    }

signals:
    void send_position(int x,int y);

public slots:
    void button_clicked();

private:
    int x;
    int y;
};

void _check(char ch1,char ch2,char* resolve,int* pos);
