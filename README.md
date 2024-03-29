# qCheckGMail

This is a Qt/C++ tray based application that monitors gmail accounts and display unread emails in each configured label.

## Supported features

1. Can be build as a KDE tray application(default) or a stand alone Qt tray application.
2. Accounts credentials are stored securely either in libsecret, KWallet or in an internal storage system for those who do not want GNOME or KDE dependencies.
3. Multiple gmail accounts.
4. Multiple labels for each gmail account.
5. Automatic new email checking with configurable time interval.
6. Manual email checking.
7. Ability to pause and un pause automatic email checking.
8. Internationalization support(currently supports greek, spanish, french, dutch and german languages).

## Screenshots

![trayTooltip.png](https://raw.githubusercontent.com/mhogomchungu/qCheckGMail/master/images/trayTooltip.png)

![configureAccounts.png](https://raw.githubusercontent.com/mhogomchungu/qCheckGMail/master/images/configureAccounts.png)

![addAccount.png](https://raw.githubusercontent.com/mhogomchungu/qCheckGMail/master/images/addAccount.png)

![options.png](https://raw.githubusercontent.com/mhogomchungu/qCheckGMail/master/images/options.png)

## Privacy-policy

1. This application access GMail accounts with least amount of privileges and the best it can do is get a list of unread and total
emails from each label of an account and that is all it does.

2. This application simply asks how many unread emails are in a configured label and display them. This app does not even know
the email address of the account it is accessing.
