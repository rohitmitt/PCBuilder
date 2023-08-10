#include <QStackedWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <iostream>
#include <QFuture>
#include <QtConcurrent>
#include <QMetaObject>
#include <QFontMetrics>

#include "initdialog.h"
#include "ui_initdialog.h"
#include "mainwindow.h"

InitDialog::InitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InitDialog)
{
    ui->setupUi(this);
    this->resize(1920,1080);

    QFont buttonFont("Arial", 20);

    // Create stackedWidget
    stackedWidget = new QStackedWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);

    // Create the common answerBox and nextButton
    answerBox = new QComboBox();

    nextButton = new QPushButton("Next");
    nextButton->setFont(buttonFont);
    mainLayout->addWidget(nextButton, 2, Qt::AlignCenter);
    nextButton->hide();

    // Add screens to stackedWidget
    stackedWidget->addWidget(createWelcomeScreen());

    // Add each query question to stackedWidget
    QString question1 = "What is the resolution of your monitor?";
    QStringList answer1 = {"1080p (1920 x 1080)",
                           "1440p (2560 x 1440)",
                           "4K (3840 x 2160)"};
    stackedWidget->addWidget(createQuestionnaire(question1, answer1));

    QString question2 = "What type of games will you play?";
    QStringList answer2 = {"Indie Games (Minecraft, Terraria, etc.)",
                           "Esports (CS:GO, Valorant, etc)",
                           "AAA (CyberPunk, Elden Rings, etc.)",
                           "None, just basic office work."};
    stackedWidget->addWidget(createQuestionnaire(question2, answer2));

    QString question3 = "What settings do you wish to play these games at?";
    QStringList answer3 = {"Low",
                           "Medium",
                           "High"};
    stackedWidget->addWidget(createQuestionnaire(question3, answer3));

    QString question4 = "What is your preferred PC Size?";
    QStringList answer4 = {"Small",
                           "Medium",
                           "Large",
                           "No preference"};
    stackedWidget->addWidget(createQuestionnaire(question4, answer4));

    QString question5 = "What is your preferred CPU brand?";
    QStringList answer5 = {"Intel",
                           "AMD",
                           "No preference"};

    stackedWidget->addWidget(createQuestionnaire(question5, answer5));

    connect(startButton, &QPushButton::clicked, this, &InitDialog::onStartButtonClicked);
    connect(nextButton, &QPushButton::clicked, this, &InitDialog::onNextButtonClicked);
    connect(&displayTimer, &QTimer::timeout, this, &InitDialog::displayNextPart);

    // Set the layout for InitDialog
    this->setLayout(mainLayout);
}

InitDialog::~InitDialog()
{
    delete ui;
}

void InitDialog::onStartButtonClicked() {
    stackedWidget->setCurrentIndex(1); // Show the next screen in the stack=
    nextButton->show();


}

