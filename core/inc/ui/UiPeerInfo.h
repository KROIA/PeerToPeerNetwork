#pragma once

#include "PeerToPeerNetwork_base.h"

#include <QWidget>
#include "ui_uiPeerInfo.h"
#include "Peerinfo.h"


namespace P2PN
{
	class P2PN_EXPORT UiPeerInfo : public QWidget
	{
		Q_OBJECT

	public:
		UiPeerInfo(QWidget* parent = nullptr);
		~UiPeerInfo();

		void update(const PeerInfo& peer);
	private:
		Ui::UiPeerInfo ui;
	};
}
