#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include <QString>
#include <QCloseEvent>
#include <QVector>
#include <QTableWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QCursor>

#include "accounts.h"
#include "kwallet.h"

namespace Ui {
class configurationDialog;
}

class configurationDialog : public QDialog
{
	Q_OBJECT
public:
	explicit configurationDialog( KWallet::Wallet ** wallet = 0,QWidget * parent = 0 ) ;
	void ShowUI( void ) ;
	~configurationDialog();
signals:
	void configurationDialogClosed( void ) ;
private slots:
	void pushButtonAdd( void ) ;
	void pushButtonClose( void ) ;
	void pushButtonDeleteEntry( void ) ;
	void checkBoxUnMaskPassphrase( bool ) ;
	void tableItemClicked( QTableWidgetItem * ) ;
	void tableItemChanged( QTableWidgetItem *,QTableWidgetItem * ) ;
	void walletOpened( bool ) ;
	void deleteRow( void ) ;
private:
	void HideUI( void ) ;
	void closeEvent( QCloseEvent * ) ;
	Ui::configurationDialog * m_ui ;

	QVector<accounts> m_accounts ;
	KWallet::Wallet * m_wallet ;
	KWallet::Wallet ** m_wallet_p ;

	int m_deleteRow ;
	QTableWidget * m_table ;
};

#endif // CONFIGURATIONDIALOG_H
