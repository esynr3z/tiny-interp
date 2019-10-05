# Simple C interpreter - parses command and arguments from a string

Pass LF-ended string to library - it will try to parse command, arguments and invoke appropriate callback.

* no platform-depended code
* no dynamic memory allocation
* supports cyrillic symbols
* printf() should be configured (in embedded systems)
* integer or string (with no non-letters and non-digits symbols) can be used as an argument
* auto strip of leading, trailing and duplicate spaces
* auto strip of non-letters and non-digits symbols
* upper case ignored

Example:
```
Command: 'set time'
Arguments: 'iii'

String to parse (very dirty):
  set TIME;%/*()+=-   0x16 ;№!0x25 0x30  

Will invoke callback for command: 'set time'
Arguments count (argc): 3
Arguments (argv): [0x16, 0x25, 0x30]
```