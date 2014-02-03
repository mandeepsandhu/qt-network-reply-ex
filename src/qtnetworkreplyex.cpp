#include "qtnetworkreplyex.h"
#include "qtnetworkreplyex_p.h"

#define MAX_REDIRECTS 10

QtNetworkReplyExPrivate::QtNetworkReplyExPrivate(QNetworkReply *reply,
                                                 QObject *parent) :
    reply(reply),
    maxRedirects(MAX_REDIRECTS),
    redirectCount(0),
    running(true),
    allowUnsecureRedirect(false),
    originalUrl(reply->url()),
    redirectError(QtNetworkReplyEx::NoRedirectError),
    q_ptr(static_cast<QtNetworkReplyEx *>(parent))
{
    connectToNetworkReply();
}

QtNetworkReplyExPrivate::~QtNetworkReplyExPrivate() {}

bool QtNetworkReplyExPrivate::connectToNetworkReply()
{
    bool result1 = connect(reply, SIGNAL(finished()),
                           this, SLOT(onFinished()));

    bool result2 = connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                           q_ptr, SIGNAL(error(QNetworkReply::NetworkError)));

    return (result1 && result2);
}

void QtNetworkReplyExPrivate::abort()
{
    reply->abort();
    running = false;
    redirectCount = 0;
    redirectError = QtNetworkReplyEx::NoRedirectError;
}

bool QtNetworkReplyExPrivate::isRedirected(QNetworkReply *reply) const
{
    QVariant redirectAttribute =
            reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    return (redirectAttribute.isValid() && !redirectAttribute.toUrl().isEmpty());
}

QUrl QtNetworkReplyExPrivate::redirectUrl(QNetworkReply *reply,
                                          const QUrl &baseUrl) const
{
    QVariant redirectAttribute =
            reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl redirectUrl = redirectAttribute.toUrl();
    if (redirectUrl.isRelative()) {
        return baseUrl.resolved(redirectUrl);
    }
    return redirectUrl;
}

void QtNetworkReplyExPrivate::onFinished()
{
    Q_Q(QtNetworkReplyEx);

    if (!isRedirected(reply)) {
        running = false;
        emit q->finished(reply);
        return;
    }

    reply->deleteLater();

    if (redirectCount == maxRedirects) {
        running = false;
        redirectError = QtNetworkReplyEx::TooManyRedirects;
        emit q->redirectError(redirectError);
        return;
    }

    QUrl previousUrl = reply->request().url();
    QUrl redUrl = redirectUrl(reply, previousUrl);

    if ((redUrl.scheme() == "http")) {
        if ((previousUrl.scheme() == "https") &&
            !allowUnsecureRedirect)
        {
            running = false;
            redirectError = QtNetworkReplyEx::UnsecureRedirect;
            emit q->redirectError(redirectError);
            return;
        }
    } else if (redUrl.scheme() == "https") {
        // Ok
    } else {
        running = false;
        redirectError = QtNetworkReplyEx::InvalidRedirectProto;
        emit q->redirectError(redirectError);
        return;
    }

    redirectCount++;

    QNetworkAccessManager *mgr = reply->manager();
    QNetworkAccessManager::Operation op = reply->operation();

    // Construct the new request using the previous request
    QNetworkRequest req(reply->request());
    req.setUrl(redUrl);
    // We are always redirecting using a GET request, irrespective of the return
    // HTTP status (301, 303, etc). This behaviour can be made modifiable by a
    // user, if thwe user wants to strictly follow the RFC.
    switch (op) {
    case QNetworkAccessManager::HeadOperation:
        reply = mgr->head(req);
        break;
    case QNetworkAccessManager::GetOperation:
    case QNetworkAccessManager::PostOperation:
    case QNetworkAccessManager::PutOperation:
        reply = mgr->get(req);
        break;
    default:
        redirectError = QtNetworkReplyEx::UnknownRedirectError;
        emit q->redirectError(redirectError);
        return;
    }
    connectToNetworkReply();

    emit q->redirected(redUrl);
}

QtNetworkReplyEx::QtNetworkReplyEx(QNetworkReply *reply, QObject *parent) :
    QObject(parent),
    d_ptr(new QtNetworkReplyExPrivate(reply, this))
{
}

QtNetworkReplyEx::~QtNetworkReplyEx()
{
}

bool QtNetworkReplyEx::isRunning() const
{
    Q_D(const QtNetworkReplyEx);
    return d->running;
}

bool QtNetworkReplyEx::isFinished() const
{
    Q_D(const QtNetworkReplyEx);
    return !d->running;
}

void QtNetworkReplyEx::abort()
{
    Q_D(QtNetworkReplyEx);
    return d->abort();
}

QUrl QtNetworkReplyEx::url() const
{
    Q_D(const QtNetworkReplyEx);
    return d->originalUrl;
}

int QtNetworkReplyEx::maxRedirects() const
{
    Q_D(const QtNetworkReplyEx);
    return d->maxRedirects;
}

void QtNetworkReplyEx::setMaxRedirect(int maxRedirects)
{
    Q_D(QtNetworkReplyEx);
    d->maxRedirects = maxRedirects;
}

bool QtNetworkReplyEx::allowUnsecureRedirect() const
{
    Q_D(const QtNetworkReplyEx);
    return d->allowUnsecureRedirect;
}

void QtNetworkReplyEx::setAllowUnsecureRedirect(bool allowUnsecureRedirect)
{
    Q_D(QtNetworkReplyEx);
    d->allowUnsecureRedirect = allowUnsecureRedirect;
}

QtNetworkReplyEx::RedirectError QtNetworkReplyEx::redirectError() const
{
    Q_D(const QtNetworkReplyEx);
    return d->redirectError;
}
