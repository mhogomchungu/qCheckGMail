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

#define DEFAULT_KDE_WALLET     "default kde wallet"
#define qCheckGMail_KDE_wALLET "qCheckGMail kde wallet"

configurationoptionsdialog::configurationoptionsdialog( QObject *,settings& s,configurationoptionsdialog::Actions ac ) :
	m_ui( new Ui::configurationoptionsdialog ),
	m_actions( std::move( ac ) ),
	m_settings( s )
{
	m_ui->setupUi( this ) ;

	//this->setFixedSize( this->size() ) ;
	m_ui->pushButtonClose->setMinimumHeight( 31 ) ;

	this->setWindowFlags( Qt::Window | Qt::Dialog ) ;

	connect( m_ui->pushButtonClose,&QPushButton::clicked,this,&configurationoptionsdialog::pushButtonClose ) ;

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

	auto value = m_settings.storageSystem() ;

	auto j = m_ui->comboBoxBackEndSystem->count() ;

	for( decltype( j ) i = 0 ; i < j ; i++ ){

		m_ui->comboBoxBackEndSystem->setCurrentIndex( i ) ;

		if( m_ui->comboBoxBackEndSystem->currentText() == value ){

			break ;
		}
	}

	this->installEventFilter( this ) ;

	auto co = m_settings.noEmailIcon() ;

	this->setWindowIcon( QIcon::fromTheme( co,QIcon( ":/" + co ) ) ) ;

	this->ShowUI() ;
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

void configurationoptionsdialog::ShowUI()
{
	auto time = m_settings.checkForUpdatesInterval() / ( 60 * 1000 ) ;

	m_ui->lineEditUpdateCheckInterval->setText( QString::number( time ) ) ;
	m_ui->checkBoxAutoStartEnabled->setChecked( m_settings.autoStartEnabled() ) ;
	m_ui->checkBoxAudioNotify->setChecked( m_settings.audioNotify() ) ;
	m_ui->checkBoxAlwaysShowTrayIcon->setChecked( m_settings.alwaysShowTrayIcon() ) ;

	this->setSupportedLanguages() ;
	this->show() ;
}

void configurationoptionsdialog::HideUI()
{
	m_settings.enableAutoStart( m_ui->checkBoxAutoStartEnabled->isChecked() ) ;

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

	m_settings.saveLocalLanguage( m_ui->comboBoxLocalLanguage->currentText() ) ;

	m_settings.setCheckForUpdatesInterval( m_ui->lineEditUpdateCheckInterval->text() ) ;

	m_settings.setAudioNotify( m_ui->checkBoxAudioNotify->isChecked() ) ;

	auto s = m_ui->comboBoxBackEndSystem->currentText() ;

	m_settings.saveStorageSystem( s ) ;

	auto m = m_ui->checkBoxAlwaysShowTrayIcon->isChecked() ;

	m_settings.setIconAlwaysVisible( m ) ;

	m_actions.alwaysShowTrayIcon( m ) ;
	m_actions.configurationWindowClosed( z * 60 * 1000 ) ;
	m_actions.audioNotify( m_ui->checkBoxAudioNotify->isChecked() ) ;
	m_actions.enablePassWordChange( s == "internal wallet" ) ;

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
	QDir d( m_settings.localLanguagePath() + "/translations.qm/" ) ;

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

	x = m_settings.localLanguage() ;

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

configurationoptionsdialog::actions::~actions()
{
}
