#include <QtTest>
#include "o0settingsstore.h"

class TestStore : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        store = new O0SettingsStore("testKey");
    }

    void cleanupTestCase() {
        delete store;
    }

    void testBasicStorage() {
        QString testKey = "testKey";
        QString testValue = "testValue";
        store->setValue(testKey, testValue);
        QCOMPARE(store->value(testKey), testValue);
    }

    void testGroupKey() {
        QString groupKey = "testGroup";
        store->setGroupKey(groupKey);
        QCOMPARE(store->groupKey(), groupKey);

        // Test storage with group key
        QString testKey = "testKey";
        QString testValue = "testValue";
        store->setValue(testKey, testValue);
        QCOMPARE(store->value(testKey), testValue);
    }

    void testDefaultValues() {
        QString defaultValue = "default";
        QCOMPARE(store->value("nonexistent", defaultValue), defaultValue);
    }

private:
    O0SettingsStore* store;
};

QTEST_MAIN(TestStore)
#include "teststore.moc"
