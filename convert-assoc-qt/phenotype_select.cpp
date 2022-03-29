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
QTextBrowser* append_word(QTextBrowser* qtb_ptr, const QString& str){
    if (qtb_ptr->toPlainText() != ""){
        qtb_ptr->insertPlainText(", ");
    }
    qtb_ptr->insertPlainText(str);
    return qtb_ptr;
}
void Phenotype_select::refresh_discrete_phenotype_view(){
    if (_ph_flag.is_discrete == false){
        return;
    }
    if (_ph_m_ptr->status() == Phenotype_status::spoiled){
        show_exception_dialogue("Invalid phenotype map!");
        close();
    }
    const auto raw_phenotype = dynamic_cast<Discrete_phenotype_map*>(_ph_m_ptr)->get_raw_phenotype();
    //_ph_m_ptr is known to be of discrete type.
    ui->pos_txt->setText("");
    ui->neg_txt->setText("");
    for (const std::string& ph_std_str : raw_phenotype){
        if (_pos_ph_set.count(ph_std_str)){
            append_word(ui->pos_txt,QString(ph_std_str.c_str()));
        }
        else{
            append_word(ui->neg_txt,QString(ph_std_str.c_str()));
        }
    }
    return;
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
            ui->phenotype_ds_gbox->setTitle("Selected: Discrete (Locked)");
            _ph_m_ptr = _d_ph_m_ptr;
        }
        else if (ui->scalar_rbtn->isChecked()){
            _ph_dis_ref = false;
            _ph_flag.is_discrete = false;
            _s_ph_m_ptr = new Scalar_phenotype_map;
            //We do not bother delete it as this is required until program is finished with its job...
            _ph_m_ptr = _s_ph_m_ptr;
            ui->phenotype_ds_gbox->setTitle("Selected: Scalar (Locked)");
            ui->pos_txt->setText("Not Applicable.");
            ui->neg_txt->setText("Not Applicable.");
            ui->pos_add_btn->setText("Not Applicable.");
            ui->pos_add_btn->setCheckable(false);
            ui->pos_rmv_btn->setText("Not Applicable.");
            ui->pos_rmv_btn->setCheckable(false);
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
    refresh_discrete_phenotype_view();
}


void Phenotype_select::on_pos_add_btn_clicked()
{
    if (_ph_flag.is_discrete == false){
        show_exception_dialogue("Invalid operation");
        return;
    }
    QString pos_str = ui->pos_add_txt->text();
    if (dynamic_cast<Discrete_phenotype_map*>(_ph_m_ptr)->get_raw_phenotype().count(pos_str.toStdString()) == 0){
        show_exception_dialogue("Nonexistent phenotype.");
    }
    else{
        _pos_ph_set.insert(pos_str.toStdString());
    }
    refresh_discrete_phenotype_view();
}


void Phenotype_select::on_pos_rmv_btn_clicked()
{
    if (_ph_flag.is_discrete == false){
        show_exception_dialogue("Invalid operation");
        return;
    }
    QString pos_str = ui->pos_add_txt->text();
    _pos_ph_set.erase(pos_str.toStdString());
    refresh_discrete_phenotype_view();
}


void Phenotype_select::on_fin_btn_clicked()
{
    switch (_ph_m_ptr->status()) {
    case Phenotype_status::empty:
        show_exception_dialogue("Phenotype map empty.");
        return;
        break;
    case Phenotype_status::bisect_wait:
        //Is discrete:
        dynamic_cast<Discrete_phenotype_map*>(_ph_m_ptr)->set_phenotype_map(_pos_ph_set);
    case Phenotype_status::ready:
        close();
        break;
    case Phenotype_status::spoiled:
        show_exception_dialogue("Invalid phenotype map!");
        close();
        break;
    }
}

