#include "plink_monitor.h"
#include "ui_plink_monitor.h"



Plink_monitor::Plink_monitor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plink_monitor)
{
    ui->setupUi(this);
#ifdef _WIN32
    plink_path  = "./plink_1.9/plink.exe";
#else
    plink_path = "./plink_1.9/plink";
#endif
}
void Plink_monitor::refresh_arg(const QStringList& args){
    QString arg_str = plink_path;
    for (const QString& arg : args){
        arg_str.append(' ');
        arg_str.append(arg);
    }
    ui->arg_txt->setText(arg_str);
    ui->arg_txt->setReadOnly(true);
}
void Plink_monitor::execute_args(const QStringList &args){
    //A primitive version without real-time monitoring
    refresh_arg(args);
    plink_proc = new QProcess();
    connect(plink_proc,&QProcess::readyReadStandardOutput,this,&Plink_monitor::refresh_output_from_proc);
    plink_proc->start(plink_path,args);
    return;
}
void Plink_monitor::refresh_output_from_proc(){
    QString str_raw = plink_proc->readAllStandardOutput();
    QStringList str_backspace_list = str_raw.split('\b');
    ui->out_txt->setText(str_backspace_list.back());
}
Plink_monitor::~Plink_monitor()
{
    delete plink_proc;
    delete ui;
}