void InitDialog::onNextButtonClicked() {
    if (stackedWidget->currentIndex() == 1) { // First question
        int index = stackedWidget->currentWidget()->findChild<QComboBox*>()->currentIndex() + 1;
        switch (index) {
        case 1:
            monitorResolution = "1080p";
            resolution = "1080p";
            break;
        case 2:
            monitorResolution = "1440p";
            resolution = "1440p";

            break;
        case 3:
            monitorResolution = "4K";
            resolution = "4K";

            break;
        }
    }

    else if (stackedWidget->currentIndex() == 2) { // Second question
        int index = stackedWidget->currentWidget()->findChild<QComboBox*>()->currentIndex() + 1;

        switch (index) {
        case 1:
            gpuRAM = "4";
            cpuSpeed = "2.5";
            cpuCores = "4";
            cpuGeneration = "current";
            driveType = "SATA";
            driveSpace = "500";
            memoryRAM = "16";
            break;
        case 2:
            gpuRAM = "8";
            cpuSpeed = "3.0";
            cpuCores = "6";
            cpuGeneration = "current";
            driveType = "SATA";
            driveSpace = "1000";
            memoryRAM = "16";
            break;
        case 3:
            gpuRAM = "16";
            cpuSpeed = "3.5";
            cpuCores = "10";
            cpuGeneration = "latest";
            driveType = "NVME";
            driveSpace = "2000";
            memoryRAM = "32";
            break;
        case 4:
            gpuRAM = "2";
            resolution = "";
            cpuSpeed = "2";
            cpuCores = "2";
            cpuGeneration = "past";
            driveType = "HDD";
            driveSpace = "1000";
            memoryRAM = "8";
            break;
        }
    }

    else if (stackedWidget->currentIndex() == 3) { // Third question
        int index = stackedWidget->currentWidget()->findChild<QComboBox*>()->currentIndex() + 1;

        if (monitorResolution == "1080p") {
            // Indie games can still run on a "1080p" monitor regardless of setting

            //Esports title on 1080p at medium/high setting, upgrade GPU base power
            if (gpuRAM == "8") {
                switch (index) {
                case 1: // low settings
                    resolution = "1080p";
                    break;
                case 2: // Medium settings
                    resolution = "1080p";
                    break;
                case 3: // High settings
                    resolution = "1080p/1440p";
                    break;
                }
            }

            //if AAA game on 1080p monitor is being played at medium/high settings, upgrade GPU base power
            else if (gpuRAM == "16") { // For AAA games
                switch (index) {
                case 2: // Medium settings
                    resolution = "1080p/1440p";
                    break;
                case 3: // High settings
                    resolution = "1440p/4K";
                    break;
                }
            }
        } else if (monitorResolution == "1440p") {
            // if Indie game on 1440p Monitor at low/medium settings, then downgrade GPU resolution
            if (gpuRAM == "4") {
                switch (index) {
                case 1: // low settings
                    resolution = "1080p";
                    break;
                case 2: // medium settings
                    resolution = "1080p";
                    break;
                case 3: // high settings
                    resolution = "1080p/1440p";
                    break;
                }
            } else if (gpuRAM ==
                       "8") { // if Esports game on 1440p Monitor at low/medium settings, then downgrade GPU resolution
                switch (index) {
                case 1: // low settings
                    resolution = "1080p/1440p";
                    break;
                case 2: // medium settings
                    resolution = "1080p/1440p";
                    break;
                case 3: // high settings
                    resolution = "1440p";
                    break;
                }
            } else if (gpuRAM == "16") { // AAA titles played on 1440p monitor
                switch (index) {
                case 1: // low settings
                    gpuRAM = "8";
                    resolution = "1080p/1440p";
                    break;
                case 2: // medium settings
                    gpuRAM = "8";
                    resolution = "1440p";
                    break;
                case 3: // High settings
                    resolution = "4K";
                    break;
                }
            }
        } else if (monitorResolution == "4K") {
            // if Indie game on 4K Monitor at low/medium settings, then downgrade GPU resolution
            if (gpuRAM == "4") {
                switch (index) {
                case 1: // low settings
                    gpuRAM = "8";
                    resolution = "1080p";
                    break;
                case 2: // medium settings
                    gpuRAM = "8";
                    resolution = "1080p";
                    break;
                case 3: // high settings
                    gpuRAM = "8";
                    resolution = "1440p";
                    break;
                }
            } else if (gpuRAM == "8") { // if Esports game on 4K Monitor at low/medium settings
                switch (index) {
                case 1: // low settings
                    resolution = "1080p/1440p";
                    break;
                case 2: // medium settings
                    resolution = "1440p";
                    break;
                case 3: // high settings
                    resolution = "1440p/4K";
                    break;
                }
            } else if (gpuRAM == "16") { // AAA titles played on 4K monitor
                switch (index) {
                case 1: // low settings
                    gpuRAM = "8";
                    resolution = "1080p/1440p";
                    break;
                case 2: // medium settings
                    gpuRAM = "8";
                    resolution = "1440p/4K";
                    break;
                case 3: // High settings
                    resolution = "4K";
                    break;
                }
            }
        }
    }

    else if (stackedWidget->currentIndex() == 4) { // Fourth question
        int index = stackedWidget->currentWidget()->findChild<QComboBox*>()->currentIndex() + 1;

        switch (index) {
        case 1:
            pcSize = ITX;
            break;
        case 2:
            pcSize = MICRO_ATX;
            break;
        case 3:
            pcSize = ATX;
            break;
        }
    }

    else if (stackedWidget->currentIndex() == 5) { // Fifth question
        int index = stackedWidget->currentWidget()->findChild<QComboBox*>()->currentIndex() + 1;

        switch (index) {
        case 1:
            cpuBrand = "Intel";
            break;
        case 2:
            cpuBrand = "AMD";
            break;
        default:
            cpuBrand = "";
            break;
        }
    }

    if (stackedWidget->currentIndex() == 5)  { // Fifth question has been answered

        mainWindow = new MainWindow(this);
        stackedWidget->addWidget(mainWindow);

        connect(mainWindow, &MainWindow::imageSavedSuccessfully,
                this, &InitDialog::handleImageSavedSuccessfully);

        connect(mainWindow, &MainWindow::startingImageDownload, this, &InitDialog::displayRandomBuild);

        newGraph.initializeQuery(pcSize,
                                 gpuRAM,
                                 resolution,
                                 cpuBrand,
                                 cpuSpeed,
                                 cpuCores,
                                 cpuGeneration,
                                 driveType,
                                 driveSpace,
                                 memoryRAM);
        newGraph.printQuery();

        displayLoadingPage();

        //asynchronously run displayLoadingPage() and the functions: loadingData() and constructGraph();
        QFuture<void> future = QtConcurrent::run([this] {
            this->loadingData();
            QMetaObject::invokeMethod(this, "updateProgressBar", Qt::QueuedConnection); // Call updateProgressBar on the main thread after loadingData() is done
            this->newGraph.constructGraph();
        });

        QFutureWatcher<void>* watcher = new QFutureWatcher<void>();

        connect(watcher, &QFutureWatcher<void>::finished, this, &InitDialog::onGraphConstructionFinished);

        watcher->setFuture(future);
    }

    // update to nextstackstac
    stackedWidget->setCurrentIndex(stackedWidget->currentIndex() + 1);
}

