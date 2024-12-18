#ifndef TWITTERAPI_H
#define TWITTERAPI_H

#include <QAbstractListModel>
#include <QObject>
#include <QNetworkAccessManager>

#include "o1twitter.h"

#include "tweetmodel.h"

/// Mini Twitter API
class TwitterApi: public QObject {
    Q_OBJECT

public:
    /// List of tweets
    Q_PROPERTY(TweetModel *tweetModel READ tweetModel NOTIFY tweetModelChanged)
    TweetModel *tweetModel() ;

    /// OAuth authenticator
    Q_PROPERTY(O1Twitter *authenticator READ authenticator WRITE setAuthenticator NOTIFY authenticatorChanged)
    O1Twitter *authenticator() const;
    void setAuthenticator(O1Twitter *v) ;

    explicit TwitterApi(QObject *parent = nullptr);
    virtual ~TwitterApi();

public slots:
    Q_INVOKABLE virtual void requestTweets();

signals:
    void tweetModelChanged();
    void authenticatorChanged();

protected:
    O1Twitter *authenticator_;
    TweetModel *tweetModel_;
    QNetworkAccessManager *manager_;

protected slots:
    void tweetsReceived();
    void requestFailed(QNetworkReply::NetworkError error);
};

#endif // TWITTERAPI_H
