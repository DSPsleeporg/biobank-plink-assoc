#include "genotype_main.h"
#include "exception_dialog.h"
#include "ui_genotype_main.h"

#include <fstream>
#include <cstdio>

int Genotype_status_table::rowCount(const QModelIndex &parent) const{
    return name_status_pair_vec.size();
}
int Genotype_status_table::columnCount(const QModelIndex &parent) const{
    return 2;
}
QVariant Genotype_status_table::data(const QModelIndex &index, int role) const{
    if (role == Qt::DisplayRole){
        if (index.column()==0){
            return name_status_pair_vec[index.row()].first;
        }
        else{
            return name_status_pair_vec[index.row()].second;
        }
    }
    return QVariant();
}
void Genotype_status_table::clear_table(){
    name_status_pair_vec.clear();
}
void Genotype_status_table::append_row(const std::pair<QString, QString> &row){
    name_status_pair_vec.push_back(row);
    return;
}
Genotype_main::Genotype_main(const Genotype_file_converter* gfc, QWidget *parent) :
    QDialog(parent),_gfc_ptr(gfc),
    ui(new Ui::Genotype_main)
{
    ui->setupUi(this);
}
Genotype_main::~Genotype_main()
{
    delete ui;
}
void Genotype_main::file_update_status(const QString& g_filename, const Genotype_file_convert_status& convert_status){
    bool attempt_finalize = false;
    file_map_mutex.lock();
    file_map[g_filename] = convert_status;
    bool all_success = refresh_table();
    if (all_success && wait_dialog_ptr != nullptr){
        attempt_finalize = true;
    }
    file_map_mutex.unlock();
    active_thread_map_mutex.lock();
    auto thread_gfc_pair = active_thread_map.find(g_filename);
    if (thread_gfc_pair != active_thread_map.end()){
        delete thread_gfc_pair->second.second;
        thread_gfc_pair->second.first->quit();
        delete thread_gfc_pair->second.first;
        active_thread_map.erase(thread_gfc_pair);
    }
    active_thread_map_mutex.unlock();
    if (attempt_finalize){
        finalize();
    }
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
    return "Fail: undefined";
}
bool Genotype_main::refresh_table(){
    //Private method that does not lock file_map. Not concurrent safe.
    bool all_finish = true;
    genotype_status_table.clear_table();
    for (const auto& name_status : file_map){
        genotype_status_table.append_row(std::make_pair(name_status.first,to_str(name_status.second)));
        if (name_status.second == Genotype_file_convert_status::in_progress){
            all_finish = false;
        }
    }
    //Set display:
    ui->gm_tbl->setModel(&genotype_status_table);
    ui->gm_tbl->setColumnWidth(0,200);
    ui->gm_tbl->setColumnWidth(1,100);
    ui->gm_tbl->show();
    return all_finish;
}
File_combine::File_combine(std::unordered_map<QString,Genotype_file_convert_status>& file_map, QObject* parent):
    QThread(parent),file_map(file_map){}
void File_combine::run(){
    FILE* out_fptr = fopen("proxy.ped","wb");
    if (!out_fptr){
        return;
    }
    /*
    std::fstream ofs;
    ofs.open("proxy.ped",std::ios_base::out | std::ios_base::binary);
    if (!ofs.is_open()){

        close();
        return;
    }
    */
    constexpr int buffer_size = 16777216;
    //std::ios::sync_with_stdio(false);
    char* read_buffer = new char[buffer_size];
    //char* write_buffer = new char[buffer_size];
    //ofs.rdbuf()->pubsetbuf(write_buffer,buffer_size);
    for (const auto& file_status_pair: file_map){
        const std::string tmp_filename = file_status_pair.first.toStdString()+".TMP";
        if (file_status_pair.second != Genotype_file_convert_status::success){
            std::remove(tmp_filename.c_str());
            continue;
        }
        FILE* in_fptr = fopen(tmp_filename.c_str(),"rb");
        if (!in_fptr){
            continue;
        }
        int read_size = 0;
        while ((read_size = fread(read_buffer,1,buffer_size,in_fptr)) == buffer_size){
            fwrite(read_buffer,1,read_size,out_fptr);
        }
        fwrite(read_buffer,1,read_size,out_fptr);
        fclose(in_fptr);
        /*
        std::fstream ifs;
        ifs.rdbuf()->pubsetbuf(read_buffer,buffer_size);
        ifs.open(file_status_pair.first.toStdString()+".TMP",std::ios_base::in|std::ios_base::binary);
        ofs << ifs.rdbuf();
        ifs.close();
        */
        std::remove(tmp_filename.c_str());
    }
    delete[] read_buffer;
    fclose(out_fptr);
    emit combine_finished();
}
void Genotype_main::finalize(){
    //Finalize by combining all temp files, close dialogs...
    //Finzalize only occurs when OK is clicked, and wait_dialog created:
    emit all_load_finished();
    QThread combine_thread;
    File_combine file_combine(file_map);
    file_combine.moveToThread(&combine_thread);
    if (wait_dialog_ptr){
        delete wait_dialog_ptr;
    }
    wait_dialog_ptr = new Info_dialog("Combining TMP files...");
    connect(&file_combine,&File_combine::combine_finished,wait_dialog_ptr,&Info_dialog::close_panel);
    file_combine.start();
    wait_dialog_ptr->exec();
    file_combine.wait();
    close();
}
void Genotype_main::on_gm_ok_btn_clicked()
{
    //Create wait dialog, attempt to finalize.
    wait_dialog_ptr = new Info_dialog("Finishing file conversion...");
    connect(this,&Genotype_main::all_load_finished,wait_dialog_ptr,&Info_dialog::close_panel);
    file_map_mutex.lock();
    bool attempt_finalize = refresh_table();
    file_map_mutex.unlock();
    if (attempt_finalize){
        finalize();
    }
    else{
        wait_dialog_ptr->exec();
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
    //Check for repeated files:
    bool file_exists;
    file_map_mutex.lock();
    file_exists = file_map.count(g_filename) > 0;
    file_map_mutex.unlock();
    if (file_exists){
        show_exception_dialogue("Repeated file");
        return;
    }
    file_update_status(g_filename,Genotype_file_convert_status::in_progress);




    QThread* compute_thread = new QThread;
    Genotype_file_compute* gfc = new Genotype_file_compute(_gfc_ptr,g_filename,gs_f);
    gfc->moveToThread(compute_thread);
    connect(gfc,&Genotype_file_compute::genotype_file_finished,this,&Genotype_main::file_update_status);
    active_thread_map_mutex.lock();
    active_thread_map[g_filename] = std::make_pair(compute_thread,gfc);
    active_thread_map_mutex.unlock();
    gfc->start();
}