void InitDialog::onGraphConstructionFinished() {
    // Set the progress bar to 100%
    progressBar->setValue(progressBar->maximum());

    disconnect(timer, &QTimer::timeout, this, &InitDialog::updateProgressBar);
    timer->stop();
    delete timer; // Delete the timer to free the memory
    timer = nullptr; // Set timer to nullptr to avoid a dangling pointer

    mainWindow->setGraph(newGraph);

    // Switch to the main window or do whatever else you need to do here
    stackedWidget->setCurrentWidget(mainWindow);
}

Graph& InitDialog::getGraph() {
    return newGraph;
}

QWidget* InitDialog::createQuestionnaire(const QString& question, const QStringList& answerOptions)
{
    QFont questionFont("Arial", 30);
    QFont answerFont("Arial", 25);

    // Create questionnaireWidget and add children
    questionnaireWidget = new QWidget(this);
    QVBoxLayout *questionnaireLayout = new QVBoxLayout(questionnaireWidget);

    QLabel *questionLabel = new QLabel(question, questionnaireWidget);
    questionLabel->setFont(questionFont);

    QComboBox* answerBox = new QComboBox();
    answerBox->addItems(answerOptions);
    answerBox->setFont(answerFont);

    // Add the spacers and widgets to the layout
    questionnaireLayout->addStretch(1); // Add stretch before
    questionnaireLayout->addWidget(questionLabel, 0, Qt::AlignCenter); // Center-align questionLabel
    questionnaireLayout->addSpacing(50); // Add 50 pixels of spacing between the welcomeLabel and subtitleLabel
    questionnaireLayout->addWidget(answerBox, 0, Qt::AlignCenter); // Center-align questionOneBox
    questionnaireLayout->addSpacing(50); // Add 50 pixels of spacing between the welcomeLabel and subtitleLabel
    return questionnaireWidget;
}

QWidget* InitDialog::createWelcomeScreen() {
    QFont titleFont("Arial", 50);
    QFont subtitleFont("Arial", 22);

    welcomeWidget = new QWidget(this);
    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomeWidget);

    QLabel *welcomeLabel = new QLabel("<div align='center'>Welcome to PCBuilder App!</div>", welcomeWidget);
    welcomeLabel->setFont(titleFont);

    QLabel *subtitleLabel = new QLabel("<div align='center'>Lets get to know you better first...</div>", welcomeWidget);
    subtitleLabel->setFont(subtitleFont);

    startButton = new QPushButton("Start Here", welcomeWidget);
    startButton->setFixedSize(400, 200);
    startButton->setFont(subtitleFont);
    startButton->setStyleSheet("text-align:center;");

    welcomeLayout->addStretch(1); // Add stretch before
    welcomeLayout->addWidget(welcomeLabel);
    welcomeLayout->addSpacing(50); // Add 50 pixels of spacing between the welcomeLabel and subtitleLabel
    welcomeLayout->addWidget(subtitleLabel);
    welcomeLayout->addSpacing(100); // Add 100 pixels of spacing between the welcomeLabel and subtitleLabel
    welcomeLayout->addWidget(startButton, 0, Qt::AlignCenter); // This will center the button in the layout
    welcomeLayout->addStretch(1); // Add stretch after

    return welcomeWidget;
}

