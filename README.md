qCheckGmail
===========

qCheckGmail is a Qt/C++ multiple gmail account checker

Supported features:

-Multiple accounts

-Multiple lables(different lables are entered separated by a comma)

-Operations not blocking the UI and plasma-desktop

-Keys are stored in kwallet


How it works.
The first configured account is bechecked for new mail.
If it has some,check will stop and new mail will be reported in plasma icon buble.
If the account has no email,the first configured account label will be checked for new email.
If it has some,check will stop and new mail will be reported in plasma icon buble.

The above steps will repeat for all configured accounts and their labels.

This project seeks to replicate some of the functionality of this project: http://code.google.com/p/gmail-plasmoid/

The project appear to be dead and with annoying issues like:

http://code.google.com/p/gmail-plasmoid/issues/detail?id=79

http://code.google.com/p/gmail-plasmoid/issues/detail?id=81

