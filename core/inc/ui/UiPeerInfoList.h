#pragma once

#include <QWidget>
#include "ui_UiPeerInfoList.h"
#include "UiPeerInfo.h"
#include <vector>

namespace P2PN
{
	class P2PN_EXPORT UiPeerInfoList : public QWidget
	{
		Q_OBJECT

	public:
		UiPeerInfoList(QWidget* parent = nullptr);
		~UiPeerInfoList();

		void updatePeers(const std::vector<PeerInfo>& peers);

	private:
		Ui::UiPeerInfoList ui;

		std::vector<UiPeerInfo*> m_infoWidgets;
		
	};
}