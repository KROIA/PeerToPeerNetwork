// peertopeernetwork.cpp
#include "peertopeernetwork.h"
#include <qhostinfo.h>
#include <QDebug>

namespace P2PN
{
    PeerToPeerNetwork::PeerToPeerNetwork(QObject* parent)
        : QObject(parent)
    {
        connect(&tcpServer, &QTcpServer::newConnection, this, &PeerToPeerNetwork::onNewTcpConnection);
        connect(&udpSocket, &QUdpSocket::readyRead, this, &PeerToPeerNetwork::onBroadcastMessageReceived);
        connect(&broadcastTimer, &QTimer::timeout, this, &PeerToPeerNetwork::onBroadcastTimerTimeout);
    }

    void PeerToPeerNetwork::startBroadcasting(quint16 broadcastPort, quint16 tcpListenPort)
    {
        if (!udpSocket.bind(QHostAddress::Any, broadcastPort))
        {
            qDebug() << __PRETTY_FUNCTION__ << " Can't bind to port: " << broadcastPort;
        }
        if (!tcpServer.listen(QHostAddress::Any, tcpListenPort))
        {
            qDebug() << __PRETTY_FUNCTION__ << " Can't start listening on port: " << tcpListenPort;
        }
        broadcastTimer.start(5000); // Broadcast every 5 seconds (adjust as needed)
    }

    void PeerToPeerNetwork::stopBroadcasting()
    {
        broadcastTimer.stop();
        udpSocket.close();
        tcpServer.close();
    }

    void PeerToPeerNetwork::startListening(quint16 listenPort)
    {
        tcpServer.listen(QHostAddress::Any, listenPort);
    }

    void PeerToPeerNetwork::stopListening()
    {
        tcpServer.close();
    }

    void PeerToPeerNetwork::onNewTcpConnection()
    {
        while (tcpServer.hasPendingConnections()) {
            QTcpSocket* peerSocket = tcpServer.nextPendingConnection();
            connectedPeers.append(peerSocket);

            connect(peerSocket, &QTcpSocket::readyRead, this, &PeerToPeerNetwork::onReadyRead);
            connect(peerSocket, &QTcpSocket::disconnected, this, &PeerToPeerNetwork::onDisconnected);

            qDebug() << __PRETTY_FUNCTION__ << " " << peerSocket->peerAddress().toString();
            emit newPeerDiscovered(peerSocket->peerAddress().toString());
        }
    }

    void PeerToPeerNetwork::onReadyRead()
    {
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        if (socket) {
            QByteArray data = socket->readAll();
            qDebug() << __PRETTY_FUNCTION__ << " " << data;
            emit dataReceived(data);
        }
    }

    void PeerToPeerNetwork::onDisconnected()
    {
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        if (socket) {
            qDebug() << __PRETTY_FUNCTION__ << " " << socket->peerAddress().toString();
            connectedPeers.removeOne(socket);
            socket->deleteLater();
        }
    }

    void PeerToPeerNetwork::onBroadcastMessageReceived()
    {
        while (udpSocket.hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(udpSocket.pendingDatagramSize());
            udpSocket.readDatagram(datagram.data(), datagram.size());

            QString broadcastMessage = QString(datagram);
            QHostAddress address = udpSocket.peerAddress();

            QString broadcasterAddress = address.toString();
            qDebug() << "Broadcast message: " << broadcastMessage;
            // Avoid connecting to self
            if (broadcastMessage != broadcasterAddress &&
                address != QHostAddress::Null) {
                qDebug() << __PRETTY_FUNCTION__ << " " << broadcastMessage<< " socket: "<< broadcasterAddress;
                // Attempt to connect to the broadcaster
                QTcpSocket* peerSocket = new QTcpSocket(this);
                peerSocket->connectToHost(broadcasterAddress, 45455); // Use the desired TCP port
                connect(peerSocket, &QTcpSocket::connected, this, &PeerToPeerNetwork::onNewTcpConnection);
            }
        }
    }

    void PeerToPeerNetwork::onBroadcastTimerTimeout()
    {
        broadcastToNetwork(QHostInfo::localHostName());
    }

    void PeerToPeerNetwork::broadcastToNetwork(const QString& message)
    {
        qDebug() << __PRETTY_FUNCTION__ << " " << message;
        udpSocket.writeDatagram(message.toUtf8(), QHostAddress::Broadcast, 45454); // Use the desired UDP port
    }

}