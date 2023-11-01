#include "PeerInfo.h"
#include <QJsonDocument>

namespace P2PN
{
	const QString PeerInfo::s_jsonKey_name = "name";
	const QString PeerInfo::s_jsonKey_ip   = "IP";
	const QString PeerInfo::s_jsonKey_port   = "PORT";

	PeerInfo::PeerInfo()
		: m_ip("")
		, m_port(0)
		, m_name("")
	{

	}
	bool PeerInfo::operator==(const PeerInfo& other) const
	{
		if (m_ip != other.m_ip)	return false;
		if (m_name != other.m_name)	return false;
		return true;
	}
	bool PeerInfo::operator!=(const PeerInfo& other) const
	{ 
		return !operator==(other);
	}


	void PeerInfo::setIP(const std::string& ip)
	{
		m_ip = ip;
	}
	const std::string& PeerInfo::getIP() const
	{
		return m_ip;
	}

	void PeerInfo::setName(const std::string& name)
	{
		m_name = name;
	}
	const std::string& PeerInfo::getName() const
	{
		return m_name;
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

		obj[s_jsonKey_name] = m_name.c_str();
		obj[s_jsonKey_ip]   = m_ip.c_str();
		obj[s_jsonKey_port] = m_port;

		return obj;
	}
	bool PeerInfo::setJson(const QJsonObject& obj)
	{
		bool success = true;

		success &= obj.contains(s_jsonKey_name);
		success &= obj.contains(s_jsonKey_ip);
		success &= obj.contains(s_jsonKey_port);
		if (!success)
			return false;

		m_name = obj[s_jsonKey_name].toString().toStdString();
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
		return jsonDoc.toJson();
	}
	bool PeerInfo::deserialize(const std::string& serialData)
	{
		QJsonDocument jsonDoc = QJsonDocument::fromJson(serialData.c_str());
		if (!jsonDoc.isNull()) {
			return setJson(jsonDoc.object());
		}
		return false;
	}
}