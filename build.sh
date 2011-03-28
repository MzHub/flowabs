#!/bin/sh
if [ `uname` = "Darwin" ]; then
	rm -rf flowabs.app flowabs.dmg
	qmake -spec macx-g++ -config release
	make
	macdeployqt flowabs.app -dmg
else
	qmake -config release
	make
fi
