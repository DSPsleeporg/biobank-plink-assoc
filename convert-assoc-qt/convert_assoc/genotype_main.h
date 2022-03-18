#ifndef GENOTYPE_MAIN_H
#define GENOTYPE_MAIN_H
#include "../../convert/genotype_convert.h"
#include <QDialog>
#include <QTableWidget>
#include <QTableView>
#include <map>
#include <QThread>
#include <QMutex>
#include "info_dialog.h"
#include "genotype_file_select.h"

namespace Ui {
class Genotype_main;
}
class Genotype_file_compute : public QThread{
    Q_OBJECT
    const Genotype_file_converter* _gfc_ptr;
    const QString _g_filename;
    const Genotype_subject_flags _gs_flag;
public:
    Genotype_file_compute(const Genotype_file_converter* _gfc_ptr, const QString& _g_filename, const Genotype_subject_flags& _gs_flag, QObject* parent = nullptr);
    void run() override;
signals:
    void genotype_file_finished(const QString& g_filename, const Genotype_file_convert_status convert_status);
};

class Genotype_main : public QDialog
{
    Q_OBJECT
    const Genotype_file_converter* _gfc_ptr;
    Info_dialog* wait_dialog_ptr = nullptr;
    std::map<QString,Genotype_file_convert_status> file_map;
    QMutex file_map_mutex;
public:
    explicit Genotype_main(const Genotype_file_converter* gfc, QWidget *parent = nullptr);
    ~Genotype_main();
    friend Genotype_file_compute;
private slots:
    void file_update_status(const QString& g_filename, const Genotype_file_convert_status convert_status);
    void on_gm_ok_btn_clicked();

    void on_gm_add_btn_clicked();

private:
    bool refresh_table();//Returns if all files are ready
    void finalize();
private:
    Ui::Genotype_main *ui;
signals:
    void all_load_finished();
};

#endif // GENOTYPE_MAIN_H
