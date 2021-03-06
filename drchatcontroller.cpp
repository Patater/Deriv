/*
    Deriv is a cross-platform client for th Wired 2.0 protocol
    Copyright (C) 2014  Rafael Warnault, rw@read-write.fr

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/



#include <QDebug>
#include "drchatcontroller.h"
#include "druserscontroller.h"



#pragma mark -

DRChatController::DRChatController(DRServerConnection *connection) :
    DRConnectionController(connection)
{
    this->topic = NULL;
    this->chatBuffer = new QString("");

//    QObject::connect(this->connection, SIGNAL(connectionClosed(DRConnection*,DRError*)),
//                     this, SLOT(connectionClosed(DRConnection*,DRError*)));

//    QObject::connect(this->connection,  SIGNAL(receivedMessage(wi_p7_message_t*)),
//                     this,              SLOT(receivedMessage(wi_p7_message_t*)));

    this->connection->addDelegateForMessage(this, "wired.chat.say");
    this->connection->addDelegateForMessage(this, "wired.chat.me");
    this->connection->addDelegateForMessage(this, "wired.chat.topic");
}


DRChatController::~DRChatController() {
    if(this->topic != NULL)
        delete this->topic, this->topic = NULL;

    if(this->chatBuffer)
        delete this->chatBuffer, this->chatBuffer = NULL;
}



void DRChatController::connectReceiver(QObject *object) {
    QObject::connect(this, SIGNAL(chatControllerReceivedChatMe(DRServerConnection*,QString,DRUser*)), object, SLOT(chatControllerReceivedChatMe(DRServerConnection*,QString,DRUser*)));
    QObject::connect(this, SIGNAL(chatControllerReceivedChatSay(DRServerConnection*,QString,DRUser*)), object, SLOT(chatControllerReceivedChatSay(DRServerConnection*,QString,DRUser*)));
    QObject::connect(this, SIGNAL(chatControllerTopicChanged(DRServerConnection*,DRTopic*,bool)), object, SLOT(chatControllerTopicChanged(DRServerConnection*,DRTopic*,bool)));
}



void DRChatController::disconnectReceiver(QObject *object) {
    QObject::disconnect(this, SIGNAL(chatControllerReceivedChatMe(DRServerConnection*,QString,DRUser*)), object, SLOT(chatControllerReceivedChatMe(DRServerConnection*,QString,DRUser*)));
    QObject::disconnect(this, SIGNAL(chatControllerReceivedChatSay(DRServerConnection*,QString,DRUser*)), object, SLOT(chatControllerReceivedChatSay(DRServerConnection*,QString,DRUser*)));
    QObject::disconnect(this, SIGNAL(chatControllerTopicChanged(DRServerConnection*,DRTopic*,bool)), object, SLOT(chatControllerTopicChanged(DRServerConnection*,DRTopic*,bool)));
}





#pragma mark -

void DRChatController::connectionSucceeded(DRServerConnection *connection) {

}

void DRChatController::connectionError(DRServerConnection *connection, DRError *error) {

}

void DRChatController::connectionClosed(DRServerConnection *connection, DRError *error) {

}


void DRChatController::receivedMessage(wi_p7_message_t *message, DRServerConnection *connection) {
    if(wi_is_equal(wi_p7_message_name(message), WI_STR("wired.chat.say"))) {
        this->receivedWiredChatSay(message);
    }
    else if(wi_is_equal(wi_p7_message_name(message), WI_STR("wired.chat.me"))) {
        this->receivedWiredChatMe(message);
    }
    else if(wi_is_equal(wi_p7_message_name(message), WI_STR("wired.chat.topic"))) {
        this->receivedWiredChatTopic(message);
    }

}

void DRChatController::receivedError(DRError *error, DRServerConnection *connection) {

}





#pragma mark -

void DRChatController::receivedWiredChatSay(wi_p7_message_t *message) {
    DRUser                  *user;
    wi_p7_int32_t           userID;

    wi_p7_message_get_int32_for_name(message, &userID, WI_STR("wired.user.id"));

    if(userID <= 0)
        return;

    user = this->connection->usersController->userWithID(userID);

    if(user == NULL)
        return;

    wi_string_t *wstring = wi_p7_message_string_for_name(message, WI_STR("wired.chat.say"));

    if(!wstring || wi_string_length(wstring) <= 0)
        return;

    QString qstring = QString("%1 : %2").arg(user->nick, QString(wi_string_cstring(wstring)));

    emit chatControllerReceivedChatSay(this->connection, qstring, user);
}




void DRChatController::receivedWiredChatMe(wi_p7_message_t *message) {
    DRUser                  *user;
    wi_p7_int32_t           userID;

    wi_p7_message_get_int32_for_name(message, &userID, WI_STR("wired.user.id"));

    if(userID <= 0)
        return;

    user = this->connection->usersController->userWithID(userID);

    if(user == NULL)
        return;

    wi_string_t *wstring = wi_p7_message_string_for_name(message, WI_STR("wired.chat.me"));

    if(!wstring || wi_string_length(wstring) <= 0)
        return;

    QString qstring = QString("*** %1 %2").arg(user->nick, QString(wi_string_cstring(wstring)));

    emit chatControllerReceivedChatMe(this->connection, qstring, user);
}




void DRChatController::receivedWiredChatTopic(wi_p7_message_t *message) {
    if(this->topic == NULL) {
        this->topic = new DRTopic(message);
        emit chatControllerTopicChanged(this->connection, this->topic, true);
    } else {
        this->topic->setObjectWithMessage(message);
        emit chatControllerTopicChanged(this->connection, this->topic, false);
    }
}




