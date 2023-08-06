#include <iostream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(1920,1080);

    useDFS = false;

    ui->traversalSwitch->setStyleSheet(
        "QCheckBox::indicator {"
        "    width: 100px;"
        "    height: 100px;"
        "}"
        "QCheckBox::indicator:checked {"
        "    image: url(:/images/assets/switchDFS.svg);"
        "}"
        "QCheckBox::indicator:unchecked {"
        "    image: url(:/images/assets/switchBFS.svg);"
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
    unordered_map<string, vector<PCPart>> currentBuild;

    if(useDFS) { // If useDFS is true, use DFS
        currentBuild = newGraph.traverseDFS();
    }
    else { // If useDFS is false, use BFS
        currentBuild = newGraph.traverseBFS();
    }

    randomBuild = newGraph.generateRandomBuild(currentBuild);

//    emit startingImageDownload();

    // Introduce async function that calls downloadImages() while displayRandomBuild() is running
    downloadImages(currentBuild);
}

void MainWindow::downloadPartImage(const QString& imageUrl, const QString& destinationPath, const QString& partType, const QString& partName)
{
    QNetworkAccessManager* nam = new QNetworkAccessManager(this);
    QNetworkRequest request;
    request.setUrl(QUrl(imageUrl));

    QNetworkReply* reply = nam->get(request);
    connect(reply, &QNetworkReply::finished, this, [reply, destinationPath, partType, partName, this]() {
        if(reply->error() == QNetworkReply::NoError) {
            QByteArray jpegData = reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(jpegData);

            QDir().mkpath(QFileInfo(destinationPath).path()); // Create directory if it doesn't exist

            if(pixmap.save(destinationPath, "JPG")) {
                emit imageSavedSuccessfully(partType, partName);
            }

            else
                qDebug() << "Failed to save image.";
        }
        else {
            qDebug() << "Failed to download image.";
        }
        reply->deleteLater();
    });
}

void MainWindow::downloadImages(const unordered_map<string, vector<PCPart>>& build)
{
    // Initialize directories
    initializeDirectories();

    for (const auto& partType : build) {
//        int count = 0;
        for (const PCPart& part : partType.second) {
//            if (count >= 30)
//                break;
//            else {
            QString filename = QString::fromStdString(part.name);
            filename.replace(QRegExp("[\\\\/:*?\"<>|]"), "_");  // Replace invalid filename characters with "_"
            filename += ".jpg";  // Append file extension

            QString fullDestinationPath = QDir(buildPath).filePath(QString::fromStdString(part.type) + "/" + filename);
            downloadPartImage(QString::fromStdString(part.image), fullDestinationPath, QString::fromStdString(part.type), QString::fromStdString(part.name));
//            count++;
//            }
        }
    }
}

#include <QDir>

// Function to initialize directories
void MainWindow::initializeDirectories()
{
    // Generate the build path
    buildPath = QString("C:/Users/RohitPC/Desktop/Coding/PCBuilder/frontend/PCBuilderApp/assets/CompleteBuilds/BUILD_%1").arg(QDateTime::currentDateTime().toString("dd.MM.yyyy_hh.mm.ssAP"));

    // Create the build directory
    if (!QDir().mkpath(buildPath)) {
        qWarning() << "Failed to create build directory";
    }
    // List of part names
    QStringList partNames = {"case", "gpu", "cpu", "motherboard", "cooler", "psu", "memory", "storage"};

    // Create a directory for each part
    for(const auto& partName : partNames) {
        QDir().mkpath(buildPath + "/" + partName);
    }
}

QString MainWindow::getLastDownloadedPartType() const {
    qDebug() << "getLastDownloadedPartType:" << lastDownloadedPartType;
    return lastDownloadedPartType;
}

QString MainWindow::getLastDownloadedPartName() const {
    qDebug() << "getLastDownloadedPartType:" << lastDownloadedPartType;
    return lastDownloadedPartName;
}
