#pragma once

#include "PeerToPeerNetwork_base.h"
#include <string>
#include <QJsonObject>

namespace P2PN
{
	class P2PN_EXPORT PeerInfo
	{
	public:
		PeerInfo();

		bool operator==(const PeerInfo& other) const;
		bool operator!=(const PeerInfo& other) const;
		
		void setIP(const std::string& ip);
		const std::string& getIP() const;

		void setName(const std::string& name);
		const std::string& getName() const;

		void setPort(quint16 port);
		quint16 getPort() const;

		QJsonObject getJson() const;
		bool setJson(const QJsonObject& obj);

		std::string serialize() const;
		bool deserialize(const std::string& serialData); 

	private:
		std::string m_ip;
		quint16 m_port;
		std::string m_name;

		static const QString s_jsonKey_name;
		static const QString s_jsonKey_ip;
		static const QString s_jsonKey_port;
	};
}