void InitDialog::displayLoadingPage() {
    nextButton->hide();
    QVBoxLayout *loadingLayout = new QVBoxLayout();

    // Create and customize QLabel for displaying loading text
    QLabel *loadingLabel = new QLabel("Connecting The Dots...", this);
    QFont loadingFont("Arial", 40);
    loadingLabel->setFont(loadingFont);

    QLabel *subLabel = new QLabel("adjacencyMatrix progress bar:", this);
    QFont subFont("Arial", 30);
    subLabel->setFont(subFont);

    loadingLabel->setAlignment(Qt::AlignCenter);
    subLabel->setAlignment(Qt::AlignCenter);

    // Create and customize QProgressBar
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100); // Set range to represent percentages
    progressBar->setValue(0); // Set initial progress to 0%

    // Add QLabel and QProgressBar to QVBoxLayout
    loadingLayout->addStretch(1); // Add stretch after
    loadingLayout->addWidget(loadingLabel);
    loadingLayout->addSpacing(100);
    loadingLayout->addWidget(subLabel);
    loadingLayout->addWidget(progressBar);
    loadingLayout->addStretch(1); // Add stretch after

    // Create QWidget to hold QVBoxLayout and add it to QStackedWidget
    QWidget *loadingPage = new QWidget(this);
    loadingPage->setLayout(loadingLayout);
    stackedWidget->addWidget(loadingPage);

    // Update current page to loadingPage
    stackedWidget->setCurrentWidget(loadingPage);

    // Start the timer to update the progress bar
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &InitDialog::updateProgressBar);
    timer->start(500); // Update every 500 milliseconds
}

void InitDialog::updateProgressBar() {
    int value = progressBar->value();
    if (value < progressBar->maximum()) {
        value += 1; // Increase the progress by 1% per second
        progressBar->setValue(value);
    } else {
        if (timer) {
            // Stop the timer if the progress is complete
            timer->stop();
        }
    }
}

void InitDialog::loadingData() {
    newGraph.loadData("../../cmake-build-debug/data/Case.csv");
    newGraph.loadData("../../cmake-build-debug/data/Cooler.csv");
    newGraph.loadData("../../cmake-build-debug/data/CPU.csv");
    newGraph.loadData("../../cmake-build-debug/data/GPU.csv");
    newGraph.loadData("../../cmake-build-debug/data/Memory.csv");
    newGraph.loadData("../../cmake-build-debug/data/Motherboard.csv");
    newGraph.loadData("../../cmake-build-debug/data/PSU.csv");
    newGraph.loadData("../../cmake-build-debug/data/Storage.csv");
}

void InitDialog::displayPart(const PCPart& part)
{
    unordered_map<string, PCPart>& randomBuild = mainWindow->getRandomBuild();

    if (randomBuild.find(part.type) != randomBuild.end() &&
        randomBuild[part.type] == part) {
        displayRandomBuildPart(part);
        foundType.insert(part.type);
        return;
    }
    else if (foundType.find(part.type) != foundType.end())
        return;

    QString filename = QString::fromStdString(part.name);
    QString partType = QString::fromStdString(part.type);

    // CLEAN UP IMG FILENAME AND DISPLAY CORRECT IMAGE
    filename.replace(QRegExp("[\\\\/:*?\"<>|]"), "_");  // Replace invalid filename characters with "_"
    // Remove content in parentheses
    int startIdx = filename.indexOf('(');
    int endIdx = filename.indexOf(')');
    if(startIdx != -1 && endIdx != -1 && startIdx < endIdx) {
        filename.remove(startIdx, endIdx - startIdx + 1);
    }
    filename += ".jpg";  // Append file extension

    QString fullImagePath = QDir(mainWindow->buildPath).filePath(partType + "/" + filename);

    QPixmap partPixmap(fullImagePath);
    if(partPixmap.isNull()) {
        return;
    }
    // Check if a QLabel exists for this part type
    if(partImageLabels.contains(partType)) {
        QLabel* imageLabel = partImageLabels[partType];
        imageLabel->setPixmap(partPixmap.scaled(256, 256, Qt::KeepAspectRatio));
    }
}

