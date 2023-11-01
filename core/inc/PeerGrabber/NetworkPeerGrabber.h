#pragma once

#include "PeerToPeerNetwork_base.h"
#include "PeerGrabber.h"
#include "PeerInfo.h"
#include <QUdpSocket>
#include <QObject>
#include <QTimer>

namespace P2PN
{
	class P2PN_EXPORT NetworkPeerGrabber: public PeerGrabber
	{
		Q_OBJECT
	public:
		NetworkPeerGrabber(quint16 broadcastPort);
		~NetworkPeerGrabber();
		
	private slots:
		void onBroadcastMessageReceived();
		void onErrorOccurred(QAbstractSocket::SocketError e);

		void onTimerFinished();
	private:
		std::vector<PeerInfo> searchPeers() override;

		QUdpSocket m_socket;
		quint16 m_port;
		QTimer m_waitTimer;

		bool m_searchFinished;
		std::vector<PeerInfo> m_peers;
	};
}