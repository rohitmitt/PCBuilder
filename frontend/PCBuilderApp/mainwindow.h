#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <unordered_map>
#include "../../graph.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr); // add Graph parameter
    ~MainWindow();
    void setGraph(Graph& graph) {
        this->newGraph = graph;
    }
    QString buildPath;

    unordered_map<string, PCPart>& getRandomBuild() {
        return randomBuild;
    }


private:
    Ui::MainWindow *ui;
    Graph newGraph; //initialize graph object in QT
    bool useDFS; // check traversal type
    unordered_map<string, PCPart> randomBuild;
    int imageDownloadCounter = 0;


private slots:
    void on_traversalSwitch_clicked();
    void on_generatePC_clicked();
    void downloadPartImage(const PCPart& part, const QString& destinationPath);
    void downloadImages(const unordered_map<string, vector<PCPart>>& build);
    void initializeDirectories();

signals:
    void imageSavedSuccessfully(const PCPart& part);
    void startingImageDownload();

};


#endif // MAINWINDOW_H
