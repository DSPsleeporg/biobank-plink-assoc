#include "info_dialog.h"
#include "ui_info_dialog.h"

Info_dialog::Info_dialog(const QString& info_str, QWidget *parent) :
    QDialog(parent),_info_str(info_str),
    ui(new Ui::Info_dialog)
{
    ui->setupUi(this);
    ui->info_txt->setText(_info_str);
}
void Info_dialog::close_panel(){
    close();
}
Info_dialog::~Info_dialog()
{
    delete ui;
}
