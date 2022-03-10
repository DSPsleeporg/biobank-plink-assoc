#include "exception_dialog.h"
#include "ui_exception_dialog.h"
bool show_exception_dialogue(const QString& error_str){
    bool rval;
    Exception_dialog w(error_str,rval);
    w.exec();
    return rval;
}
Exception_dialog::Exception_dialog(const QString& error_str, bool& restart, QWidget *parent) :
    QDialog(parent),_error_str(error_str),_restart(restart),
    ui(new Ui::Exception_dialog)
{
    ui->setupUi(this);
    ui->exc_txt->setText(_error_str);
}

Exception_dialog::~Exception_dialog()
{
    delete ui;
}


void Exception_dialog::on_exc_ok_btn_clicked()
{
    _restart = false;
    close();
}

