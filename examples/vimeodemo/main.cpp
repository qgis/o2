#include <QApplication>
#include <QStringList>
#include <QTimer>
#include <QDebug>

#include <QUrl>
#include <QDesktopServices>

#include "vimeodemo.h"

const char OPT_OAUTH_CODE[] = "-o";

class Helper : public QObject {
    Q_OBJECT

public:
    Helper() : QObject(), demo_(this) {}

public slots:
    void run() {
        connect(&demo_, &VimeoDemo::linkingFailed, this, &Helper::onLinkingFailed);
        connect(&demo_, &VimeoDemo::linkingSucceeded, this, &Helper::onLinkingSucceeded);
        connect(&demo_, &VimeoDemo::userNameReceived, this, &Helper::onUserNameReceived);
        connect(&demo_, &VimeoDemo::userNameFailed, this, &Helper::onUserNameFailed);

        demo_.doOAuth(O2::GrantFlowAuthorizationCode);
    }

    void onLinkingFailed() {
        qDebug() << "Linking failed!";
        qApp->exit(1);
    }

    void onLinkingSucceeded() {
        qDebug() << "Linking succeeded!";
        demo_.getUserName();
    }

    void onUserNameFailed() {
        qDebug() << "Error getting user name!";
        qApp->exit(1);
    }

    void onUserNameReceived() {
        qDebug() << "User namereceived!";
        qApp->quit();
    }

private:
    VimeoDemo demo_;
    bool waitForMsg_{false};
    QString msg_;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("O2");
    QCoreApplication::setApplicationName("Vimeo Test");
    Helper helper;
    QTimer::singleShot(0, &helper, &Helper::run);
    return a.exec();
}

#include "main.moc"
