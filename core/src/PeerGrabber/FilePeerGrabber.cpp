#include "PeerGrabber/FilePeerGrabber.h"
#include <filesystem>
#include <QFile>
#include <QDir>

namespace P2PN
{
	const std::string FilePeerGrabber::s_fileEnding = ".json";

	void FilePeerGrabber::setPeerFilesPath(const std::string& directory)
	{
		m_peerFilesDir = directory;
	}
	const std::string& FilePeerGrabber::getPeerFilesPath() const
	{
		return m_peerFilesDir;
	}

	bool FilePeerGrabber::savePeer(const PeerInfo& peer) const
	{
		return savePeerInfoToFile(peer, m_peerFilesDir, createFileName(peer));
	}
	bool FilePeerGrabber::savePeers(const std::vector<PeerInfo>& peers) const
	{
		bool success = true;
		for (const PeerInfo& p : peers)
			success &= savePeer(p);
		return success;
	}
	bool FilePeerGrabber::removePeer(const PeerInfo& peer) const
	{
		std::vector<std::string> jsonFiles = getFileList(m_peerFilesDir, s_fileEnding);

		for (const std::string& file : jsonFiles)
		{
			PeerInfo loaded;
			std::string filePath = m_peerFilesDir + "\\" + file;
			if (readPeerInfoFromFile(filePath, loaded))
			{
				if (loaded == peer)
				{
					return deleteFile(filePath + s_fileEnding);
				}
			}
		}
		return false;
	}
	bool FilePeerGrabber::removePeer(const std::vector<PeerInfo>& peers) const
	{
		std::vector<std::string> jsonFiles = getFileList(m_peerFilesDir, s_fileEnding);
		bool success = true;
		for (const PeerInfo& peer : peers)
		{
			for (const std::string& file : jsonFiles)
			{
				PeerInfo loaded;
				std::string filePath = m_peerFilesDir + "\\" + file;
				if (readPeerInfoFromFile(filePath, loaded))
				{
					if (loaded == peer)
					{
						success &= deleteFile(filePath);
					}
				}
			}
		}
		return success;
	}

	std::vector<PeerInfo> FilePeerGrabber::searchPeers()
	{
		std::vector<std::string> jsonFiles = getFileList(m_peerFilesDir, s_fileEnding);

		std::vector<PeerInfo> peers;
		for (const std::string& file : jsonFiles)
		{
			PeerInfo peer;
			if (readPeerInfoFromFile(m_peerFilesDir + "\\" + file, peer))
				peers.push_back(peer);
		}
		return peers;
	}

	std::vector<std::string> FilePeerGrabber::getFileList(const std::string& dir, const std::string& fileEnding) const
	{
		std::vector<std::string> fileList;

		if (!std::filesystem::is_directory(dir))
		{
			qDebug() << "no directory: " << dir.c_str();
			return fileList;
		}

		for (const auto& entry : std::filesystem::directory_iterator(dir)) {
			if (entry.is_regular_file()) {
				std::string filename = entry.path().filename().string();
				if (filename.size() >= fileEnding.size() &&
					filename.compare(filename.size() - fileEnding.size(), fileEnding.size(), fileEnding) == 0) {
					size_t endingPos = filename.rfind(fileEnding);
					if (endingPos != std::string::npos)
						filename = filename.substr(0, endingPos);
					fileList.push_back(filename);
				}
			}
		}

		return fileList;
	}
	bool FilePeerGrabber::readPeerInfoFromFile(const std::string& peerFile, PeerInfo& infoOut) const
	{
		QFile file((peerFile + s_fileEnding).c_str());
		if (!file.open(QIODevice::ReadOnly)) {
			qDebug() << "Failed to open file for reading:" << peerFile.c_str();
			return false;
		}
		std::string jsonData = file.readAll();
		if (!infoOut.deserialize(jsonData))
			return false;
		return true;
	}
	bool FilePeerGrabber::savePeerInfoToFile(const PeerInfo& info, const std::string& directory, const std::string& peerFileName) const
	{
		QDir dir(directory.c_str());
		if (!dir.exists())
		{
			QDir d;
			d.mkpath(directory.c_str());
		}
		QFile file((directory + "\\" + peerFileName + s_fileEnding).c_str());
		if (file.open(QIODevice::WriteOnly)) {
			file.write(info.serialize().c_str());
			file.close();

			//qDebug() << "Saved JSON object to file:" << peerFile.c_str();
			return true;
		}
		else {
			//qDebug() << "Failed to open file for writing:" << peerFile.c_str();

		}
		return false;
	}
	std::string FilePeerGrabber::createFileName(const PeerInfo& info) const
	{
		return "PEER_" + info.getNetworkID();
	}

	bool FilePeerGrabber::deleteFile(const std::string& filePath) const
	{
		QFile f(filePath.c_str());
		if (f.exists())
		{
			f.remove();
		}
		return !f.exists();
	}
}