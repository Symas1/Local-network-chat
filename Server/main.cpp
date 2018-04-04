#include "chat_server.h"
#include <QCoreApplication>
#include <QTcpServer>


int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);

	Chat_server* server = new Chat_server();

	bool success = server->listen(QHostAddress::Any, 4200);
	if (!success)
	{
		qFatal("Could not listen on port");
	}

	qDebug() << "Server started on port"<< server->serverPort();

	return app.exec();
}
