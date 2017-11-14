#include "mainwindow.h"

static inline int tranfer_2(int num){
    /*rule2 for stage2, a easy way to generate a cyclic group*/

    return 2*num%BOARDHEIGHT;
}

static inline int tranfer_1(int num){
    /*rule1 for stage1, usual way in flap game*/
    return num;
}

void MainWindow::check_if_clear(){
    int i,j;

    if(under_stage==true){
        for(i=0;i<BOARDWIDTH_1;i++){
            for(j=0;j<BOARDHEIGHT_1;j++){
                if(this->button_board[i][j]->isEnabled()){
                    return;
                }
            }
        }
    }
    else{

        /*limit the stage2's step count*/

        this->count++;
        if(this->count > 7){
            QMessageBox::information(this,"Sorry","too many steps :)");
            exit(1);
        }

        for(i=0;i<BOARDWIDTH;i++){
            for(j=0;j<BOARDHEIGHT;j++){
                if(this->button_board[i][j]->isEnabled()){
                    return;
                }
            }
        }
    }
    if(under_stage == true){

        /*if stage 1 is clear,goto stage 2*/

        QMessageBox::information(0,"congratulation","succeess");
        for(i=0;i<BOARDWIDTH_1;i++){
            for(j=0;j<BOARDHEIGHT_1;j++){
                this->button_board[i][j]->close();
                delete this->button_board[i][j];
            }
        }
        this->under_stage = false;
        QLabel* tips = this->findChild<QLabel*>("tips");
        tips->setText("final stage!");
        this->foo();
    }
    else{
        /*if stage 2 is clear,go to input the flag*/

        for(i=0;i<BOARDWIDTH;i++){
            for(j=0;j<BOARDHEIGHT;j++){
                this->button_board[i][j]->close();
                delete this->button_board[i][j];
            }
        }
        QLabel* tips = this->findChild<QLabel*>("tips");
        tips->setText("welcome!");
        this->new_world();
    }
}

void MainWindow::_change(int x,int y){

    /*change one block for two stages, seems like low ;; */

    if(under_stage==true){
        if(x<0||y<0||x>BOARDWIDTH_1-1||y>BOARDHEIGHT_1-1){
            return;
        }
    }
    else{
        if(x<0||y<0||x>BOARDWIDTH-1||y>BOARDHEIGHT-1){
            return;
        }
    }
    if(this->button_board[x][y]->isEnabled()){
        this->button_board[x][y]->set_disable();
    }
    else{
        this->button_board[x][y]->set_enable();
    }
}

void MainWindow::change_status(int x,int y){
    _change(x-1,y);//left
    _change(x+1,y);//right
    _change(x,y-1);//up
    _change(x,y+1);//down
    _change(x,y);//itself
    if(under_stage == false){

        /*record the step(important!)*/

        this->step.append(3*x+y);
    }
    this->check_if_clear();
    return;
}

void MainWindow::foo(){
    int i,j;
    /* set layout: stage2*/
    this->stage->setHorizontalSpacing(0);
    this->stage->setVerticalSpacing(0);
    if(under_stage == false){
        for(i = 0;i < BOARDWIDTH;i++){
            for(j = 0;j < BOARDHEIGHT;j++){
                this->button_board[i][j] = new Button(tranfer_2(i),tranfer_2(j));
                this->button_board[i][j]->setFixedSize(120,120);
                this->button_board[i][j]->setStyleSheet("background-color: rgb(255,255,255)");
                stage->addWidget(this->button_board[i][j],i,j);
                connect(this->button_board[i][j],SIGNAL(clicked()),this->button_board[i][j],SLOT(button_clicked()));
                connect(this->button_board[i][j],SIGNAL(send_position(int,int)),this,SLOT(change_status(int,int)));
            }
        }
    }

    /* set layout: stage1*/

    else{
        for(i = 0;i < BOARDWIDTH_1;i++){
            for(j = 0;j < BOARDHEIGHT_1;j++){
                this->button_board[i][j] = new Button(tranfer_1(i),tranfer_1(j));
                this->button_board[i][j]->setFixedSize(120,120);
                this->button_board[i][j]->setStyleSheet("background-color: rgb(255,255,255)");
                stage->addWidget(this->button_board[i][j],i,j);
                connect(this->button_board[i][j],SIGNAL(clicked()),this->button_board[i][j],SLOT(button_clicked()));
                connect(this->button_board[i][j],SIGNAL(send_position(int,int)),this,SLOT(change_status(int,int)));
            }
        }
    }
}

