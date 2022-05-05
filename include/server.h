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
        time,             //系统时间
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
        queryallitem,     //查询所有快递
        query,            //查询所有符合条件的快递
        queryExpress,     //查询当前快递员所属所有符合条件的快递
        querysrc,         //查询当前用户发出的的符合条件的快递
        send,             //发送快递
        receive           //接收快递
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
     * @param response 用户管理类的处理结果
     * @note 如果response为空，则回复状态正常，否则回复状态异常
     */
    void constructRet(QJsonObject &ret, const QString &response) const;

        /**
     * @brief 构造回复
     * @param ret 回复
     * @param response 用户管理类的处理结果
     * @param result 用户管理类的回复信息
     * @note 如果response为空，则回复状态正常，并且把result附加到回复中，否则回复状态异常
     */
    void constructRet(QJsonObject &ret, const QString &response, const QJsonValue &result) const;

    UserManage *userManage;
    QUdpSocket socket;
    const QByteArray secret = "JWTTokenSecret"; // JWT token 加密密钥
};

#endif