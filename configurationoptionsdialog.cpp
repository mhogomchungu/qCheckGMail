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

#define PROGRAM_NAME "qCheckGMail"
#define ORGANIZATION_NAME "qCheckGMail"

configurationoptionsdialog::configurationoptionsdialog( QWidget * parent ) :
	QDialog( parent ),m_ui( new Ui::configurationoptionsdialog )
{
	m_ui->setupUi( this );

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog );

	connect( m_ui->pushButtonClose,SIGNAL( clicked() ),this,SLOT( pushButtonClose() ) ) ;
}

void configurationoptionsdialog::setDefaultQSettingOptions( QSettings& settings )
{
	KStandardDirs k ;
	settings.setPath( QSettings::IniFormat,QSettings::UserScope,k.localxdgconfdir() ) ;
}

QString configurationoptionsdialog::walletName()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt   = QString( "walletName" ) ;
	QString value = QString( "qCheckGMail" ) ;
	if( settings.contains( opt ) ){
		return settings.value( opt ).toString() ;
	}else{
		settings.setValue( opt,value ) ;
		return value ;
	}
}

QString configurationoptionsdialog::passwordFolderName()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt   = QString( "passwordFolder" ) ;
	QString value = QString( "qCheckGMail" ) ;
	if( settings.contains( opt ) ){
		return settings.value( opt ).toString() ;
	}else{
		settings.setValue( opt,value ) ;
		return value ;
	}
}

QString configurationoptionsdialog::defaultWalletName()
{
	return QString( "qCheckGMail" ) ;
}

QString configurationoptionsdialog::logFile()
{
	KStandardDirs k ;
	return k.localxdgconfdir() + QString( "/%1/%2.log" ).arg( PROGRAM_NAME ).arg( PROGRAM_NAME ) ;
}

bool configurationoptionsdialog::autoStartEnabled()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "autostart" ) ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toBool() ;
	}else{
		settings.setValue( opt,true ) ;
		return true ;
	}
}

void configurationoptionsdialog::enableAutoStart( bool b )
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;
	QString opt = QString( "autostart" ) ;
	settings.setValue( opt,b ) ;
}

bool configurationoptionsdialog::reportOnAllAccounts()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "reportOnAllAccounts" ) ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toBool() ;
	}else{
		settings.setValue( opt,true ) ;
		return true ;
	}
}

void configurationoptionsdialog::reportOnAllAccounts_1( bool b )
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;
	QString opt = QString( "reportOnAllAccounts" ) ;
	settings.setValue( opt,b ) ;
	emit reportOnAllAccounts( b ) ;
}

QString configurationoptionsdialog::localLanguage()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "language" ) ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toString() ;
	}else{
		QString lang = QString( "english_US" ) ;
		settings.setValue( opt,lang ) ;
		return lang ;
	}
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
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;
	QString opt = QString( "language" ) ;
	QString language = m_ui->comboBoxLocalLanguage->currentText() ;
	settings.setValue( opt,language ) ;
}

void configurationoptionsdialog::saveTimeToConfigFile()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;
	QString opt = QString( "interval" ) ;
	QString time = m_ui->lineEditUpdateCheckInterval->text() ;
	settings.setValue( opt,time ) ;
}

int configurationoptionsdialog::getTimeFromConfigFile()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;
	QString opt = QString( "interval" ) ;

	if( settings.contains( opt ) ){
		bool ok ;
		int time = settings.value( opt ).toInt( &ok ) ;
		if( ok ){
			return time * 60 * 1000 ;
		}else{
			return 30 * 60 * 1000 ;
		}
	}else{
		settings.setValue( opt,QString( "30" ) ) ;
		return 30 * 60 * 1000 ;
	}
}

void configurationoptionsdialog::ShowUI()
{
	int time = configurationoptionsdialog::getTimeFromConfigFile() / ( 60 * 1000 ) ;
	m_ui->lineEditUpdateCheckInterval->setText( QString::number( time ) ) ;
	m_ui->checkBoxAutoStartEnabled->setChecked( configurationoptionsdialog::autoStartEnabled() ) ;
	m_ui->checkBoxReportOnAllAccounts->setChecked( configurationoptionsdialog::reportOnAllAccounts() ) ;
	this->setSupportedLanguages();
	this->show();
}

void configurationoptionsdialog::HideUI()
{
	this->hide() ;

	configurationoptionsdialog::enableAutoStart( m_ui->checkBoxAutoStartEnabled->isChecked() ) ;

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

	this->reportOnAllAccounts_1( m_ui->checkBoxReportOnAllAccounts->isChecked() ) ;

	this->saveTimeToConfigFile() ;

	emit setTimer( z * 60 * 1000 ) ;
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

