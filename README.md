# [HyperSheetsPreprocessor (HSPP)](https://utilizer.de/hspp/)
HSPP is an optimizer and minifier for CSS files written in C++11.

## Why use HSPP?
 * To save your servers bandwidth
 * To speed up network transfer (in most cases it will also lead to better gzip results)
 * To speed up parsing by the browser
 * To make your production code harder to read for the others (obfuscation).\
   At the same time your original code will stay unchanged.

## The following operations can be performed by HSPP:
 * Removing of optional whitespace
 * Removing comments (preserving comments containing specific terms is possible)
 * Number minification
 * Color minification
 * Minification of IDs
 * Minification of class names
 * Minification of custom property names
 * Minification of keyframes animation names
 * Removing of empty rules

## Prebuilt binaries
Visit the [download page](https://utilizer.de/hspp/?nav=3) to download
the prebuilt binaries for Linux and Windows.

## How to use?
Visit the [documentation page](https://utilizer.de/hspp/?nav=1) to get started.

## License
This project is published under the GNU General Public License Version 3 (GPLv3).\
[Read more](https://github.com/excurso/hspp/blob/master/LICENSE)
