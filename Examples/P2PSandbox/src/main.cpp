#include <iostream>

#include "P2PN.h"
#include <QApplication>
#include<windows.h>  

using namespace P2PN;

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);


	// Create Peer 1
	PeerToPeerNetwork peer1;
	peer1.startBroadcasting(45454, 45455); // UDP port, TCP listen port

	//Sleep(3000);
	//qDebug() << "Start new peer";

	// Create Peer 2
	//PeerToPeerNetwork peer2;
	//peer2.startListening(45455); // TCP listen port

	app.exec();
	return 0;
}