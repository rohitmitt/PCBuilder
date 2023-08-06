#ifndef INITDIALOG_H
#define INITDIALOG_H
#include <string>
#include <unordered_set>
#include <QDialog>
#include <QComboBox>
#include <QStackedWidget>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>
#include <QQueue>
#include "../../graph.h"

class MainWindow;

using namespace std;

namespace Ui {
class InitDialog;
}

class InitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InitDialog(QWidget *parent = nullptr);
    ~InitDialog();
    Graph& getGraph(); // Declaration of the function
    QWidget* welcomeWidget; // New widget for welcome screen

    QWidget* questionnaireWidget; // New widget for questionnaire
    QComboBox* questionOneBox;

    QWidget* questionTwoWidget;
    QComboBox* questionTwoBox;

    QWidget* createQuestionnaire(const QString& question, const QStringList& answerOptions);
    QWidget* createWelcomeScreen();

    void loadingData();
    void onGraphConstructionFinished();
    void displayNextPart();
    void displayRandomBuildPart(const QString& partType, const PCPart& part);
    QString splitPartName(const QString &partName);


private:
    Ui::InitDialog *ui;
    Graph newGraph;
    MainWindow *mainWindow;

    QMap<QString, QLabel*> partImageLabels; // This seems to be missing from the info you've given but is used in the code
    QMap<QString, QVector<QLabel*>> partAttributeLabels;


    QStackedWidget *stackedWidget;
    QPushButton *startButton;
    QPushButton *nextButton;
    QComboBox *answerBox;

    int currentQuestion;

    //initialize variables needed for query
    string monitorResolution;
    string pcSize;
    string cpuBrand;
    string cpuGeneration;
    string driveType;
    string driveSpace;
    string gpuRAM;
    string resolution;
    string cpuSpeed;
    string cpuCores;
    string memoryRAM;

    QProgressBar* progressBar;
    QTimer *timer;

    QQueue<QPair<QString, QString>> partsQueue;
    QTimer displayTimer;

    unordered_set<string> foundType;


private slots:
    void onStartButtonClicked(); // Slot for start button clicked
    void onNextButtonClicked(); // Slot for next button clicked
    void updateProgressBar();
    void displayLoadingPage();
    void displayRandomBuild();
    void handleImageSavedSuccessfully(const QString& partType, const QString& partName);

public slots:
    void displayPart(const QString& partType, const QString& partName);
};

#endif // INITDIALOG_H
