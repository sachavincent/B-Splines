#include "ctrlPoints_dialog.h"
#include "ui_ctrlPoints_dialog.h"

CtrlPointsDialog::CtrlPointsDialog(QWidget *parent) : QDialog(parent), ui(new Ui::CtrlPointsDialog)
{
    ui->setupUi(this);

    connect(ui->delBtn, &QPushButton::pressed, this, &CtrlPointsDialog::removeSelection);
    connect(ui->addBtn, &QPushButton::pressed, this, &CtrlPointsDialog::openDialog);
    connect(ui->updateBtn, &QPushButton::pressed, this, &CtrlPointsDialog::updateBSpline);

    for (auto& v : ((MainWindow*)parentWidget())->openglWidget->getControlPoints())
        addControlPoint(v);
}

void CtrlPointsDialog::openDialog() {
    AddCtrlPointsDialog* dialog = new AddCtrlPointsDialog(this);
    dialog->setModal(true);
    dialog->exec();
}

void CtrlPointsDialog::addControlPoint(glm::vec3 controlPoint) {
    ui->ctlPointsList->addItem(formatControlPoint(controlPoint));
    _controlPoints.push_back(controlPoint);
}

void CtrlPointsDialog::updateBSpline() {
    ((MainWindow*) parentWidget())->openglWidget->setControlPoints(_controlPoints);
    close();
}

void CtrlPointsDialog::removeSelection() {
    for (auto& item : ui->ctlPointsList->selectedItems()) {
        std::string str = item->text().toStdString();
        _controlPoints.erase(std::remove(_controlPoints.begin(), _controlPoints.end(), controlPointFromString(str.substr(3, str.size() - 5))), _controlPoints.end());
        delete ui->ctlPointsList->takeItem(ui->ctlPointsList->row(item));
    }
}

QString CtrlPointsDialog::formatControlPoint(glm::vec3 controlPoint) {
    std::ostringstream s;
    s << " { " << controlPoint.x << " " << controlPoint.y << " " << controlPoint.z << " }";

    return QString::fromStdString(s.str());
}

glm::vec3 CtrlPointsDialog::controlPointFromString(std::string str) {
    std::vector<std::string> words{};
    size_t pos = 0;

    while ((pos = str.find(" ")) != std::string::npos) {
        words.push_back(str.substr(0, pos));
        str.erase(0, pos + 1);
    }
    words.push_back(str.substr(0, pos));

   return { std::stof(words[0]), std::stof(words[1]), std::stof(words[2]) };
}
