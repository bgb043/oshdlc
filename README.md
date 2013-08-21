OSHDLC
======

OSHDLC is the software implementation of High Level Data Link Control (HDLC) standard. 
It is a fully optimized C-code and It utilizes a state table based, efficient algorithm. 
This code may be used on lots of platforms easily as desired.

This toolkit basically implements HDLC zero-bit insersion and deletion algorithm for packet creation and 
extraction.

How To Build
============

Use CodeBlocks IDE (http://www.codeblocks.org/) with MINGW compiler to build and test the toolkit.

If you desire to modify and rebuild the state tables use the project file under the folder "maker" which
will generate both transmitter and receiver state tables in files "hdlct_table.h" and "hdlcr_table.h".
Copy these files into folder "include" in order to test and utilize OSHDLC toolkit.

In order to test OSHDLC transceiver toolkit, use the project under the folder so called "tester".

When it comes to utilize OSHDLC transceiver in your real-time system use the codes under folders "source" and "include".

GOOD LUCK!



