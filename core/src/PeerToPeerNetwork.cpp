// peertopeernetwork.cpp
#include <iostream>
#include <Windows.h>
#include "peertopeernetwork.h"
#include <qhostinfo.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkInterface>

namespace P2PN
{
    const QString PeerToPeerNetwork::s_message_senderKey            = "sender";
    const QString PeerToPeerNetwork::s_message_dataKey              = "data";
    const QString PeerToPeerNetwork::s_message_commandKey           = "P2PNC";

    const QString PeerToPeerNetwork::s_message_command_default      = "P2PN_default";
    const QString PeerToPeerNetwork::s_message_command_internal     = "P2PN_internal";
    const QString PeerToPeerNetwork::s_message_command_ping         = "P2PN_ping";
    const QString PeerToPeerNetwork::s_message_command_pong         = "P2PN_pong";

    std::string PeerToPeerNetwork::getThisHostIP()
    {
        QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();

        // Find the first non-localhost IPv4 address
        for (const QHostAddress& address : ipAddressesList) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol && !address.isLoopback()) {
                return address.toString().toStdString();
            }
        }
        return "";
    }
    std::string PeerToPeerNetwork::getThisHostName()
    {
        return QHostInfo::localHostName().toStdString();
    }
    std::string PeerToPeerNetwork::getThisUserName()
    {
        char acUserName[100];
        DWORD nUserName = sizeof(acUserName);
        if (GetUserName(acUserName, &nUserName)) 
            return acUserName;
        return "";
    }
    PeerInfo PeerToPeerNetwork::getThisPeerInfoInitial()
    {
        PeerInfo info;
        info.setHostName(getThisHostName());
        info.setUserName(getThisUserName());
        info.setIP(getThisHostIP());

        return info;
    }

    void PeerToPeerNetwork::startProfiler()
    {
#ifdef P2PN_PROFILING
        EASY_PROFILER_ENABLE;
#endif
    }
    void PeerToPeerNetwork::stopProfiler(const std::string profileFilePath)
    {
#ifdef P2PN_PROFILING
        profiler::dumpBlocksToFile(profileFilePath.c_str());
#endif
    }

    PeerToPeerNetwork::PeerToPeerNetwork(const PeerInfo thisPeer, QObject* parent)
        : QObject(parent)
        , m_thisPeerInfo(thisPeer)
    {
        
        connect(&tcpServer, &QTcpServer::newConnection, this, &PeerToPeerNetwork::onNewTcpConnection);
    }
    PeerToPeerNetwork::~PeerToPeerNetwork()
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_1);

        stopListening();
        std::vector<Peer> tmpConnected = m_connectedPeers;
        std::vector<Peer> tmpConnecting = m_connectingPeers;

        m_connectedPeers.clear();
        m_connectingPeers.clear();

        for (Peer& p : tmpConnected)
        {
            disconnect(p.socket, &QTcpSocket::readyRead, this, &PeerToPeerNetwork::onReadyRead);
            disconnect(p.socket, &QTcpSocket::disconnected, this, &PeerToPeerNetwork::onDisconnected);
            disconnect(p.socket, &QTcpSocket::errorOccurred, this, &PeerToPeerNetwork::onSocketError);

            p.socket->disconnectFromHost();
            p.socket->deleteLater();
        }
        

        for (Peer& p : tmpConnecting)
        {
            disconnect(p.socket, &QTcpSocket::connected, this, &PeerToPeerNetwork::onNewTcpConnection);
            disconnect(p.socket, &QTcpSocket::errorOccurred, this, &PeerToPeerNetwork::onSocketError);
            p.socket->abort();
            p.socket->deleteLater();
        }

        clearPeerGrabber();
    }

    const PeerInfo& PeerToPeerNetwork::getPeerInfo() const
    {
        return m_thisPeerInfo;
    }

    void PeerToPeerNetwork::addPeerGrabber(PeerGrabber* grabber)
    {
        if (!grabber)
            return;
        auto it = std::find(m_peerGrabbers.begin(), m_peerGrabbers.end(), grabber);
        if (it != m_peerGrabbers.end()) {
            return; // already in list
        }
        connect(grabber, &PeerGrabber::newPeersAvailable, this, &PeerToPeerNetwork::onNewPeersAvailable);
        grabber->setPeerToPeerNetwork(this);
        m_peerGrabbers.push_back(grabber);
    }
    void PeerToPeerNetwork::removePeerGrabber(PeerGrabber* grabber)
    {
        if (!grabber)
            return;
        auto it = std::find(m_peerGrabbers.begin(), m_peerGrabbers.end(), grabber);
        if (it != m_peerGrabbers.end()) {
            m_peerGrabbers.erase(it);
            grabber->setPeerToPeerNetwork(nullptr);
            disconnect(grabber, &PeerGrabber::newPeersAvailable, this, &PeerToPeerNetwork::onNewPeersAvailable);
        }
    }
    void PeerToPeerNetwork::clearPeerGrabber()
    {
        for (PeerGrabber* p : m_peerGrabbers)
        {
            p->setPeerToPeerNetwork(nullptr);
            disconnect(p, &PeerGrabber::newPeersAvailable, this, &PeerToPeerNetwork::onNewPeersAvailable);
        }
        m_peerGrabbers.clear();
    }
    const std::vector<PeerGrabber*>& PeerToPeerNetwork::getPeerGrabber() const
    {
        return m_peerGrabbers;
    }
    bool PeerToPeerNetwork::containsPeerGrabber(PeerGrabber* grabber) const
    {
        for (const PeerGrabber* p : m_peerGrabbers)
            if (p == grabber)
                return true;
        return false;
    }


    bool PeerToPeerNetwork::startListening()
    {
        return tcpServer.listen(QHostAddress::Any, m_thisPeerInfo.getPort());
    }
    void PeerToPeerNetwork::startListening(quint16 listenPort)
    {
        m_thisPeerInfo.setPort(listenPort);
        startListening();
    }

    void PeerToPeerNetwork::stopListening()
    {
        tcpServer.close();
    }

    void PeerToPeerNetwork::checkNewPeers()
    {
        if (!m_peerGrabbers.size())
            return;
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_1);

        for (PeerGrabber* grabber : m_peerGrabbers)
        {
            std::vector<PeerInfo> peers = grabber->getPeers();
            for (const PeerInfo p : peers)
            {
                if (hasConnection(p))
                    continue;

                // this peer already try's to connect to "p"
                for (const Peer& j : m_connectingPeers)
                {
                    if (j.info == p)
                        goto overjump;
                }
                if (p != m_thisPeerInfo)
                    connectToPeer(p);

                overjump:;
            }
        }
    }
    std::vector<PeerInfo> PeerToPeerNetwork::getPeers() const
    {
        std::vector<PeerInfo> peers;
        peers.reserve(m_connectedPeers.size());
        for (const Peer& p : m_connectedPeers)
            peers.push_back(p.info);
        return peers;
    }
    void PeerToPeerNetwork::printPeers() const
    {
        std::cout << "Peers connected to this host: " << m_connectedPeers.size();
        QJsonObject data;
        data["Hostinfo"] = m_thisPeerInfo.getJson();
        QJsonArray array;
        for (const Peer& p : m_connectedPeers)
        {
            array.append(p.toJson());
        }
        data["Peers"] = array;

        QJsonDocument jsonDoc(data);
        std::cout << std::string(jsonDoc.toJson()).c_str();
    }

    bool PeerToPeerNetwork::sendMessage(const PeerInfo& target, const std::string& message)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_1);
        bool valid;
        Peer& p = getConnectedPeer(target, valid);
        if (valid)
            return sendMessage(p, message);
        return false;
    }
    bool PeerToPeerNetwork::sendMessage(const PeerInfo& target, const QJsonObject& data)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_1);
        bool valid;
        Peer& p = getConnectedPeer(target, valid);
        if (valid)
            return sendMessage(p, data);
        return false;
    }
    bool PeerToPeerNetwork::sendMessage(const PeerInfo& target, const QJsonArray& dataArray)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_1);
        bool valid;
        Peer& p = getConnectedPeer(target, valid);
        if (valid)
            return sendMessage(p, dataArray);
        return false;
    }
    bool PeerToPeerNetwork::broadcastMessage(const PeerInfo& target, const std::string& message)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_1);
        bool success = true;
        for (Peer& p : m_connectedPeers)
        {
            success &= sendMessage(p, message);
        }
        return success;
    }
    bool PeerToPeerNetwork::broadcastMessage(const PeerInfo& target, const QJsonObject& data)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_1);
        bool success = true;
        for (Peer& p : m_connectedPeers)
        {
            success &= sendMessage(p, data);
        }
        return success;
    }
    bool PeerToPeerNetwork::broadcastMessage(const PeerInfo& target, const QJsonArray& dataArray)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_1);
        bool success = true;
        for (Peer& p : m_connectedPeers)
        {
            success &= sendMessage(p, dataArray);
        }
        return success;
    }

    void PeerToPeerNetwork::connectToPeer(const PeerInfo& info)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_1);
        if (info == m_thisPeerInfo)
        {
            qDebug() << __PRETTY_FUNCTION__ << " Can't connect to self";
            return;
        }
        qDebug() << "Trying to connect to: " << info.getJson();

        // Attempt to connect to peer
        QTcpSocket* peerSocket = new QTcpSocket(this);
        peerSocket->connectToHost(info.getIP().c_str(), info.getPort());
        connect(peerSocket, &QTcpSocket::connected, this, &PeerToPeerNetwork::onConnectionSuccess);
        connect(peerSocket, &QTcpSocket::errorOccurred, this, &PeerToPeerNetwork::onSocketError);

        Peer newPeer;
        newPeer.info = info;
        newPeer.pendingInfo = true;
        newPeer.isValid = false;
        newPeer.selfOpenedConnection = true;
        newPeer.socket = peerSocket;
        m_connectingPeers.push_back(newPeer);
        //if (peerSocket->waitForConnected(5000)) {
        //    qDebug() << "Connected";
        //}
        //else {
        //    qDebug() << "Not connected";
        //    qDebug() << peerSocket->errorString();
        //}
    }
    void PeerToPeerNetwork::disconnectFromPeer(const PeerInfo& info)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_1);
        bool valid;
        Peer& p = getConnectedPeer(info, valid);
        if (!valid)
            return;
        p.socket->disconnectFromHost();
        Peer p2 = getConnectingPeer(info, valid);
        if (!valid)
            return;

        for (size_t i=0; i< m_connectingPeers.size(); ++i)
        {
            if (m_connectingPeers[i].info == info)
            {
                m_connectingPeers.erase(m_connectingPeers.begin() + i);
                break;
            }
        }
        p2.socket->abort();
        p2.socket->deleteLater();
    }
    bool PeerToPeerNetwork::hasConnection(const PeerInfo& info)
    {
        for (const Peer& p : m_connectedPeers)
        {
            if (p.info == info)
                return true;
        }
        return false;
    }
    bool PeerToPeerNetwork::hasConnection(const std::string& IP)
    {
        for (const Peer& p : m_connectedPeers)
        {
            if (p.info.getIP() == IP)
                return true;
        }
        return false;
    }
    void PeerToPeerNetwork::onNewPeersAvailable()
    {
        PeerGrabber* grabber = qobject_cast<PeerGrabber*>(sender());
        if (!grabber)
            return;

        qDebug() << "PeerToPeerNetwork::onNewPeersAvailable()";
        std::vector<PeerInfo> peers = grabber->getPeers();
        for (const PeerInfo p : peers)
        {
            if (hasConnection(p))
                continue;

            // this peer already try's to connect to "p"
            for (const Peer& j : m_connectingPeers)
            {
                if (j.info == p)
                    goto overjump;
            }
            if (p != m_thisPeerInfo)
                connectToPeer(p);

        overjump:;
        }
    }



    void PeerToPeerNetwork::onConnectionSuccess()
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_2);
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket)
            return;

        for (size_t i = 0; i < m_connectingPeers.size(); ++i)
        {

            Peer p = m_connectingPeers[i];
            p.connectionStartTime = QDateTime::currentDateTime();
            if (p.socket != socket)
                continue;

            m_connectingPeers.erase(m_connectingPeers.begin() + i);
            disconnect(p.socket, &QTcpSocket::connected, this, &PeerToPeerNetwork::onConnectionSuccess);

            connect(p.socket, &QTcpSocket::readyRead, this, &PeerToPeerNetwork::onReadyRead);
            connect(p.socket, &QTcpSocket::disconnected, this, &PeerToPeerNetwork::onDisconnected);

            m_connectedPeers.push_back(p);
            qDebug() << __PRETTY_FUNCTION__ << " Connection to " << p.socket->peerAddress().toString() << " success";
            sendPing(p);
            
            return;
        }
    }
    void PeerToPeerNetwork::onNewTcpConnection()
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_2);
        while (tcpServer.hasPendingConnections()) {
            QTcpSocket* peerSocket = tcpServer.nextPendingConnection();
            Peer peer;
            peer.pendingInfo = true;
            peer.isValid = false;
            peer.selfOpenedConnection = false;
            peer.socket = peerSocket;
            peer.connectionStartTime = QDateTime::currentDateTime();
            peer.info.setIP(peerSocket->peerAddress().toString().toStdString());

           /* for (size_t i = 0; i < m_connectingPeers.size(); ++i)
            {
                if (peer.info.getIP() == m_connectingPeers[i].info.getIP())
                {
                    qDebug() << __PRETTY_FUNCTION__ << " Already trying to connect to the host: " << m_connectingPeers[i].info.getJson();

                }
            }*/

            m_connectedPeers.push_back(peer);

            connect(peerSocket, &QTcpSocket::readyRead, this, &PeerToPeerNetwork::onReadyRead);
            connect(peerSocket, &QTcpSocket::disconnected, this, &PeerToPeerNetwork::onDisconnected);
            connect(peerSocket, &QTcpSocket::errorOccurred, this, &PeerToPeerNetwork::onSocketError);

            qDebug() << __PRETTY_FUNCTION__ << " New connection from: " << peerSocket->peerAddress().toString();
            // emit peerConnected(peer.info);
        }
    }

    void PeerToPeerNetwork::onReadyRead()
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_2);
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket)
            return;
        for (Peer& p : m_connectedPeers)
        {
            if (p.socket != socket)
                continue;
            
            onReadyReadPeer(p);
            return;
        }
    }

    void PeerToPeerNetwork::onDisconnected()
    {
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket)
            return;
        
        for (size_t i=0; i< m_connectedPeers.size(); ++i)
        {
            Peer p = m_connectedPeers[i];
            if (p.socket != socket)
                continue;
            
            qDebug() << __PRETTY_FUNCTION__ << " " << p.info.getJson();
            
            m_connectedPeers.erase(m_connectedPeers.begin() + i);
            p.socket->deleteLater();

            emit peerDisconnected(p.info);
            return; 
        }
    }

    void PeerToPeerNetwork::onSocketError(QAbstractSocket::SocketError e)
    {
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        if (!socket)
            return;
        Peer p;
        for (size_t i = 0; i < m_connectedPeers.size(); ++i)
        {
            p = m_connectedPeers[i];
            if (p.socket != socket)
                continue;

        }
        for (size_t i = 0; i < m_connectingPeers.size(); ++i)
        {
            p = m_connectingPeers[i];
            if (p.socket != socket)
                continue;

        }
        qDebug() << __PRETTY_FUNCTION__ << " error code: " << e << " peer: "<< p.info.getJson();
    }

    QJsonObject PeerToPeerNetwork::Peer::toJson() const
    {
        QJsonObject connection;
        connection["connectionStartTime"] = connectionStartTime.toString(Qt::ISODate);
        connection["pendingInfo"] = pendingInfo;
        connection["selfOpenedConnection"] = selfOpenedConnection;
        connection["isValid"] = isValid;

        connection["Peerinfo"] = info.getJson();
        return connection;
    }

    PeerToPeerNetwork::Peer& PeerToPeerNetwork::getConnectedPeer(const PeerInfo& peerInfo, bool& valid)
    {
        for (Peer& p : m_connectedPeers)
        {
            if (p.info == peerInfo)
            {
                valid = true;
                return p;
            }
        }
        valid = false;
        static Peer dummy;
        return dummy;
    }
    PeerToPeerNetwork::Peer& PeerToPeerNetwork::getConnectingPeer(const PeerInfo& peerInfo, bool& valid)
    {
        for (Peer& p : m_connectingPeers)
        {
            if (p.info == peerInfo)
            {
                valid = true;
                return p;
            }
        }
        valid = false;
        static Peer dummy;
        return dummy;
    }

    void PeerToPeerNetwork::onReadyReadPeer(Peer& peer)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_3);
        QByteArray data = peer.socket->readAll();


        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if (jsonDoc.isNull()) 
            return;
        QJsonObject obj = jsonDoc.object();

        bool success = true;
        success &= obj.contains(s_message_senderKey);
        success &= obj.contains(s_message_dataKey);
        success &= obj.contains(s_message_commandKey);

        if (!success)
        {
            qDebug() << __PRETTY_FUNCTION__ << " Invalid data packet: " << data;
            return;
        }

        PeerInfo senderPeer;
        QJsonValue senderVal = obj[s_message_senderKey];
        if (!senderVal.isObject())
        {
            qDebug() << __PRETTY_FUNCTION__ << " Invalid sender packet: " << senderVal;
            return;
        }
        QJsonObject senderObj = senderVal.toObject();
        success &= senderPeer.setJson(senderObj);

        peer.info = senderPeer;
        peer.isValid = true;
        

        QString command = obj[s_message_commandKey].toString();

        if (command == s_message_command_default) 
        {
            QJsonValue dataVal = obj[s_message_dataKey];
            
            if (dataVal.isObject())
            {
                QJsonObject obj = dataVal.toObject();
                qDebug() << __PRETTY_FUNCTION__ << " " << obj;
                emit dataReceived(peer.info, obj);
            }
            else if (dataVal.isArray())
            {
                QJsonArray array = dataVal.toArray();
                qDebug() << __PRETTY_FUNCTION__ << " " << array;
                emit dataReceived(peer.info, array);
            }
            else
            {
                std::string data = obj[s_message_dataKey].toString().toStdString();
                qDebug() << __PRETTY_FUNCTION__ << " " << data.c_str();
                emit dataReceived(peer.info, data);
            }
            return;
        }
        else if (command == s_message_command_internal)
        {
            QJsonValue dataVal = obj[s_message_dataKey];
            if (dataVal.isObject())
            {
                QJsonObject data = dataVal.toObject();
                onInternalMessageReceived(peer, data);
            }
            else if (dataVal.isArray())
            {
                QJsonArray dataArray = dataVal.toArray();
                onInternalMessageReceived(peer, dataArray);
            }
            else
            {
                onInternalMessageReceived(peer, dataVal);
            }
            return;            
        }
        else if (command == s_message_command_ping)
        {
            QString dataVal = obj[s_message_dataKey].toString();
            peer.connectionStartTime = QDateTime::fromString(dataVal, Qt::ISODate);
            onPingReceived(peer);
        }
        else if (command == s_message_command_pong)
        {
            QString dataVal = obj[s_message_dataKey].toString();
            peer.connectionStartTime = QDateTime::fromString(dataVal, Qt::ISODate);
            onPongReceived(peer);
        }
        else
        {
            qDebug() << __PRETTY_FUNCTION__ << " Unknown command: " << command;
        }

        if (m_disconnectingPeers.size())
        {
            for (size_t i = 1; i < m_disconnectingPeers.size(); ++i)
            {
                disconnectFromPeer(m_disconnectingPeers[i].info);
            }
            m_disconnectingPeers.clear();
        }
    }
    void PeerToPeerNetwork::onInternalMessageReceived(Peer& peer, const QJsonObject& internalData)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_3);
        qDebug() << __PRETTY_FUNCTION__ << " " << internalData;

    }
    void PeerToPeerNetwork::onInternalMessageReceived(Peer& peer, const QJsonArray& internalDataArray)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_3);
        qDebug() << __PRETTY_FUNCTION__ << " " << internalDataArray;

    }
    void PeerToPeerNetwork::onInternalMessageReceived(Peer& peer, const QJsonValue& internalData)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_3);
        qDebug() << __PRETTY_FUNCTION__ << " " << internalData;

    }
    void PeerToPeerNetwork::onPingReceived(Peer& peer)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_3);
        qDebug() << __PRETTY_FUNCTION__ << " " << peer.info.getJson();

        if (peer.pendingInfo)
        {
            int removedCount = disconnectMultipleConnections(peer);
            peer.pendingInfo = false;
            emit peerConnected(peer.info);
        }
        if (peer.isValid)
            sendPong(peer);
        
    }
    void PeerToPeerNetwork::onPongReceived(Peer& peer)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_3);
        qDebug() << __PRETTY_FUNCTION__ << " " << peer.info.getJson();

        if (peer.pendingInfo)
        {
            int removedCount = disconnectMultipleConnections(peer);
            peer.pendingInfo = false;
            emit peerConnected(peer.info);
        }
    }

    bool PeerToPeerNetwork::sendMessage(const Peer& target, const std::string& message)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_2);
        return sendMessage(target, message, s_message_command_default);
    }
    bool PeerToPeerNetwork::sendMessage(const Peer& target, const QJsonObject& data)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_2);
        return sendMessage(target, data, s_message_command_default);
    }
    bool PeerToPeerNetwork::sendMessage(const Peer& target, const QJsonArray& dataArray)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_2);
        return sendMessage(target, dataArray, s_message_command_default);
    }
    bool PeerToPeerNetwork::sendMessage(const Peer& target, const std::string& message, const QString& p2pcommand)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_3);
        if (target.socket->state() != QAbstractSocket::ConnectedState)
        {
            qDebug() << __PRETTY_FUNCTION__ << " Not connected to peer: "<<target.info.getIP().c_str();
            return false;
        }
        QJsonObject sendingObj;

        QJsonObject hostInfo = m_thisPeerInfo.getJson();
        
        sendingObj[s_message_senderKey] = hostInfo;
        sendingObj[s_message_commandKey] = p2pcommand;
        sendingObj[s_message_dataKey] = message.c_str();

        QJsonDocument jsonDoc(sendingObj);
        QByteArray transmittStr = jsonDoc.toJson();

        return transmittString(target, transmittStr);
    }
    bool PeerToPeerNetwork::sendMessage(const Peer& target, const QJsonObject& data, const QString& p2pcommand)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_3);
        if (target.socket->state() != QAbstractSocket::ConnectedState)
        {
            qDebug() << __PRETTY_FUNCTION__ << " Not connected to peer: " << target.info.getIP().c_str();
            return false;
        }
        QJsonObject sendingObj;

        QJsonObject hostInfo = m_thisPeerInfo.getJson();

        sendingObj[s_message_senderKey] = hostInfo;
        sendingObj[s_message_commandKey] = p2pcommand;
        sendingObj[s_message_dataKey] = data;

        QJsonDocument jsonDoc(sendingObj);
        QByteArray transmittStr = jsonDoc.toJson();

        return transmittString(target, transmittStr);
    }
    bool PeerToPeerNetwork::sendMessage(const Peer& target, const QJsonArray& dataArray, const QString& p2pcommand)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_3);
        if (target.socket->state() != QAbstractSocket::ConnectedState)
        {
            qDebug() << __PRETTY_FUNCTION__ << " Not connected to peer: " << target.info.getIP().c_str();
            return false;
        }
        QJsonObject sendingObj;

        QJsonObject hostInfo = m_thisPeerInfo.getJson();

        sendingObj[s_message_senderKey] = hostInfo;
        sendingObj[s_message_commandKey] = p2pcommand;
        sendingObj[s_message_dataKey] = dataArray;

        QJsonDocument jsonDoc(sendingObj);
        QByteArray transmittStr = jsonDoc.toJson();

        return transmittString(target, transmittStr);
    }
    

    bool PeerToPeerNetwork::sendPing(const Peer& target)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_2);
        return sendMessage(target, target.connectionStartTime.toString(Qt::ISODate).toStdString(), s_message_command_ping);
    }
    bool PeerToPeerNetwork::sendPong(const Peer& target)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_2);
        return sendMessage(target, target.connectionStartTime.toString(Qt::ISODate).toStdString(), s_message_command_pong);
    }

    bool PeerToPeerNetwork::transmittString(const Peer& target, const QByteArray& data)
    {
        P2PNP_NETWORK_FUNCTION(P2PNP_COLOR_STAGE_4);
        target.socket->write(data);
        return target.socket->waitForBytesWritten();
    }

    int PeerToPeerNetwork::disconnectMultipleConnections(Peer peer)
    {
        std::vector<Peer> equalPeers;
        for (size_t i = 0; i < m_connectedPeers.size(); ++i)
        {
            if (m_connectedPeers[i].info == peer.info)
            {
                equalPeers.push_back(m_connectedPeers[i]);
            }
        }

        if (equalPeers.size() < 1)
            return 0;

        // Lambda function to sort based on connectionStartTime
        std::sort(equalPeers.begin(), equalPeers.end(), [](const Peer& a, const Peer& b) {
            return a.connectionStartTime < b.connectionStartTime;
        });

        for (size_t i = 1; i < equalPeers.size(); ++i)
        {
            qDebug() << "Multiple peer gets disconnected: " << equalPeers[i].info.getJson();
            equalPeers[i].isValid = false;
            m_disconnectingPeers.push_back(equalPeers[i]);
            disconnectFromPeer(equalPeers[i].info);
        }
        return equalPeers.size() - 1;
    }
}