#ifndef PHENOTYPE_FILE_SELECT_H
#define PHENOTYPE_FILE_SELECT_H

#include <QDialog>
#include <QThread>
#include "../../convert/phenotype_convert.h"

namespace Ui {
class Phenotype_file_select;
}
class Phenotype_file_compute : public QThread{
    Q_OBJECT
    Phenotype_map& _ph_map_ref;
    const Phenotype_flags _ph_flag_ref;
    const QString _ph_filename;
public:
    Phenotype_file_compute(Phenotype_map&, const Phenotype_flags&, const QString&, QObject* parent=nullptr);
    void run() override;
signals:
    void load_finished();
};

class Phenotype_file_select : public QDialog
{
    Q_OBJECT
    Phenotype_map& _ph_map_ref;
    Phenotype_flags& _ph_flag_ref;
    QString _ph_filename;
    QThread _compute_thread;
public:
    explicit Phenotype_file_select(Phenotype_map& phenotype_map,Phenotype_flags& phenotype_flags, QWidget *parent = nullptr);
    ~Phenotype_file_select();

private slots:
    void on_uid_box_valueChanged(int arg1);

    void on_pht_box_valueChanged(int arg1);

    void on_skip_cbox_stateChanged(int arg1);

    void on_tab_rbtn_clicked();

    void on_cmm_rbtn_clicked();

    void on_scn_rbtn_clicked();

    void on_preview_btn_clicked();

    void on_ok_btn_clicked();

private:
    Ui::Phenotype_file_select *ui;
    void refresh_preview();
    void refresh_preview_highlight(const QString& line);
};

#endif // PHENOTYPE_FILE_SELECT_H
