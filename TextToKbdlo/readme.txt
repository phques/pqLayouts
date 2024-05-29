
Very, very simple C# program to read a text file and generate 'K2K' entries for each read keys.
Console app, parameter is input filename, outputs to console.

It simply reads in 3 sections: template, main layer, shifted layer
Each key is separated by space(s).
Each section must have exaclty the same number of rows and nbr of keys for each corresponding row in the template

Template represents the keyborad we want to remap,
mains and shifted layers are the output we want for each key.


Format is :

empty line(s)
template

empty line(s)
main layer

empty line(s)
shift layer


 .. any text after the empty lines after the shifted section is ignored,
 can be used to enter notes, comments etc

Example:

CAPS a s d f g h j k l ; ' CR
LSH  z x c v b n m , . / RSH


 -     i  h  e  a  ,     d  s  t  n  r  w  z
 /      j k  LSH  .  ;    f  g  m  b  RSH =

 _     I  H  E  A  <     D  S  T  N  R  W  Z
 ?      J  K LSH  >  :     F  G  M  B  RSH +


This will generate this text, to be used inside a .kbdlo 'low level layout description' :

! Generated from [./example1.txt]
K2K CAPS  - _
K2K a  i I
K2K s  h H
K2K d  e E
K2K f  a A
K2K g  , <
K2K h  d D
K2K j  s S
K2K k  t T
K2K l  n N
K2K ;  r R
K2K '  w W
K2K CR  z Z

K2K LSH  / ?
K2K z  j J
K2K x  k K
K2K c  LSH LSH
K2K v  . >
K2K b  ; :
K2K n  f F
K2K m  g G
K2K ,  m M
K2K .  b B
K2K /  RSH RSH
K2K RSH  = +

