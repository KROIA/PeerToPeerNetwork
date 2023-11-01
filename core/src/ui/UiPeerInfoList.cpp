#include "ui/UiPeerInfoList.h"
#include <QLayout>

namespace P2PN
{
	UiPeerInfoList::UiPeerInfoList(QWidget* parent)
		: QWidget(parent)
	{
		ui.setupUi(this);
		QLayout* layout = ui.contentWidget->layout();
		layout->setAlignment(Qt::AlignTop);

	}

	UiPeerInfoList::~UiPeerInfoList()
	{
	}

	void UiPeerInfoList::updatePeers(const std::vector<PeerInfo>& peers)
	{
		QLayout* layout = ui.contentWidget->layout();
		size_t size = m_infoWidgets.size();
		for (size_t i = peers.size(); i < size; ++i)
		{
			UiPeerInfo* widget = m_infoWidgets[m_infoWidgets.size() - 1];
			layout->removeWidget(widget);
			delete widget;
			m_infoWidgets.pop_back();
		}

		for (size_t i = 0; i < peers.size(); ++i)
		{
			if (i >= m_infoWidgets.size())
			{
				UiPeerInfo* info = new UiPeerInfo();
				layout->addWidget(info);
				info->show();
				m_infoWidgets.push_back(info);
			}
			m_infoWidgets[i]->update(peers[i]);
		}
	}
}