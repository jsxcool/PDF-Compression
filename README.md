# PDF-Compression:
- This project can accomplish compress and decompress of Latex PDF File.
- The PDF file must be pure text. Picutres or Equations may be disregared or cause some errors.

## Shell 
1. The executive file is packaged in shell.sh file. First Run ./shell.sh 
  - Command of Compression:
    - ./pdf -c input.pdf outputFileName
  - Command of Decompression:
    - ./pdf -d input.bz2 outputFileName


## Prerequisition:
- qpdf library: for parsing PDF File
[qpdf](http://qpdf.sourceforge.net/)
- bzip2: compressing plain text file
- ps2pdf: converting postscript file to PDF file
- pthread c++ library: for multithreading

