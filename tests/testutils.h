#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <QtTest>
#include "o0abstractstore.h"

// Mock store for testing
class MockStore : public O0AbstractStore {
    Q_OBJECT
public:
    explicit MockStore(QObject *parent = nullptr) : O0AbstractStore(parent) {}

    QString value(const QString &key, const QString &defaultValue = QString()) override {
        return values.value(key, defaultValue);
    }

    void setValue(const QString &key, const QString &value) override {
        values[key] = value;
    }

    // Public access to stored values for verification
    QMap<QString, QString> values;
};

class MockReply : public QNetworkReply {
    Q_OBJECT
public:
    explicit MockReply(const QByteArray& data, QObject* parent = nullptr) : QNetworkReply(parent), data_(data) {
        setOpenMode(QIODevice::ReadOnly);
    }

    void abort() override {}
    qint64 readData(char* data, qint64 maxSize) override {
        qint64 size = qMin(maxSize, qint64(data_.size() - offset_));
        if (size <= 0) return 0;
        memcpy(data, data_.constData() + offset_, size);
        offset_ += size;
        return size;
    }

    qint64 writeData(const char*, qint64) override { return 0; }

    void setError(QNetworkReply::NetworkError error, const QString& errorString) {
        QNetworkReply::setError(error, errorString);
        emit errorOccurred(error);
        emit finished();
    }

    void complete() {
        setFinished(true);
        emit finished();
    }

private:
    QByteArray data_;
    qint64 offset_ = 0;
};

class MockManager : public QNetworkAccessManager {
    Q_OBJECT
public:
    explicit MockManager(QObject* parent = nullptr) : QNetworkAccessManager(parent) {}

    QNetworkReply* createRequest(Operation op, const QNetworkRequest& request,
                                 QIODevice* outgoingData = nullptr) override {
        lastOperation = op;
        lastRequest = request;
        if (outgoingData) {
            lastData = outgoingData->readAll();
        }

        MockReply* reply = new MockReply(responseData, this);
        if (shouldFail) {
            QTimer::singleShot(0, reply, [reply]() {
                reply->setError(QNetworkReply::AuthenticationRequiredError, "Authentication failed");
            });
        } else {
            QTimer::singleShot(0, reply, &MockReply::complete);
        }
        return reply;
    }

    // Test control
    void setResponse(const QByteArray& data) { responseData = data; }
    void setShouldFail(bool fail) { shouldFail = fail; }

    // Test verification
    Operation lastOperation;
    QNetworkRequest lastRequest;
    QByteArray lastData;
    QByteArray responseData;
    bool shouldFail = false;
};
#endif // TESTUTILS_H
