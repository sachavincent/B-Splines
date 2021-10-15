#include "addctrlpoints_dialog.h"
#include "ui_addctrlpoints_dialog.h"
#include <QPushButton>

AddCtrlPointsDialog::AddCtrlPointsDialog(QWidget* parent) : QDialog(parent), ui(new Ui::AddCtrlPointsDialog)
{
    ui->setupUi(this);

    QPushButton* addBtn = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton* cancelBtn = ui->buttonBox->button(QDialogButtonBox::Cancel);

    connect(addBtn, SIGNAL(pressed()), SLOT(addControlPoint()));
    connect(cancelBtn, SIGNAL(pressed()), SLOT(close()));
}

void AddCtrlPointsDialog::addControlPoint()
{
    std::string text = ui->textEdit->toPlainText().toStdString();
    std::regex r{"^-?[0-9](\.[0-9]+)? -?[0-9](\.[0-9]+)? -?[0-9](\.[0-9]+)?$"};

    if (!std::regex_match(text, r)) {
        return;
    }

    ((CtrlPointsDialog*)parentWidget())->addControlPoint(((CtrlPointsDialog*)parentWidget())->controlPointFromString(text));

    close();
}