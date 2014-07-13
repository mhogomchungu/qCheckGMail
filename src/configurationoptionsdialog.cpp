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
        #if KF5
                #include <QStandardPaths>
        #else
                #include <kstandarddirs.h>
        #endif
#endif

#define DEFAULT_KDE_WALLET     "default kde wallet"
#define qCheckGMail_KDE_wALLET "qCheckGMail kde wallet"

static QString _profile ;
static QSettings _settings( QString( ORGANIZATION_NAME ),QString( PROGRAM_NAME ) ) ;

static QString _getOption( const char * opt )
{
	if( _profile.isEmpty() ){
		return opt ;
	}else{
		return QString( "%1/%2").arg( _profile ).arg( opt ) ;
	}
}

#if USE_KDE_STATUS_NOTIFIER
        #if KF5
                #include <QStandardPaths>
                static QString _configPath( void )
                {
                        return QStandardPaths::writableLocation( QStandardPaths::GenericConfigLocation ) ;
                }
        #else
                #include <kstandarddirs.h>
                static QString _configPath( void )
                {
                        KStandardDirs k ;
                        return k.localxdgconfdir() ;
                }
        #endif
#else
        static QString _configPath( void )
        {
                  return QDir::homePath() + "/.config" ;
        }
#endif

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

	#if USE_KDE_STATUS_NOTIFIER
		m_ui->checkBoxAudioNotify->setEnabled( true ) ;
	#else
		m_ui->checkBoxAudioNotify->setChecked( false ) ;
		m_ui->checkBoxAudioNotify->setEnabled( false ) ;
	#endif

	QString opt = _getOption( "storageSystem" ) ;

	QString value = _settings.value( opt ).toString() ;

	int j = m_ui->comboBoxBackEndSystem->count() ;

	for( int i = 0 ; i < j ; i++ ){
		m_ui->comboBoxBackEndSystem->setCurrentIndex( i ) ;
		if( m_ui->comboBoxBackEndSystem->currentText() == value ){
			break ;
		}
	}

	this->installEventFilter( this ) ;
}

void configurationoptionsdialog::setProfile( const QString& profile )
{
	if( !profile.isEmpty() && _settings.contains( profile ) ){
		_profile = profile ;
	}

        _settings.setPath( QSettings::IniFormat,QSettings::UserScope,_configPath() ) ;
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

QString configurationoptionsdialog::walletName( LxQt::Wallet::walletBackEnd backEnd )
{
	if( backEnd == LxQt::Wallet::kwalletBackEnd ){
		QString opt = _getOption( "storageSystem" ) ;
		if( _settings.value( opt ).toString() == qCheckGMail_KDE_wALLET ){
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
	QString opt = _getOption( "storageSystem" ) ;
	_settings.setValue( opt,system ) ;
	_settings.sync() ;
}

QString configurationoptionsdialog::logFile()
{
        return _configPath() + QString( "/%1/%2.log" ).arg( PROGRAM_NAME ).arg( PROGRAM_NAME ) ;
}

LxQt::Wallet::Wallet * configurationoptionsdialog::secureStorageSystem()
{
	QString opt = _getOption( "storageSystem" ) ;
	LxQt::Wallet::Wallet * w ;

	if( _settings.contains( opt ) ){
		QString value = _settings.value( opt ).toString() ;
		if( value == QString( "gnome wallet" ) ){
			if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::secretServiceBackEnd ) ){
				w = LxQt::Wallet::getWalletBackend( LxQt::Wallet::secretServiceBackEnd ) ;
			}else{
				_settings.setValue( opt,QString( "internal wallet" ) ) ;
				w = LxQt::Wallet::getWalletBackend( LxQt::Wallet::internalBackEnd ) ;
			}
		}else if( value.contains( QString( "kde" ) ) ){
			if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::kwalletBackEnd ) ){
				w = LxQt::Wallet::getWalletBackend( LxQt::Wallet::kwalletBackEnd ) ;
			}else{
				_settings.setValue( opt,QString( "internal wallet" ) ) ;
				w = LxQt::Wallet::getWalletBackend( LxQt::Wallet::internalBackEnd ) ;
			}
		}else{
			QString wallet( "internal wallet" ) ;
			if( value != wallet ){
				_settings.setValue( opt,wallet ) ;
			}
			w = LxQt::Wallet::getWalletBackend( LxQt::Wallet::internalBackEnd ) ;
		}
	}else{
		if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::kwalletBackEnd ) ){
			_settings.setValue( opt,QString( DEFAULT_KDE_WALLET ) ) ;
			w = LxQt::Wallet::getWalletBackend( LxQt::Wallet::kwalletBackEnd ) ;
		}else if( LxQt::Wallet::backEndIsSupported( LxQt::Wallet::secretServiceBackEnd ) ){
			_settings.setValue( opt,QString( "gnome wallet" ) ) ;
			w = LxQt::Wallet::getWalletBackend( LxQt::Wallet::secretServiceBackEnd ) ;
		}else{
			_settings.setValue( opt,QString( "internal wallet" ) ) ;
			w = LxQt::Wallet::getWalletBackend( LxQt::Wallet::internalBackEnd ) ;
		}
	}

	_settings.sync() ;
	return w ;
}

