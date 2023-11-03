#pragma once

#include "PeerToPeerNetwork_base.h"
#include "PeerGrabber/PeerGrabber.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QDateTime>
#include <vector>

namespace P2PN
{

    class P2PN_EXPORT PeerToPeerNetwork : public QObject
    {
        Q_OBJECT

    public:
        static std::string getThisHostIP();
        static std::string getThisHostName();
        static std::string getThisUserName();
        static PeerInfo getThisPeerInfoInitial();

        static void startProfiler();
        static void stopProfiler(const std::string profileFilePath);


        PeerToPeerNetwork(const PeerInfo thisPeer, QObject* parent = nullptr);
        ~PeerToPeerNetwork();

        const PeerInfo& getPeerInfo() const;

        void addPeerGrabber(PeerGrabber* grabber);
        void removePeerGrabber(PeerGrabber* grabber);
        void clearPeerGrabber();
        const std::vector<PeerGrabber*>& getPeerGrabber() const;
        bool containsPeerGrabber(PeerGrabber* grabber) const;


        bool startListening();
        void startListening(quint16 listenPort);
        void stopListening();

        void checkNewPeers();

        std::vector<PeerInfo> getPeers() const;
        void printPeers() const;

    
        bool sendMessage(const PeerInfo& target, const std::string& message);
        bool sendMessage(const PeerInfo& target, const QJsonObject& data);
        bool sendMessage(const PeerInfo& target, const QJsonArray& dataArray);

        bool broadcastMessage(const PeerInfo& target, const std::string& message);
        bool broadcastMessage(const PeerInfo& target, const QJsonObject& data);
        bool broadcastMessage(const PeerInfo& target, const QJsonArray& dataArray);

        void connectToPeer(const PeerInfo& info);
        void connectToPeer(const std::vector<PeerInfo> &peers);
        void disconnectFromPeer(const PeerInfo& info);
        bool hasConnection(const PeerInfo& info);
        bool hasConnection(const std::string& IP);

    public slots:
        void onNewPeersAvailable();

    signals:
        void dataReceived(const PeerInfo &peer, const std::string& data);
        void dataReceived(const PeerInfo &peer, const QJsonObject& obj);
        void dataReceived(const PeerInfo &peer, const QJsonArray& objArray);
        void peerConnected(const PeerInfo& peer);
        void peerDisconnected(const PeerInfo& peer);

    private slots:

        void onConnectionSuccess();

        void onNewTcpConnection();
        void onReadyRead();
        void onDisconnected();
        void onSocketError(QAbstractSocket::SocketError e);

    private:
        struct Peer
        {
            QDateTime connectionStartTime;
            PeerInfo info;
            QTcpSocket* socket;
            bool pendingInfo;
            bool selfOpenedConnection;
            bool isValid;

            QJsonObject toJson() const;
        };

        Peer& getConnectedPeer(const PeerInfo& peerInfo, bool& valid);
        Peer& getConnectingPeer(const PeerInfo& peerInfo, bool& valid);

        void onReadyReadPeer(Peer& peer);
        void onInternalMessageReceived(Peer& peer, const QJsonObject &internalData);
        void onInternalMessageReceived(Peer& peer, const QJsonArray &internalDataArray);
        void onInternalMessageReceived(Peer& peer, const QJsonValue &internalData);
        void onPingReceived(Peer& peer);
        void onPongReceived(Peer& peer);


        bool sendMessage(const Peer& target, const std::string& message);
        bool sendMessage(const Peer& target, const QJsonObject& data);
        bool sendMessage(const Peer& target, const QJsonArray& dataArray);
        bool sendMessage(const Peer& target, const QJsonObject& data, const QString &p2pcommand);
        bool sendMessage(const Peer& target, const QJsonArray& dataArray, const QString &p2pcommand);
        bool sendMessage(const Peer& target, const std::string& message, const QString &p2pcommand);
        bool sendPing(const Peer& target);
        bool sendPong(const Peer& target);

        bool transmittString(const Peer& target, const QByteArray& data);

        int disconnectMultipleConnections(Peer peer);

        PeerInfo m_thisPeerInfo;
        QTcpServer tcpServer;
        std::vector<Peer> m_connectedPeers;
        std::vector<Peer> m_connectingPeers;
        std::vector<Peer> m_disconnectingPeers;


        std::vector<PeerGrabber*> m_peerGrabbers;


        static const QString s_message_senderKey;
        static const QString s_message_dataKey;
        static const QString s_message_commandKey;

        static const QString s_message_command_default;
        static const QString s_message_command_internal;
        static const QString s_message_command_ping;
        static const QString s_message_command_pong;
    };

}