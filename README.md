```
   __                             __         ___ ___
  / /__ __ __ _   ___  __ __ ___ / /_ __ __ / _// _/
 / // // //  ' \ / _ \/ // /(_-</ __// // // _// _/
/_/ \_,_//_/_/_// .__/\_, //___/\__/ \_,_//_/ /_/
               /_/   /___/
```
## What?
lumpystuff is a simple command line tool which can dump lump data from Source
Engine BSP and LMP files.

## Why?
I'm currently working on a _secret project_ which should be out soon™.

## How?
The help text pretty much tells you everything:
```
Usage:
    lumpystuff.exe extract_bsp [input.bsp] [lump_id] > output.dat
Or: lumpystuff.exe extract_lmp [input.lmp] > output.dat
Or: lumpystuff.exe create_lump [output.lmp] [lump_id] [lump_ver] [map_rev] < input.dat
Or: lumpystuff.exe lump_params_bsp [input.bsp] [lump_id]
Or: lumpystuff.exe lump_params_lmp [input.lmp]

- where:
  * lump_id is a numeric identifier (for a list of valid IDs, see
    https://developer.valvesoftware.com/wiki/Source_BSP_File_Format#Lump_types)
  * lump_ver and map_rev are lump headers which should match those in the map
    (these can be found using lump_params_*).

The lump_params commands return space-separated values for lump_ver and
map_rev. A batch file or shell script can pass these to create_lump.
```
**Note**: This is a very simplistic (lazy) command-line interface, based on
pipes. The primary purpose of this tool is to be used in some other script as
part of an automated process; as such, not much effort was put into making it
user-friendly.

## Building
If you want to use this, you should be able to do simply by building the
included Visual Studio project. This currently targets Visual Studio 2015 with
the `v140_xp` toolset. It should be simple enough to build on other platforms
using a Makefile, but that isn't currently implemented.