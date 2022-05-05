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
        QByteArray response;
        if (!json.contains("type") || !json.contains("payload"))
            continue;
        int type = json["type"].toInt();

        switch (type)
        {
        case time:
            /* code */
            break;

        case addTime:
            /* code */
            break;
        case userRegister:
            /* code */
            break;
        case login:
            /* code */
            break;
        case logout:
            /* code */
            break;
        case changePassword:
            /* code */
            break;
        case info:
            /* code */
            break;
        case allUserInfo:
            /* code */
            break;
        case addExpressman:
            /* code */
            break;
        case deleteExpressman:
            /* code */
            break;
        case assign:
            /* code */
            break;
        case delivery:
            /* code */
            break;
        case addBalance:
            /* code */
            break;
        case queryallitem:
            /* code */
            break;
        case query:
            /* code */
            break;
        case queryExpress:
            /* code */
            break;
        case querysrc:
            /* code */
            break;
        case send:
            /* code */
            break;
        case receive:
            /* code */
            break;
        default:
            break;
        }

        qint64 status = socket.writeDatagram(response, datagram.senderAddress(), datagram.senderPort());
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

void Server::constructRet(QJsonObject &ret, const QString &response) const
{
    if (response.isEmpty())
    {
        ret.insert("status", true);
    }
    else
    {
        ret.insert("status", false);
        ret.insert("payload", response);
    }
}

void Server::constructRet(QJsonObject &ret, const QString &response, const QJsonValue &result) const
{
    if (response.isEmpty())
    {
        ret.insert("status", true);
        ret.insert("payload", result);
    }
    else
    {
        ret.insert("status", false);
        ret.insert("payload", response);
    }
}