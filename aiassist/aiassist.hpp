#ifndef AIASSIT_H
#define AIASSIT_H

#include "QString"
#include <QObject>
#include <QThread>
#include <QList>
#include <QFileSystemWatcher>
#include <QTimer>

class AiassistAsync;

class VssThread : public QThread
{
    Q_OBJECT

public:
    VssThread(AiassistAsync *parent);
    void run();

private:
    AiassistAsync *m_parent;

    QString getVssApiValue(QString apiName);

};

class AiassistAsync: public QObject
{
    Q_OBJECT
public:
    AiassistAsync();
    ~AiassistAsync();

    Q_INVOKABLE void setTextToSpeech(QString msg);

signals:
    void updateTextToSpeech(QString msg);

private:
    VssThread *vssThread;
};

#endif // AIASSIT_H
