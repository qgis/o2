#include <QApplication>
#include <QStringList>
#include <QTimer>
#include <QDebug>

#include "fbdemo.h"

const char OPT_OAUTH_CODE[] = "-o";
const char OPT_VALIDATE_TOKEN[] = "-v";

const char USAGE[] = "\n"
                     "Usage: facebookdemo [OPTION]...\n"
                     "Get OAuth2 access tokens from Facebook's OAuth service\n"
                     "\nOptions:\n"
                     "  %1\t\tLink with Facebook OAuth2 service using Authorization Code\n"
                     "  %2\t\tValidate Access Token\n";


class Helper : public QObject {
    Q_OBJECT

public:
    Helper() : QObject(), fbdemo_(this) {}

public slots:
    void processArgs() {
        QStringList argList = qApp->arguments();
        QByteArray help = QString(USAGE).arg(OPT_OAUTH_CODE,
                                             OPT_VALIDATE_TOKEN).toLatin1();
        const char* helpText = help.constData();
        connect(&fbdemo_, &FBDemo::linkingFailed, this, &Helper::onLinkingFailed);
        connect(&fbdemo_, &FBDemo::linkingSucceeded, this, &Helper::onLinkingSucceeded);
        if (argList.contains(OPT_OAUTH_CODE)) {
            // Start OAuth
            fbdemo_.doOAuth(O2::GrantFlowAuthorizationCode);
        } else if (argList.contains(OPT_VALIDATE_TOKEN)) {
            fbdemo_.validateToken();
        } else {
            qDebug() << helpText;
            qApp->exit(1);
        }
    }

    void onLinkingFailed() {
        qDebug() << "Linking failed!";
        qApp->exit(1);
    }

    void onLinkingSucceeded() {
        qDebug() << "Linking succeeded!";
        if (waitForMsg_) {
            //postStatusUpdate(msg_);
        } else {
            qApp->quit();
        }
    }

private:
    FBDemo fbdemo_;
    bool waitForMsg_{false};
    QString msg_;
};

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("MySoft");
    QCoreApplication::setOrganizationDomain("mysoft.com");
    QCoreApplication::setApplicationName("facebookdemo");
    Helper helper;
// Suppress warning: Potential leak of memory in qtimer.h [clang-analyzer-cplusplus.NewDeleteLeaks]
#ifndef __clang_analyzer__
    QTimer::singleShot(0, &helper, &Helper::processArgs);
#endif
    return a.exec();
}

#include "main.moc"
