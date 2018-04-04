#include "chat_server.h"

#include <QTcpSocket>
#include <QRegExp>

Chat_server::Chat_server(QObject *parent) : QTcpServer(parent)
{

}

QTcpSocket * Chat_server::find_user_address_by_name(const QString & user_name)
{
	return this->users.key(user_name);
}

void Chat_server::incomingConnection(int socketfd)
{
	QTcpSocket* client = new QTcpSocket(this);
	client->setSocketDescriptor(socketfd);

	clients.insert(client);

	qDebug() << "New client from:" << client->peerAddress().toString();

	connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
	//connect(this, SIGNAL(write_private()), client, SLOT(read_private_message()));
	connect(client, SIGNAL(client->private_message(QString)), this, SLOT(readPrivate(QString)));
}

void Chat_server::readyRead()
{
	QTcpSocket *client = (QTcpSocket*)sender();
	while (client->canReadLine())
	{
		QString line = QString::fromUtf8(client->readLine()).trimmed();
		qDebug() << "Read line:" << line;

		QRegExp meRegex("^/me:(.*)$");

		auto list = line.split(QRegExp("\\b"));

		if (meRegex.indexIn(line) != -1)
		{
			QString user = meRegex.cap(1);
			users[client] = user;
			foreach(QTcpSocket *client, clients)
				client->write(QString("Server:" + user + " has joined.\n").toUtf8());
			sendUserList();
		}
		else if (list[0]+list[1] == "/private")
		{
			QString receiver_name = list[3];
			QString message{};
			for (int i = 5; i < list.size(); ++i)
			{
				message += list[i];
			}
			this->send_private_message(message, receiver_name,client);
		}
		else if (users.contains(client))
		{
			QString message = line;
			QString user = users[client];
			qDebug() << "User:" << user;
			qDebug() << "Message:" << message;

			foreach(QTcpSocket *otherClient, clients)
				otherClient->write(QString(user + ":" + message + "\n").toUtf8());
		}
		else
		{
			qWarning() << "Got bad message from client:" << client->peerAddress().toString() << line;
		}
	}
}

void Chat_server::disconnected()
{
	QTcpSocket *client = (QTcpSocket*)sender();
	qDebug() << "Client disconnected:" << client->peerAddress().toString();

	clients.remove(client);

	QString user = users[client];
	users.remove(client);

	sendUserList();
	foreach(QTcpSocket *client, clients)
		client->write(QString("Server:" + user + " has left.\n").toUtf8());
}

void Chat_server::sendUserList()
{
	QStringList userList;
	foreach(QString user, users.values())
		userList << user;

	foreach(QTcpSocket *client, clients)
		client->write(QString("/users:" + userList.join(",") + "\n").toUtf8());
}

void Chat_server::send_private_message(const QString& message,const QString& receiver_name, QTcpSocket* sender_address)
{
		QTcpSocket* receiver_address = find_user_address_by_name(receiver_name);

		QString name_user_sender = this->users[sender_address];
		QString name_user_receiver = this->users[receiver_address];

		sender_address->write(QString("/private:" + name_user_sender + ":" + name_user_receiver + ":" + message+"\n").toUtf8());
		if (name_user_receiver != name_user_sender)
		{
			receiver_address->write(QString("/private:" + name_user_sender + ":" + name_user_receiver + ":" + message+"\n").toUtf8());
		}
}
