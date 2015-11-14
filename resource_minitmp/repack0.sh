#!/bin/bash

find textures > textures_tmplst.txt
../btjpack.exe -o textures.pk -l textures_tmplst.txt

zip -r basedata.pk fonts maps models shaders \
	lights game images voxel sprites tile2d
