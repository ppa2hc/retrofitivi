#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "digitalauto.hpp"
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QFileInfo>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

//#include <notification/notification.hpp>
//extern NotificationAsync *carNotifAsync;

//QString digitalautoDeployFile = "/opt/Kit-Agency/deploy/prototypes.json";
//QString digitalautoDeployFolder = "/opt/Kit-Agency/deploy/";
QString digitalautoDeployFolder = "/usr/bin/dreamkit/prototypes/";
QString digitalautoDeployFile   = digitalautoDeployFolder + "prototypes.json";

QMutex digitalAutoPrototypeMutex;

DigitalAutoAppCheckThread::DigitalAutoAppCheckThread(DigitalAutoAppAsync *parent)
{
    m_digitalAutoAppAsync = parent;
    m_filewatcher = new QFileSystemWatcher(this);

    if (m_filewatcher) {
        QString path = digitalautoDeployFile;

        if (QFile::exists(path)) {
            m_filewatcher->addPath(path);

//            connect(m_filewatcher, &QFileSystemWatcher::fileChanged, m_digitalAutoAppAsync, &DigitalAutoAppAsync::fileChanged);
            connect(m_filewatcher, SIGNAL(fileChanged(QString)), m_digitalAutoAppAsync, SLOT(fileChanged(QString)));
        }
    }
}

void DigitalAutoAppCheckThread::triggerCheckAppStart(QString id, QString name)
{
    m_appId = id;
    m_appName = name;
    m_istriggeredAppStart = true;
}

void DigitalAutoAppCheckThread::run()
{
    while(1) {
        if (m_istriggeredAppStart && !m_appId.isEmpty() && !m_appName.isEmpty()) {
            QThread::msleep(6000); // workaround: wait 2s for the app to start. TODO: consider to check if the start time is more than 2s
            system("dapr list > /usr/bin/dreamkit/prototypes/listcmd.log");
            QThread::msleep(10);
            QFile MyFile("/usr/bin/dreamkit/prototypes/listcmd.log");
            MyFile.open(QIODevice::ReadWrite);
            QTextStream in (&MyFile);
            QString raw = in.readAll();
            qDebug() << "reprint dapr list file: \n" << raw;
            if (raw.contains(m_appId, Qt::CaseSensitivity::CaseSensitive)) {
                emit resultReady(m_appId, true, "<b>"+m_appName+"</b>" + " is started successfully.");
            }
            else {
                emit resultReady(m_appId, false, "<b>"+m_appName+"</b>" + " is NOT started successfully.<br><br>Please contact the car OEM for more information !!!");
            }
            system("> /usr/bin/dreamkit/prototypes/listcmd.log");

            m_istriggeredAppStart = false;
            m_appId.clear();
            m_appName.clear();
        }

        QThread::msleep(100);
    }
}

bool digitalAutoFileExists(std::string path) {
    QFileInfo check_file(QString::fromStdString(path));
    // check if path exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}

DigitalAutoAppAsync::DigitalAutoAppAsync()
{
    m_appListInfo.clear();

    workerThread = new DigitalAutoAppCheckThread(this);
    connect(workerThread, &DigitalAutoAppCheckThread::resultReady, this, &DigitalAutoAppAsync::handleResults);
    connect(workerThread, &DigitalAutoAppCheckThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateDeploymentProgress()));
    m_timer->stop();
    m_deploymentProgressPercent = 0;

    std::string DK_BOARD_UNIQUE_SERIAL_NUMBER_FILE = "/proc/device-tree/serial-number";
    QString serialNo = "dreamKIT-";
    if(digitalAutoFileExists(DK_BOARD_UNIQUE_SERIAL_NUMBER_FILE)) {
        QFile serialNoFile(QString::fromStdString(DK_BOARD_UNIQUE_SERIAL_NUMBER_FILE));
        if (!serialNoFile.open(QIODevice::ReadOnly)) {
            qDebug() << __func__ << __LINE__ << serialNoFile.errorString();
        }
        else {
            QTextStream outputStream (&serialNoFile);
            serialNo += outputStream.readAll();
            serialNoFile.close();
        }
    }
    else {
        serialNo += "defaultName";
    }
    serialNo.remove(QChar::Null);
    qDebug() << __func__ << __LINE__ << "serialNo: " << serialNo;
    m_serialNo = serialNo;
}

