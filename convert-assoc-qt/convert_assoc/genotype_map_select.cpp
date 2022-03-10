#include "genotype_map_select.h"
#include "./ui_genotype_map_select.h"
#include <QFile>
#include <QTextStream>
#include <QSyntaxHighlighter>
Genotype_map_select::Genotype_map_select(Genotype_proxy_map& genotype_proxy_map,QWidget *parent)
    : QWidget(parent),_genotype_proxy_map_ref(genotype_proxy_map)
    , ui(new Ui::Genotype_map_select)
{
    ui->setupUi(this);
    //Set gp_flag default values.
    _gp_flag.skip_first_row = false;
    _gp_flag.delimiter = '\t';
    _gp_flag.SNP_idx = 1;
    _gp_flag.chromosome_idx = 2;
    _gp_flag.base_start_idx = 4;
}
void Genotype_map_select::get_genotype_map_path(){
    _gp_filename = ui->gmap_path_txt->text();
    //TODO: Change flags using radio buttons, etc.
    _genotype_proxy_map_ref.read_map(_gp_filename.toStdString(),_gp_flag);
    close();
}
Genotype_map_select::~Genotype_map_select()
{
    delete ui;
}


void Genotype_map_select::on_gmap_ok_btn_clicked()
{
    get_genotype_map_path();

}

void Genotype_map_select::refresh_preview(){
    if (_gp_filename == ""){
        return;
    }
    QFile inputFile(_gp_filename);
    inputFile.open(QIODevice::ReadOnly);
    QTextStream iss(&inputFile);
    QString line = iss.readLine();
    if (_gp_flag.skip_first_row){
        line = iss.readLine();
    }
    ui->gmap_preview_txt->setText("");
    refresh_preview_highlight(line);
}
enum class Proxy_text_state{plain,SNP,chro,base};
Proxy_text_state find_state_at_index(const int idx, const std::pair<int,int>& SNP_range, const std::pair<int,int>& chromosome_range, const std::pair<int,int>& base_range){
    if (idx >= SNP_range.first && idx < SNP_range.first + SNP_range.second){
        return Proxy_text_state::SNP;
    }
    else if (idx >= chromosome_range.first && idx < chromosome_range.first + chromosome_range.second){
        return Proxy_text_state::chro;
    }
    else if (idx >= base_range.first && idx < base_range.first + base_range.second){
        return Proxy_text_state::base;
    }
    else{
        return Proxy_text_state::plain;
    }
}
void Genotype_map_select::refresh_preview_highlight(const QString& line){

    const auto SNP_range = Genotype_line_parser::find_SNP_idx_range(_gp_flag,line.toStdString());
    const auto chromosome_range = Genotype_line_parser::find_chromosome_idx_range(_gp_flag,line.toStdString());
    const auto base_1_range = Genotype_line_parser::find_base_idx_range(_gp_flag,line.toStdString(),1);
    const auto base_4_range = Genotype_line_parser::find_base_idx_range(_gp_flag,line.toStdString(),4);
    const std::pair<int,int> base_range = std::make_pair(base_1_range.first,base_4_range.first - base_1_range.first + base_4_range.second);
    const int N = line.size();
    for (int i=0; i < N; i++){
        switch (find_state_at_index(i,SNP_range,chromosome_range,base_range)) {
        case Proxy_text_state::SNP:
            ui->gmap_preview_txt->setTextBackgroundColor(Qt::gray);
            ui->gmap_preview_txt->setTextColor(Qt::cyan);
            break;
        case Proxy_text_state::chro:
            ui->gmap_preview_txt->setTextBackgroundColor(Qt::gray);
            ui->gmap_preview_txt->setTextColor(Qt::magenta);
            break;
        case Proxy_text_state::base:
            ui->gmap_preview_txt->setTextBackgroundColor(Qt::gray);
            ui->gmap_preview_txt->setTextColor(Qt::blue);
            break;
        case Proxy_text_state::plain:
            ui->gmap_preview_txt->setTextBackgroundColor(Qt::white);
            ui->gmap_preview_txt->setTextColor(Qt::black);
            break;
        }
        ui->gmap_preview_txt->insertPlainText(line.at(i));
    }
}
void Genotype_map_select::on_gmap_preview_btn_clicked()
{
    _gp_filename = ui->gmap_path_txt->text();
    refresh_preview();
}


void Genotype_map_select::on_gmap_UID_box_valueChanged(int arg1)
{
    _gp_flag.SNP_idx = arg1;
    refresh_preview();
}


void Genotype_map_select::on_gmap_chrmosome_box_valueChanged(int arg1)
{
    _gp_flag.chromosome_idx = arg1;
    refresh_preview();
}


void Genotype_map_select::on_gmap_base_box_valueChanged(int arg1)
{
    _gp_flag.base_start_idx = arg1;
    refresh_preview();
}

