#ifndef DECODE_H
#define DECODE_H

#include "common.h"
#include "types.h"
#include <stdio.h>

char decode_byte_from_lsb(char *image_buffer);

Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image);

Status decode_magic_string(FILE *fptr_stego_image);

Status decode_secret_file_extn(char *extn, FILE *fptr_stego_image);

Status decode_secret_file_size(long *size, FILE *fptr_stego_image);

Status decode_secret_file_data(FILE *fptr_stego_image, FILE *fptr_output,
                               long file_size);

#endif