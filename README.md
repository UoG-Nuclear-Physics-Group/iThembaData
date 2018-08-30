# iThembaData

This library provides the data parser and the detector classes for data taken at iThemba with the tape station.
The names of the classes typically start with TTdr.

The data parser is set up to process data from the UK midas DAQ.

Detectors implemented are the tigress detector, the clover detectors, the plastic scintillator, and the Si(Li) detector. 
The two types of germanium detectors also have their BGO shield implemented.

Other classes included are:
 - the file and event classes for TDR files,
 - the TTdrMnemonic class that provides an enumeration for the detector systems,
 - the TTdrDetectorInformation which sets the build mode to be timestamp based, and the functions determining which detectors are present in the data, and
 - the TTdrDetectorHit which overrides the GetTime function of the TDetectorHit with the approriate algorithm.
