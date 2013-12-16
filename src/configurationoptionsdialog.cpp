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

#if USE_KDE_STATUS_NOTIFIER
#include <kstandarddirs.h>
#endif

#define DEFAULT_KDE_WALLET     "default kde wallet"
#define qCheckGMail_KDE_wALLET "qCheckGMail kde wallet"

configurationoptionsdialog::configurationoptionsdialog( QWidget * parent ) :
	QDialog( parent ),m_ui( new Ui::configurationoptionsdialog )
{
	m_ui->setupUi( this ) ;

	this->setFixedSize( this->size() ) ;
	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	connect( m_ui->pushButtonClose,SIGNAL( clicked() ),this,SLOT( pushButtonClose() ) ) ;

	if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::internalBackEnd ) ){
		m_ui->comboBoxBackEndSystem->addItem( QString( "internal wallet" ) ) ;
	}
	if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::kwalletBackEnd ) ){
		m_ui->comboBoxBackEndSystem->addItem( QString( DEFAULT_KDE_WALLET ) ) ;
		m_ui->comboBoxBackEndSystem->addItem( QString( qCheckGMail_KDE_wALLET ) ) ;
	}
	if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::secretServiceBackEnd ) ){
		m_ui->comboBoxBackEndSystem->addItem( QString( "gnome wallet" ) ) ;
	}

	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "storageSystem" ) ;

	QString value = settings.value( opt ).toString() ;

	int j = m_ui->comboBoxBackEndSystem->count() ;

	for( int i = 0 ; i < j ; i++ ){
		m_ui->comboBoxBackEndSystem->setCurrentIndex( i ) ;
		if( m_ui->comboBoxBackEndSystem->currentText() == value ){
			break ;
		}
	}

	this->installEventFilter( this ) ;
}

bool configurationoptionsdialog::eventFilter( QObject * watched,QEvent * event )
{
	if( watched == this ){
		if( event->type() == QEvent::KeyPress ){
			QKeyEvent * keyEvent = static_cast< QKeyEvent* >( event ) ;
			if( keyEvent->key() == Qt::Key_Escape ){
				this->HideUI() ;
				return true ;
			}
		}
	}

	return false ;
}

void configurationoptionsdialog::setDefaultQSettingOptions( QSettings& settings )
{
	#if USE_KDE_STATUS_NOTIFIER
		KStandardDirs k ;
		settings.setPath( QSettings::IniFormat,QSettings::UserScope,k.localxdgconfdir() ) ;
	#else
		settings.setPath( QSettings::IniFormat,QSettings::UserScope,QDir::homePath() + QString( "/.config" ) ) ;
	#endif
}

QString configurationoptionsdialog::walletName( LxQt::Wallet::walletBackEnd backEnd )
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	if( backEnd == LxQt::Wallet::kwalletBackEnd ){
		QString opt = QString( "storageSystem" ) ;
		if( settings.value( opt ).toString() == qCheckGMail_KDE_wALLET ){
			return QString( "qCheckGMail" ) ;
		}else{
			LxQt::Wallet::Wallet * w = LxQt::Wallet::getWalletBackend( LxQt::Wallet::kwalletBackEnd ) ;
			if( w ){
				QString value = w->localDefaultWalletName() ;
				w->deleteLater() ;
				return value ;
			}else{
				return QString( "kdewallet" ) ;
			}
		}
	}else{
		return QString( "qCheckGMail" ) ;
	}
}

void configurationoptionsdialog::saveStorageSystem( const QString& system )
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;
	QString opt = QString( "storageSystem" ) ;
	settings.setValue( opt,system ) ;
	settings.sync() ;
}

QString configurationoptionsdialog::logFile()
{
	#if USE_KDE_STATUS_NOTIFIER
		KStandardDirs k ;
		return k.localxdgconfdir() + QString( "/%1/%2.log" ).arg( PROGRAM_NAME ).arg( PROGRAM_NAME ) ;
	#else
		return QString( "%1/.config/%2/%1.log" ).arg( QDir::homePath() ).arg( PROGRAM_NAME ).arg( PROGRAM_NAME ) ;
	#endif
}

LxQt::Wallet::Wallet * configurationoptionsdialog::secureStorageSystem()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "storageSystem" ) ;

	if( settings.contains( opt ) ){
		QString value = settings.value( opt ).toString() ;
		if( value == QString( "gnome wallet" ) ){
			if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::secretServiceBackEnd ) ){
				return LxQt::Wallet::getWalletBackend( LxQt::Wallet::secretServiceBackEnd ) ;
			}else{
				settings.setValue( opt,QString( "internal wallet" ) ) ;
				return LxQt::Wallet::getWalletBackend( LxQt::Wallet::internalBackEnd ) ;
			}
		}else if( value.contains( QString( "kde" ) ) ){
			if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::kwalletBackEnd ) ){
				return LxQt::Wallet::getWalletBackend( LxQt::Wallet::kwalletBackEnd ) ;
			}else{
				settings.setValue( opt,QString( "internal wallet" ) ) ;
				return LxQt::Wallet::getWalletBackend( LxQt::Wallet::internalBackEnd ) ;
			}
		}else{
			settings.setValue( opt,QString( "internal wallet" ) ) ;
			return LxQt::Wallet::getWalletBackend( LxQt::Wallet::internalBackEnd ) ;
		}
	}else{
		if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::kwalletBackEnd ) ){
			settings.setValue( opt,QString( DEFAULT_KDE_WALLET ) ) ;
			settings.sync() ;
			return LxQt::Wallet::getWalletBackend( LxQt::Wallet::kwalletBackEnd ) ;
		}else if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::secretServiceBackEnd ) ){
			settings.setValue( opt,QString( "gnome wallet" ) ) ;
			settings.sync() ;
			return LxQt::Wallet::getWalletBackend( LxQt::Wallet::secretServiceBackEnd ) ;
		}else{
			settings.setValue( opt,QString( "internal wallet" ) ) ;
			settings.sync() ;
			return LxQt::Wallet::getWalletBackend( LxQt::Wallet::internalBackEnd ) ;
		}
	}
}

