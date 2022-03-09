#include "genotype_map_select.h"
#include "..\..\convert\genotype_convert.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

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
    Genotype_map_select w(genotype_proxy_map);
    w.show();
    return a.exec();
}
