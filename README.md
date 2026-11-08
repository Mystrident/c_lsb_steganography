# LSB Image Steganography

## Overview
This project implements a BMP-based steganography tool in C. It hides a secret text file inside an image using the least significant bit (LSB) strategy and later extracts the hidden content by decoding the embedded bits.

## Problem statement
The project addresses the challenge of embedding information into a carrier file without noticeably changing the visual output. In this case, the carrier is a BMP image and the hidden payload is a plain text file.

The implementation must coordinate several concerns:

- preserve the BMP header so the file remains valid as an image
- encode the secret data and metadata in a recoverable layout
- convert text/data into bit streams and spread them across image bytes
- decode the same data reliably and restore the original file content

## Architecture and implementation
The repository is structured around a small set of C files:

- `main.c` handles command-line parsing and execution mode
- `encode.c` manages source image, secret-file validation, and the encoding pipeline
- `decode.c` reconstructs the hidden payload from the image
- `types.h`, `common.h`, `encode.h`, `decode.h` define shared structures and status codes

The encoding process is intentionally low-level:

1. validate that the secret file fits within the image capacity
2. preserve the first 54 bytes of the BMP header
3. encode a magic string to confirm the image contains hidden data
4. encode the secret file extension and file size
5. encode the file payload itself bit by bit into the image's LSBs
6. write the stego image to disk

Decoding reverses this process by scanning the LSB bits, reading the stored headers and payload length, and writing the recovered text to a new output file.

## Features actually implemented
- encode a text file into a BMP image using LSB steganography
- decode a hidden text payload from a stego image
- validate input and capacity before writing the payload
- maintain a magic signature to detect whether hidden data is present
- store the secret file extension and size before embedding the actual text
- preserve the BMP header while modifying only the pixel data area

## Technologies used
- C
- file I/O
- bitwise operations
- BMP file structure handling
- memory allocation and buffer management
- manual byte-order handling

## Engineering challenges faced
### 1. Bit-level encoding without damaging the image visually
The implementation does not rewrite the image arbitrarily; it changes only the least significant bit of selected bytes. The code comments explain this explicitly and the logic in `encode_byte_to_lsb()` shows the exact bit-clear and bit-insert sequence used to keep the image visually stable.

This is a confirmed low-level engineering decision, not a generic statement: the algorithm is intentionally built around `image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) & 1);`.

### 2. Correct file and payload layout for reliable decoding
The code stores several pieces of metadata before the actual secret data: a magic string, the extension, the file size, and then the payload. This design matters because the decoder must know exactly what to read and where to stop. The capacity check in `check_capacity()` confirms the implementation is designed to prevent data loss or incomplete recovery.

### 3. Handling BMP file offsets and byte ordering correctly
The image width and height are read from fixed offset positions in the BMP structure and the file pointer is deliberately moved to those offsets. The code also treats the payload length as a binary field, reinforcing that this project depends on correct file-format understanding rather than a high-level library.

### 4. Detecting whether hidden content exists before decoding
The decoder checks for a magic string before reading the rest of the payload. This is important because the same source image may not contain any hidden data, and the decoder must fail gracefully rather than producing garbage output.

## Important technical decisions
- The BMP header is copied untouched, which preserves the file as a valid image.
- The code encodes not only the payload but also metadata required to reconstruct it later.
- The payload is split into bits and stored in selected LSB positions, which is the defining trade-off of this steganography approach.
- The implementation treats the image as a binary carrier rather than a high-level abstraction, which is appropriate for a lower-level C project.

## Learning outcomes
- bit manipulation and binary data encoding in C
- structure-aware file handling for real file formats
- manual metadata layout and decoding logic
- understanding the trade-off between data capacity and visual preservation
- careful validation when working with binary file formats

## Installation and execution
### Compile
```bash
gcc main.c encode.c decode.c -o lsb_steg
```

### Encode
```bash
./lsb_steg -e beautiful.bmp secret.txt stego.bmp
```

### Decode
```bash
./lsb_steg -d stego.bmp decoded.txt
```

## Screenshots
No screenshots are committed in the repository snapshot. The sample files `beautiful.bmp`, `stego.bmp`, `secret.txt`, and `decoded.txt` are the main visual outputs used for validation.

## Repository information
- Repository: `c_lsb_steganography`
- Stack: C, binary I/O, BMP parsing, bit manipulation
- Primary focus: image steganography and hidden-data recovery
