#include <stdio.h>
#include <string.h>

#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
/*
argc = argument count
argv = argument values

./lsb_steg -e beautiful.bmp secret.txt stego.bmp

argv[0] → ./lsb_steg
argv[1] → -e
argv[2] → beautiful.bmp
argv[3] → secret.txt
argv[4] → stego.bmp*/
{
    EncodeInfo encInfo;

    if(argc < 2)
    /*
    If the user types only :
        ./lsb_steg
    There are no arguments.
    So we print usage instructions.*/
    {
        printf("Usage:\n");
        printf("./lsb_steg -e <source.bmp> <secret.txt> <stego.bmp>\n");
        printf("./lsb_steg -d <stego.bmp> <output.txt>\n");
        return 1;
    }

    /* Encoding mode */
    if(strcmp(argv[1], "-e") == 0)
    {
        if(argc != 5)
        /*
        Correct encoding syntax requires:
        program name
        -e
        source image
        secret file
        output image*/
        {
            printf("ERROR: Invalid arguments for encoding\n");
            return 1;
        }

        encInfo.src_image_fname = argv[2];
        encInfo.secret_fname = argv[3];
        encInfo.stego_image_fname = argv[4];
        //storing file names in structure

        /* extract extension from secret file 
        
        IMPORTANT: strrchr
        
        strrchr() finds the last occurrence of a character.
            example: secret.txt
        s e c r e t . t x t
            ^
        strrchr(argv[3], '.') returns pointer to:".txt" thus
        encInfo.extn_secret_file = ".txt"*/
        strcpy(encInfo.extn_secret_file, strrchr(argv[3], '.'));

        if(do_encoding(&encInfo) == e_success)
        {
            printf("Encoding completed successfully\n");
        }
        else
        {
            printf("Encoding failed\n");
        }
    }

    /* Decoding mode */
    else if(strcmp(argv[1], "-d") == 0)
{
    if(argc != 4)
    {
        printf("ERROR: Invalid arguments for decoding\n");
        return 1;
    }

    FILE *fptr_stego = fopen(argv[2], "rb");
    if(fptr_stego == NULL)
    {
        printf("ERROR: Unable to open stego image\n");
        return 1;
    }

    FILE *fptr_output = fopen(argv[3], "w");
    if(fptr_output == NULL)
    {
        printf("ERROR: Unable to create output file\n");
        return 1;
    }

    /* Skip BMP header */
    fseek(fptr_stego, 54, SEEK_SET);

    printf("INFO: Decoding magic string\n");

    if(decode_magic_string(fptr_stego) == e_failure)
    {
        printf("ERROR: No hidden data found\n");
        return 1;
    }

    printf("INFO: Decoding extension\n");

    char extn[10];
    decode_secret_file_extn(extn, fptr_stego);

    printf("INFO: Decoding secret file size\n");

    long file_size;
    decode_secret_file_size(&file_size, fptr_stego);

    printf("INFO: Decoding secret file data\n");

    decode_secret_file_data(fptr_stego, fptr_output, file_size);

    printf("INFO: Decoding completed successfully\n");

    fclose(fptr_stego);
    fclose(fptr_output);
}

    else
    {
        printf("ERROR: Unknown option\n");
        return 1;
    }

    return 0;
}