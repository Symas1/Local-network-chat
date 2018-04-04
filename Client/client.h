#ifndef CLIENT_H
#define CLIENT_H

#include <QtWidgets/QMainWindow>
#include <QTcpSocket>
#include <QMap>
#include <QString>
#include <QVector>
#include "ui_client.h"

class Client : public QMainWindow, public Ui::ClientClass
{
	Q_OBJECT

public:
	Client(QWidget *parent = 0);

	private slots:

	void on_loginButton_clicked();

	void on_sayButton_clicked();

	void readyRead();

	// This function gets called when the socket tells us it's connected.
	void connected();

    void on_userListWidget_clicked(const QModelIndex &index);

    void on_privateUserListWidget_clicked(const QModelIndex &index);

    void on_privateSayButton_clicked();

private:
	void load_message_history_with_user(QString& user);
	void read_private_message(QStringList& list);

	QTcpSocket *socket;
	QString private_receiver_name;
	QMap<QString, QVector<QString>> messages;
};

#endif // CLIENT_H
