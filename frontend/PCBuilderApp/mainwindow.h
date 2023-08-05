#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../../graph.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_traversalSwitch_clicked();

    void on_generatePC_clicked();

private:
    Ui::MainWindow *ui;
    Graph newGraph; //initialize graph object in QT
    bool useDFS;
};
#endif // MAINWINDOW_H
