#ifndef QTNETWORKREPLYEX_P_H
#define QTNETWORKREPLYEX_P_H

#include <QObject>
#include <QUrl>

#include "qtnetworkreplyex.h"

class QtNetworkReplyExPrivate : public QObject
{
    Q_OBJECT

public:
    explicit QtNetworkReplyExPrivate(QNetworkReply *reply, QObject* parent);
    virtual ~QtNetworkReplyExPrivate();

    bool isRedirected(QNetworkReply *reply) const;
    QUrl redirectUrl(QNetworkReply *reply, const QUrl &baseUrl) const;
    void abort();

public Q_SLOTS:
    void onFinished();

private:
    bool connectToNetworkReply();

    QNetworkReply *reply;
    int maxRedirects;
    int redirectCount;
    bool running, allowUnsecureRedirect;
    QUrl originalUrl;
    QtNetworkReplyEx::RedirectError redirectError;
    QtNetworkReplyEx * const q_ptr;
    Q_DECLARE_PUBLIC(QtNetworkReplyEx)
};

#endif // QTNETWORKREPLYEX_P_H
