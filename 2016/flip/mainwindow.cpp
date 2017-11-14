#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->stage = this->ui->stage;
    /*the under_stage is a signal to judge if you are stage 1(6x6)*/

    this->under_stage = true;

    /*init the tips*/

    this->ui->tips->setText(tr("Easy game"));
    connect(ui->actionI_want_help,SIGNAL(triggered(bool)),this,SLOT(help()));

    /*set gif*/

    QMovie* movie = new QMovie(":/new/resource/test3.gif");
    ui->label->setMovie(movie);
    movie->start();

    /*set fix size*/

    this->setFixedSize(this->size());

    /*set icon(star)*/

    this->setWindowIcon(QIcon(":/new/resource/star-full.ico"));

    /*init the count*/
    /*at stage2, count must small than 8 to ensure the correct result*/

    this->count = 0;

    /*game entry*/

    this->foo();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::help(){
    /*tips1: you needn't flip 6x6 (although baidu could solve this problem)*/

    if(this->under_stage){
        QMessageBox::information(this,"tips","Jst flip!");
    }

    /*the steps of stage2 will be record and decrypt for flag checker*/

    else{
        QMessageBox::information(this,"tips","try the easiest way!");
    }

    /*set picture if you click the tips(((*/

    QPixmap* pic = new QPixmap(":/new/resource/test4.png");
    ui->label->setPixmap(*pic);
}
