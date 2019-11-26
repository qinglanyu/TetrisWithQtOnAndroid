#ifndef QALOG_H
#define QALOG_H

#include <qlogging.h>

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>

// debug level less than warning will not be wrote into log file
// on in release version stream into log file
namespace QA_LOG
{
static int m_logLevel = 1;
static QString m_logFile = QString("%1.log").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hhmmss.zzz"));

void customMsgHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context)

    if(type < m_logLevel)
        return;

    QString logInfo;
    switch (type)
    {
    case QtDebugMsg:
        //logInfo = QString("%1:%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),msg);
        logInfo=msg+":"+QString().arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        break;
    case QtWarningMsg:
        //logInfo = QString("%1[Warning]:%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),msg);
        logInfo=msg+"[Warning]:"+QString().arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        break;
    case QtCriticalMsg:
        //logInfo = QString("%1[Critical]:%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),msg);
        logInfo = msg+"[Critical]:"+QString().arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),msg);
        break;
    case QtInfoMsg:
        break;
    case QtFatalMsg:
        //logInfo = QString("%1[Fatal]:%2").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),msg);
        logInfo = msg+"[Fatal]:"+QString().arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"),msg);
        abort();
    }

    // for thead safety
    QMutex m_logMutex;
    {
        m_logMutex.lock();
        QFile outFile(m_logFile);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
        QTextStream ts(&outFile);
        ts << logInfo << endl;
        outFile.close();
        m_logMutex.unlock();
    }
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
  {
      QByteArray localMsg = msg.toLocal8Bit();
      switch (type) {
      case QtDebugMsg:
          fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtInfoMsg:
          fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtWarningMsg:
          fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtCriticalMsg:
          fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          break;
      case QtFatalMsg:
          fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
          abort();
      }
  }

void logInit(QString logFile = "", int logLevel = 1)
{
#ifndef _DEBUG
    if((logLevel<0) || (logLevel>3))
        m_logLevel = 1;
    else
        m_logLevel = logLevel;

    if(!logFile.isEmpty())
        m_logFile = logFile;
    qInstallMessageHandler(customMsgHandler);
#else
    Q_UNUSED(logFile)
    Q_UNUSED(logLevel)
#endif
}

}

#endif // QALOG_H
