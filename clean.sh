#!/bin/bash
rm -rf `find -name \*.obj`
rm -rf `find -name \*.ilk`
rm -rf `find -name \*.pdb`
rm -rf `find -name \*.exp`
rm -rf `find -name \*.lib`
rm -rf `find */ -name \*.exe`
# rm -rf `find */ -name \*.dll`
