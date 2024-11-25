#include <QApplication>
#include <QStringList>
#include <QTimer>
#include <QDebug>

#include "ytdemo.h"

class Helper : public QObject {
    Q_OBJECT

public:
    Helper() : QObject(), ytdemo_(this) {}

public slots:
    void run() {
        connect(&ytdemo_, &YTDemo::linkingFailed, this, &Helper::onLinkingFailed);
        connect(&ytdemo_, &YTDemo::linkingSucceeded, this, &Helper::onLinkingSucceeded);
        connect(&ytdemo_, &YTDemo::channelInfoReceived, this, &Helper::onChannelInfoReceived);
        connect(&ytdemo_, &YTDemo::channelInfoFailed, this, &Helper::onChannelInfoFailed);

        ytdemo_.doOAuth(O2::GrantFlowAuthorizationCode);
    }

    void onLinkingFailed() {
        qDebug() << "Linking failed!";
        qApp->exit(1);
    }

    void onLinkingSucceeded() {
        qDebug() << "Linking succeeded!";
        ytdemo_.getUserChannelInfo();
    }

    void onChannelInfoFailed() {
        qDebug() << "Error getting channel info!";
        qApp->exit(1);
    }

    void onChannelInfoReceived() {
        qDebug() << "Channel info received!";
        qApp->quit();
    }

private:
    YTDemo ytdemo_;
    QString msg_;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("O2");
    QCoreApplication::setApplicationName("YouTube Test");
    Helper helper;
// Suppress warning: Potential leak of memory in qtimer.h [clang-analyzer-cplusplus.NewDeleteLeaks]
#ifndef __clang_analyzer__
    QTimer::singleShot(0, &helper, &Helper::run);
#endif
    return a.exec();
}

#include "main.moc"
