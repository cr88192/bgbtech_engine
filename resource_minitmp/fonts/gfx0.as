;header, 0xBF00, header-len
dw 0xBF00, 4
;header data, width height
db 0, 16, 16, 1

;span from 32 to 127
dw 0x20, 0x2A

;0x20
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........

;0x21
db 0x3F, 0xFC	;..xxxxxx xxxxxx..
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x01, 0x80	;.......x x.......
db 0x3F, 0xFC	;..xxxxxx xxxxxx..

;0x22
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x01, 0x80	;.......x x.......
db 0x03, 0xC0	;......xx xx......
db 0x06, 0x60	;.....xx. .xx.....
db 0x0C, 0x30	;....xx.. ..xx....
db 0x18, 0x18	;...xx... ...xx...
db 0x30, 0x0C	;..xx.... ....xx..
db 0x60, 0x06	;.xx..... .....xx.
db 0xFF, 0xFF	;xxxxxxxx xxxxxxxx
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........

;0x23
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0xFF, 0xFF	;xxxxxxxx xxxxxxxx
db 0x60, 0x06	;.xx..... .....xx.
db 0x30, 0x0C	;..xx.... ....xx..
db 0x18, 0x18	;...xx... ...xx...
db 0x0C, 0x30	;....xx.. ..xx....
db 0x06, 0x60	;.....xx. .xx.....
db 0x03, 0xC0	;......xx xx......
db 0x01, 0x80	;.......x x.......
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........

;0x24
db 0x00, 0x10	;........ ...x....
db 0x00, 0x30	;........ ..xx....
db 0x00, 0x70	;........ .xxx....
db 0x00, 0xD0	;........ xx.x....
db 0x01, 0x90	;.......x x..x....
db 0x03, 0x10	;......xx ...x....
db 0x06, 0x10	;.....xx. ...x....
db 0x0C, 0x10	;....xx.. ...x....
db 0x0C, 0x10	;....xx.. ...x....
db 0x06, 0x10	;.....xx. ...x....
db 0x03, 0x10	;......xx ...x....
db 0x01, 0x90	;.......x x..x....
db 0x00, 0xD0	;........ xx.x....
db 0x00, 0x70	;........ .xxx....
db 0x00, 0x30	;........ ..xx....
db 0x00, 0x10	;........ ...x....

;0x25
db 0x08, 0x00	;....x... ........
db 0x0C, 0x00	;....xx.. ........
db 0x0E, 0x00	;....xxx. ........
db 0x0B, 0x00	;....x.xx ........
db 0x09, 0x80	;....x..x x.......
db 0x08, 0xC0	;....x... xx......
db 0x08, 0x60	;....x... .xx.....
db 0x08, 0x30	;....x... ..xx....
db 0x08, 0x30	;....x... ..xx....
db 0x08, 0x60	;....x... .xx.....
db 0x08, 0xC0	;....x... xx......
db 0x09, 0x80	;....x..x x.......
db 0x0B, 0x00	;....x.xx ........
db 0x0E, 0x00	;....xxx. ........
db 0x0C, 0x00	;....xx.. ........
db 0x08, 0x00	;....x... ........

;0x26
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x01, 0x80	;.......x x.......
db 0x03, 0xC0	;......xx xx......
db 0x07, 0xE0	;.....xxx xxx.....
db 0x0F, 0xF0	;....xxxx xxxx....
db 0x1F, 0xF8	;...xxxxx xxxxx...
db 0x3F, 0xFC	;..xxxxxx xxxxxx..
db 0x7F, 0xFE	;.xxxxxxx xxxxxxx.
db 0xFF, 0xFF	;xxxxxxxx xxxxxxxx
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........

;0x27
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0xFF, 0xFF	;xxxxxxxx xxxxxxxx
db 0x7F, 0xFE	;.xxxxxxx xxxxxxx.
db 0x3F, 0xFC	;..xxxxxx xxxxxx..
db 0x1F, 0xF8	;...xxxxx xxxxx...
db 0x0F, 0xF0	;....xxxx xxxx....
db 0x07, 0xE0	;.....xxx xxx.....
db 0x03, 0xC0	;......xx xx......
db 0x01, 0x80	;.......x x.......
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........
db 0x00, 0x00	;........ ........

;0x28
db 0x00, 0x10	;........ ...x....
db 0x00, 0x30	;........ ..xx....
db 0x00, 0x70	;........ .xxx....
db 0x00, 0xF0	;........ xxxx....
db 0x01, 0xF0	;.......x xxxx....
db 0x03, 0xF0	;......xx xxxx....
db 0x07, 0xF0	;.....xxx xxxx....
db 0x0F, 0xF0	;....xxxx xxxx....
db 0x0F, 0xF0	;....xxxx xxxx....
db 0x07, 0xF0	;.....xxx xxxx....
db 0x03, 0xF0	;......xx xxxx....
db 0x01, 0xF0	;.......x xxxx....
db 0x00, 0xF0	;........ xxxx....
db 0x00, 0x70	;........ .xxx....
db 0x00, 0x30	;........ ..xx....
db 0x00, 0x10	;........ ...x....

;0x29
db 0x08, 0x00	;....x... ........
db 0x0C, 0x00	;....xx.. ........
db 0x0E, 0x00	;....xxx. ........
db 0x0F, 0x00	;....xxxx ........
db 0x0F, 0x80	;....xxxx x.......
db 0x0F, 0xC0	;....xxxx xx......
db 0x0F, 0xE0	;....xxxx xxx.....
db 0x0F, 0xF0	;....xxxx xxxx....
db 0x0F, 0xF0	;....xxxx xxxx....
db 0x0F, 0xE0	;....xxxx xxx.....
db 0x0F, 0xC0	;....xxxx xx......
db 0x0F, 0x80	;....xxxx x.......
db 0x0F, 0x00	;....xxxx ........
db 0x0E, 0x00	;....xxx. ........
db 0x0C, 0x00	;....xx.. ........
db 0x08, 0x00	;....x... ........

;0x2A
db 0xC0, 0x03	;xx...... ......xx
db 0xE0, 0x07	;xxx..... .....xxx
db 0x70, 0x0E	;.xxx.... ....xxx.
db 0x38, 0x1C	;..xxx... ...xxx..
db 0x1C, 0x38	;...xxx.. ..xxx...
db 0x0E, 0x70	;....xxx. .xxx....
db 0x07, 0xE0	;.....xxx xxx.....
db 0x03, 0xC0	;......xx xx......
db 0x03, 0xC0	;......xx xx......
db 0x07, 0xE0	;.....xxx xxx.....
db 0x0E, 0x70	;....xxx. .xxx....
db 0x1C, 0x38	;...xxx.. ..xxx...
db 0x38, 0x1C	;..xxx... ...xxx..
db 0x70, 0x0E	;.xxx.... ....xxx.
db 0xE0, 0x07	;xxx..... .....xxx
db 0xC0, 0x03	;xx...... ......xx

;terminal
dw 0, 0
