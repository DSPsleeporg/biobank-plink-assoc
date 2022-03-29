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
    if (str_backspace_list.back().size()> 10){
        //If the new line is longer than 9 chars: use this line by itself.
        ui->out_txt->setText(str_backspace_list.back());
    }
    else{
        //Retain the information from previous time,
        // except for the last line if this line is shorter than 10 chars.
        QString old_txt = ui->out_txt->toPlainText();
        QStringList old_txt_list = old_txt.split('\n');
        if (old_txt_list.back().size()<= 10){
            old_txt_list.pop_back();
        }
        QString new_txt;
        for (QString& line : old_txt_list){
            new_txt.append(line);
            new_txt.append('\n');
        }
        new_txt.append(str_backspace_list.back());
        ui->out_txt->setText(new_txt);
    }

}
Plink_monitor::~Plink_monitor()
{
    delete plink_proc;
    delete ui;
}
