#include "genotype_map_select.h"
#include "./ui_genotype_map_select.h"

Genotype_map_select::Genotype_map_select(Genotype_proxy_map& genotype_proxy_map,QWidget *parent)
    : QWidget(parent),_genotype_proxy_map_ref(genotype_proxy_map)
    , ui(new Ui::Genotype_map_select)
{
    ui->setupUi(this);
}
void Genotype_map_select::get_genotype_map_path(){
    const std::string genotype_filename = ui->gmap_path_txt->text().toStdString();
    //TODO: Change flags using radio buttons, etc.
    _gp_flag.skip_first_row = false;
    _gp_flag.delimiter = '\t';
    _gp_flag.SNP_idx = 1;
    _gp_flag.chromosome_idx = 2;
    _gp_flag.base_start_idx = 4;
    _genotype_proxy_map_ref.read_map(genotype_filename,_gp_flag);
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


void Genotype_map_select::on_gmap_preview_btn_clicked()
{

}


void Genotype_map_select::on_gmap_UID_box_valueChanged(int arg1)
{

}


void Genotype_map_select::on_gmap_chrmosome_box_valueChanged(int arg1)
{

}


void Genotype_map_select::on_gmap_base_box_valueChanged(int arg1)
{

}

