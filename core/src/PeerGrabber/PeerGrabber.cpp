#include "PeerGrabber/PeerGrabber.h"
#include "PeerToPeerNetwork.h"

namespace P2PN
{
	PeerGrabber::PeerGrabber()
		: m_net(nullptr)
	{

	}
	PeerGrabber::~PeerGrabber() 
	{}
	std::vector<PeerInfo> PeerGrabber::getPeers()
	{
		return searchPeers();
	}

	quint16 PeerGrabber::getNetworkPort() const
	{
		if (m_net)
			return m_net->getPeerInfo().getPort();
		return 0;
	}
	PeerInfo PeerGrabber::getNetworkPeerInfo() const
	{
		if (m_net)
			return m_net->getPeerInfo();
		return PeerInfo();
	}
	void PeerGrabber::setPeerToPeerNetwork(PeerToPeerNetwork* net)
	{
		m_net = net;
	}

}