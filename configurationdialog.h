#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include <QString>
#include <QCloseEvent>
#include <QVector>

#include "accounts.h"
#include "kwallet.h"

namespace Ui {
class configurationDialog;
}

class configurationDialog : public QDialog
{
	Q_OBJECT
public:
	explicit configurationDialog( KWallet::Wallet * wallet = 0,QWidget * parent = 0 ) ;
	void ShowUI( void ) ;
	~configurationDialog();
signals:
	void accountsInfo( KWallet::Wallet * ) ;
private:
	void HideUI( void ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::configurationDialog * m_ui ;

	QVector<accounts> m_accounts ;
	KWallet::Wallet * m_wallet ;
};

#endif // CONFIGURATIONDIALOG_H
