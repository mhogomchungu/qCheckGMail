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

#include "configurationoptionsdialog.h"
#include "ui_configurationoptionsdialog.h"

configurationoptionsdialog::configurationoptionsdialog( QWidget * parent ) :
	QDialog( parent ),m_ui( new Ui::configurationoptionsdialog )
{
	m_ui->setupUi( this );
	connect( m_ui->pushButtonClose,SIGNAL( clicked() ),this,SLOT( pushButtonClose() ) ) ;
}

QString configurationoptionsdialog::getConfigPath()
{
	KStandardDirs k ;
	QString path = k.localxdgconfdir() + QString( "/qCheckGMail/" ) ;
	QDir d ;
	d.mkpath( path ) ;
	return path ;
}

bool configurationoptionsdialog::autoStartEnabled()
{
	QFile f( configurationoptionsdialog::getConfigPath() + QString( "qCheckGMailNoAutoStart.option" ) ) ;
	return !f.exists() ;
}

void configurationoptionsdialog::enableAutoStart()
{
	QFile::remove( configurationoptionsdialog::getConfigPath() + QString( "qCheckGMailNoAutoStart.option" ) ) ;
}

void configurationoptionsdialog::disableAutoStart()
{
	QFile f( configurationoptionsdialog::getConfigPath() + QString( "qCheckGMailNoAutoStart.option" ) ) ;
	f.open( QIODevice::WriteOnly ) ;
	f.close();
}

QString configurationoptionsdialog::localLanguage()
{
	QFile f( configurationoptionsdialog::getConfigPath() + QString( "qCheckGMailLocalLanguage.option" ) ) ;

	if( !f.exists() ){
		f.open( QIODevice::WriteOnly ) ;
		f.write( "english_US" ) ;
		f.close();
	}

	f.open( QIODevice::ReadOnly ) ;
	QString e = f.readAll() ;
	f.close();
	return e ;
}

QString configurationoptionsdialog::localLanguagePath()
{
	/*
	 * LANGUAGE_FILE_PATH is set by the build system and
	 * it will contain something line $install_prefix/share/qCheckGMail/translations.qm/
	 */
	return QString( LANGUAGE_FILE_PATH ) ;
}

void configurationoptionsdialog::saveLocalLanguage()
{
	QFile f( configurationoptionsdialog::getConfigPath() + QString( "qCheckGMailLocalLanguage.option" ) ) ;

	f.open( QIODevice::WriteOnly ) ;
	f.write( m_ui->comboBoxLocalLanguage->currentText().toAscii() ) ;
	f.close() ;
}

void configurationoptionsdialog::saveTimeToConfigFile()
{
	QFile f( configurationoptionsdialog::getConfigPath() + QString( "qCheckGMailTimeInterval.option" ) ) ;
	f.open( QIODevice::WriteOnly | QIODevice::Truncate ) ;
	f.write( m_ui->lineEditUpdateCheckInterval->text().toAscii() ) ;
	f.close() ;
}

int configurationoptionsdialog::getTimeFromConfigFile()
{
	QFile f( configurationoptionsdialog::getConfigPath() + QString( "qCheckGMailTimeInterval.option" ) ) ;

	if( !f.exists() ){
		f.open( QIODevice::WriteOnly ) ;
		f.write( "5" ) ; // 5 minutes
		f.close() ;
	}

	f.open( QIODevice::ReadOnly ) ;
	QString time = f.readAll() ;
	f.close() ;

	bool ok ;
	int t = time.toInt( &ok ) ;

	if( ok ){
		return t * 60 * 1000 ;
	}else{
		return 5 * 60 * 1000 ;
	}
}

void configurationoptionsdialog::ShowUI()
{
	int time = configurationoptionsdialog::getTimeFromConfigFile() / ( 60 * 1000 ) ;
	m_ui->lineEditUpdateCheckInterval->setText( QString::number( time ) ) ;
	m_ui->checkBoxAutoStartEnabled->setChecked( configurationoptionsdialog::autoStartEnabled() ) ;
	this->setSupportedLanguages();
	this->show();
}

void configurationoptionsdialog::HideUI()
{
	if( m_ui->checkBoxAutoStartEnabled->isChecked() ){
		configurationoptionsdialog::enableAutoStart() ;
	}else{
		configurationoptionsdialog::disableAutoStart() ;
	}

	QString x = m_ui->lineEditUpdateCheckInterval->text() ;
	bool y ;
	int z = x.toInt( &y ) ;

	if( !y ){
		QMessageBox msg( this ) ;
		msg.setText( tr( "\n\nERROR: illegal characters detected in the the update interval field\n\n" ) ) ;
		msg.exec() ;
		return ;
	}

	if( z < 5 ){
		QMessageBox msg( this ) ;
		msg.setText( tr( "\n\nERROR: minimum time interval is 5 minutes\n\n" ) ) ;
		msg.exec() ;
		return ;
	}

	this->saveLocalLanguage() ;

	this->saveTimeToConfigFile() ;
	emit setTimer( z * 60 * 1000 ) ;
	this->hide() ;
	this->deleteLater() ;
}

configurationoptionsdialog::~configurationoptionsdialog()
{
	delete m_ui;
}

void configurationoptionsdialog::closeEvent( QCloseEvent * e )
{
	e->ignore();
	this->HideUI();
}


void configurationoptionsdialog::pushButtonClose()
{
	this->HideUI();
}

void configurationoptionsdialog::setSupportedLanguages()
{
	QDir d( configurationoptionsdialog::localLanguagePath() ) ;

	QStringList l = d.entryList() ;
	l.removeOne( QString( "." ) ) ;
	l.removeOne( QString( ".." ) ) ;

	int j = l.size() ;

	QComboBox * cbox = m_ui->comboBoxLocalLanguage ;
	QString x ;
	for( int i = 0 ; i < j ; i++ ){
		x = l.at( i ) ;
		x.truncate( x.size() - 3 ) ; //remove the .qm extension
		cbox->addItem( x ) ;
	}

	x = configurationoptionsdialog::localLanguage() ;

	j = cbox->count() ;
	/*
	 * have the user's preferred language highlighted
	 */
	for( int i = 0 ; i < j ; i++ ){
		cbox->setCurrentIndex( i ) ;
		if( cbox->currentText() == x ){
			break ;
		}
	}
}

