/*
 *
 *  Copyright (c) 2013
 *  name : Francis Banyikwa
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

#include <memory>

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
static QSettings _settings( ORGANIZATION_NAME,PROGRAM_NAME ) ;

static QString _getOption( const char * opt )
{
	if( _profile.isEmpty() ){
		return opt ;
	}else{
		return QString( "%1/%2").arg( _profile,opt ) ;
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

configurationoptionsdialog::configurationoptionsdialog( QObject * parent ) :
        m_ui( new Ui::configurationoptionsdialog )
{
	m_ui->setupUi( this ) ;

        //this->setFixedSize( this->size() ) ;
        m_ui->pushButtonClose->setMinimumHeight( 31 ) ;

	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	connect( m_ui->pushButtonClose,SIGNAL( clicked() ),this,SLOT( pushButtonClose() ) ) ;

	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::internal ) ){

		m_ui->comboBoxBackEndSystem->addItem( "internal wallet" ) ;
	}
	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::kwallet ) ){

		m_ui->comboBoxBackEndSystem->addItem( DEFAULT_KDE_WALLET ) ;
		m_ui->comboBoxBackEndSystem->addItem( qCheckGMail_KDE_wALLET ) ;
	}
	if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::libsecret ) ){

		m_ui->comboBoxBackEndSystem->addItem( "gnome wallet" ) ;
	}

	m_ui->checkBoxAudioNotify->setEnabled( true ) ;

        auto opt = _getOption( "storageSystem" ) ;

        auto value = _settings.value( opt ).toString() ;

        auto j = m_ui->comboBoxBackEndSystem->count() ;

        for( decltype( j ) i = 0 ; i < j ; i++ ){

		m_ui->comboBoxBackEndSystem->setCurrentIndex( i ) ;

		if( m_ui->comboBoxBackEndSystem->currentText() == value ){

			break ;
		}
	}

        connect( this,SIGNAL( setTimer( int ) ),parent,SLOT( configurationWindowClosed( int ) ) ) ;
        connect( this,SIGNAL( enablePassWordChange( bool ) ),parent,SLOT( enablePassWordChange( bool ) ) ) ;
        connect( this,SIGNAL( reportOnAllAccounts( bool ) ),parent,SLOT( reportOnAllAccounts( bool ) ) ) ;
        connect( this,SIGNAL( audioNotify( bool ) ),parent,SLOT( audioNotify( bool ) ) ) ;

	this->installEventFilter( this ) ;

        this->setWindowIcon( QIcon( ":/" + configurationoptionsdialog::noEmailIcon() ) ) ;

        this->ShowUI() ;
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

                        auto keyEvent = static_cast< QKeyEvent* >( event ) ;

                        if( keyEvent->key() == Qt::Key_Escape ){

                                this->HideUI() ;

                                return true ;
			}
		}
	}
	return false ;
}

QString configurationoptionsdialog::walletName( LXQt::Wallet::BackEnd backEnd )
{
	if( backEnd == LXQt::Wallet::BackEnd::kwallet ){

                auto opt = _getOption( "storageSystem" ) ;

                if( _settings.value( opt ).toString() == qCheckGMail_KDE_wALLET ){

                        return "qCheckGMail" ;
		}else{

			std::unique_ptr< LXQt::Wallet::Wallet > w( LXQt::Wallet::getWalletBackend( backEnd ) ) ;

                        if( w ){

				return w->localDefaultWalletName() ;
			}else{
				return "kdewallet" ;
			}
		}
	}else{
		return "qCheckGMail" ;
	}
}

void configurationoptionsdialog::saveStorageSystem( const QString& system )
{
        auto opt = _getOption( "storageSystem" ) ;
	_settings.setValue( opt,system ) ;
	_settings.sync() ;
}

QString configurationoptionsdialog::logFile()
{
	return _configPath() + QString( "/%1/%2.log" ).arg( PROGRAM_NAME,PROGRAM_NAME ) ;
}

LXQt::Wallet::Wallet * configurationoptionsdialog::secureStorageSystem()
{
        auto opt = _getOption( "storageSystem" ) ;
	LXQt::Wallet::Wallet * w ;

	if( _settings.contains( opt ) ){

                auto value = _settings.value( opt ).toString() ;

		if( value == "gnome wallet" ){

			if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::libsecret ) ){

				w = LXQt::Wallet::getWalletBackend( LXQt::Wallet::BackEnd::libsecret ).release() ;
			}else{
				_settings.setValue( opt,QString( "internal wallet" ) ) ;
				w = LXQt::Wallet::getWalletBackend( LXQt::Wallet::BackEnd::internal ).release() ;
			}

		}else if( value.contains( "kde" ) ){

			if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::kwallet ) ){

				w = LXQt::Wallet::getWalletBackend( LXQt::Wallet::BackEnd::kwallet ).release() ;
			}else{
				_settings.setValue( opt,"internal wallet" ) ;
				w = LXQt::Wallet::getWalletBackend( LXQt::Wallet::BackEnd::internal ).release() ;
			}
		}else{
			QString wallet( "internal wallet" ) ;

                        if( value != wallet ){

                                _settings.setValue( opt,wallet ) ;
			}

			w = LXQt::Wallet::getWalletBackend( LXQt::Wallet::BackEnd::internal ).release() ;
		}
	}else{
		if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::kwallet ) ){

                        _settings.setValue( opt,QString( DEFAULT_KDE_WALLET ) ) ;
			w = LXQt::Wallet::getWalletBackend( LXQt::Wallet::BackEnd::kwallet ).release() ;

		}else if( LXQt::Wallet::backEndIsSupported( LXQt::Wallet::BackEnd::libsecret ) ){

                        _settings.setValue( opt,QString( "gnome wallet" ) ) ;
			w = LXQt::Wallet::getWalletBackend( LXQt::Wallet::BackEnd::libsecret ).release() ;
		}else{
			_settings.setValue( opt,QString( "internal wallet" ) ) ;
			w = LXQt::Wallet::getWalletBackend( LXQt::Wallet::BackEnd::internal ).release() ;
		}
	}

	_settings.sync() ;	

	w->log( []( const QString& e ){

		Q_UNUSED( e )
	} ) ;

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

QString configurationoptionsdialog::clientID()
{
        QString opt = _getOption( "clientID" ) ;

        if( _settings.contains( opt ) ){

                return _settings.value( opt ).toString() ;
        }else{
                QString e = "90790670661-5jnrcfsocksfsh2ajnnqihhhk82798aq.apps.googleusercontent.com" ;
                _settings.setValue( opt,e ) ;
                return e ;
        }
}

QString configurationoptionsdialog::clientSecret()
{
        QString opt = _getOption( "clientSecret" ) ;

        if( _settings.contains( opt ) ){

                return _settings.value( opt ).toString() ;
        }else{
                QString e = "LRfPCp9m4PLK-WTo3jHMAQ4i" ;
                _settings.setValue( opt,e ) ;
                return e ;
        }
}

QString configurationoptionsdialog::audioPlayer()
{
        auto opt = _getOption( "audioPlayer" ) ;

	if( _settings.contains( opt ) ){

                return _settings.value( opt ).toString() ;
	}else{
		QString e( "mplayer" ) ;
		_settings.setValue( opt,e ) ;
		return e ;
	}
}

QString configurationoptionsdialog::noEmailIcon()
{
	QString opt = _getOption( "noEmailIconColor" ) ;

	if( _settings.contains( opt ) ){

                return _settings.value( opt ).toString() ;
	}else{
		QString value( "grey" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::newEmailIcon()
{
        auto opt = _getOption( "newEmailIconColor" ) ;

	if( _settings.contains( opt ) ){

                return _settings.value( opt ).toString() ;
	}else{
		QString value( "blue" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::errorIcon()
{
        auto opt = _getOption( "errorIconColor" ) ;

	if( _settings.contains( opt ) ){

                return _settings.value( opt ).toString() ;
	}else{
		QString value( "red" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::fontFamily()
{
        auto opt = _getOption( "displayEmailCountFontFamily" ) ;

	if( _settings.contains( opt ) ){

                return _settings.value( opt ).toString() ;
	}else{
		QString value( "Helvetica" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::fontColor()
{
        auto opt = _getOption( "displayEmailCountFontColor" ) ;

	if( _settings.contains( opt ) ){

                return _settings.value( opt ).toString() ;
	}else{
		QString value( "black" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QString configurationoptionsdialog::defaultApplication()
{
        auto opt = _getOption( "defaultApplication" ) ;

	if( _settings.contains( opt ) ){

                return _settings.value( opt ).toString() ;
	}else{
		QString value( "browser" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return value ;
	}
}

QStringList configurationoptionsdialog::profileEmailList()
{
	if( !_profile.isEmpty() && _settings.contains( _profile ) ){

                auto z = _settings.value( _profile ).toString() ;

#if QT_VERSION < QT_VERSION_CHECK( 5,15,0 )
                return z.split( ",",QString::SkipEmptyParts ) ;
#else
		return z.split( ",",Qt::SkipEmptyParts ) ;
#endif
	}else{
		return QStringList() ;
	}
}

bool configurationoptionsdialog::usingInternalStorageSystem()
{
        auto opt = _getOption( "storageSystem" ) ;
	return _settings.value( opt ).toString() == "internal wallet" ;
}

int configurationoptionsdialog::fontSize()
{
        auto opt = _getOption( "displayEmailCountFontSize" ) ;

	if( _settings.contains( opt ) ){

                return _settings.value( opt ).toInt() ;
	}else{
		QString value( "80" ) ;
		_settings.setValue( opt,value ) ;
		_settings.sync() ;
		return 80 ;
	}
}

bool configurationoptionsdialog::displayEmailCount()
{
        auto opt = _getOption( "displayEmailCount" ) ;

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
        auto opt = _getOption( "networkTimeOut" ) ;

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
	QString opt( "audioNotify" ) ;
	_settings.setValue( opt,audioNotify ) ;
}

bool configurationoptionsdialog::autoStartEnabled()
{
        auto opt = _getOption( "autostart" ) ;

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
	QString opt( "autostart" ) ;
	_settings.setValue( opt,b ) ;
}

bool configurationoptionsdialog::reportOnAllAccounts()
{
        auto opt = _getOption( "reportOnAllAccounts" ) ;

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
        auto opt = _getOption( "language" ) ;

	if( _settings.contains( opt ) ){

                return _settings.value( opt ).toString() ;
	}else{
		QString lang( "english_US" ) ;
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
	return LANGUAGE_FILE_PATH ;
}

void configurationoptionsdialog::saveLocalLanguage()
{
        auto opt = _getOption( "language" ) ;
        auto language = m_ui->comboBoxLocalLanguage->currentText() ;
	_settings.setValue( opt,language ) ;
}

void configurationoptionsdialog::saveTimeToConfigFile()
{
        auto opt = _getOption( "interval" ) ;
        auto time = m_ui->lineEditUpdateCheckInterval->text() ;
	_settings.setValue( opt,time ) ;
	_settings.sync() ;
}

int configurationoptionsdialog::getTimeFromConfigFile()
{
        auto opt = _getOption( "interval" ) ;

	if( _settings.contains( opt ) ){

                bool ok ;
                auto time = _settings.value( opt ).toInt( &ok ) ;

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
        auto time = configurationoptionsdialog::getTimeFromConfigFile() / ( 60 * 1000 ) ;
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

        auto x = m_ui->lineEditUpdateCheckInterval->text() ;
	bool y ;
        auto z = x.toInt( &y ) ;

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

        auto s = m_ui->comboBoxBackEndSystem->currentText() ;

	this->saveStorageSystem( s ) ;

	emit enablePassWordChange( s == "internal wallet" ) ;

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
	QDir d( configurationoptionsdialog::localLanguagePath() + "/translations.qm/" ) ;

	QStringList l = d.entryList() ;

        l.removeOne( "." ) ;
	l.removeOne( ".." ) ;

        auto j = l.size() ;

        auto cbox = m_ui->comboBoxLocalLanguage ;

        QString x ;

        for( decltype( j ) i = 0 ; i < j ; i++ ){

                x = l.at( i ) ;
		x.truncate( x.size() - 3 ) ; //remove the .qm extension
		cbox->addItem( x ) ;
	}

	x = configurationoptionsdialog::localLanguage() ;

	j = cbox->count() ;

        /*
	 * have the user's preferred language highlighted
	 */

        for( decltype( j ) i = 0 ; i < j ; i++ ){

                cbox->setCurrentIndex( i ) ;

                if( cbox->currentText() == x ){

                        break ;
		}
	}
}
