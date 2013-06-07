qCheckGmail
===========

qCheckGmail is a Qt/C++ multiple gmail account checker

Supported features:
-Multiple accounts
-Operations not blocking the UI and plasma-desktop
-Keys are stored in kwallet

How it works,when multiple accounts are configured,checks will done one account at a time and will stop when new emails are found.
This means,if new emails are in 2 different accounts,only the first account that was checked will report of its new emails.

This project seeks to replicate some of the functionality of this project: http://code.google.com/p/gmail-plasmoid/

The project appear to be dead and with annoying issues like:

http://code.google.com/p/gmail-plasmoid/issues/detail?id=79

http://code.google.com/p/gmail-plasmoid/issues/detail?id=81

