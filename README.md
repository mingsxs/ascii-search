# ascii_search
  A very handy tool to convert/search between `ascii` strings/characters and corresponding `ascii` values.

## Introduction  

  This simple utility is implemented with `C`, using standard libraries `stdio.h`, `stdlib.h` and `string.h` only, which lends enough portability among various platforms. It's designed for coding especially for embedded coding. If you often deal with massive, tedious `VPD field` programing with `Hex` or `Decimal` data format, this utility should be helpful to transform or validate data.

## Compiling  

  Just go with `gcc`/`clang` or any other `C` compiler following C99 standard, the executable `ascii` under folder `./bin/MacOS` is compiled by `clang` of MacOS `Mojave` with `-O2` level compiler optimization. If you are Mac users, use this executable directly.   
  
## Help  

  Only `--help`, `-h` or `-?` option given will get `help` usage, otherwise, the argument will be treated as an input string.  
<p align="center">
<img src="https://github.com/mingsxs/ascii_search/blob/master/screenshot/help.png" />
</p>

## How to use 
- #### `space`  
Command doesn't take `Null` argument, so if no argument given, default argument is `space`.  
   
<img src="https://github.com/mingsxs/ascii_search/blob/master/screenshot/help-space.gif" />

- #### character
<img src="https://github.com/mingsxs/ascii_search/blob/master/screenshot/help-single-char.gif" />

- #### string   
<img src="https://github.com/mingsxs/ascii_search/blob/master/screenshot/help-string.gif" />  

- #### single index   
<img src="https://github.com/mingsxs/ascii_search/blob/master/screenshot/help-single-index.gif" />

- #### multiple indexes  
<img src="https://github.com/mingsxs/ascii_search/blob/master/screenshot/help-indexes.gif" />  
  
## Feature  
- #### data format detection 
Utility detects data automatically, if a number string containing character `A-F` or `a-f` is given, string will be taken as `Hex` data, otherwise, it wil be taken as `Decimal` data, you can also add prefix `0x`/`0X` or suffix `h`/`H` to mark it as a `Hex` format data string.   
<img src="https://github.com/mingsxs/ascii_search/blob/master/screenshot/format-detect.gif" />   
    
If some invalid data is taken, it will print error message.   
   
<img src="https://github.com/mingsxs/ascii_search/blob/master/screenshot/wrong-format-detect.gif" /> 
   
- #### overflow detection  
If input data exceeds `127`, it will print `overflow` error message.   
<img src="https://github.com/mingsxs/ascii_search/blob/master/screenshot/overflow-detect.gif" />   
