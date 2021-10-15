#ifndef CTRLPOINTSDIALOG_H
#define CTRLPOINTSDIALOG_H

#include <QDialog>
#include "../opengl_stuff.h"
#include "addctrlpoints_dialog.h"
#include "../mainwindow.h"
#include <sstream>
#include <algorithm>

namespace Ui {
class CtrlPointsDialog;
}

class CtrlPointsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CtrlPointsDialog(QWidget *parent = nullptr);

    ~CtrlPointsDialog() {
        delete ui;
    }

    void addControlPoint(glm::vec3);
    void updateBSpline();
    void removeSelection();
    void openDialog();

    QString formatControlPoint(glm::vec3);

    glm::vec3 controlPointFromString(std::string);
private:
    Ui::CtrlPointsDialog *ui;

    std::vector<glm::vec3> _controlPoints;
};

#endif // CTRLPOINTSDIALOG_H
