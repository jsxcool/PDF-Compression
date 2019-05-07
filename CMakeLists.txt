cmake_minimum_required(VERSION 3.13)
project(pdf)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-pthread")

add_executable(pdf main.cpp parse.h readin.h compressAlgorithm.h PDFObject.h encodeMap.h decompress.h recoverPDF.h)

include_directories("/usr/local/include/qpdf")
target_link_libraries(pdf "/usr/local/lib/libqpdf.so")