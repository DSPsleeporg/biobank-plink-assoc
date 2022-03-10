#include "phenotype_select.h"
#include "phenotype_file_select.h"
#include "ui_phenotype_select.h"
#include "exception_dialog.h"

Phenotype_select::Phenotype_select(Phenotype_map*& phenotype_map_ptr, bool& phenotype_discrete, QWidget *parent) :
    QDialog(parent),_ph_dis_ref(phenotype_discrete),_ph_m_ptr(phenotype_map_ptr),
    ui(new Ui::Phenotype_select)
{
    _ph_flag.is_discrete = _ph_dis_ref;
    ui->setupUi(this);
}

Phenotype_select::~Phenotype_select()
{
    delete ui;
}

void Phenotype_select::on_add_file_btn_clicked()
{
    if (_ph_m_ptr == nullptr){
        //First time:
        if (ui->discrete_rbtn->isChecked()){
            _ph_dis_ref = true;
            _ph_flag.is_discrete = true;
            _d_ph_m_ptr = new Discrete_phenotype_map;
            //We do not bother delete it as this is required until program is finished with its job...
            _ph_m_ptr = _d_ph_m_ptr;
        }
        else if (ui->scalar_rbtn->isChecked()){
            _ph_dis_ref = false;
            _ph_flag.is_discrete = false;
            _s_ph_m_ptr = new Scalar_phenotype_map;
            //We do not bother delete it as this is required until program is finished with its job...
            _ph_m_ptr = _s_ph_m_ptr;
        }
        else{
            show_exception_dialogue("Please select whether phenotype is discrete or scalar.");
            return;
        }
        //Lock UI elements:
       ui->scalar_rbtn->setCheckable(false);
       ui->discrete_rbtn->setCheckable(false);
        //Set _ph_flag initial value:
        _ph_flag.skip_first_row = false;
        _ph_flag.delimiter = '\t';
        _ph_flag.UID_idx = 1;
        _ph_flag.phenotype_idx = 2;
    }
    Phenotype_file_select phenotype_file_select(*_ph_m_ptr,_ph_flag,this);
    phenotype_file_select.exec();
}