bool configurationoptionsdialog::audioNotify()
{
	QString opt = _getOption( "audioNotify" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toBool() ;
	}else{
		_settings.setValue( opt,true ) ;
		return true ;
	}
}

QString configurationoptionsdialog::noEmailIcon()
{
	QString opt = _getOption( "noEmailIconColor" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toString() ;
	}else{
		QString value = QString( "grey" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::newEmailIcon()
{
	QString opt = _getOption( "newEmailIconColor" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toString() ;
	}else{
		QString value = QString( "blue" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::errorIcon()
{
	QString opt = _getOption( "errorIconColor" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toString() ;
	}else{
		QString value = QString( "red" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::fontFamily()
{
	QString opt = _getOption( "displayEmailCountFontFamily" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toString() ;
	}else{
		QString value = QString( "Helvetica" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::fontColor()
{
	QString opt = _getOption( "displayEmailCountFontColor" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toString() ;
	}else{
		QString value = QString( "black" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::defaultApplication()
{
	QString opt = _getOption( "defaultApplication" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toString() ;
	}else{
		QString value = QString( "browser" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QStringList configurationoptionsdialog::profileEmailList()
{
	if( !_profile.isEmpty() && _settings.contains( _profile ) ){
		QString z = _settings.value( _profile ).toString() ;
		return z.split( "," ) ;
	}else{
		return QStringList() ;
	}
}

bool configurationoptionsdialog::usingInternalStorageSystem()
{
	QString opt = _getOption( "storageSystem" ) ;
	return _settings.value( opt ).toString() == QString( "internal wallet" ) ;
}

int configurationoptionsdialog::fontSize()
{
	QString opt = _getOption( "displayEmailCountFontSize" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toInt() ;
	}else{
		QString value = QString( "80" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return 80 ;
	}
}

bool configurationoptionsdialog::displayEmailCount()
{
	QString opt = _getOption( "displayEmailCount" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toBool() ;
	}else{
		_settings.setValue( opt,true ) ;
		_settings.sync() ;
		return true ;
	}
}

int configurationoptionsdialog::networkTimeOut()
{
	QString opt = _getOption( "networkTimeOut" ) ;

	if( _settings.contains( opt ) ){
		return 1000 * 60 * _settings.value( opt ).toInt() ;
	}else{
		_settings.setValue( opt,2 ) ;
		_settings.sync() ;
		return 1000 * 60 * 2 ;
	}
}

void configurationoptionsdialog::setAudioNotify( bool audioNotify )
{
	QString opt = QString( "audioNotify" ) ;
	_settings.setValue( opt,audioNotify ) ;
}

bool configurationoptionsdialog::autoStartEnabled()
{
	QString opt = _getOption( "autostart" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toBool() ;
	}else{
		_settings.setValue( opt,true ) ;
		_settings.sync() ;
		return true ;
	}
}

void configurationoptionsdialog::enableAutoStart( bool b )
{
	QString opt = QString( "autostart" ) ;
	_settings.setValue( opt,b ) ;
}

bool configurationoptionsdialog::reportOnAllAccounts()
{
	QString opt = _getOption( "reportOnAllAccounts" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toBool() ;
	}else{
		_settings.setValue( opt,true ) ;
		_settings.sync() ;
		return true ;
	}
}

void configurationoptionsdialog::saveReportOnAllAccounts( bool b )
{
	QString opt = _getOption( "reportOnAllAccounts" ) ;
	_settings.setValue( opt,b ) ;
	emit reportOnAllAccounts( b ) ;
}

QString configurationoptionsdialog::localLanguage()
{
	QString opt = _getOption( "language" ) ;

	if( _settings.contains( opt ) ){
		return _settings.value( opt ).toString() ;
	}else{
		QString lang = QString( "english_US" ) ;
		_settings.setValue( opt,lang ) ;
		_settings.sync() ;
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
	QString opt = _getOption( "language" ) ;
	QString language = m_ui->comboBoxLocalLanguage->currentText() ;
	_settings.setValue( opt,language ) ;
}

void configurationoptionsdialog::saveTimeToConfigFile()
{
	QString opt = _getOption( "interval" ) ;
	QString time = m_ui->lineEditUpdateCheckInterval->text() ;
	_settings.setValue( opt,time ) ;
	_settings.sync() ;
}

int configurationoptionsdialog::getTimeFromConfigFile()
{
	QString opt = _getOption( "interval" ) ;

	if( _settings.contains( opt ) ){
		bool ok ;
		int time = _settings.value( opt ).toInt( &ok ) ;
		if( ok ){
			return time * 60 * 1000 ;
		}else{
			return 30 * 60 * 1000 ;
		}
	}else{
		_settings.setValue( opt,QString( "30" ) ) ;
		_settings.sync() ;
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

	if( z < 1 ){
		QMessageBox msg( this ) ;
		msg.setText( tr( "\n\nERROR: minimum time interval is 1 minute\n\n" ) ) ;
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

	this->hide() ;

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
