#include "randombuildwindow.h"
#include "mainwindow.h"
#include <QLabel>
#include <QVBoxLayout>

RandomBuildWindow::RandomBuildWindow(std::unordered_map<std::string, PCPart>& build, MainWindow *parent)
    : QWidget(parent), currentBuild(build)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Random Build Window", this);
    layout->addWidget(label);
    this->setLayout(layout);
}