void MainWindow::new_world(){
    /*layout*/

    QLabel* label = new QLabel();
    label->setText(tr("flag:"));
    QLineEdit* edit = new QLineEdit();
    QPushButton* sub = new QPushButton();
    label->setFixedWidth(50);
    edit->setFixedWidth(200);
    sub->setText(tr("submit!"));
    connect(sub,SIGNAL(clicked(bool)),this,SLOT(submit()));
    this->stage->addWidget(label,2,1);
    this->stage->addWidget(edit,2,2);
    this->stage->addWidget(sub,2,3);
    this->stage->setMargin(10);
    return;
}

void Button::button_clicked(){

    /* create a signal send the pos of the block*/

    emit send_position(this -> x,this -> y);
}

void MainWindow::submit(){

    /* the slot of button submit(final check)*/

    int i;
    QLineEdit* edit = this->findChild<QLineEdit*>();
    QString s = edit->text();
    if(s.length()!=32){
        QMessageBox::information(this,"sorry","try again");
        exit(1);
    }

    QByteArray s1 = s.toLatin1();
    this->step.append(0xa);
    for(i = 0;i < s1.count();i++){
        this->step.append(s1.at(i)&0xf);
        this->step.append(s1.at(i)>>4&0xf);
    }

    this->check();
    return;
}

void MainWindow::test(){
    int i;
    char* str;
    QByteArray* s = new QByteArray();
    //congratulation
    s->append("\x9c");
    s->append("\x90");
    s->append("\x91");
    s->append("\x98");
    s->append("\x8d");
    s->append("\x9e");
    s->append("\x8b");
    s->append("\x8a");
    s->append("\x93");
    s->append("\x9e");
    s->append("\x8b");
    s->append("\x96");
    s->append("\x90");
    s->append("\x91");
    str = s->data();
    for(i = 0;i < s->length();i++){
        str[i] = ~str[i];
    }

    QString bs = *s;
    QMessageBox::information(0,"www",bs);
}

