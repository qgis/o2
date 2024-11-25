#include <QApplication>
#include <QStringList>
#include <QTimer>
#include <QDebug>

#include "msgraphdemo.h"

class Helper : public QObject {
    Q_OBJECT

public:
    Helper() : QObject(), demo_(this) {}

public slots:
    void run() {
        connect(&demo_, &MsgraphDemo::linkingFailed, this, &Helper::onLinkingFailed);
        connect(&demo_, &MsgraphDemo::linkingSucceeded, this, &Helper::onLinkingSucceeded);
        connect(&demo_,
                &MsgraphDemo::userPrincipalNameReceived,
                this,
                &Helper::onUserPrincipalNameReceived);
        connect(&demo_,
                &MsgraphDemo::userPrincipalNameFailed,
                this,
                &Helper::onUserPrincipalNameFailed);

        // Start OAuth
        demo_.doOAuth(O2::GrantFlowAuthorizationCode);
    }

    void onLinkingFailed() {
        qDebug() << "Linking failed!";
        qApp->exit(1);
    }

    void onLinkingSucceeded() {
        qDebug() << "Linking succeeded!";
        demo_.getUserPrincipalName();
    }

    void onUserPrincipalNameFailed() {
        qDebug() << "Error getting userPrincipalName!";
        qApp->exit(1);
    }

    void onUserPrincipalNameReceived() {
        qDebug() << "UserPrincipalName received!";
        qApp->quit();
    }

private:
    MsgraphDemo demo_;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("O2");
    QCoreApplication::setApplicationName("Msgraph Example");
    Helper helper;
// Suppress warning: Potential leak of memory in qtimer.h [clang-analyzer-cplusplus.NewDeleteLeaks]
#ifndef __clang_analyzer__
    QTimer::singleShot(0, &helper, &Helper::run);
#endif
    return a.exec();
}

#include "main.moc"
