#ifndef PHENOTYPE_SELECT_H
#define PHENOTYPE_SELECT_H

#include <QDialog>
#include "../../convert/phenotype_convert.h"
namespace Ui {
class Phenotype_select;
}

class Phenotype_select : public QDialog
{
    Q_OBJECT
    Phenotype_map*& _ph_m_ptr;
    Discrete_phenotype_map* _d_ph_m_ptr;
    Scalar_phenotype_map* _s_ph_m_ptr;
    bool& _ph_dis_ref;
    Phenotype_flags _ph_flag;
public:
    explicit Phenotype_select(Phenotype_map*& phenotype_map_ptr, bool& phenotype_discrete, QWidget *parent = nullptr);
    ~Phenotype_select();

private slots:
    void on_add_file_btn_clicked();

private:
    Ui::Phenotype_select *ui;
};

#endif // PHENOTYPE_SELECT_H
