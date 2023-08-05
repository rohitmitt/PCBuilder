#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    useDFS = false;

    ui->traversalSwitch->setStyleSheet(
        "QCheckBox::indicator {"
        "    width: 26px;"
        "    height: 26px;"
        "}"
        "QCheckBox::indicator:checked {"
        "    image: url(:/images/switchDFS.svg);"
        "}"
        "QCheckBox::indicator:unchecked {"
        "    image: url(:/images/switchBFS.svg);"
        "}"
        );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_traversalSwitch_clicked()
{
    useDFS = ui->traversalSwitch->isChecked();
}


void MainWindow::on_generatePC_clicked()
{
    unordered_map<string, PCPart> currentBuild;

    if(useDFS) { // If useDFS is true, use DFS
        currentBuild = newGraph.generateRandomBuild(newGraph.traverseDFS());
    }
    else { // If useDFS is false, use BFS
        currentBuild = newGraph.generateRandomBuild(newGraph.traverseBFS());
    }
}

