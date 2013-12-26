#ifndef QTNETWORKREPLYEX_H
#define QTNETWORKREPLYEX_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QScopedPointer>

class QtNetworkReplyExPrivate;

class QtNetworkReplyEx : public QObject
{
    Q_OBJECT
    Q_ENUMS(RedirectError)

public:
    enum RedirectError {
        NoRedirectError,        // No error
        TooManyRedirects,       // Redirects limit exceeded
        UnsecureRedirect,       // secure (https) to unsecure (http) redirect
        InvalidRedirectProto    // Redirect from http/https to some other protocol
    };

    explicit QtNetworkReplyEx(QNetworkReply *reply, QObject *parent = 0);
    ~QtNetworkReplyEx();

    bool isRunning() const;

    bool isFinished() const;

    QUrl url() const;

    int maxRedirects() const;

    void setMaxRedirect(int maxRedirects);

    bool allowUnsecureRedirect() const;

    void setAllowUnsecureRedirect(bool allowUnsecureRedirect);

    RedirectError redirectError() const;

Q_SIGNALS:
    void finished(QNetworkReply *reply);
    void redirected(const QUrl &url);
    void error(QNetworkReply::NetworkError code);
    void redirectError(QtNetworkReplyEx::RedirectError error);

public Q_SLOTS:
    void abort();

private:
    QScopedPointer<QtNetworkReplyExPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtNetworkReplyEx)
};

#endif // QTNETWORKREPLYEX_H
