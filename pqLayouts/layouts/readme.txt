
lo level keyboard definition text file
=====

nb: this software maps KEYS, not characters, this is how the Windows hook we use works !
 So to get '?' when we press key 'q', we need to send shift, then send '/' (ie Shift-/)
 For displayable characters (letters, digits, symbols on the kbd),
 the software can figure out if it needs to send Shift or not (or REMOVING shift)  before sending the key we want to output.

 Known 'keyToMap's (1st entry on k2k commands):
  single char: the key of that char,
  esc tab caps space enter cr backspace bs del
  lshift rshift 
  lcontrol rcontrol lctrl rctrl
  lalt ralt
  left right up down home end pgup pgdn ins
  F1-F12

+ one k2k mapping command per line,
+ command is 1st word on beginning of line,
! this is a comment (! at beginning of line)


Mapping commands
---------

+ k2k map key to key(s)
  any text after the 2 parameters is ignored (can be used for comments)

k2k a y			a outputs y, shift-a not mapped
k2k +a Y		shift-a outputs Y 

k2k d *			d outputs *,  shift-8 !!
k2k d +8		d outputs *, shift-8 same as prev example

k2k Tab Esc		tab outputs Escape
k2k +Tab +Esc	shift-tab outputs shift-escape
k2k +Tab Esc	shift-tab outputs non-shifted escape

k2k +SP ;		shift-space outputs ';' (NON shifted key ;)
k2k SP  " 		space outputs " (shift-')


+ K2K, k2k's big brother ;-)
  map both non-shifted and shifted output values for a key

K2K a  e  E
K2K s  sp "


+ k2ck for control-xx output

k2ck a  v   press a, outputs ctrl-v (pase)


Layer command
---------

addlayer layerName accessKey

This creates a new layer,
sets its access key on main layer
and switches to the new layer

 'main' is reserved layer name for the main layer.


