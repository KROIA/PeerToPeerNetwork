#include <iostream>
#include <csignal>

#include <QThread>
#include <QDebug>

#include <QApplication>
#include <windows.h>  
#include "Runner.h"

Runner* runner = nullptr;


int main(int argc, char* argv[])
{
	PeerToPeerNetwork::startProfiler();
	QApplication app(argc, argv);

	runner = new Runner();

	app.exec();
	return 0;
}