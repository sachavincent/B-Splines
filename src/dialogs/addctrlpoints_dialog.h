#ifndef ADDCTRLPOINTSDIALOG_H
#define ADDCTRLPOINTSDIALOG_H

#include <QDialog>
#include "../opengl_stuff.h"
#include "ctrlpoints_dialog.h"
#include <regex>
#include <string>

namespace Ui {
    class AddCtrlPointsDialog;
    class CtrlPointsDialog;
}

class AddCtrlPointsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddCtrlPointsDialog(QWidget* parent = nullptr);

    ~AddCtrlPointsDialog() {
        delete ui;
    }

private slots:
    void addControlPoint();

private:
    Ui::AddCtrlPointsDialog* ui;
};
#endif // ADDCTRLPOINTSDIALOG_H
