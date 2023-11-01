#pragma once

#include "PeerToPeerNetwork_base.h"
#include "PeerGrabber.h"
#include "PeerInfo.h"

namespace P2PN
{
	class P2PN_EXPORT FilePeerGrabber: public PeerGrabber
	{
	public:

		void setPeerFilesPath(const std::string& directory);
		const std::string& getPeerFilesPath() const;

		bool savePeer(const PeerInfo& peer) const;
		bool savePeers(const std::vector<PeerInfo>& peers) const;
		bool removePeer(const PeerInfo& peer) const;
		bool removePeer(const std::vector<PeerInfo>& peers) const;

	private:
		std::vector<PeerInfo> searchPeers() override;

		std::vector<std::string> getFileList(const std::string& dir, const std::string& fileEnding) const;
		bool readPeerInfoFromFile(const std::string& peerFile, PeerInfo& infoOut) const;
		bool savePeerInfoToFile(const PeerInfo& info, const std::string &directory, const std::string& peerFileName) const;
		std::string createFileName(const PeerInfo& info) const;

		bool deleteFile(const std::string& filePath) const;

		std::string m_peerFilesDir;

		static const std::string s_fileEnding;
	};
}