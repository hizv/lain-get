# lain-get
Download files from imageboards.
## Features
* Specify file types
* Download from a large number of imageboards (5)

## Requirements
* pip
* cmake

## Installation
```
curl http://0x0.st/i49Y.sh | bash
```
## Usage Examples
* Download all files from thread no. 3670653 of 4chan's /jp/ board.
```
lget -c4 jp/3670653
```
* Download all PDFs from thread no. 2730 of lainchan's /λ/ board.
```
lget lambda/2730 -fpdf # since lainchan is the default site
```
* Download all PNGs and JPEGs from thread no. 147 of 8chan's /rec/ board.
```
lget -c8 rec/147 -f{png,jpg}
```
## Supported Forums
* [lainchan](https://lainchan.org)
* [4chan](https://4chan.org)
* [wizchan](https://wizchan.org)
* [arisuchan](https://arisuchan.jp/)
* [8chan](https://8ch.net/)
