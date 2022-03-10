#ifndef EXCEPTION_DIALOG_H
#define EXCEPTION_DIALOG_H

#include <QDialog>
bool show_exception_dialogue(const QString& error_str);
namespace Ui {
class Exception_dialog;
}

class Exception_dialog : public QDialog
{
    Q_OBJECT
    const QString& _error_str;
    bool& _restart;
public:
    explicit Exception_dialog(const QString& error_str, bool& restart, QWidget *parent = nullptr);
    ~Exception_dialog();

private slots:
    void on_exc_ok_btn_clicked();

private:
    Ui::Exception_dialog *ui;
};

#endif // EXCEPTION_DIALOG_H
