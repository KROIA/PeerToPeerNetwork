#pragma once

#include "PeerToPeerNetwork_base.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>

namespace P2PN
{

    class P2PN_EXPORT PeerToPeerNetwork : public QObject
    {
        Q_OBJECT

    public:
        PeerToPeerNetwork(QObject* parent = nullptr);

        void startBroadcasting(quint16 broadcastPort, quint16 tcpListenPort);
        void stopBroadcasting();

        void startListening(quint16 listenPort);
        void stopListening();

    signals:
        void dataReceived(const QByteArray& data);
        void newPeerDiscovered(const QString& peerAddress);

    private slots:
        void onNewTcpConnection();
        void onReadyRead();
        void onDisconnected();
        void onBroadcastMessageReceived();
        void onBroadcastTimerTimeout();

    private:
        QTcpServer tcpServer;
        QList<QTcpSocket*> connectedPeers;
        QUdpSocket udpSocket;
        QTimer broadcastTimer;

        void broadcastToNetwork(const QString& message);
    };

}