qCheckGmail
===========

qCheckGmail is a Qt/C++ multiple gmail account checker

Supported features:

-Multiple accounts

-Multiple labels

-Operations not blocking the UI and plasma-desktop

-Keys are stored in kwallet

-Automatic new email checks with configurable time interval

-Manual email check

-Ability to pause and unpause automatic email checks


-The tray icon is the primary way the application informs the user of the state of the application
and of the email accounts.

On any error encountered,the tray icon will turn red and the pop up bubble with have
an explanation of the error.

After an update check,if new emails are found,the tray icon will turn bluish and the pop up bubble will
have an explanation of what accounts have new email.The icon will change its state to "need attention" and
plasma desktop will unhide it by default.

After an update check,if all accounts have no new email,the icon will turn greyish and will change its
state to "normal" and plasma desktop will hide it by default.

This project seeks to replicate some of the functionality of this project: http://code.google.com/p/gmail-plasmoid/

The project appear to be dead and with annoying issues like:

http://code.google.com/p/gmail-plasmoid/issues/detail?id=79

http://code.google.com/p/gmail-plasmoid/issues/detail?id=81

