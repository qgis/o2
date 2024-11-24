#include <QApplication>
#include <QStringList>
#include <QTimer>
#include <QDebug>

#include <QUrl>
#include <QDesktopServices>

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
    QTimer::singleShot(0, &helper, &Helper::run);
    return a.exec();
}

#include "main.moc"
