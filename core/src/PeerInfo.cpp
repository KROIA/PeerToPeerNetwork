#include "PeerInfo.h"
#include <QJsonDocument>

namespace P2PN
{
	const QString PeerInfo::s_jsonKey_hostName = "host";
	const QString PeerInfo::s_jsonKey_userName = "user";
	const QString PeerInfo::s_jsonKey_ip   = "IP";
	const QString PeerInfo::s_jsonKey_port = "PORT";

	PeerInfo::PeerInfo()
		: m_ip("")
		, m_port(0)
		, m_hostName("")
		, m_userName("")
	{

	}
	bool PeerInfo::operator==(const PeerInfo& other) const
	{
		if (m_ip != other.m_ip)	return false;
		if (m_port != other.m_port)	return false;
		if (m_userName != other.m_userName)	return false;
		if (m_hostName != other.m_hostName)	return false;

		return true;
	}
	bool PeerInfo::operator!=(const PeerInfo& other) const
	{ 
		return !operator==(other);
	}

	bool PeerInfo::equalNetworkID(const PeerInfo& other) const
	{
		if (m_ip != other.m_ip) return false;
		if (m_port != other.m_port) return false;
		return true;
	}


	void PeerInfo::setIP(const std::string& ip)
	{
		m_ip = ip;
	}
	const std::string& PeerInfo::getIP() const
	{
		return m_ip;
	}

	void PeerInfo::setHostName(const std::string& name)
	{
		m_hostName = name;
	}
	const std::string& PeerInfo::getHostName() const
	{
		return m_hostName;
	}
	void PeerInfo::setUserName(const std::string& name)
	{
		m_userName = name;
	}
	const std::string& PeerInfo::getUserName() const
	{
		return m_userName;
	}
	void PeerInfo::setPort(quint16 port)
	{
		m_port = port;
	}
	
	quint16 PeerInfo::getPort() const
	{
		return m_port;
	}

	QJsonObject PeerInfo::getJson() const
	{
		QJsonObject obj;

		obj[s_jsonKey_hostName] = m_hostName.c_str();
		obj[s_jsonKey_userName] = m_userName.c_str();
		obj[s_jsonKey_ip]   = m_ip.c_str();
		obj[s_jsonKey_port] = m_port;

		return obj;
	}
	bool PeerInfo::setJson(const QJsonObject& obj)
	{
		bool success = true;

		success &= obj.contains(s_jsonKey_hostName);
		success &= obj.contains(s_jsonKey_userName);
		success &= obj.contains(s_jsonKey_ip);
		success &= obj.contains(s_jsonKey_port);
		if (!success)
			return false;

		m_hostName = obj[s_jsonKey_hostName].toString().toStdString();
		m_userName = obj[s_jsonKey_userName].toString().toStdString();
		m_ip   = obj[s_jsonKey_ip].toString().toStdString();

		int portValue = obj[s_jsonKey_port].toInt(10);
		m_port		  =	obj[s_jsonKey_port].toInt(0);

		if (portValue != m_port)
			return false;

		return true;
	}

	std::string PeerInfo::serialize() const
	{
		QJsonDocument jsonDoc(getJson());
		return std::string(jsonDoc.toJson().data());
	}
	bool PeerInfo::deserialize(const std::string& serialData)
	{
		QJsonDocument jsonDoc = QJsonDocument::fromJson(serialData.c_str());
		if (!jsonDoc.isNull()) {
			return setJson(jsonDoc.object());
		}
		return false;
	}

	std::string PeerInfo::getNetworkID() const
	{
		return m_ip + "_" + std::to_string(m_port);
	}
}