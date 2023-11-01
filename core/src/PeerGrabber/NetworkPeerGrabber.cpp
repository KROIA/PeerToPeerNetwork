#include "PeerGrabber/NetworkPeerGrabber.h"
#include "PeerToPeerNetwork.h"
#include <filesystem>
#include <QFile>
#include <QDir>

namespace P2PN
{

	NetworkPeerGrabber::NetworkPeerGrabber(quint16 broadcastPort)
	{
		m_port = broadcastPort;
		connect(&m_socket, &QUdpSocket::readyRead, this, &NetworkPeerGrabber::onBroadcastMessageReceived);
		connect(&m_socket, &QUdpSocket::errorOccurred, this, &NetworkPeerGrabber::onBroadcastMessageReceived);
		if (m_socket.bind(QHostAddress::Any, m_port))
		{
			qDebug() << "Can't bind";
		}		

		connect(&m_waitTimer, &QTimer::timeout, this, &NetworkPeerGrabber::onTimerFinished);

		m_searchFinished = false;
	}
	NetworkPeerGrabber::~NetworkPeerGrabber()
	{

	}

	std::vector<PeerInfo> NetworkPeerGrabber::searchPeers()
	{
		std::vector<PeerInfo> peers;
		if (m_waitTimer.isActive())
			return peers;
		QString thisHostIP = PeerToPeerNetwork::getThisHostIP().c_str();
		

		if (m_searchFinished)
		{
			peers = m_peers;
			m_peers.clear();
			m_searchFinished = false;
		}
		else
		{
			m_peers.clear();
			m_waitTimer.start(300);
			if (m_socket.writeDatagram(("Ping [" + thisHostIP + "]").toUtf8(), QHostAddress::Broadcast, m_port) == -1) // Use the desired
				qDebug() << "Can't send data";
		}
		
		
		return peers;
	}

	void NetworkPeerGrabber::onBroadcastMessageReceived()
	{
		while (m_socket.hasPendingDatagrams()) {
			QByteArray datagram;
			datagram.resize(m_socket.pendingDatagramSize());
			m_socket.readDatagram(datagram.data(), datagram.size());

			QString broadcastMessage = QString(datagram);
			QString thisHostIP = PeerToPeerNetwork::getThisHostIP().c_str();
			QString broadcasterAddress = m_socket.peerAddress().toString();

			

			QString ip = broadcastMessage.mid(broadcastMessage.indexOf("[")+1);
			ip = ip.mid(0, ip.indexOf("]"));

			PeerInfo thisNetworkInfo = getNetworkPeerInfo();
			
			if (broadcastMessage.indexOf("Ping") != -1)
			{
				if (ip != thisHostIP)
				{
					
					if (m_socket.writeDatagram(("Pong [" + ip + "]" + thisNetworkInfo.serialize().c_str()).toUtf8(), QHostAddress::Broadcast, m_port) == -1) // Use the desired
						qDebug() << "Can't send data";
				}
			}
			else if (broadcastMessage.indexOf("Pong") != -1)
			{
				QString otherHostInfoData = broadcastMessage.mid(broadcastMessage.indexOf("]") + 1);

				PeerInfo otherHostInfo;
				otherHostInfo.deserialize(otherHostInfoData.toStdString());

				if (otherHostInfo.getIP() != thisNetworkInfo.getIP() && ip == thisHostIP)
				{
					qDebug() << thisHostIP << " new IP found: " << otherHostInfo.getIP().c_str() << " Port: " << otherHostInfo.getPort();
					m_peers.push_back(otherHostInfo);
				}

				/*
				QString otherIP = broadcastMessage.mid(broadcastMessage.indexOf("]")+2);
				otherIP = otherIP.mid(0, otherIP.indexOf("]"));
				QString otherPort = otherIP.mid(otherIP.indexOf(":") + 1);
				otherIP = otherIP.mid(0, otherIP.indexOf(":"));
				qDebug() << "senderIP: " << ip << " otherIP: " << otherIP;
				if (otherIP != thisHostIP && ip == thisHostIP)
				{
					qDebug() << thisHostIP << " new IP found: " << otherIP << " Port: "<< otherPort;
					PeerInfo peer;
					peer.setIP(otherIP.toStdString());
					peer.setPort(otherPort.toInt());
					m_peers.push_back(peer);
				}*/
			}
			qDebug() << thisHostIP << " broadcast receive: " << broadcastMessage;
		}
	}
	void NetworkPeerGrabber::onErrorOccurred(QAbstractSocket::SocketError e)
	{
		qDebug() << "Error: " << e;
	}
	void NetworkPeerGrabber::onTimerFinished()
	{
		qDebug() << "Search finished. " << m_peers.size() << " peers found";
		m_waitTimer.stop();
		m_searchFinished = true;
		emit newPeersAvailable();
	}
}