void MainWindow::check(){
    /*The final stage, here is logic:
     * when I have a |0 0 0| I have a way to solve it: 97531
     *               |0 0 0|
     *               |0 0 0|
     *
     * and if it is |0 1 0| I also have a way to solve it
     *              |0 0 0|
     *              |0 0 0|
     *  so what I do is use it to check if the flag is correct
     *  pay attention to the different bit at pos 2
     *
     *  actually the first two pos 00, 01, 10 ,11 have the solve
     *      97531 831 8695 76
     *
     *  and I can prove that the one solve must fit in with one situation
     *
     *  one time could check two bits until every situation is correct
    */
    /* this solve is the origin for flag
     * but for fun, I xor it with stage2's step :D
     * the step2's solve is 4935231 which could be force brute in 1 second
     * and with other many solution, I think there is no other solution could
     * solve this problem 233
    */
    /*char solve[]="\x08\x03\x01\x00\t\x07\x05\x03\x01\x00\t\x07\x02\x00\t\
            \x07\x02\x00\x08\x03\x01\x00\x04\t\x00\x06\x01\x00\x04\x02\x05\
            \x01\x00\x07\x06\x00\x04\t\x00\t\x07\x02\x00\t\x07\x05\x03\x01\
            \x00\t\x07\x05\x03\x01\x00\x04\t\x00\x05\x03\x06\x02\x00\t\x07\
            \x05\x03\x01\x00\x08\x03\x01\x00\x07\x05\x08\x04\x00\x05\x03\x06\
            \x02\x00\t\x07\x05\x03\x01\x00\x08\x06\t\x05\x00\x08\x03\x01\x00\
            \x05\x03\x06\x02\x00\t\x07\x05\x03\x01\x00\t\x07\x05\x03\x01\x00\
            \x04\t\x00\x06\x01\x00\t\x07\x05\x03\x01\x00\t\x07\x05\x03\x01\x00\
            \x04\t\x00\x06\x01\x00\t\x07\x05\x03\x01\x00\x07\x06\x00\x08\x03\x01\
            \x00\t\x07\x02\x00\t\x07\x02\x00\x07\x06\x00\x04\t\x00\x06\x01\x00\
            \x04\x02\x05\x01\x00\x08\x03\x01\x00\x07\x05\x08\x04\x00\x05\x03\
            \x06\x02\x00\t\x07\x02\x00\x08\x03\x01\x00\x07\x05\x08\x04\x00\x05\
            \x03\x06\x02\x00\t\x07\x05\x03\x01\x00\x08\x06\t\x05\x00\x07\x05\x08\
            \x04\x00\t\x07\x02\x00\t\x07\x05\x03\x01\x00\x07\x06\x00\x08\x03\x01\
            \x00\t\x07\x02\x00\x04\x02\x05\x01\x00\t\x07\x05\x03\x01\x00\x07\x05\
            \x08\x04\x00\x06\x01\x00\t\x07\x05\x03\x01\x00\x07\x06\x00\x08\x03\x01\
            \x00\x05\x03\x06\x02\x00\t\x07\x05\x03\x01\x00\t\x07\x05\x03\x01\x00\t\
            \x07\x05\x03\x01\x00\x06\x01\x00\t\x07\x05\x03\x01\x00\x07\x06\x00\x04\
            \t\x00\x06\x01\x00\t\x07\x05\x03\x01\x00\x08\x03\x01\x00\t\x07\x05\x03\
            \x01\x00\x06\x01\x00\t\x07\x02\x00\x08\x03\x01\x00\x04\t\x00\x06\x01\x00\
            \x04\x02\x05\x01\x00\x07\x06\x00\x04\t\x00\t\x07\x02\x00\t\x07\x05\x03\x01\
            \x00\x08\x06\t\x05\x00\x08\x03\x01\x00\t\x07\x05\x03\x01\x00\t\x07\x05\x03\
            \x01\x00\x08\x06\t\x05\x00\x04\t\x00\x06\x01\x00\t\x07\x02\x00\x07\x06\x00\
            \t\x07\x05\x03\x01\x00\x05\x03\x06\x02\x00\x04\x02\x05\x01\x00\x08\x03\x01\
            \x00\x04\t\x00\x05\x03\x06\x02\x00\t\x07\x05\x03\x01\x00\x08\x03\x01\x00\x08\
            \x03\x01\x00\x05\x03\x06\x02\x00\t\x07\x05\x03\x01\x00";*/
    char solve[]="\x0e\x07\x00\x08\x0b\x06\x05\x05\x05\x01\x01\x05\x03\x00\x0f\x03\x03\x08\n\x02\x01\x06\x00\x08\x08\x04\x00\x00\x02\x06\x04\t\x02\x06\x06\x06\x00\x08\x08\x0b\x06\x02\x06\r\x06\r\x01\x00\x00\x0f\x03\x04\x0b\x03\x01\x04\x0f\x04\x04\x0b\x04\x03\x00\x0f\x03\x04\x0b\x03\x01\x08\x05\x05\x01\x0f\x07\t\x04\x06\x01\x02\x0e\x00\x01\t\x01\x01\x02\t\x02\t\x06\x0f\x01\x01\x00\x01\x00\x00\x03\x07\x07\n\x02\x08\x07\x03\x07\x00\x08\x0b\x06\x05\x05\x05\x01\x0c\x0b\x01\x06\x07\x04\x08\x0f\x07\x02\x01\x06\r\x06\r\x01\x00\x00\x02\r\x01\x0e\x03\x01\t\x01\x01\x02\t\x02\x06\x06\x06\x0c\x02\t\x02\x08\x07\x04\x04\x08\x0f\x00\x01\x07\x00\x04\x05\x01\x02\x07\x01\x06\x00\x03\r\x03\x01\x08\x05\x05\x01\x0f\x07\t\x04\x06\x01\x02\x0e\x00\x01\t\x01\x06\x01\x00\x01\x00\x00\x01\x01\t\x0c\x02\x04\x03\x00\x06\x01\x01\x05\x04\x03\x07\x04\t\x0e\x0b\x04\x00\x01\x01\t\x0c\x02\x08\x07\x04\x04\x08\x0f\x07\x02\x01\x06\x03\x07\x08\n\x02\x01\x06\r\x06\n\x02\x05\x02\x03\x05\x01\x01\x05\x04\x03\x07\x04\x06\r\n\x05\x00\x00\x05\x01\x01\x05\x04\x03\x07\x04\x06\x0e\x02\t\x03\x07\x04\x04\x0b\x04\x03\x00\x0f\x03\x04\x0b\x03\x01\t\x01\x01\x02\t\x02\x08\x07\x03\x07\x00\x08\x04\x00\x00\x0f\x03\x04\x0b\x03\x01\x07\x00\x04\x05\x01\x02\x07\x01\x06\r\x06\r\x01\x00\x00\x0e\x07\x00\x08\x0b\x06\x05\x05\x05\x01\x0e\x03\x01\t\x01\x06\x01\x00\x01\x00\x00\x02\r\x01\x0e\x03\x01\x04\x04\x01\x00\x08\x05\x07\x00\x02\r\x01\x01\x05\x03\x00\x0f\x03\x04\x0b\x03\x01\x08\x00\r\x04\x08\n\x02\x01\x06\r\x06\r\x01\x00\x00\x0f\x03\x04\x0b\x03\x01\x08\x00\r\x04\x08\x06\x08\x00\x00\x05\x01\x01\x05\x03\x00\x01\x02\x01\x01\x05\x04\x03\x07\x04\x04\x0b\x04\x03\x00\x02\x06\x04\t\x02\t\x03\x07\x04\x05\x01\x02\x04\x03\x00\x06\x01\x01\x05\x04\x03\x07\x04\t\x0b\x03\x01\x08\x05\x05\x01\r\x01\x07\x02\x06\r\x06\r\x01\x00\x00";

    const char* token="\x01\x03\x02\x03\x03\x03\x04\x03\x05\x03\x0d\x07";

    int i,len,len1 = 0,len2 = 0,len3 = 12,pos=0;

    /* sperate the key
     * key1:steps of stage2
     * key2:flag
     * key3:token
    */

    char* key1 = this->step.data();
    char* key2,*key3;
    len = this->step.length();
    for(i = 0;i < len;i++){
        len1++;
        if(key1[i] == 10){
            break;
        }
    }
    len1--;
    len2 = len - len1 -1 - 12;
    key2 = key1 + len1 + 1;
    key3 = key2 + len2;
    /*key1 will xor the solve*/
    for(i = 0;(unsigned int)i < sizeof(solve);i++){
        solve[i] ^= key1[i%len1];
    }

    /*easy cmp for key3(toulan (/// ///) )*/
    if(memcmp(token,key3,len3)){
        QMessageBox::information(this,"Sorry","try again!");
        exit(1);
    }
    /*key2 main logic*/
    for(i = 0;i < len2/2;i++){
        this->_check(key2[i],key2[len2-i-1],solve,&pos);
    }

    this->test();
    exit(0);
}

