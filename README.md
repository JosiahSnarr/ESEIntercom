
ESE Intercom
============
Intercom and Paging System  

Project By: Natesh, Josiah, Shiv

Objective
---------

The goal of this project is to design and build a in house intercom and paging system that can 
communicate over house hold wiring. The Intercom will be able to send text messages to other station
 as well as audio messages.


Prerequisites
-------------

ESEIntercom uses QT as its user interface ( QT 5.3 ).
Required Qt libraries must be placed in the executable directory or findable on the system path.

How to Use
----------

Serial and Audio settings can be configured via settings menu. Configure the COM port settings and open a new session(Messages -> New Session).

To send text
* select a user from the user list.
*Enter text into the bottom window and press send to send text messages.

To send audio
* Press "Record"
* Stop when desired
* Optionally listen to the audio message before sending by pressing "Listen"
* Press "Send to Send"

Note: Audio streaming is partially implemented but not fully there. Audio will reach the other side and play however there are some "blank" spot.



Note
----
MainWindow class contains mainly ui code

