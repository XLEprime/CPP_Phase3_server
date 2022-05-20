/**
 * @file main.cpp
 * @author Haolin Yang
 * @brief 主程序
 * @version 0.1
 * @date 2022-04-10
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <QtCore>
#include <QTextStream>
#include "include/user.h"
#include "include/server.h"

#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_RESET "\x1b[0m"

//Qt自带的输出详细日志 可以删除
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type)
    {
    case QtDebugMsg:
        fprintf(stdout, ANSI_COLOR_BLUE "[Debug]" ANSI_COLOR_CYAN "(%s:%u)" ANSI_COLOR_RESET " %s\n", file, context.line, localMsg.constData());
        break;
    case QtInfoMsg:
        fprintf(stdout, ANSI_COLOR_YELLOW "[Info]" ANSI_COLOR_CYAN "(%s:%u)" ANSI_COLOR_RESET " %s\n", file, context.line, localMsg.constData());
        break;
    case QtWarningMsg:
        fprintf(stdout, ANSI_COLOR_MAGENTA "[Warning]" ANSI_COLOR_CYAN "(%s:%u)" ANSI_COLOR_RESET " %s\n", file, context.line, localMsg.constData());
        break;
    case QtCriticalMsg:
        fprintf(stdout, ANSI_COLOR_RED "[Critical]" ANSI_COLOR_CYAN "(%s:%u)" ANSI_COLOR_RESET " %s\n", file, context.line, localMsg.constData());
        break;
    case QtFatalMsg:
        fprintf(stdout, ANSI_COLOR_RED "[Fatal]" ANSI_COLOR_CYAN "(%s:%u)" ANSI_COLOR_RESET " %s\n", file, context.line, localMsg.constData());
        break;
    }
}

int main(int argc, char * argv[])
{
    qInstallMessageHandler(messageHandler); // Qt自带的输出详细日志
    QCoreApplication a(argc, argv);
    Database database("defaultConnection", "../data/users.txt");
    ItemManage itemManage(&database);
    UserManage userManage(&database, &itemManage);
    Server server(&a, 8946, &userManage);
    Time::init();

    return a.exec();
}