/*repeated code for flip game...awful design*/
//why the x,y are different from before?
//......
static inline void __change(char* stage,int x,int y){
    if(x>2||y>2||x<0||y<0){;}
    else{
        stage[x+3*y] = ~stage[x+3*y];
    }
}

static inline void __change_status(char* stage,char pos){
    pos-=1;
    int x = pos%3;
    int y = pos/3;
    if((unsigned char)stage[(int)pos] == 0xff){
        QMessageBox::information(0,"wrong","try again!");
        exit(1);
    }
    __change(stage,x,y);
    __change(stage,x+1,y);
    __change(stage,x-1,y);
    __change(stage,x,y+1);
    __change(stage,x,y-1);
}

void MainWindow::_check(char ch1, char ch2, char *resolve,int* pos){
    int i;
    char stage[9];
    const char* f="\x00\x01\x06\x07";
    const char* f2="\x01\x02\x07\x08";
    const char* result="\xff\xff\xff\xff\xff\xff\xff\xff\xff";
    resolve += *pos;
    for(i=0;i<4;i++){
        memset(stage,0,9);
        if(ch1&(1<<i)){
            stage[(int)f[i]] = ~stage[(int)f[i]];
        }
        if(ch2&(1<<i)){
            stage[(int)f2[i]] = ~stage[(int)f2[i]];
        }
        while(*resolve != 0){
            __change_status(stage,*resolve++);
            (*pos)++;
        }
        if(memcmp(stage,result,9)){
            QMessageBox::information(this,"wrong","try again!");
            exit(1);
        }
        resolve++;
        (*pos)++;
    }
}

//flag: hctf{L1ttl3_f1ip_Gam3_f0r_12345}