void DigitalAutoAppAsync::updateDeploymentProgress()
{
//    qDebug() << "updateDeploymentProgress = " << m_deploymentProgressPercent;

    m_deploymentProgressPercent += 10;

    updateProgressValue(m_deploymentProgressPercent);
    if(m_deploymentProgressPercent == 100) {
        initSubscribeAppFromDB();
    }
    else if(m_deploymentProgressPercent == 200) {
        m_timer->stop();
        setProgressVisibility(false);
    }
}

Q_INVOKABLE void DigitalAutoAppAsync::initSubscribeAppFromDB()
{
    // using mutex to project run-time data struct. e.g., if removing app and deploying app occurs quite the same time,
    // then this function shall be called at the same time. it would corrupt the m_appListInfo
    digitalAutoPrototypeMutex.lock();

    clearAppListView();
    updateBoardSerialNumber(m_serialNo);

    QString filename = digitalautoDeployFile;

    QFile file(filename);
    file.open(QIODevice::ReadOnly|QIODevice::Text);
    if (file.isOpen()) {
        QString data = QString(file.readAll());
        file.close();
//        qDebug() << "raw file: " << data;
        QJsonArray jsonAppList = QJsonDocument::fromJson(data.toUtf8()).array();

        QList<DigitalAutoAppListStruct> appListInfo;

        for (const auto obj : jsonAppList) {
            DigitalAutoAppListStruct appInfo;

//            qDebug() << obj.toObject().value("name").toString();
            appInfo.name = obj.toObject().value("name").toString();

//            qDebug() << obj.toObject().value("id").toString();
            appInfo.appId = obj.toObject().value("id").toString();

//            qDebug() << QString().setNum(obj.toObject().value("lastDeploy").toDouble(), 'g', 13);
            appInfo.lastDeploy = QString().setNum(obj.toObject().value("lastDeploy").toDouble(), 'g', 13);

            appInfo.isSubscribed = false;

            int len = m_appListInfo.size();
            for (int i = 0; i < len; i++) {
                if (m_appListInfo[i].appId == appInfo.appId) {
                    appInfo.isSubscribed = m_appListInfo[i].isSubscribed;
                    break;
                }
            }

            appListInfo.append(appInfo);

//            qDebug() << appInfo.name << " - " << appInfo.appId << " - " << appInfo.isSubscribed << " -------------------------- ";
            appendAppInfoToAppList(appInfo.name, appInfo.appId, appInfo.isSubscribed);
        }

        m_appListInfo.clear();
        m_appListInfo = appListInfo;
    }
    else {
        qDebug() << filename << " is not existing";
    }

    digitalAutoPrototypeMutex.unlock();
}


Q_INVOKABLE void DigitalAutoAppAsync::openAppEditor(int idx)
{
    qDebug() << __func__ << __LINE__ << " index = " << idx;

    if (idx >= m_appListInfo.size()) {
        qDebug() << "index out of range";
        return;
    }

    QString;
    m_appListInfo[idx].appId;


    QString cmd;
    cmd += "cd " + digitalautoDeployFolder + m_appListInfo[idx].appId + ";code .;";
    qDebug() << cmd;
    system(cmd.toUtf8());
}

