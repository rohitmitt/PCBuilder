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
    QString getLastDownloadedPartType() const;

    QString getLastDownloadedPartName() const;

    unordered_map<string, PCPart>& getRandomBuild() {
        return randomBuild;
    }


private:
    Ui::MainWindow *ui;
    Graph newGraph; //initialize graph object in QT
    bool useDFS;
    QString lastDownloadedPartType;
    QString lastDownloadedPartName;
    unordered_map<string, PCPart> randomBuild;

private slots:
    void on_traversalSwitch_clicked();
    void on_generatePC_clicked();
    void downloadPartImage(const QString& imageUrl, const QString& destinationPath, const QString& partType, const QString& partName);
    void downloadImages(const unordered_map<string, vector<PCPart>>& build);
    void initializeDirectories();

signals:
    void imageSavedSuccessfully(const QString& partType, const QString& partName);
    void startingImageDownload();

};


#endif // MAINWINDOW_H
