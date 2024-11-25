#include <QtTest>
#include "o2replyserver.h"

class TestReplyServer : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        server = new O2ReplyServer();
    }

    void cleanupTestCase() {
        delete server;
    }

    void testConfiguration() {
        int testTimeout = 30;
        server->setTimeout(testTimeout);
        QCOMPARE(server->timeout(), testTimeout);

        int testTries = 5;
        server->setCallbackTries(testTries);
        QCOMPARE(server->callbackTries(), testTries);
    }

    void testReplyContent() {
        QByteArray content = "<html>Test</html>";
        server->setReplyContent(content);
        QCOMPARE(server->replyContent(), content);
    }

    void testQueryParsing() {
        QByteArray request = "GET /callback?code=test_code&state=test_state HTTP/1.1\r\n"
                             "Host: localhost\r\n\r\n";
        QMap<QString, QString> params = server->parseQueryParams(&request);
        QCOMPARE(params["code"], QString("test_code"));
        QCOMPARE(params["state"], QString("test_state"));
    }

private:
    O2ReplyServer* server;
};

QTEST_MAIN(TestReplyServer)
#include "testreplyserver.moc"
