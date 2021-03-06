#ifndef GENOTYPE_MAP_SELECT_H
#define GENOTYPE_MAP_SELECT_H
#include <string>
#include <QDialog>
#include <QWidget>
#include <QtCore>
#include "../convert/genotype_convert.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Genotype_map_select; }
QT_END_NAMESPACE
class Genotype_map_compute : public QThread{
    Q_OBJECT
    Genotype_proxy_map* _gpm;
    const Genotype_proxy_flags* _gpf;
    const QString* _gpfn;
public:
    Genotype_map_compute(Genotype_proxy_map* gpm, const Genotype_proxy_flags* gpf, const QString* gpfn, QObject* parent = nullptr):
        QThread(parent),_gpm(gpm),_gpf(gpf),_gpfn(gpfn){}
    void run() override;
signals:
    void load_finished();
};

class Genotype_map_select : public QDialog
{
    Q_OBJECT
    Genotype_proxy_map& _genotype_proxy_map_ref;
    Genotype_proxy_flags _gp_flag;
    QString _gp_filename;
public:
    Genotype_map_select(Genotype_proxy_map& genotype_proxy_map,QWidget *parent = nullptr);
    ~Genotype_map_select();

private slots:
    void on_gmap_ok_btn_clicked();

    void on_gmap_preview_btn_clicked();

    void on_gmap_UID_box_valueChanged(int arg1);

    void on_gmap_chrmosome_box_valueChanged(int arg1);

    void on_gmap_base_box_valueChanged(int arg1);

    void refresh_preview();
    void refresh_preview_highlight(const QString& line);
    void on_tab_rbtn_clicked();

    void on_comma_rbtn_clicked();

    void on_semicolon_rbtn_clicked();

    void on_skip_cbox_stateChanged(int arg1);

    void on_space_rbtn_clicked();

private:
    Ui::Genotype_map_select *ui;
    void get_genotype_map_path();

};
#endif // GENOTYPE_MAP_SELECT_H
