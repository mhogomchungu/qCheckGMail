/*
 *
 *  Copyright (c) 2013
 *  name : mhogo mchungu
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ADDACCOUNT_H
#define ADDACCOUNT_H

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QCloseEvent>
#include <QMessageBox>
#include <QDebug>

#include "accounts.h"

namespace Ui {
class addaccount;
}

class addaccount : public QDialog
{
	Q_OBJECT
public:
	explicit addaccount( QWidget * parent = 0 );
	addaccount( int row = -1,QString accName = QString(),QString accPassword = QString(),
		    QString accDisplayName = QString(),QString accLabels = QString(),QWidget * parent = 0 ) ;

	void ShowUI( void ) ;
	void HideUI( void ) ;
	~addaccount();
signals:
	void addAccount( QString,QString,QString,QString ) ;
	void editAccount( int,QString,QString,QString,QString ) ;
private slots:
	void add( void ) ;
	void cancel( void ) ;
private:
	void closeEvent( QCloseEvent * ) ;
	Ui::addaccount * m_ui ;
	int m_acc ;
};

#endif // ADDACCOUNT_H
