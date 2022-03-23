#include "genotype_file_select.h"
#include "ui_genotype_file_select.h"
#include <QFile>
#include <QTextStream>
Genotype_file_select::Genotype_file_select(QString& _g_filename, Genotype_subject_flags& _gs_flag, QWidget *parent) :
    QDialog(parent),_g_filename(_g_filename),_gs_flag(_gs_flag),
    ui(new Ui::Genotype_file_select)
{
    ui->setupUi(this);
    _gs_flag.skip_first_row = false;
    _gs_flag.delimiter = '\t';
    _gs_flag.UID_idx = 1;
    _gs_flag.genotype_idx = 2;
}
Genotype_file_select::~Genotype_file_select()
{
    delete ui;
}
enum class Genotype_text_state{plain,UID,geno};
Genotype_text_state find_state_at_index(const int idx, const std::pair<int,int> UID_range, const std::pair<int,int> geno_range){
    if (idx >= UID_range.first && idx < UID_range.first + UID_range.second){
        return Genotype_text_state::UID;
    }
    else if (idx >= geno_range.first){
        return Genotype_text_state::geno;
    }
    else{
        return Genotype_text_state::plain;
    }
}
void Genotype_file_select::refresh_preview(){
    if (_g_filename == ""){
        return;
    }
    QFile inputFile(_g_filename);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream iss(&inputFile);
    QString line;
    if (_gs_flag.skip_first_row){
        line = iss.readLine();
    }
    line = iss.readLine(256);
    ui->preview_txt->setText("");
    refresh_preview_highlight(line);
}
void Genotype_file_select::refresh_preview_highlight(const QString &line){
    const auto UID_range = Genotype_subject_line_parser::find_UID_idx_range(_gs_flag,line.toStdString());
    const auto geno_range = Genotype_subject_line_parser::find_genotype_idx_range(_gs_flag,line.toStdString());
    const int N = line.size();
    for (int i=0; i < N; i++){
        switch (find_state_at_index(i,UID_range,geno_range)) {
        case Genotype_text_state::UID:
            ui->preview_txt->setTextBackgroundColor(Qt::gray);
            ui->preview_txt->setTextColor(Qt::cyan);
            break;
        case Genotype_text_state::geno:
            ui->preview_txt->setTextBackgroundColor(Qt::gray);
            ui->preview_txt->setTextColor(Qt::magenta);
            break;
        case Genotype_text_state::plain:
            ui->preview_txt->setTextBackgroundColor(Qt::white);
            ui->preview_txt->setTextColor(Qt::black);
            break;
        }
        ui->preview_txt->insertPlainText(line.at(i));
    }
}
void Genotype_file_select::on_tab_rbox_clicked()
{
    _gs_flag.delimiter = '\t';
    refresh_preview();
}


void Genotype_file_select::on_cmm_rbox_clicked()
{
    _gs_flag.delimiter = ',';
    refresh_preview();
}


void Genotype_file_select::on_scn_rbox_clicked()
{
    _gs_flag.delimiter = ';';
    refresh_preview();
}
void Genotype_file_select::on_space_rbox_clicked()
{
    _gs_flag.delimiter = ' ';
    refresh_preview();
}

void Genotype_file_select::on_skip_cbox_stateChanged(int arg1)
{
    _gs_flag.skip_first_row = arg1;
    refresh_preview();
}


void Genotype_file_select::on_uid_box_valueChanged(int arg1)
{
    _gs_flag.UID_idx = arg1;
    refresh_preview();
}


void Genotype_file_select::on_gt_box_valueChanged(int arg1)
{
    _gs_flag.genotype_idx = arg1;
    refresh_preview();
}


void Genotype_file_select::on_queue_btn_clicked()
{
    close();
}


void Genotype_file_select::on_preview_btn_clicked()
{
    _g_filename = ui->path_txt->text();
    refresh_preview();
}




