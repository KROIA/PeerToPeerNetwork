#pragma once
#include <QTimer>
#include <qdir.h>

#include "P2PN.h"


using namespace P2PN;

class Runner : public QObject
{
	Q_OBJECT
public:
	Runner()
		: networkPeerGrabber(1232)
	{
		timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, &Runner::onUpdateTimerFinished);
		infoWidget = new UiPeerInfoList();
		

		//grabber.setPeerFilesPath("peers");
		PeerInfo thisHost = PeerToPeerNetwork::getThisPeerInfoInitial();
		thisHost.setPort(1230);
		//grabber.savePeer(thisHost);

		network = new PeerToPeerNetwork(thisHost, this);
		network->addPeerGrabber(&networkPeerGrabber);

		connect(network, &PeerToPeerNetwork::peerConnected, this, &Runner::onPeerChange);
		connect(network, &PeerToPeerNetwork::peerDisconnected, this, &Runner::onPeerChange);

		if (!network->startListening())
		{
			qDebug() << "Can't start network. Port already taken?";
			return;
		}

		
		infoWidget->show();
		timer->start(1000);

		QDir dir;
		
		//dir.mkdir(grabber.getPeerFilesPath().c_str());
	}

	~Runner()
	{
		timer->stop();
		delete timer;

		//grabber.removePeer(network->getPeerInfo());
		delete network;

		delete infoWidget;
		
	}
private slots:
	void onUpdateTimerFinished()
	{
		qDebug() << "Update...";
		++counter;


		/*if (counter == 1)
			network->printPeers();*/
		if(counter == 2)
			network->checkNewPeers();
		
		/*if (counter > 9)
		{
			grabber.removePeer(network->getPeerInfo());
		}
		if (counter == 10)
		{
			network->printPeers();
			PeerToPeerNetwork::stopProfiler("network.prof");
			//timer->stop();
		}*/
		if(counter > 10 && counter % 3 == 0)
		{
			network->checkNewPeers();
		}

		/*if (counter == 3)
		{
			networkPeerGrabber.getPeers();
		}

		if (counter == 10)
		{
			networkPeerGrabber.getPeers();
		}*/

		//std::vector<PeerInfo> peers = network->getPeers();
		//infoWidget->updatePeers(peers);

	}

	void onPeerChange(const PeerInfo& peer)
	{
		std::vector<PeerInfo> peers = network->getPeers();
		infoWidget->updatePeers(peers);
	}

private:
	//FilePeerGrabber grabber;
	NetworkPeerGrabber networkPeerGrabber;
	PeerToPeerNetwork* network;
	QTimer* timer;
	int counter = 0;

	UiPeerInfoList* infoWidget;

};