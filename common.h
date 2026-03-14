// header guard
#ifndef COMMON_H
#define COMMON_H

#define MAGIC_STRING "#*"
/* #define is a preprocessor macro,replacement happens before compilation
If we write
#define SIZE 10
and later write
int a[SIZE];
the compiler actually sees:
int a[10];
The replacement happened earlier.

why have we written #*
when we encode a data in the image, the decoder must later understand that the
image its reading is an encoded one ie, we embed a signature at the beginning of
the encoded data, so when decoding begins and decoder stumbles upon #*   it
knows that this is an encoded img

PNG files start with: 89 50 4E 47
PDF files start with: %PDF
ZIP files start with: PK

what happens during encoding,
IMPORTANT:
we know that for 1 character, we need 8 lsb image bytes to encode,
so for '#' and '*' we would need 16 lsb image bytes

*/

#endif