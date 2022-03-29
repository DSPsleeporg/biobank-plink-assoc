#include "genotype_map_select.h"
#include "phenotype_select.h"
#include "exception_dialog.h"
#include "plink_monitor.h"
#include "../convert/genotype_convert.h"
#include "../convert/phenotype_convert.h"
#include "genotype_main.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QEventLoop>
#include <QStringList>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "convert_assoc_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    Genotype_proxy_map genotype_proxy_map;
    Genotype_map_select genotype_map_select(genotype_proxy_map);
    genotype_map_select.exec();
    if (genotype_proxy_map.status() != Genotype_proxy_status::ready){
        show_exception_dialogue("Loading genotype map file failed.");
        return -1;
    }
    else{
        genotype_proxy_map.print("proxy.map");
    }
    bool phenotype_discrete = true;
    Phenotype_map* phenotype_map_ptr = nullptr;//Set as ptr since phenotype_map is abstract.
    Phenotype_select phenotype_select(phenotype_map_ptr,phenotype_discrete);
    phenotype_select.exec();
    if (phenotype_map_ptr->status() != Phenotype_status::ready){
        show_exception_dialogue("Loading phenotype failed.");
        return -1;
    }
    Genotype_file_converter genotype_file_converter(&genotype_proxy_map,phenotype_map_ptr);
    Genotype_main genotype_main(&genotype_file_converter);
    genotype_main.exec();
    Plink_monitor plink_monitor;
    QStringList plink_args = {"--file","proxy","--no-sex","--no-fid","--no-parents","--allow-no-sex","--assoc","--out","assoc-result"};
    plink_monitor.execute_args(plink_args);
    plink_monitor.exec();
    return 0;
    //return a.exec();
}
