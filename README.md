qt-network-reply-ex
===================

A wrapper class for automatically handling HTTP redirects.

Use this class to wrap your **<code>QNetworkReply</code>** object that you would get on initiating a request. Once the final url (after redirects) is reached, it emits the **<code>finished(QNetworkReply *)</code>**.
The parameter of the signal is the final reply object representing the last redirect.

## Signals
1. <code>void finished(QNetworkReply *reply)</code>: Emitted on receiving the final (non-redirect) response from the server
2. <code>void redirected(const QUrl &url)</code>: Emitted on every redirect
3. <code>void error(QNetworkReply::NetworkError code)</code>: Relayed **<code>QNetworkReply</code>** error signal
4. <code>void redirectError(QtNetworkReplyEx::RedirectError error)</code>: Emitted on encountering a redirect related error

## Redirect Errors

The following errors can occur on redirects:

1. <code>QtNetworkReplyEx::TooManyRedirects</code>: Set when total number of redirects exceeds the maximum allowed redirects (<code>QtNetworkReplyEx::maxRedirects()</code>). Default is set to 10.
2. <code>QtNetworkReplyEx::UnsecureRedirect</code>: Set when a redirect occurs from a secure (**https**) to a unsecure (**http**) connection and <code>QtNetworkReplyEx::allowUnsecureRedirect()</code> is <code>false</code>.
3. <code>QtNetworkReplyEx::InvalidRedirectProto</code>: If rediect url's protocol is anything other than **http** ot **https**.

## Usage

```C++
    ...
    QtNetworkReplyEx redirector(manager->get(networkRequest), this);

    connect(&redirector, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onFinished(QNetworkReply*)));
    connect(&redirector, SIGNAL(redirected(QUrl)),
            this, SLOT(onRedirected(QUrl)));
    connect(&redirector, SIGNAL(redirectError(QtNetworkReplyEx::RedirectError)),
            this, SLOT(onRedirectError(QtNetworkReplyEx::RedirectError)));
    connect(&redirector, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(onError(QNetworkReply::NetworkError)));
    ...
    ...

void onFinished(QNetworkReply *reply)
{
...
    qDebug() << "Original url is" << redirector.url();
    qDebug() << "Final url is" << reply->url();
...
}
```
