#include "client.h"
#include <QRegExp>


Client::Client(QWidget *parent) : QMainWindow(parent)
{
	setupUi(this);

	stackedWidget->setCurrentWidget(loginPage);

	socket = new QTcpSocket(this);

	connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(socket, SIGNAL(connected()), this, SLOT(connected()));
}

void Client::on_loginButton_clicked()
{
	socket->connectToHost(serverLineEdit->text(), 4200);
}

void Client::on_sayButton_clicked()
{
	QString message = sayLineEdit->text().trimmed();

	if (!message.isEmpty())
	{
		socket->write(QString(message + "\n").toUtf8());
	}

	sayLineEdit->clear();
	sayLineEdit->setFocus();
}

void Client::readyRead()
{
	while (socket->canReadLine())
	{
		QString line = QString::fromUtf8(socket->readLine()).trimmed();

		// These two regular expressions describe the kinds of messages
		// the server can send us:

		//  Normal messges look like this: "username:The message"
		QRegExp messageRegex("^([^:]+):(.*)$");

		// Any message that starts with "/users:" is the server sending us a
		// list of users so we can show that list in our GUI:
		QRegExp usersRegex("^/users:(.*)$");

		auto list = line.split(QRegExp("\\b"));

		// Is this a users message:
		if (usersRegex.indexIn(line) != -1)
		{
			// If so, udpate our users list on the right:
			QStringList users = usersRegex.cap(1).split(",");
			userListWidget->clear();
			foreach(QString user, users)
				new QListWidgetItem(QPixmap(":/user.png"), user, userListWidget);
		}
		else if (list[0] + list[1] == "/private")
		{
			this->read_private_message(list);
		}
		else if (messageRegex.indexIn(line) != -1)
		{
			// If so, append this message to our chat box:
			QString user = messageRegex.cap(1);
			QString message = messageRegex.cap(2);

			roomTextEdit->append("<b>" + user + "</b>: " + message);
		}
	}
}

void Client::connected()
{
	stackedWidget->setCurrentWidget(chatPage);

	socket->write(QString("/me:" + userLineEdit->text() + "\n").toUtf8());
}

void Client::on_userListWidget_clicked(const QModelIndex &index)
{
	stackedWidget->setCurrentWidget(privateChatPage);
	this->private_receiver_name = index.data().toString();
	privateRoomText->clear();
	privateRoomText->append("This is your chat with: " + QString("<b>") + private_receiver_name + "</b>");
	this->load_message_history_with_user(private_receiver_name);
}

void Client::on_privateUserListWidget_clicked(const QModelIndex &index)
{
	if (index.row() == 0)stackedWidget->setCurrentWidget(chatPage);
}

void Client::load_message_history_with_user(QString & user)
{
	auto messages_it = this->messages.find(user);
	if (messages_it != this->messages.end())
	{
		for (auto&message : *messages_it)
		{
			privateRoomText->append(message);
		}
	}
	else
	{
		privateRoomText->append(QString("No previous messages with this user"));
	}
}

void Client::on_privateSayButton_clicked()
{
	QString message = privateSayLineEdit->text().trimmed();

	if (!message.isEmpty())
	{
		this->socket->write(QString("/private:" + this->private_receiver_name + ":" + message + "\n").toUtf8());
	}

	privateSayLineEdit->clear();
	privateSayLineEdit->setFocus();
}

void Client::read_private_message(QStringList& list)
{
	QString sender_name = list[3];
	QString receiver_name = list[5];
	QString message{};
	for (int i = 7; i < list.size(); ++i)
	{
		message += list[i];
	}

	QString final_message = "<b>" + sender_name + "</b>" + ": " + message;

	if (receiver_name == private_receiver_name)
	{
		if (!this->messages.contains(receiver_name))
		{
			this->messages.insert(receiver_name, {});
		}
		this->messages[receiver_name].push_back(final_message);
	}
	else
	{
		if (!this->messages.contains(sender_name))
		{
			this->messages.insert(sender_name, {});
		}
		this->messages[sender_name].push_back(final_message);
	}

	privateRoomText->append(final_message);
}
