#ifndef GENOTYPE_FILE_SELECT_H
#define GENOTYPE_FILE_SELECT_H
#include <QDialog>
#include "../convert/genotype_convert.h"
namespace Ui {
class Genotype_file_select;
}

class Genotype_file_select : public QDialog
{
    Q_OBJECT
    QString& _g_filename;
    Genotype_subject_flags& _gs_flag;
public:
    explicit Genotype_file_select(QString& _g_filename, Genotype_subject_flags& _gs_flag, QWidget *parent = nullptr);
    ~Genotype_file_select();

private slots:
    void on_tab_rbox_clicked();

    void on_cmm_rbox_clicked();

    void on_scn_rbox_clicked();

    void on_skip_cbox_stateChanged(int arg1);

    void on_uid_box_valueChanged(int arg1);

    void on_gt_box_valueChanged(int arg1);

    void on_queue_btn_clicked();
    void on_preview_btn_clicked();

    void on_space_rbox_clicked();

private:
    Ui::Genotype_file_select *ui;
    void refresh_preview();
    void refresh_preview_highlight(const QString& line);
};

#endif // GENOTYPE_FILE_SELECT_H
