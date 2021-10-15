#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "myopenglwidget.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    MyOpenGLWidget* openglWidget;
private slots:
    void onDegreChanged(int);
    void onModalChanged(int);
    void onCtrlPointAdded();
private:
    Ui::MainWindow *ui;

    bool eventFilter(QObject*, QEvent*);
};

#endif // MAINWINDOW_H
