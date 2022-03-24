#ifndef GENOTYPE_MAIN_H
#define GENOTYPE_MAIN_H
#include "../../convert/genotype_convert.h"
#include <QDialog>
#include <QTableWidget>
#include <QTableView>
#include <unordered_map>
#include <QThread>
#include <QMutex>
#include "info_dialog.h"
#include "genotype_file_select.h"
#include <utility>
#include <QAbstractTableModel>

namespace Ui {
class Genotype_main;
}
class File_combine : public QThread{
    Q_OBJECT
    std::unordered_map<QString,Genotype_file_convert_status>& file_map;
public:
    File_combine(std::unordered_map<QString,Genotype_file_convert_status>& file_map, QObject* parent = nullptr);
    void run() override;
signals:
    void combine_finished();
};

class Genotype_file_compute : public QThread{
    Q_OBJECT
    const Genotype_file_converter* _gfc_ptr;
    const QString _g_filename;
    const Genotype_subject_flags _gs_flag;
public:
    Genotype_file_compute(const Genotype_file_converter* _gfc_ptr, const QString& _g_filename, const Genotype_subject_flags& _gs_flag, QObject* parent = nullptr);
    void run() override;
signals:
    void genotype_file_finished(const QString& g_filename, Genotype_file_convert_status& convert_status);
};
class Genotype_status_table : public QAbstractTableModel{
    Q_OBJECT
    std::vector<std::pair<QString,QString>> name_status_pair_vec;
public:
    Genotype_status_table(QObject* parent = nullptr){}
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void append_row(const std::pair<QString,QString>& row);
    void clear_table();
};
class Genotype_main : public QDialog
{
    Q_OBJECT
    const Genotype_file_converter* _gfc_ptr;
    Info_dialog* wait_dialog_ptr = nullptr;
    std::unordered_map<QString,Genotype_file_convert_status> file_map;
    std::unordered_map<QString, std::pair<QThread*,Genotype_file_compute*>> active_thread_map;
    Genotype_status_table genotype_status_table;
    QMutex file_map_mutex;
    QMutex active_thread_map_mutex;
public:
    explicit Genotype_main(const Genotype_file_converter* gfc, QWidget *parent = nullptr);
    ~Genotype_main();
    friend Genotype_file_compute;
private slots:
    void file_update_status(const QString& g_filename, const Genotype_file_convert_status& convert_status);
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
