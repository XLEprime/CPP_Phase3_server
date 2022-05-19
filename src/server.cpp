/**
 * @file server.cpp
 * @author Haolin Yang
 * @brief 服务器类的实现
 * @version 0.1
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <QNetworkDatagram>
#include <QJsonDocument>

#include "../include/server.h"

Server::Server(QObject *parent, quint16 port, UserManage *_usermanage) : QObject(parent), userManage(_usermanage), socket(this)
{
    socket.bind(QHostAddress::LocalHost, port);
    QObject::connect(&socket, &QUdpSocket::readyRead, this, &Server::messageHandler);
}

void Server::messageHandler()
{
    while (socket.hasPendingDatagrams())
    {
        QNetworkDatagram datagram = socket.receiveDatagram(); //数据报
        QJsonObject json = QJsonDocument::fromJson(datagram.data()).object();
        QByteArray res;
        if (!json.contains("type") || !json.contains("payload"))
            continue;
        int type = json["type"].toInt();
        QJsonObject payload = json["payload"].toObject();

        qDebug() << "收到报文，类型为" << type;

        switch (type)
        {
        case time:
            res = timeHandler();
            break;
        case addTime:
            res = addTimeHandler(payload);
            break;
        case userRegister:
            res = registerHandler(payload);
            break;
        case login:
            res = loginHandler(payload);
            break;
        case logout:
            res = logoutHandler(payload);
            break;
        case changePassword:
            res = changePasswordHandler(payload);
            break;
        case info:
            res = infoHandler(payload);
            break;
        case allUserInfo:
            res = allUserInfoHandler(payload);
            break;
        case addExpressman:
            res = addExpressmanHandler(payload);
            break;
        case deleteExpressman:
            res = deleteExpressmanHandler(payload);
            break;
        case assign:
            res = assignHandler(payload);
            break;
        case delivery:
            res = deliveryHandler(payload);
            break;
        case addBalance:
            res = addBalanceHandler(payload);
            break;
        case query:
            res = queryHandler(payload);
            break;
        case send:
            res = sendHandler(payload);
            break;
        case receive:
            res = receiveHandler(payload);
            break;
        case deleteItem:
            res = deleteItemHandler(payload);
            break;
        default:
            break;
        }

        qint64 status = socket.writeDatagram(res, datagram.senderAddress(), datagram.senderPort());
        if (status == -1)
            qCritical() << "UDP socket出错";
    }
}

QString Server::jwtEncoding(const QJsonObject &payload, const QByteArray &secret) const
{
    QJsonObject header;
    header.insert("alg", "HS256"); //签名算法 HS256
    header.insert("typ", "jwt");   //类型 jwt

    QByteArray header_encoded = QByteArray(QJsonDocument(header).toJson(QJsonDocument::Compact)).toBase64();
    QByteArray payload_encoded = QByteArray(QJsonDocument(payload).toJson(QJsonDocument::Compact)).toBase64();
    QByteArray sig = QMessageAuthenticationCode::hash(header_encoded + "." + payload_encoded, secret, QCryptographicHash::Sha256).toBase64();

    return QString(header_encoded + '.' + payload_encoded + '.' + sig);
}

bool Server::jwtVerify(const QString &jwt, const QByteArray &secret) const
{
    auto splited = jwt.split('.');
    auto header_encoded = splited[0].toUtf8();
    auto payload_encoded = splited[1].toUtf8();
    auto sig = splited[2].toUtf8();
    return sig == QMessageAuthenticationCode::hash(header_encoded + "." + payload_encoded, secret, QCryptographicHash::Sha256).toBase64();
}

QJsonObject Server::jwtGetPayload(const QString &jwt) const
{
    auto splited = jwt.split('.');
    auto payload = QByteArray::fromBase64(splited[1].toUtf8());
    return QJsonDocument::fromJson(payload).object();
}

void Server::constructRet(QJsonObject &ret) const
{
    ret.insert("status", false);
    ret.insert("payload", "字段有误");
}

void Server::constructRet(QJsonObject &ret, const QString &res) const
{
    if (res.isEmpty())
    {
        ret.insert("status", true);
    }
    else
    {
        ret.insert("status", false);
        ret.insert("payload", res);
    }
}

void Server::constructRet(QJsonObject &ret, const QString &res, const QJsonValue &result) const
{
    if (res.isEmpty())
    {
        ret.insert("status", true);
        ret.insert("payload", result);
    }
    else
    {
        ret.insert("status", false);
        ret.insert("payload", res);
    }
}

QByteArray Server::timeHandler() const
{
    QJsonObject ret, retTime;
    QString response = Time::getTime(retTime);
    constructRet(ret, response, retTime);
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::addTimeHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("days"))
        constructRet(ret);
    else
    {
        QString res = Time::addDays(payload["days"].toInt());
        constructRet(ret, res);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::registerHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("username") || !payload.contains("password") || !payload.contains("name") || !payload.contains("phonenumber") || !payload.contains("address"))
        constructRet(ret);
    else
    {
        QString res = userManage->registerUser(payload);
        constructRet(ret, res);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::loginHandler(const QJsonObject &payload) const
{
    QJsonObject ret, token;
    if (!payload.contains("username") || !payload.contains("password"))
        constructRet(ret);
    else
    {
        QString res = userManage->login(payload["username"].toString(), payload["password"].toString(), token);
        if (res.isEmpty())
        {
            ret.insert("status", true);
            ret.insert("payload", jwtEncoding(token, secret));
        }
        else
        {
            ret.insert("status", false);
            ret.insert("payload", res);
        }
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact)); // QJsonDocument::Compact使得结果紧凑
}

QByteArray Server::logoutHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret))
        constructRet(ret);
    else
    {
        QString res = userManage->logout(jwtGetPayload(payload["token"].toString()));
        constructRet(ret, res);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::changePasswordHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret) || !payload.contains("password"))
        constructRet(ret);
    else
    {
        QString res = userManage->changePassword(jwtGetPayload(payload["token"].toString()), payload["password"].toString());
        constructRet(ret, res);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::infoHandler(const QJsonObject &payload) const
{
    QJsonObject ret, result;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret))
        constructRet(ret);
    else
    {
        QString response = userManage->getUserInfo(jwtGetPayload(payload["token"].toString()), result);
        constructRet(ret, response, result);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::allUserInfoHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    QJsonArray result;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret))
        constructRet(ret);
    else
    {
        QString response = userManage->queryAllUserInfo(jwtGetPayload(payload["token"].toString()), result);
        constructRet(ret, response, result);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::addExpressmanHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret) || !payload.contains("username") || !payload.contains("password") || !payload.contains("name") || !payload.contains("phonenumber") || !payload.contains("address"))
        constructRet(ret);
    else
    {
        QJsonObject info(payload);
        info.remove("token");
        QString res = userManage->registerExpressman(jwtGetPayload(payload["token"].toString()), info);
        constructRet(ret, res);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::deleteExpressmanHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret) || !payload.contains("username"))
        constructRet(ret);
    else
    {
        QString response = userManage->deleteExpressman(jwtGetPayload(payload["token"].toString()), payload["username"].toString());
        constructRet(ret, response);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::assignHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret) || !payload.contains("expressman") || !payload.contains("itemId"))
        constructRet(ret);
    else
    {
        QJsonObject info(payload);
        info.remove("token");
        QString response = userManage->assignExpressman(jwtGetPayload(payload["token"].toString()), info);
        constructRet(ret, response);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::deliveryHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret) || !payload.contains("itemId"))
        constructRet(ret);
    else
    {
        QString response = userManage->deliveryItem(jwtGetPayload(payload["token"].toString()), payload["itemId"].toInt());
        constructRet(ret, response);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::addBalanceHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret) || !payload.contains("money"))
        constructRet(ret);
    else
    {
        auto response = userManage->addBalance(jwtGetPayload(payload["token"].toString()), payload["money"].toInt());
        constructRet(ret, response);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::queryHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret) || !payload.contains("type"))
        constructRet(ret);
    else
    {
        QJsonObject filter(payload);
        filter.remove("token");
        QJsonArray result;
        auto response = userManage->queryItem(jwtGetPayload(payload["token"].toString()), filter, result);
        constructRet(ret, response, result);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::sendHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    int cost;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret))
        constructRet(ret);
    else
    {
        QJsonObject info(payload);
        info.remove("token");
        QString response = userManage->sendItem(jwtGetPayload(payload["token"].toString()), info, cost);
        constructRet(ret, response, cost);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::receiveHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret) || !payload.contains("id"))
        constructRet(ret);
    else
    {
        QString response = userManage->receiveItem(jwtGetPayload(payload["token"].toString()), payload["id"].toInt());
        constructRet(ret, response);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}

QByteArray Server::deleteItemHandler(const QJsonObject &payload) const
{
    QJsonObject ret;
    if (!payload.contains("token") || !jwtVerify(payload["token"].toString(), secret) || !payload.contains("id"))
        constructRet(ret);
    else
    {
        QString response = userManage->deleteItem(jwtGetPayload(payload["token"].toString()), payload["id"].toInt());
        constructRet(ret, response);
    }
    return QByteArray(QJsonDocument(ret).toJson(QJsonDocument::Compact));
}
