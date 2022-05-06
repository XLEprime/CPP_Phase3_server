/**
 * @file server.h
 * @author Haolin Yang
 * @brief 服务器类的声明
 * @version 0.1
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QJsonValue>

#include "user.h"

//服务器类
class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent, quint16 _port, UserManage *_usermanage);

private:
    enum RequestType
    {
        time,             //查询系统时间
        addTime,          //加快系统时间
        userRegister,     //注册
        login,            //登录
        logout,           //登出
        changePassword,   //修改密码
        info,             //查看个人信息
        allUserInfo,      //查看所有用户信息
        addExpressman,    //添加快递员
        deleteExpressman, //删除快递员
        assign,           //为一个快递指定快递员
        delivery,         //运送一个快递
        addBalance,       //充值
        query, //查询符合条件的快递
        send,   //发送快递
        receive //接收快递
    };

    /**
     * @brief 接收message
     */
    void messageHandler();

    /**
     * @brief 将凭据打包成JWT token字符串
     * @param payload 凭据
     * @param secret 密钥
     * @return QString JWT token字符串
     */
    QString jwtEncoding(const QJsonObject &payload, const QByteArray &secret) const;

    /**
     * @brief 验证JWT token的指纹
     * @param jwt JWT token
     * @param secret 密钥
     * @return bool 如果验证成功，返回true
     */
    bool jwtVerify(const QString &jwt, const QByteArray &secret) const;

    /**
     * @brief 从JWT token字符串中提取凭据
     * @param jwt JWT token
     * @return 凭据
     */
    QJsonObject jwtGetPayload(const QString &jwt) const;

    /**
     * @brief 构造仅包含字段错误信息的回复
     * @param ret 回复
     */
    void constructRet(QJsonObject &ret) const;

    /**
     * @brief 构造回复
     * @param ret 回复
     * @param res 用户管理类的处理结果
     * @note 如果res为空，则回复状态正常，否则回复状态异常
     */
    void constructRet(QJsonObject &ret, const QString &res) const;

    /**
     * @brief 构造回复
     * @param ret 回复
     * @param res 用户管理类的处理结果
     * @param result 用户管理类的回复信息
     * @note 如果res为空，则回复状态正常，并且把result附加到回复中，否则回复状态异常
     */
    void constructRet(QJsonObject &ret, const QString &res, const QJsonValue &result) const;

    UserManage *userManage;
    QUdpSocket socket;
    const QByteArray secret = "JWTTokenSecret"; // JWT token 加密密钥

    /**
     * @brief 处理查询系统时间
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray timeHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理加快系统时间
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray addTimeHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理注册
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray registerHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理登录
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray loginHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理登出
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray logoutHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理修改密码
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray changePasswordHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理查看个人信息
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray infoHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理查看所有个人信息
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray allUserInfoHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理添加快递员
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray addExpressmanHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理删除快递员
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray deleteExpressmanHandler(const QJsonObject &payload) const;

    /**
     * @brief 为一个快递指定快递员
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray assignHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理运送一个快递
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray deliveryHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理充值
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray addBalanceHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理查询符合条件的快递
     * @param payload 有效载荷
     * @return QByteArray
     * @note 包括各种类型的快递查询：管理员：所有快递 用户：寄件 收件 快递员：所属的快递
     */
    QByteArray queryHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理发送快递
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray sendHandler(const QJsonObject &payload) const;

    /**
     * @brief 处理接收快递
     * @param payload 有效载荷
     * @return QByteArray
     */
    QByteArray receiveHandler(const QJsonObject &payload) const;
};

#endif