void InitDialog::displayRandomBuildPage() {
    nextButton->hide();
    QWidget* randomBuildWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(randomBuildWidget);

    QStringList partTypes = {"CASE", "GPU", "CPU", "MOBO", "FAN", "PSU", "MEMORY", "STORAGE"};

    // Create a placeholder image to be used for all parts initially
    QPixmap placeholder(":/images/assets/placeholder.jpg");

    // Creating the parts layout
    QHBoxLayout* partsLayout = new QHBoxLayout();
    for(const auto& partType : partTypes) {
        // Create a widget for each part to hold the image and text
        QWidget* partWidget = new QWidget();
        QVBoxLayout* partLayout = new QVBoxLayout(partWidget);

        // Create a label for the image
        QLabel* imageLabel = new QLabel(this); // 'this' assumes it's inside a QWidget derived class, adjust accordingly
        imageLabel->setPixmap(placeholder.scaled(256, 256, Qt::KeepAspectRatio));
        partLayout->addWidget(imageLabel);

        // Map the QLabel pointer based on the part type
        if (partType == "MOBO")
            partImageLabels["motherboard"] = imageLabel;
        else if (partType == "FAN")
            partImageLabels["cooler"] = imageLabel;
        else
            partImageLabels[partType.toLower()] = imageLabel;

        // Create labels for the text
        QLabel* partLabel = new QLabel(partType);
        partLabel->setFont(QFont("Arial", 20));
        partLayout->addWidget(partLabel);

        QVector<QLabel*> attributeLabels; // Temporary QVector to hold labels

        QLabel* attribute1Label = new QLabel("Attribute 1");
        partLayout->addWidget(attribute1Label);
        attributeLabels.append(attribute1Label);

        QLabel* attribute2Label = new QLabel("Attribute 2");
        partLayout->addWidget(attribute2Label);
        attributeLabels.append(attribute2Label);

        QLabel* attribute3Label = new QLabel("Attribute 3");
        partLayout->addWidget(attribute3Label);
        attributeLabels.append(attribute3Label);

        QLabel* attribute4Label = new QLabel("Attribute 4");
        partLayout->addWidget(attribute4Label);
        attributeLabels.append(attribute4Label);

        // After setting all the attributes for this part type, store them in the QMap
        partAttributeLabels[partType.toLower()] = attributeLabels;

        // Add the part widget to the parts layout
        partsLayout->addWidget(partWidget);
    }

    // Add the parts layout to the main layout
    mainLayout->addLayout(partsLayout);

    // Set the widget's layout
    randomBuildWidget->setLayout(mainLayout);

    // Add the new widget to the stack
    stackedWidget->addWidget(randomBuildWidget);

    // Set the new widget to be the visible one
    stackedWidget->setCurrentWidget(randomBuildWidget);
}