bool configurationoptionsdialog::audioNotify()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "audioNotify" ) ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toBool() ;
	}else{
		settings.setValue( opt,true ) ;
		return true ;
	}
}

QString configurationoptionsdialog::noEmailIcon()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "noEmailIconColor" ) ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toString() ;
	}else{
		QString value = QString( "grey" ) ;
		settings.setValue( opt,value ) ;
		settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::newEmailIcon()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "newEmailIconColor" ) ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toString() ;
	}else{
		QString value = QString( "blue" ) ;
		settings.setValue( opt,value ) ;
		settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::errorIcon()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "errorIconColor" ) ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toString() ;
	}else{
		QString value = QString( "red" ) ;
		settings.setValue( opt,value ) ;
		settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::fontFamily()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "displayEmailCountFontFamily" ) ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toString() ;
	}else{
		QString value = QString( "Helvetica" ) ;
		settings.setValue( opt,value ) ;
		settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::fontColor()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "displayEmailCountFontColor" ) ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toString() ;
	}else{
		QString value = QString( "black" ) ;
		settings.setValue( opt,value ) ;
		settings.sync() ;
		return value ;
	}
}

bool configurationoptionsdialog::usingInternalStorageSystem()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "storageSystem" ) ;
	return settings.value( opt ).toString() == QString( "internal wallet" ) ;
}

int configurationoptionsdialog::fontSize()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "displayEmailCountFontSize" ) ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toInt() ;
	}else{
		QString value = QString( "60" ) ;
		settings.setValue( opt,value ) ;
		settings.sync() ;
		return 60 ;
	}
}

bool configurationoptionsdialog::displayEmailCount()
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;

	QString opt = QString( "displayEmailCount" ) ;

	if( settings.contains( opt ) ){
		return settings.value( opt ).toBool() ;
	}else{
		settings.setValue( opt,true ) ;
		settings.sync() ;
		return true ;
	}
}

void configurationoptionsdialog::setAudioNotify( bool audioNotify )
{
	QSettings settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;
	configurationoptionsdialog::setDefaultQSettingOptions( settings ) ;
	QString opt = QString( "audioNotify" ) ;
	settings.setValue( opt,audioNotify ) ;
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
		settings.sync() ;
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
		settings.sync() ;
		return true ;
	}
}

void configurationoptionsdialog::saveReportOnAllAccounts( bool b )
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
		settings.sync() ;
		return lang ;
	}
}

QString configurationoptionsdialog::localLanguagePath()
{
	/*
	 * LANGUAGE_FILE_PATH is set by the build system and
	 * it will contain something line $install_prefix/share/qCheckGMail/
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
	settings.sync() ;
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
		settings.sync() ;
		return 30 * 60 * 1000 ;
	}
}

void configurationoptionsdialog::ShowUI()
{
	int time = configurationoptionsdialog::getTimeFromConfigFile() / ( 60 * 1000 ) ;
	m_ui->lineEditUpdateCheckInterval->setText( QString::number( time ) ) ;
	m_ui->checkBoxAutoStartEnabled->setChecked( configurationoptionsdialog::autoStartEnabled() ) ;
	m_ui->checkBoxReportOnAllAccounts->setChecked( configurationoptionsdialog::reportOnAllAccounts() ) ;
	m_ui->checkBoxAudioNotify->setChecked( configurationoptionsdialog::audioNotify() ) ;

	this->setSupportedLanguages() ;
	this->show() ;
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

	this->saveReportOnAllAccounts( m_ui->checkBoxReportOnAllAccounts->isChecked() ) ;

	this->saveTimeToConfigFile() ;

	emit setTimer( z * 60 * 1000 ) ;

	emit audioNotify( m_ui->checkBoxAudioNotify->isChecked() ) ;

	this->setAudioNotify( m_ui->checkBoxAudioNotify->isChecked() ) ;

	QString s = m_ui->comboBoxBackEndSystem->currentText() ;

	this->saveStorageSystem( s ) ;

	emit enablePassWordChange( s == QString( "internal wallet" ) ) ;

	this->deleteLater() ;
}

configurationoptionsdialog::~configurationoptionsdialog()
{
	delete m_ui ;
}

void configurationoptionsdialog::closeEvent( QCloseEvent * e )
{
	e->ignore() ;
	this->HideUI() ;
}


void configurationoptionsdialog::pushButtonClose()
{
	this->HideUI() ;
}

void configurationoptionsdialog::setSupportedLanguages()
{
	QDir d( configurationoptionsdialog::localLanguagePath() + QString( "/translations.qm/" ) ) ;

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

