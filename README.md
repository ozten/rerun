# rerun 
A linux command line program for watching a directory and rerunning a command
when it's contents change.

## BUILD
Requires gcc

    make

You can copy 'rerun' into a local ~/bin or some other location that is on your PATH.

## USAGE
    rerun DIRECTORY FILE_PATTERN COMMAND...

## DESCRIPTION
Watches  DIRECTORY  for  changes  in any file that matches FILE_PATTERN and then runs COMMAND. FILE_PATTERN should be in the glob format.

You probably want to wrap both the FILE_PATTERN and COMMAND in quotes, so  the  shell doesn't pre-expand patterns and the command is seen as a single argument.

Depends on inotify which is a Linux Kernel 2.6.13 and later feature.

## EXAMPLES
    rerun web/js "*.js" "juicer merge -min '' web/js/behavior.js"
    rerun web "*.css" bin/package_site.sh

## BUGS 
 * rerun doesn't watch directories recursively.
 * rerun doesn't handle lots of file modifications in a short amount of time well. It will call your command multiple times.
 * Usage isn't written in stone and can be improved


## LICENSE
This code is licensed under the Mozilla Tri-License MPL 1.1/GPL 2.0/LGPL 2.1

<pre>
================================77777$$$$$$Z$$$OZZZOOOOOOOOOOOOOOZZ7++++++++++++
=======================~===~~+I7777$$777$Z$$$$$$ZZ$OOOOOOOOOOOOOOOOZZ7===+++++++
=====================7$~==~=7777$7$77$$$$$ZZ$ZO$$$$ZOOOOOOOOOOOOOOOOOO$+=+====++
================~=~~=OZ~~=II7$$$$$$7ZZZZ$ZZOZOOZZ$$$$$$OOZOZOOOOOOOZZOOZ?+=+====
============~==~~~=~~ZZ++7777$$$$$ZZZZZZZZZZZZOZZZZ$Z$$Z$ZZZOOOOOOOOZOOOZ?======
==========~~~~~~~~~~~$$77777$$7$$$$7ZZZZZOOOOOOOOOZZOZZOZOZOZOOOOOOOZZOOOZ?=====
==~~~~~~~~~~~~~~~~~~~I7777$7$7$ZZZZZZZOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO$ZOOOO=====
~~=~~~~~~~~~~~~~~~~~?7I77Z$$$ZZZZZZOOOOOOOOOOOOOO88O8888888888888888OOZOOOO?====
~~~~~~~~~~~~~~~~~~~I777$$$$ZZZZZZZOOOOOOOOOOOO8888888888888888888888OOOOOOO$====
~~~~~~~~~~~~~~~~:~777$$Z$ZZZZZOOZOOOOOOOOOO888888DDDDDD888888888888888OOOOO$====
~~~~~~~~~~~~~~~~+77$7$$$$ZZOOOOOOOOOO8O888888DNMMMMMMMNM8DMIZNNN8OO88888OOO$~===
~~~~~~~~~~~~~~:+777$$$$ZZZOOOOOOOO88888888NMMMMMMMMMNDD8$77I??78MMND88O88OO+~===
~~~~~~~~~~~~~:+I77$$$$$ZOOOOOOOOO8O888DNMMMMMMMMMMMMMMMMNMNMMMMMMMMMMNN8O8O==~~~
~~~~~~~~~~~~:=777777ZOZOOOOOO888888DNMNNND88OOO888OOZ$7I?????I7ZZ8DNMMMMNOO~~~~~
~~~~~~~~~:::+77777$ZZOOOOOOO8888DNNNDOOOZ$$ZZZOOZZZ$$$$$II????I77ZZO8MMMMMD~~~~~
~~~~~::~~::II7I7$ZOOOOOO88888DNNOZ$$ZZZZ7$Z$7$$ZZ$$777I?????III7777$ZOMMMMMI~~~~
:~~~:::::~?II7$ZZOOOOOO888DDNDOI++++I7777777$$$$7777I77$ZZ$$$$$$77777$DMMMMN=~~~
::::~::::I777$ZOOOOOO8O88NNNO$II7$$Z7?++I7777777I777$$Z888DDD8DDD888OZZNMMMN7~~~
~~::::::?I7ZZOOOOOO888DNMNNDN8NNNNDNN$I?+I7II777I77$Z8DDNNMMMMMMMMMMMM8OMMMM7~~~
::::::~I7$ZZZOOOOO88DNMMMMMNND88O$I??I7I+=+I?I77I77ZZO8888DDNNMNMMMMMMN8NMMM$~~~
:~:::=7$ZZOOOOOO88DMMMMMMNDO$7?II+++==++===???I77$7$ZZZOOO88OZ$7$Z$ONMNDDMMNN~~~
::::=I7ZZZOOOO888NMMNNMND$Z$$$$77???++=====??I7777$ZZZOOOOZ$$77II77Z8NND8NMMN~~~
:::~7$ZZOOOOO88DMMMMN8ZZZ$7IIII?II77?+?+=~++?I777$7$Z8OZ$$ZO888OOOOZODDDODNMN~~~
::=7$ZZOOOO88DMMMMNND$I$7I????7$$OO$$$I7I++++??7$$$Z8O$7DO77?~?88DDDD888ZONMN~~~
::7$ZZOOOO88NMMMMMND8$I$$Z$OZ7+$D8DDN8$Z?7++=+?II7ZD8ZDOZMO$I$8MNNDNND8OZZDMN~:~
:=$ZOOOOO88NMMMMMND8Z77$ZZ8Z7M7,,..INDNO+++=+?+=?7Z8ODI8~.:$=..INNNNNN8OZZ8NN=::
:7ZOOOOO8DNMMMMMMNO$$I7$$ODN=..ZMMM,:7DD++?I?+=~=??O8MZ:...MNM==IDDN88ZZOOZNN+::
?$ZOOOO8DNMMMMMMNDZ$7+??7$DZ:.,O,M8??O8O7$77?+=~~+?7ZOZZ7?DMNO?7$N8O$$I7ZZZN7D=:
7ZOOO88NNNMMMMMMDO$$7II77Z888$I?$O88DO8O7III?+=~~+?7?IOOOZ8DD8ZDNNO7I??I$ZODD8::
7ZOO88DNNNMMMMMM8OZ$$$Z888OOOZZOO$$ZO7I??III?=~::~+II???OD8DOOZ777?+++??7ZZOO7::
IZOO88DDNNMMMNNNOOZZZOOO8OZ$I?+?7$$$7I??I7II?=:,,:+IIIIII?$ZZ$I+=~~~++??7$ZZ$I::
=ZOOO88DDNMMMNDDOOZZZ$$$$$$$$777$77I????III?=~:,::=7I?7I77I?++++??+++?II$$ZOO?::
:IZOO88DDNMMDZO8ZZZZ$77I?IIIII?IIII?I++II??+=~~++=~??=+7Z77I?+=~~~~=+?I7$ZZOO7::
,,ZZOO8DDNN87ZZ8ZZ$$7777III??+??II77$Z??++=~~:~?I7III+==IOZ$7II?++==++I7$ZOOOZ?:
,,=ZOOO8DNN$OOOOZZ$$7III?++++???77$OD??+===+==?7$$$OO$777INDO$7I7????I77ZZOOO$I:
,,,:$ZOO8DDIO8DOZZ$777?+=~~~=+=I$ZODZIII$8O77$7ZZ8DDNNNDD8MDZ$$$$7I77$ZZOOOOO$?:
,,,,:,~7ZO8IIO8ZZZ$77I?=~~:~=+II$Z8ZI?$8DZ=+7Z$O8NNDNNMN8DDO$$$$$$$7ZOO888888OI:
,,,,,,,,,,,$IZZ$ZO$777I+=+~=?77$Z88?+???++~=?I$$ZO8OOZZ$$$8DOZZOZZZZO88D8DD888$:
,,,,,,,,,,,77$Z$ZO$777I?I++?I7$Z8D7++????+II$Z$ZO88OZZ$Z$ZOD8ZZOOOOODDDDDDDD88$~
,,,,,,,,,,,$$Z$$$O$$$$$777I77$ZODOII?III$7I$O8OOZOO$$$$$$ZDDD8OO888DDNNDNND888$$
,,,,,,,,,,,$7Z$7$$ZZZZZZZ$$7$Z8NO77II7II7II7$ZZZZ$$7I7Z7$ZDNNDDDDDDDNNNDDDDD8DZ8
,,,,,,,,,,,$7Z$777$$$ZZZZZZZZ8NOZ7$$$7I?+===+??7$$$$$7$I$O8NNNDDDDDNDDDDNND888$,
,,,,,,,,,.$ZOZ$777$$$$77$$$$OOZ7DZZ$I?IIII?I7$ZO88O8DDDDDZ8NMNDNDDDDDDDDDDD888ON
:~++?II7$D888Z$77777$$$7777$7ZZO8$77$Z$$OO,:...=.,~??7DNNNNNMNDNDDDDDDDDDDD88DO+
ZZZZOZOZDN$MDZ777II77$$7777777$OZZOND:.,.I.....~.::?:?+~=8NNNNN888DD8DDDDDDDD88O
OOOOOOZ+?7.+?O$7IIIII7I7III77IZO7$ZNNMNNO7+I7++??77O8DNMMDDN8888O8DDDDDDDDDDDD8N
OOOOZI8NM8MMDO$777IIIIIIIII7II$ZZ$$ONMN8?=~:::~===?7ZDON8O8NZO8O88DDDDDDDDDDDDII
OOOOOZ8NNZMMDOZ$77IIIII7I7I$777$$II?ONN8?~::,:,:~++?7ODDZOD8OOO88DDDDDDDDDDDDDD?
OOOOIZODDDD8ZOZZ$7II7II777777$$$III+IZ8N$+::,:,~~=~~IO$$$O88OZO88DDDDDDDDDDDDDOZ
OOZ$ZZ8DIDD8OOZZ$$77II77777777$$I????I$Z8O+=~~:~~~~=?7Z7Z888ZO8DDDDDDDDDDDDDDD88
OOZ7ONND8MMDO8OOZ$77II777$$7$$$$II????I77$ZOI?+====ZZI7Z888OOO8DDNDDDDDDDDDDDNMD
OOZZ$$ZIMMMD88OZZZ$77I7I7777$7$7II?++?I???????7$$I??I7OO888OOO8DDNDDDDDDDNNDNMNN
7+,78DM$OZ87I7$OZ$$Z$7III77$$$77III?++I??++=+++=?I7$Z8O8888OO8DNDNDNDDDNNNNNO$MM
ZZ$Z8DONMMNDD8M~OZZZ$77777$7$$77I??I?+?7II??I77$ZZOO8DDD888OO8DDDNDDDDNNNNNOMMON
7IZZZO=DDDDDD8M:OOOZZ$$$77$$$$77II??????7$ZZOOOOO8DDDN8888OOODDDNNDDNDNNNNNZDNNZ
Z+ZZ$IIDND88O8D~ONOOOOZ$777$$$77I????I???I$ZZZODDDDDD8888OOOODDNNDDDNNNNNM8O8NNO
$?Z$Z~8DD88DDNN+ZN8O8OZ$$77$$$7$II+?II???II7$$ZZOOOO8O88OZZO8DDNNDDDNNNNMNZO8DMM
=ZZZ$INDOOO8DNN?7OMMNOOZZ$$$$$$$7I???III?II77$7ZZOOOOOOO$ZZODDNNNNNNNNNM$O88DDND
$ZOO:DN8ODDDNNN+ZO8$MM8OOZ$$$$7$$III?IIIIII$$7Z$ZOOZOZZ$Z$ODNNNNNNNNNMDN8N=+?I7D
?II7~I=.~?ODMMN8DZ8NMMMD8OOO$ZZZZ$7III?II??7$ZZZZZZZZZ$ZZODNNNNNNNNMMDOO$MD888Z7
Z8O8DN8==+7Z8Z.=DNDNNNNNND88OOZ$ZOZ7777??I7$$$Z$$Z$ZZZZZODNNNNNNNMMM88O8?NNZOZZ$
ZZO:NN8ZZOZONM+M7+8DD88O8NNN88OOZZ8OZ$77I?7$ZZZOOZ$OOOODNNMMNNNMMMNN=OOO~8DO$8OI
O8ONND8I$OZZZ$?ZNNNDOOO8DDDOMMD8OOOO88OZZ$ZOO88DOO88DDNNMMNNNNMMMMNN=8O88+~Z7,=$
OO8D8OD77D8$ZZZ888D8DDNMMN$NNDMMMND8888NDD8DDDDDDDNNNNMMNMMMMMMNDNMNZDDMMMMNNNNN
OO8$+??MMN7:?88D8D~?$DNMM7MMNDNMMMMNDDD88DNNNNNNNNNNMMNNMMMMMN88DNDMDDZNMMMNDN$8
OODN??OII77ZDNMN7=O8$~=D?7OMDNDNMMMMMMMNNNNNNNNNNNNNNMMMMMNNNMM8+I=ZM8ZMMNNDN.~Z
O8ZZ~+Z$7$ZZO88OODOOONN$$M8=~=7IMMMMMMMMMMMMMMMMMMMDDNNNMMM7DI7N8OO8MM8ZMMNIM?+=
OOI++7Z8DD877IZOOOZ$ZDD7ND88+88MOI+MMMNNNNNNNMNNMMMMD7~7ZDMM88ND7$$ODO+NMNZ78?+=
ODN8DDD8~=78MDND88$I8D~8Z8O8ZOD8M=MMMMMNNNNNNNNDO,IONNN8N88O$:?8O+7O8$+NN8$Z8+++
                                                             via GlassGiant.com
</pre>
