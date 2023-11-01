#pragma once

#include "PeerToPeerNetwork_base.h"
#include "PeerInfo.h"
#include <vector>
#include <QObject>

namespace P2PN
{
	class PeerToPeerNetwork;
	class P2PN_EXPORT PeerGrabber: public QObject
	{
		Q_OBJECT
		friend PeerToPeerNetwork;
	public:
		PeerGrabber();
		virtual ~PeerGrabber();

		std::vector<PeerInfo> getPeers();

	signals:
		void newPeersAvailable();
	protected:
		virtual std::vector<PeerInfo> searchPeers() = 0;

		quint16 getNetworkPort() const;
		PeerInfo getNetworkPeerInfo() const;

	private:
		void setPeerToPeerNetwork(PeerToPeerNetwork* net);

		PeerToPeerNetwork* m_net;
	};
}