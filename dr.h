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


#ifndef DR_H
#define DR_H

#include <QObject>
#include <wired/wired.h>


class DR : public QObject
{
    Q_OBJECT
public:
    static QString      loadPasswordForULRIdentifier(QString identifier);
    static bool         savePasswordForULRIdentifier(QString password, QString identifier);
    static wi_data_t*   getDefaultBase64UserIcon();
};

#endif // DR_H