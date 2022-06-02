#include "ledserver.h"
#include "config.h"
#include <QDebug>

LedServer::LedServer(quint16 port, QObject *parent) : QObject(parent), m_port(port)
{
    m_gpio = new Gpio(this);
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &LedServer::myNewConnection);
}

void LedServer::start()
{
    m_server->listen(QHostAddress::Any, PORT);
    qDebug() << "Server lauscht auf port" << PORT;
}

// Client hat Verbindung zum Server aufgebaut
void LedServer::myNewConnection()
{
    m_socket = m_server->nextPendingConnection();
    connect(m_socket, &QTcpSocket::readyRead, this, &LedServer::myServerRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &LedServer::myClientDisconnect);
    qDebug() << "Neue Client-Verbindung";
    m_socket->write("Bitte Zahl von 0 bis 15 eingeben: ");
}

// Client hat Verbindung zum Server getrennt
void LedServer::myClientDisconnect()
{
    m_socket->close();
    qDebug() << "Client hat Verbindung getrennt\n";
    m_gpio->set(0);
}

// Client hat eine Zahl (0...15) zum Server gesendet
void LedServer::myServerRead()
{
    QString message = m_socket->readAll();
    int message_int = message.toInt();
    qDebug() << "number: " << message_int;
    if(message_int >= 0 && message_int <= 15) {
        m_socket->write("OK\n\n");
        m_gpio->set(message_int);
    }
    else {
        m_socket->write("NOT OK\n");
        m_socket->write("Only a number from 0 to 15 is valid! Try again!\n\n");
        m_gpio->set(0);
    }
    m_socket->write("Bitte Zahl von 0 bis 15 eingeben: ");
}
