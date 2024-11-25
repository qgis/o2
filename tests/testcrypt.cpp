#include <QtTest>
#include "o0simplecrypt.h"

class TestSimpleCrypt : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        crypt = new O0SimpleCrypt(Q_UINT64_C(0x0c2ad4a4acb9f023));
    }

    void cleanupTestCase() {
        delete crypt;
    }

    void testStringEncryption() {
        QString original = "Test string for encryption";
        QString encrypted = crypt->encryptToString(original);
        QString decrypted = crypt->decryptToString(encrypted);

        QVERIFY(encrypted != original);
        QCOMPARE(decrypted, original);
    }

    void testCompressionModes_data() {
        QTest::addColumn<int>("compressionMode");
        QTest::addColumn<QString>("testData");

        QTest::newRow("always") << static_cast<int>(O0SimpleCrypt::CompressionAlways)
                                << QString("A").repeated(1000);
        QTest::newRow("never") << static_cast<int>(O0SimpleCrypt::CompressionNever)
                               << QString("A").repeated(1000);
    }

    void testCompressionModes() {
        QFETCH(int, compressionMode);
        QFETCH(QString, testData);

        crypt->setCompressionMode(static_cast<O0SimpleCrypt::CompressionMode>(compressionMode));
        QString encrypted = crypt->encryptToString(testData);
        QString decrypted = crypt->decryptToString(encrypted);
        QCOMPARE(decrypted, testData);
    }

    void testIntegrityProtection() {
        QString testData = "Test data for integrity check";

        crypt->setIntegrityProtectionMode(O0SimpleCrypt::ProtectionChecksum);
        QString encrypted = crypt->encryptToString(testData);

        // Test tampering detection
        QString tampered = encrypted;
        tampered[tampered.length()/2] = QChar( tampered[tampered.length()/2].toLatin1() + 1 );
        QVERIFY(crypt->decryptToString(tampered).isEmpty());
        QCOMPARE(crypt->lastError(), O0SimpleCrypt::ErrorIntegrityFailed);
    }

private:
    O0SimpleCrypt* crypt;
};

QTEST_MAIN(TestSimpleCrypt)
#include "testcrypt.moc"