Q_INVOKABLE void DigitalAutoAppAsync::removeApp(int idx)
{
    qDebug() << __func__ << __LINE__ << " index = " << idx;

    if (idx >= m_appListInfo.size()) {
        qDebug() << "index out of range";
        return;
    }

    // if the app is open, then stop it
    if (m_appListInfo[idx].isSubscribed) {
        // popup a window saying "The app is still open. Please stop it first."
        executeApp(m_appListInfo[idx].name, m_appListInfo[idx].appId, false);
    }

    // could remove from the file system.
//    QThread::msleep(1000);
//    QString cmd = "rm -rf " + digitalautoDeployFolder + m_appListInfo[idx].appId;
//    qDebug() << "remove folder: " << cmd;
//    system(cmd.toUtf8());

    // delete in Json file
    QString filename = digitalautoDeployFile;
    QFile file(filename);
    file.open(QIODevice::ReadWrite|QIODevice::Text);
    if (file.isOpen()) {
        QString data = QString(file.readAll());
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
        QJsonArray jsonAppList = doc.array();
//        qDebug() << "raw file: " << data;
//        qDebug() << "before: \n" << doc;
        jsonAppList.removeAt(idx);
        QJsonDocument newDoc(jsonAppList);
//        qDebug() << "after: \n" << newDoc;
        file.resize(0);
        file.write(newDoc.toJson());
        file.close();
    }

    // delete in the list
    m_appListInfo.remove(idx);

    QThread::msleep(100);
    system("sync");
}

Q_INVOKABLE void DigitalAutoAppAsync::executeApp(const QString name, const QString appId, bool isSubsribed)
{

//    dapr run --app-id Flapbird --app-protocol grpc --resources-path /home/retrofit/.dapr/components --config /home/retrofit/.dapr/config.yaml --app-port 50008 ./Flappy-Bird-Qt

    if (isSubsribed) {
        {
            system("dapr list > /usr/bin/dreamkit/prototypes/listcmd.log");
            QThread::msleep(100);
            QFile MyFile("/usr/bin/dreamkit/prototypes/listcmd.log");
            MyFile.open(QIODevice::ReadWrite);
            QTextStream in (&MyFile);
            if (in.readAll().contains(appId, Qt::CaseSensitivity::CaseSensitive)) {
                qDebug() << appId << " is already open";
                system("> /usr/bin/dreamkit/prototypes/listcmd.log");
                return;
            }
            system("> /usr/bin/dreamkit/prototypes/listcmd.log");
        }

        // start digital.auto app
        QString homepath = "/home/jetson";
        if (const char* env_p = std::getenv("HOME_PATH")) {
        	homepath.clear();
        	homepath = QString::fromLocal8Bit(env_p);
        	qDebug()  << "Your PATH is: " << homepath << '\n';
        }


        QString cmd;
        cmd += "cd " + digitalautoDeployFolder + appId + ";> main.log;";
        cmd += "dapr run --app-id " + appId + " --app-protocol grpc --resources-path ";
        cmd += homepath + "/.dapr/components --config ";
        cmd += homepath + "/.dapr/config.yaml --app-port 50008 ";
//        cmd += "dapr run --app-id " + appId + " --app-protocol grpc --resources-path /home/jetson/.dapr/components --config /home/jetson/.dapr/config.yaml --app-port 50008 ";
//        cmd += "python3 " + digitalautoDeployFolder + appId + "/main.py  &";
        cmd += "python3 main.py  > main.log 2>&1 &";
        qDebug() << cmd;
        system(cmd.toUtf8());

        if (workerThread) {
            workerThread->triggerCheckAppStart(appId, name);
        }
    }
    else {
        QString cmd;
        cmd += "dapr stop --app-id " + appId + "  &";
        qDebug() << cmd;
        system(cmd.toUtf8());

        int len = m_appListInfo.size();
        for (int i = 0; i < len; i++) {
            if (m_appListInfo[i].appId == appId) {
                m_appListInfo[i].isSubscribed = false;
                return;
            }
        }
    }
}

void DigitalAutoAppAsync::handleResults(QString appId, bool isStarted, QString msg)
{
    updateStartAppMsg(appId, isStarted, msg);
    if (isStarted) {
        int len = m_appListInfo.size();
        for (int i = 0; i < len; i++) {
            if (m_appListInfo[i].appId == appId) {
                m_appListInfo[i].isSubscribed = true;
                return;
            }
        }
    }
}

void DigitalAutoAppAsync::fileChanged(const QString &path)
{
    m_timer->start(200);
    m_deploymentProgressPercent = 0;
    updateProgressValue(m_deploymentProgressPercent);
    qDebug() << "file changed: " << path;
    setProgressVisibility(true);

//    initSubscribeAppFromDB();
//    carNotifAsync->showNotificationFromBackend("There is an update from digital.auto", 1);
}
