#include "genotype_main.h"
#include "ui_genotype_main.h"
#include <QStandardItemModel>
#include <fstream>
#include <cstdio>
Genotype_main::Genotype_main(const Genotype_file_converter* gfc, QWidget *parent) :
    QDialog(parent),_gfc_ptr(gfc),
    ui(new Ui::Genotype_main)
{
    ui->setupUi(this);
}
inline QString to_str(const Genotype_file_convert_status gfcs){
    switch (gfcs){
    case Genotype_file_convert_status::success:
        return "Success";
    case Genotype_file_convert_status::UID_miss_fail:
        return "Fail: UID not found.";
    case Genotype_file_convert_status::genotype_file_invalid_fail:
        return "Fail: genotype file invalid.";
    case Genotype_file_convert_status::genotype_map_fail:
        return "Fail: genotype map invalid.";
    case Genotype_file_convert_status::in_progress:
        return "In progress.";
    case Genotype_file_convert_status::input_file_fail:
        return "Fail: cannot open input file.";
    case Genotype_file_convert_status::output_file_fail:
        return "Fail: cannot open output file.";
    case Genotype_file_convert_status::phenotype_map_fail:
        return "Fail: phenotype map invalid.";
    case Genotype_file_convert_status::phenotype_miss_fail:
        return "Fail: phenotype not found for subject.";
    case Genotype_file_convert_status::size_mismatch_fail:
        return "Fail: subject genome count unequal to genotype map.";
    }
}
Genotype_main::~Genotype_main()
{
    delete ui;
}
void Genotype_main::file_update_status(const QString& g_filename, const Genotype_file_convert_status convert_status){
    bool attempt_finalize = false;
    file_map_mutex.lock();
    file_map[g_filename] = convert_status;
    bool all_success = refresh_table();
    if (all_success && wait_dialog_ptr){
        attempt_finalize = true;
    }
    file_map_mutex.unlock();
    if (attempt_finalize){
        finalize();
    }
}
bool Genotype_main::refresh_table(){
    QStandardItemModel model;
    bool all_finish = true;
    model.setHorizontalHeaderLabels({"Filename","Status"});
    QList<QStandardItem *> items;
    for (const auto& file_status_pair : file_map){
        items.clear();
        items.append(new QStandardItem(file_status_pair.first));
        items.append(new QStandardItem(to_str(file_status_pair.second)));
        if (file_status_pair.second == Genotype_file_convert_status::in_progress){
            all_finish = false;
        }
        model.appendRow(items);
    }
    //Set display:
    ui->gm_tbl->setModel(&model);
    ui->gm_tbl->verticalHeader()->hide();
    ui->gm_tbl->horizontalHeader()->setStretchLastSection(true);
    return all_finish;
}
void Genotype_main::finalize(){
    //Finalize by combining all temp files, close dialogs...
    //Finzalize only occurs when OK is clicked, and wait_dialog created:
    std::fstream ofs;
    ofs.open("proxy.ped",std::ios_base::out | std::ios_base::binary);
    if (!ofs.is_open()){
        emit all_load_finished();
        close();
        return;
    }
    for (const auto& file_status_pair: file_map){
        const std::string tmp_filename = file_status_pair.first.toStdString()+".TMP";
        if (file_status_pair.second != Genotype_file_convert_status::success){
            std::remove(tmp_filename.c_str());
            continue;
        }
        std::fstream ifs;
        ifs.open(file_status_pair.first.toStdString()+".TMP",std::ios_base::in|std::ios_base::binary);
        ofs << ifs.rdbuf();
        ifs.close();
        std::remove(tmp_filename.c_str());
    }
    emit all_load_finished();
    close();
}
void Genotype_main::on_gm_ok_btn_clicked()
{
    //Create wait dialog, attempt to finalize.
    bool attempt_finalize = false;
    file_map_mutex.lock();
    wait_dialog_ptr = new Info_dialog("Finishing file conversion...");
    connect(this,&Genotype_main::all_load_finished,wait_dialog_ptr,&Info_dialog::close_panel);
    if (refresh_table()){
        attempt_finalize = true;
    }
    file_map_mutex.unlock();
    if (attempt_finalize){
        finalize();
    }
}
Genotype_file_compute::Genotype_file_compute(const Genotype_file_converter* _gfc_ptr, const QString& _g_filename, const Genotype_subject_flags& _gs_flag, QObject* parent):
QThread(parent),_gfc_ptr(_gfc_ptr),_g_filename(_g_filename),_gs_flag(_gs_flag){ }
void Genotype_file_compute::run(){
    Genotype_file_convert_status convert_status = _gfc_ptr->convert(_g_filename.toStdString(),_g_filename.toStdString()+".TMP",_gs_flag);
    emit genotype_file_finished(_g_filename,convert_status);
}
void Genotype_main::on_gm_add_btn_clicked()
{
    //Creates genotype_file_select, which then creates Genoype_file_compute
    QString g_filename;
    Genotype_subject_flags gs_f;
    Genotype_file_select gfs(g_filename,gs_f);
    gfs.exec();
    file_update_status(g_filename,Genotype_file_convert_status::in_progress);
    Genotype_file_compute gfc(_gfc_ptr,g_filename,gs_f);
    gfc.moveToThread(&_compute_thread);
    connect(&gfc,&Genotype_file_compute::genotype_file_finished,this,&Genotype_main::file_update_status);
    gfc.start();
}

