#include "ui/uiPeerInfo.h"


namespace P2PN
{
	UiPeerInfo::UiPeerInfo(QWidget* parent)
		: QWidget(parent)
	{
		ui.setupUi(this);
	}

	UiPeerInfo::~UiPeerInfo()
	{

	}

	void UiPeerInfo::update(const PeerInfo& peer)
	{
		ui.hostName_label->setText(peer.getName().c_str());
		ui.ip_label->setText(peer.getIP().c_str());
		ui.port_label->setText(QString::number(peer.getPort()));
	}
}