#ifndef PLINK_MONITOR_H
#define PLINK_MONITOR_H
#include <QProcess>
#include <QStringList>
#include <QString>
#include <QDialog>

namespace Ui {
class Plink_monitor;
}

class Plink_monitor : public QDialog
{
    Q_OBJECT
    QProcess* plink_proc;
    QString plink_path;
public:
    explicit Plink_monitor(QWidget *parent = nullptr);
    ~Plink_monitor();
private slots:
    void refresh_output_from_proc();
    void refresh_arg(const QStringList& args);
public slots:
    void execute_args(const QStringList& args = {});
private:
    Ui::Plink_monitor *ui;
};

#endif // PLINK_MONITOR_H
