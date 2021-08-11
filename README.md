![alt text](https://raw.githubusercontent.com/odrevet/gjitenkai/master/gjitenkai/gjitenkai.png)

[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)


# Gjiten kai

<i>A modern re-writting of Gjiten, a japanese dictionary</i>

# Goal
The goal of this rewrite of Gjiten is to have a Gjiten with new features and a
new interface, while keeping the original overall design. 

# Setup

## Edict 
To use the worddic (word dictionary) you will need an EDICT dictionary file.
Get one from https://www.edrdg.org/wiki/index.php/JMdict-EDICT_Dictionary_Project

Dictionary files can be downloaded directly from Gjitenkai in the ''preference/Download'' menu

Then set the worrdic file in Edit/preferences/ Worddic - Dictionary - New - Browse ...  

## Other

You also need Japanese font and a Japanse input system. See your OS documentation
for more details. 

# Documentation

The documentation in available in the project Wiki 

## Build from source / create an appimage 

https://github.com/odrevet/gjitenkai/wiki/build

## Learn how to search japanese expressions

By reading the use case located at https://github.com/odrevet/gjiten-kai/wiki/Search-Japanese-expressions

# Differences with the Original Gjiten 2.6

* Rewritten from scratch, with GTK3 and Glade
* Very fast (even on old computer)
* Regex search
* Configurable output color, font and character separator
* JMDict, JMneDict, EDICT, EDICT2 support
* gzipped and unzipped dictionary support
* Unicode and JP-EU encoding support
* Autoexpend kanji to it's radical list
* Threaded dictionary loading 
* Download dictionaries 

# Credits
* Gjiten kai developer (2015 - current) Olivier Drevet
* Special Thanks to the Original Gjiten developer (1999 - 2005) Botond Botyanszki 
* EDICT Dictionary Released under Creative Commons Attribution-ShareAlike Licence (V3.0)
