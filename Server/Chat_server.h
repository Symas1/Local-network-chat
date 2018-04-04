#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>
#include <QStringList>

class Chat_server : public QTcpServer
{
	Q_OBJECT
public:
	Chat_server(QObject *parent = 0);

	public slots :
	void readyRead();

	void disconnected();
	void sendUserList();
	
protected:
	QTcpSocket* find_user_address_by_name(const QString& user_name);
	void incomingConnection(int socketfd);
	void send_private_message(const QString& message, const QString& receiver_name,QTcpSocket* sender_address);
private:
	QSet<QTcpSocket*> clients;
	QMap<QTcpSocket*, QString> users;
};

#endif // CHAT_SERVER_H
