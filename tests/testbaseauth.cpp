#include <QtTest>
#include "o0baseauth.h"
#include "o0globals.h"
#include "testutils.h"

// Concrete implementation of O0BaseAuth for testing
class TestableAuth : public O0BaseAuth {
    Q_OBJECT
public:
    explicit TestableAuth(QObject *parent = nullptr, O0AbstractStore *store = nullptr)
        : O0BaseAuth(parent, store) {}

    // Implement pure virtual functions
    void link() override {
    }

    void unlink() override {
    }
};

class TestBaseAuth : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        store = new MockStore(this);
        auth = new TestableAuth(this, store);
    }

    void cleanupTestCase() {
        delete auth;  // store will be deleted by parent-child relationship
    }

    void init() {
    }

    void testClientConfiguration() {
        QString testClientId = "testClient";
        QString testClientSecret = "testSecret";

        auth->setClientId(testClientId);
        auth->setClientSecret(testClientSecret);

        QCOMPARE(auth->clientId(), testClientId);
        QCOMPARE(auth->clientSecret(), testClientSecret);
    }

    void testLinkedState() {
        QVERIFY(!auth->linked());
        auth->setLinked(true);
        QVERIFY(auth->linked());

        // Verify the actual stored value
        QString key = QString(O2_KEY_LINKED).arg(auth->clientId());
        QCOMPARE(store->values[key], QString("1"));
    }

    void testTokenManagement() {
        QString testToken = "testToken";
        auth->setToken(testToken);
        QCOMPARE(auth->token(), testToken);

        // Verify the actual stored value
        QString tokenKey = QString(O2_KEY_TOKEN).arg(auth->clientId());
        QCOMPARE(store->values[tokenKey], testToken);
    }

    void testTokenSecretManagement() {
        QString testSecret = "testSecret";
        auth->setTokenSecret(testSecret);
        QCOMPARE(auth->tokenSecret(), testSecret);

        // Verify the actual stored value
        QString secretKey = QString(O2_KEY_TOKEN_SECRET).arg(auth->clientId());
        QCOMPARE(store->values[secretKey], testSecret);
    }

    void testExtraTokens() {
        QVariantMap tokens;
        tokens["extra1"] = "value1";
        tokens["extra2"] = "value2";

        auth->setExtraTokens(tokens);
        QVariantMap retrievedTokens = auth->extraTokens();

        QCOMPARE(retrievedTokens["extra1"].toString(), QString("value1"));
        QCOMPARE(retrievedTokens["extra2"].toString(), QString("value2"));
    }

    void testLocalPort() {
        int testPort = 8080;
        auth->setLocalPort(testPort);
        QCOMPARE(auth->localPort(), testPort);
    }

private:
    MockStore* store;
    TestableAuth* auth;  // Changed to TestableAuth
};

QTEST_MAIN(TestBaseAuth)
#include "testbaseauth.moc"
