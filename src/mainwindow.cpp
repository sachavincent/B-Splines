#include "mainwindow.h"

#include "ui_mainwindow.h"
#include "dialogs/ctrlPoints_dialog.h"

#include <QMessageBox>
#include <QVBoxLayout>

#include <iostream>
#include <sstream>
#include <iomanip>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    QSurfaceFormat format;
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    ui->setupUi(this);

    connect(ui->degreCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onDegreChanged(int)));

    connect(ui->modalCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onModalChanged(int)));

    connect(ui->addCtrlPointBtn, &QPushButton::pressed, this, &MainWindow::onCtrlPointAdded);

    ui->openGLWidget->installEventFilter(this);
    openglWidget = ui->openGLWidget;
}

void MainWindow::onCtrlPointAdded()
{
    CtrlPointsDialog dialog(this);
    dialog.setModal(true);
    dialog.exec();
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::FocusIn)
    {
        if (object == ui->openGLWidget)
        {
            ui->openGLWidget->createBSpline();
        }
    }
    return false;
}

void MainWindow::onDegreChanged(int index)
{
    ui->openGLWidget->setDegreIndex(index);
}

void MainWindow::onModalChanged(int index)
{
    ui->openGLWidget->setModalIndex(index);
}

MainWindow::~MainWindow() {
    delete ui;
}