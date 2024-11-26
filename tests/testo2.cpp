// testo2.cpp
#include <QtTest>
#include "o2.h"
#include "o2requestor.h"
#include "testutils.h"

class TestO2: public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        manager = new MockManager(this);
        store = new MockStore(this);
        o2 = new O2(nullptr, manager, store);

        // Set up default configuration
        o2->setClientId("test_client");
        o2->setClientSecret("test_secret");
        o2->setScope("test_scope");
        o2->setLocalPort(0); // Let O2 choose a free port

        // Set up default URLs
        o2->setRequestUrl("http://localhost/auth");
        o2->setTokenUrl("http://localhost/token");
        o2->setRefreshTokenUrl("http://localhost/refresh");
    }

    void cleanupTestCase() {
        delete o2;
    }

    void init() {
        // Reset state before each test
        o2->setLinked(false);
        o2->setToken("");
        o2->setRefreshToken("");
        o2->setExpires(0);
        manager->setShouldFail(false);
        manager->lastOperation = QNetworkAccessManager::UnknownOperation;
        manager->lastData.clear();
        manager->responseData.clear();
    }

    void testBasicConfiguration() {
        QCOMPARE(o2->clientId(), QString("test_client"));
        QCOMPARE(o2->clientSecret(), QString("test_secret"));
        QCOMPARE(o2->scope(), QString("test_scope"));
        QCOMPARE(o2->requestUrl(), QString("http://localhost/auth"));
        QCOMPARE(o2->tokenUrl(), QString("http://localhost/token"));
        QCOMPARE(o2->refreshTokenUrl(), QString("http://localhost/refresh"));
    }

    void testGrantFlow() {
        o2->setGrantFlow(O2::GrantFlowAuthorizationCode);
        QCOMPARE(o2->grantFlow(), O2::GrantFlowAuthorizationCode);
        o2->setGrantFlow(O2::GrantFlowImplicit);
        QCOMPARE(o2->grantFlow(), O2::GrantFlowImplicit);
    }

    void testAuthorizationCodeFlow() {
        QSignalSpy openBrowserSpy(o2, &O2::openBrowser);
        QSignalSpy closeBrowserSpy(o2, &O2::closeBrowser);
        QSignalSpy linkingSucceededSpy(o2, &O2::linkingSucceeded);
        QSignalSpy linkingFailedSpy(o2, &O2::linkingFailed);

        o2->setGrantFlow(O2::GrantFlowAuthorizationCode);

        // Start auth flow
        o2->link();

        // Verify browser signal
        QCOMPARE(openBrowserSpy.count(), 1);
        QUrl authUrl = openBrowserSpy.at(0).at(0).toUrl();
        QVERIFY(authUrl.toString().contains("response_type=code"));
        QVERIFY(authUrl.toString().contains("client_id=test_client"));
        QVERIFY(authUrl.toString().contains("scope=test_scope"));

        // Extract state parameter for verification
        QString state = QUrlQuery(authUrl).queryItemValue("state");
        QVERIFY(!state.isEmpty());

        // Prepare mock token response
        QByteArray tokenResponse = R"({
            "access_token": "test_access_token",
            "refresh_token": "test_refresh_token",
            "expires_in": 3600,
            "token_type": "Bearer"
        })";
        manager->setResponse(tokenResponse);

        // Simulate callback with auth code
        QMap<QString, QString> response;
        response["code"] = "test_auth_code";
        response["state"] = state;
        o2->onVerificationReceived(response);

        // Verify token request
        QTRY_COMPARE(manager->lastOperation, QNetworkAccessManager::PostOperation);
        QCOMPARE(manager->lastRequest.url().toString(), QString("http://localhost/token"));
        QVERIFY(manager->lastData.contains("code=test_auth_code"));
        QVERIFY(manager->lastData.contains("grant_type=authorization_code"));
        QVERIFY(manager->lastData.contains("client_id=test_client"));
        QVERIFY(manager->lastData.contains("client_secret=test_secret"));

        // Verify success
        QTRY_COMPARE(linkingSucceededSpy.count(), 1);
        QCOMPARE(linkingFailedSpy.count(), 0);
        QCOMPARE(closeBrowserSpy.count(), 1);
        QVERIFY(o2->linked());
        QCOMPARE(o2->token(), QString("test_access_token"));
        QCOMPARE(o2->refreshToken(), QString("test_refresh_token"));
        QVERIFY(o2->expires() > QDateTime::currentSecsSinceEpoch());
    }

    void testAuthorizationCodeFlowPost2038() {
        QSignalSpy openBrowserSpy(o2, &O2::openBrowser);
        QSignalSpy closeBrowserSpy(o2, &O2::closeBrowser);
        QSignalSpy linkingSucceededSpy(o2, &O2::linkingSucceeded);
        QSignalSpy linkingFailedSpy(o2, &O2::linkingFailed);

        o2->setGrantFlow(O2::GrantFlowAuthorizationCode);

        // Start auth flow
        o2->link();

        // Verify browser signal
        QCOMPARE(openBrowserSpy.count(), 1);
        QUrl authUrl = openBrowserSpy.at(0).at(0).toUrl();
        QVERIFY(authUrl.toString().contains("response_type=code"));
        QVERIFY(authUrl.toString().contains("client_id=test_client"));
        QVERIFY(authUrl.toString().contains("scope=test_scope"));

        // Extract state parameter for verification
        QString state = QUrlQuery(authUrl).queryItemValue("state");
        QVERIFY(!state.isEmpty());

        // Prepare mock token response with far-future expiry
        // Using 20 years (630,720,000 seconds) for testing post-2038 dates
        QByteArray tokenResponse = R"({
        "access_token": "test_access_token_2038",
        "refresh_token": "test_refresh_token_2038",
        "expires_in": 630720000,
        "token_type": "Bearer"
    })";
        manager->setResponse(tokenResponse);

        // Simulate callback with auth code
        QMap<QString, QString> response;
        response["code"] = "test_auth_code_2038";
        response["state"] = state;
        o2->onVerificationReceived(response);

        // Verify token request
        QTRY_COMPARE(manager->lastOperation, QNetworkAccessManager::PostOperation);
        QCOMPARE(manager->lastRequest.url().toString(), QString("http://localhost/token"));
        QVERIFY(manager->lastData.contains("code=test_auth_code_2038"));
        QVERIFY(manager->lastData.contains("grant_type=authorization_code"));
        QVERIFY(manager->lastData.contains("client_id=test_client"));
        QVERIFY(manager->lastData.contains("client_secret=test_secret"));

        // Verify success
        QTRY_COMPARE(linkingSucceededSpy.count(), 1);
        QCOMPARE(linkingFailedSpy.count(), 0);
        QCOMPARE(closeBrowserSpy.count(), 1);
        QVERIFY(o2->linked());
        QCOMPARE(o2->token(), QString("test_access_token_2038"));
        QCOMPARE(o2->refreshToken(), QString("test_refresh_token_2038"));

        // Post-2038 specific verifications
        qint64 currentTime = QDateTime::currentSecsSinceEpoch();
        qint64 expectedExpiry = currentTime + 630720000;

        // Verify expiry is handled correctly
        QCOMPARE(o2->expires(), expectedExpiry);
        QVERIFY(o2->expires() > 2147483647); // Unix timestamp for 2038-01-19

        // Verify expiry calculation is using 64-bit arithmetic
        QVERIFY(expectedExpiry > INT_MAX);
    }

    void testImplicitFlow() {
        QSignalSpy openBrowserSpy(o2, &O2::openBrowser);
        QSignalSpy linkingSucceededSpy(o2, &O2::linkingSucceeded);
        QSignalSpy linkingFailedSpy(o2, &O2::linkingFailed);

        o2->setGrantFlow(O2::GrantFlowImplicit);

        // Start auth flow
        o2->link();

        // Verify browser signal
        QCOMPARE(openBrowserSpy.count(), 1);
        QUrl authUrl = openBrowserSpy.at(0).at(0).toUrl();
        QVERIFY(authUrl.toString().contains("response_type=token"));
        QVERIFY(authUrl.toString().contains("client_id=test_client"));

        // Extract state parameter
        QString state = QUrlQuery(authUrl).queryItemValue("state");

        // Simulate receiving token directly
        QMap<QString, QString> response;
        response["access_token"] = "test_implicit_token";
        response["expires_in"] = "3600";
        response["token_type"] = "Bearer";
        response["state"] = state;
        o2->onVerificationReceived(response);

        // Verify success
        QTRY_COMPARE(linkingSucceededSpy.count(), 1);
        QCOMPARE(linkingFailedSpy.count(), 0);
        QVERIFY(o2->linked());
        QCOMPARE(o2->token(), QString("test_implicit_token"));
        QVERIFY(o2->expires() > QDateTime::currentSecsSinceEpoch());
    }

    void testImplicitFlowPost2038Expiry() {
        QSignalSpy openBrowserSpy(o2, &O2::openBrowser);
        QSignalSpy linkingSucceededSpy(o2, &O2::linkingSucceeded);
        QSignalSpy linkingFailedSpy(o2, &O2::linkingFailed);

        // Set up implicit flow
        o2->setGrantFlow(O2::GrantFlowImplicit);

        // Start auth flow
        o2->link();

        // Verify browser signal
        QCOMPARE(openBrowserSpy.count(), 1);
        QUrl authUrl = openBrowserSpy.at(0).at(0).toUrl();
        QVERIFY(authUrl.toString().contains("response_type=token"));
        QVERIFY(authUrl.toString().contains("client_id=test_client"));

        // Extract state parameter
        QString state = QUrlQuery(authUrl).queryItemValue("state");

        // Calculate an expiry time well beyond 2038
        // Adding a large number of seconds to simulate a far future expiry
        // 20 years in seconds: 20 * 365 * 24 * 60 * 60 = 630,720,000
        QString farFutureExpiry = QString::number(630720000);

        // Simulate receiving token with far future expiry
        QMap<QString, QString> response;
        response["access_token"] = "test_post_2038_token";
        response["expires_in"] = farFutureExpiry;
        response["token_type"] = "Bearer";
        response["state"] = state;

        o2->onVerificationReceived(response);

        // Verify success
        QTRY_COMPARE(linkingSucceededSpy.count(), 1);
        QCOMPARE(linkingFailedSpy.count(), 0);
        QVERIFY(o2->linked());
        QCOMPARE(o2->token(), QString("test_post_2038_token"));

        // Verify expiry is handled correctly post-2038
        qint64 expectedExpiry = QDateTime::currentSecsSinceEpoch() + farFutureExpiry.toLongLong();
        QCOMPARE(o2->expires(), expectedExpiry);

        // Additional post-2038 specific checks
        QVERIFY(o2->expires() > 2147483647); // Unix timestamp for 2038-01-19
    }

    void testPasswordFlow() {
        QSignalSpy linkingSucceededSpy(o2, &O2::linkingSucceeded);
        QSignalSpy linkingFailedSpy(o2, &O2::linkingFailed);

        o2->setGrantFlow(O2::GrantFlowResourceOwnerPasswordCredentials);
        o2->setUsername("testuser");
        o2->setPassword("testpass");

        // Prepare mock token response
        QByteArray tokenResponse = R"({
            "access_token": "password_token",
            "refresh_token": "password_refresh",
            "expires_in": 3600,
            "token_type": "Bearer"
        })";
        manager->setResponse(tokenResponse);

        // Start authentication
        o2->link();

        // Verify token request
        QTRY_COMPARE(manager->lastOperation, QNetworkAccessManager::PostOperation);
        QCOMPARE(manager->lastRequest.url().toString(), QString("http://localhost/token"));
        QVERIFY(manager->lastData.contains("grant_type=password"));
        QVERIFY(manager->lastData.contains("username=testuser"));
        QVERIFY(manager->lastData.contains("password=testpass"));
        QVERIFY(manager->lastData.contains("client_id=test_client"));
        QVERIFY(manager->lastData.contains("client_secret=test_secret"));

        // Verify success
        QTRY_COMPARE(linkingSucceededSpy.count(), 1);
        QCOMPARE(linkingFailedSpy.count(), 0);
        QVERIFY(o2->linked());
        QCOMPARE(o2->token(), QString("password_token"));
        QCOMPARE(o2->refreshToken(), QString("password_refresh"));
    }

    void testRefreshToken() {
        QSignalSpy refreshFinishedSpy(o2, &O2::refreshFinished);

        // Setup initial state
        o2->setLinked(true);
        o2->setToken("old_token");
        o2->setRefreshToken("test_refresh_token");

        // Prepare mock refresh response
        QByteArray refreshResponse = R"({
            "access_token": "new_access_token",
            "refresh_token": "new_refresh_token",
            "expires_in": 3600
        })";
        manager->setResponse(refreshResponse);

        // Verify refresh URL is set
        QVERIFY(!o2->refreshTokenUrl().isEmpty());
        QCOMPARE(o2->refreshTokenUrl(), QString("http://localhost/refresh"));

        // Trigger refresh
        o2->refresh();

        // Verify refresh request
        QTRY_COMPARE(manager->lastOperation, QNetworkAccessManager::PostOperation);
        QCOMPARE(manager->lastRequest.url().toString(), QString("http://localhost/refresh"));
        QVERIFY(manager->lastData.contains("refresh_token=test_refresh_token"));
        QVERIFY(manager->lastData.contains("grant_type=refresh_token"));
        QVERIFY(manager->lastData.contains("client_id=test_client"));
        QVERIFY(manager->lastData.contains("client_secret=test_secret"));

        // Verify tokens updated
        QTRY_COMPARE(refreshFinishedSpy.count(), 1);
        QCOMPARE(refreshFinishedSpy.at(0).at(0).toInt(),
                 static_cast<int>(QNetworkReply::NoError));
        QCOMPARE(o2->token(), QString("new_access_token"));
        QCOMPARE(o2->refreshToken(), QString("new_refresh_token"));
    }

    void testRefreshTokenFailsWithNoUrl() {
        QSignalSpy refreshFinishedSpy(o2, &O2::refreshFinished);

        // Setup initial state
        o2->setLinked(true);
        o2->setToken("old_token");
        o2->setRefreshToken("test_refresh_token");
        o2->setRefreshTokenUrl(""); // Clear refresh URL

        // Trigger refresh
        o2->refresh();

        // Verify immediate failure due to no URL
        QTRY_COMPARE(refreshFinishedSpy.count(), 1);
        QCOMPARE(refreshFinishedSpy.at(0).at(0).toInt(),
                 static_cast<int>(QNetworkReply::AuthenticationRequiredError));
    }

    void testRefreshTokenFailsWithNoRefreshToken() {
        QSignalSpy refreshFinishedSpy(o2, &O2::refreshFinished);

        // Setup initial state with no refresh token
        o2->setLinked(true);
        o2->setToken("old_token");
        o2->setRefreshToken("");

        // Trigger refresh
        o2->refresh();

        // Verify immediate failure due to no refresh token
        QTRY_COMPARE(refreshFinishedSpy.count(), 1);
        QCOMPARE(refreshFinishedSpy.at(0).at(0).toInt(),
                 static_cast<int>(QNetworkReply::AuthenticationRequiredError));
    }

    void testAuthenticationError() {
        QSignalSpy openBrowserSpy(o2, &O2::openBrowser);
        QSignalSpy linkingFailedSpy(o2, &O2::linkingFailed);

        o2->setGrantFlow(O2::GrantFlowAuthorizationCode);
        manager->setShouldFail(true);

        // Start auth flow
        o2->link();

        // Get state from auth URL
        QCOMPARE(openBrowserSpy.count(), 1);
        QUrl authUrl = openBrowserSpy.at(0).at(0).toUrl();
        QString state = QUrlQuery(authUrl).queryItemValue("state");

        // Simulate callback with auth code
        QMap<QString, QString> response;
        response["code"] = "test_auth_code";
        response["state"] = state;
        o2->onVerificationReceived(response);

        // Verify failure
        QTRY_COMPARE(linkingFailedSpy.count(), 1);
        QVERIFY(!o2->linked());
        QVERIFY(o2->token().isEmpty());
    }

    void testUnlink() {
        // Set up linked state
        o2->setLinked(true);
        o2->setToken("test_token");
        o2->setRefreshToken("test_refresh");
        o2->setExpires(QDateTime::currentSecsSinceEpoch() + 3600);

        QSignalSpy linkingSucceededSpy(o2, &O2::linkingSucceeded);

        // Unlink
        o2->unlink();

        // Verify unlinked state
        QVERIFY(!o2->linked());
        QVERIFY(o2->token().isEmpty());
        QVERIFY(o2->refreshToken().isEmpty());
        QCOMPARE(o2->expires(), 0);
        QCOMPARE(linkingSucceededSpy.count(), 1);
    }

    void testExtraTokens() {
        // Setup the test with a successful auth code flow response
        QByteArray tokenResponse = R"({
            "access_token": "test_token",
            "refresh_token": "test_refresh",
            "expires_in": 3600,
            "extra_param1": "value1",
            "extra_param2": "value2"
        })";
        manager->setResponse(tokenResponse);

        o2->setGrantFlow(O2::GrantFlowAuthorizationCode);
        o2->link();

        // Simulate successful auth code callback
        QMap<QString, QString> response;
        response["code"] = "test_auth_code";
        response["state"] = QUrlQuery(QUrl(o2->requestUrl())).queryItemValue("state");
        o2->onVerificationReceived(response);

        // Verify extra tokens were captured
        QTRY_VERIFY(o2->linked());
        QVariantMap extraTokens = o2->extraTokens();
        QVERIFY(!extraTokens.isEmpty());
        QCOMPARE(extraTokens["extra_param1"].toString(), QString("value1"));
        QCOMPARE(extraTokens["extra_param2"].toString(), QString("value2"));
    }

private:
    MockManager* manager;
    MockStore* store;
    O2* o2;
};

QTEST_MAIN(TestO2)
#include "testo2.moc"
