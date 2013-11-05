SLVision
========

Description:
------------
Second Light Vision System (SLVision) is a camera based tracking software developed for Microsoft Second Light. It can track several fingers, hand gestures on and above the table and ReacTivision inspired 6DoF markers.

Documentation online: 
---------------------
https://github.com/MTG/SLVision/wiki/SLVision


Installation HOWTO:
--------------------
This software compiles under visual studio 2010  (vc10) and needs a windows system with the openCV libraries installed and located at c:/. To compile it follow these steps:

easy way:
  * Install visual studio 2010 (this step is not nedded if you have it installed)
  * Download openCV 2.4.5 libraries [http://sourceforge.net/projects/opencvlibrary/files/opencv-win/2.4.5/OpenCV-2.4.5.exe/download]
  * extract openCV Libraries to "c:/opencv"
  * add C:\opencv\build\x86\vc10\bin to the windows path

"hard" way (if you need to use a diferent ocv location or newer ocv releases):
  * Install visual studio 2010 (this step is not nedded if you have it installed)
  * Download and extract openCV libraries
  * Modify project properties:
    - Project -> properties -> "VC++ Directories" -> Include Directories
    - Project -> properties -> "VC++ Directories" -> Library Directories
    - Project -> properties -> "Linker" -> "Input" -> Additional Dependences