void InitDialog::displayRandomBuildPart(const PCPart& part)
{
    QString filename = QString::fromStdString(part.name);
    QString partType = QString::fromStdString(part.type);

    // CLEAN UP IMG FILENAME AND DISPLAY
    filename.replace(QRegExp("[\\\\/:*?\"<>|]"), "_");  // Replace invalid filename characters with "_"
    // Remove content in parentheses
    int startIdx = filename.indexOf('(');
    int endIdx = filename.indexOf(')');
    if(startIdx != -1 && endIdx != -1 && startIdx < endIdx) {
        filename.remove(startIdx, endIdx - startIdx + 1);
    }
    filename += ".jpg";  // Append file extension

    QString fullImagePath = QDir(mainWindow->buildPath).filePath(partType + "/" + filename);

    QPixmap partPixmap(fullImagePath);
    if(partPixmap.isNull()) {
        return;
    }

    // Check if a QLabel exists for this part type
    if(partImageLabels.contains(partType)) {
        QLabel* imageLabel = partImageLabels[partType];
        QString linkUrl = "LINK_URL_HERE"; // Replace with actual URL.
        QString htmlContent = QString("<a href='%1'><img src='%2' width='256' height='256'></a>").arg(linkUrl, fullImagePath);
        imageLabel->setText(htmlContent);

        QStringList attributes;

        if (partType == "case") {
            QString refinedName = splitPartName(QString::fromStdString(part.name));
            attributes = QStringList{
                refinedName,
                "Size: " + QString::fromStdString(part.attributes.at("size")),
                "",
                ""
            };
        }
        else if (partType  == "gpu") {
            QString refinedName = splitPartName(QString::fromStdString(part.name));
            attributes = QStringList{
                refinedName,
                "Brand: " + QString::fromStdString(part.attributes.at("brand")),
                "VRAM: " + QString::fromStdString(part.attributes.at("VRAM")),
                "Power: " + QString::fromStdString(part.attributes.at("power")) + "W"
            };
        }
        else if (partType == "cpu") {
            QString refinedName = splitPartName(QString::fromStdString(part.name));
            attributes = QStringList{
                refinedName,
                "Brand: " + QString::fromStdString(part.attributes.at("brand")),
                "Socket: " + QString::fromStdString(part.attributes.at("socket")),
                "Speed: " + QString::fromStdString(part.attributes.at("speed")) + " GHZ"
            };
        }
        else if (partType  == "motherboard" || partType == "MOBO") {
            QString refinedName = splitPartName(QString::fromStdString(part.name));
            attributes = QStringList{
                refinedName,
                "Size: " + QString::fromStdString(part.attributes.at("size")),
                "Brand: " + QString::fromStdString(part.attributes.at("brand")),
                "Socket: " + QString::fromStdString(part.attributes.at("socket")),
            };
        }
        else if (partType == "cooler" || partType == "FAN") {
            QString refinedName = splitPartName(QString::fromStdString(part.name));
            attributes = QStringList{
                refinedName,
                "Type: " + QString::fromStdString(part.attributes.at("type")),
                "",
                ""
            };
        }
        else if (partType == "psu") {
            QString refinedName = splitPartName(QString::fromStdString(part.name));
            attributes = QStringList{
                refinedName,
                "Power: " + QString::fromStdString(part.attributes.at("power"))  +"W",
                "Size: " + QString::fromStdString(part.attributes.at("size")),
                ""
            };
        }
        else if (partType == "memory") {
            QString refinedName = splitPartName(QString::fromStdString(part.name));
            attributes = QStringList{
                refinedName,
                "Size: " + QString::fromStdString(part.attributes.at("size")),
                "Type: " + QString::fromStdString(part.attributes.at("type")),
                ""
            };
        }
        else if (partType == "storage") {
            QString refinedName = splitPartName(QString::fromStdString(part.name));
            attributes = QStringList{
                refinedName,
                "Size: " + QString::fromStdString(part.attributes.at("space")),
                "Type: " + QString::fromStdString(part.attributes.at("driveType")),
                "",
            };
        }

        // Get the associated attribute labels
        if(partAttributeLabels.contains(partType)) {
            QVector<QLabel*> attributeLabelsForPart = partAttributeLabels[partType];
            for(int i = 0; i < attributes.size() && i < attributeLabelsForPart.size(); i++) {
                attributeLabelsForPart[i]->setText(attributes[i]);
            }
        }
        else {
            qDebug() << "No attribute labels found for part type: " << partType;
        }
    }
    else {
        qDebug() << "No image label found for part type: " << partType;
    }
}

void InitDialog::handleImageSavedSuccessfully(const PCPart& part)
{
    partsQueue.enqueue(&part);

    if (!displayTimer.isActive()) {  // Only start timer if not already active
        displayTimer.start(1000);  // 2-second delay
    }
}

void InitDialog::displayNextPart()
{
    if (!partsQueue.isEmpty()) {
        const PCPart* partPtr = partsQueue.dequeue();
        displayPart(*partPtr);
    } else {
        displayTimer.stop();  // Stop the timer if no more parts to display
    }
}

QString InitDialog::splitPartName(const QString& fullName) {
    // Remove content in parentheses
    QString refinedName = fullName;
    int startIdx = refinedName.indexOf('(');
    int endIdx = refinedName.indexOf(')');

    if(startIdx != -1 && endIdx != -1 && startIdx < endIdx) {
        refinedName.remove(startIdx, endIdx - startIdx + 1);
    }

    // Split the name by space for further processing
    QStringList parts = refinedName.split(' ', Qt::SkipEmptyParts);

    // Process the name parts SkipEmptyPartsand add newlines
    QStringList result;
    for(int i = 0; i < parts.size(); i++) {
        if(i % 2 == 0 && i + 1 < parts.size()) {
            // Combine every two words into one line
            result.append(parts[i] + " " + parts[i+1]);
            i++;  // Skip the next word since we've already processed it
        } else {
            // For the last word or if there's no word to combine with
            result.append(parts[i]);
        }
    }

    return result.join("\n");
}

