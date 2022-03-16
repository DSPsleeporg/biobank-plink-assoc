#include "phenotype_file_select.h"
#include "ui_phenotype_file_select.h"
#include <QFile>
#include <QTextStream>
#include <QSyntaxHighlighter>
#include <info_dialog.h>
Phenotype_file_compute::Phenotype_file_compute(Phenotype_map& _ph_map_ref,const Phenotype_flags& _ph_flag_ref, const QString& _ph_filename, QObject* parent):
    QThread(parent),
    _ph_map_ref(_ph_map_ref),_ph_flag_ref(_ph_flag_ref),_ph_filename(_ph_filename){}
void Phenotype_file_compute::run(){
    _ph_map_ref.read_phenotype(_ph_filename.toStdString(),_ph_flag_ref);
    emit load_finished();
}
Phenotype_file_select::Phenotype_file_select(Phenotype_map& phenotype_map,Phenotype_flags& phenotype_flags, QWidget *parent) :
    QDialog(parent),_ph_map_ref(phenotype_map),_ph_flag_ref(phenotype_flags),
    ui(new Ui::Phenotype_file_select)
{
    ui->setupUi(this);
    //Set UI element to match flag:
    ui->skip_cbox->setChecked(_ph_flag_ref.skip_first_row);
    if (_ph_flag_ref.delimiter == '\t'){
        ui->tab_rbtn->setChecked(true);
    }
    else if (_ph_flag_ref.delimiter == ','){
        ui->cmm_rbtn->setChecked(true);
    }
    else if (_ph_flag_ref.delimiter == ';'){
        ui->scn_rbtn->setChecked(true);
    }
}

Phenotype_file_select::~Phenotype_file_select()
{
    delete ui;
}
void Phenotype_file_select::refresh_preview(){
    if (_ph_filename == ""){
        return;
    }
    QFile inputFile(_ph_filename);
    inputFile.open((QIODevice::ReadOnly));
    QTextStream iss(&inputFile);
    QString line = iss.readLine();
    if (_ph_flag_ref.skip_first_row){
        line = iss.readLine();
    }
    ui->preview_txt->setText("");
    refresh_preview_highlight(line);
}
enum class Phenotype_text_state{plain,UID,pheno};
Phenotype_text_state find_state_at_index(const int idx, const std::pair<int,int>& UID_range, const std::pair<int,int>& pheno_range){
    if (idx >= UID_range.first && idx < UID_range.first + UID_range.second){
        return Phenotype_text_state::UID;
    }
    else if (idx >= pheno_range.first && idx < pheno_range.first + pheno_range.second){
        return Phenotype_text_state::pheno;
    }
    else{
        return Phenotype_text_state::plain;
    }
}
void Phenotype_file_select::refresh_preview_highlight(const QString &line){
    const auto UID_range = Phenotype_line_parser::find_UID_idx_range(_ph_flag_ref,line.toStdString());
    const auto pheno_range = Phenotype_line_parser::find_phenotype_idx_range(_ph_flag_ref,line.toStdString());
    const int N = line.size();
    for (int i=0; i < N; i++){
        switch (find_state_at_index(i,UID_range,pheno_range)) {
        case Phenotype_text_state::UID:
            ui->preview_txt->setTextBackgroundColor(Qt::gray);
            ui->preview_txt->setTextColor(Qt::cyan);
            break;
        case Phenotype_text_state::pheno:
            ui->preview_txt->setTextBackgroundColor(Qt::gray);
            ui->preview_txt->setTextColor(Qt::magenta);
            break;
        case Phenotype_text_state::plain:
            ui->preview_txt->setTextBackgroundColor(Qt::white);
            ui->preview_txt->setTextColor(Qt::black);
            break;
        }
        ui->preview_txt->insertPlainText(line.at(i));
    }
}
void Phenotype_file_select::on_uid_box_valueChanged(int arg1)
{
    _ph_flag_ref.UID_idx = arg1;
    refresh_preview();
}


void Phenotype_file_select::on_pht_box_valueChanged(int arg1)
{
    _ph_flag_ref.phenotype_idx = arg1;
    refresh_preview();
}


void Phenotype_file_select::on_skip_cbox_stateChanged(int arg1)
{
    _ph_flag_ref.skip_first_row = ui->skip_cbox->isChecked();
    refresh_preview();
}


void Phenotype_file_select::on_tab_rbtn_clicked()
{
    _ph_flag_ref.delimiter = '\t';
    refresh_preview();
}


void Phenotype_file_select::on_cmm_rbtn_clicked()
{
    _ph_flag_ref.delimiter = ',';
    refresh_preview();
}


void Phenotype_file_select::on_scn_rbtn_clicked()
{
    _ph_flag_ref.delimiter = ';';
    refresh_preview();
}


void Phenotype_file_select::on_preview_btn_clicked()
{
    _ph_filename = ui->path_txt->text();
    refresh_preview();
}


void Phenotype_file_select::on_ok_btn_clicked()
{
    Phenotype_file_compute phenotype_file_compute(_ph_map_ref,_ph_flag_ref,_ph_filename);
    Info_dialog info_dialog("Loading...");
    phenotype_file_compute.moveToThread(&_compute_thread);
    connect(&phenotype_file_compute,&Phenotype_file_compute::load_finished,&info_dialog,&Info_dialog::close);
    phenotype_file_compute.start();
    info_dialog.exec();
    phenotype_file_compute.wait();
    close();
}

