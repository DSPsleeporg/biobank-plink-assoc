#ifndef INFO_DIALOG_H
#define INFO_DIALOG_H

#include <QDialog>

namespace Ui {
class Info_dialog;
}

class Info_dialog : public QDialog
{
    Q_OBJECT
    const QString& _info_str;
public:
    explicit Info_dialog(const QString& info_str, QWidget *parent = nullptr);
    ~Info_dialog();

private:
    Ui::Info_dialog *ui;
public slots:
    void close_panel();
};

#endif // INFO_DIALOG_H
