#include<stdio.h> //provides fopen fread fwrite etc, ability to work with files
#include<string.h> //strlen strcmp strcpy etc
#include "decode.h"

char decode_byte_from_lsb(char *image_buffer){
    //this helps us construct one data byte from 8 image bytes
    char data=0;

    for(int i=0;i<8;i++){

        data=data<<1; // making space for the next bit
        data|=(image_buffer[i]&1); //extracting lsb and adding it to data
    }

    return data;
}

Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image){

    char image_buffer[8];

    for(int i=0;i<size;i++){

        fread(image_buffer,1,8,fptr_stego_image);
        data[i]=decode_byte_from_lsb(image_buffer);
    }

    return e_success;

    //this function repeatedly reads 8 image bytes until secret file size runs out , extract their lsbs and rebuild original bytes
}

Status decode_magic_string(FILE *fptr_stego_image){
    char magic[3]; //size 3 because magic string is #* and a null terminator

    decode_data_from_image(magic,2,fptr_stego_image); //reading 16 image bytes to read 2 bytes of magic data using this function from stego image and storing it inside magic string char array

    magic[2]='\0'; // null terminating the character array

    if(strcmp(magic, MAGIC_STRING)!=0){
        //checking whether the magic string we have extracted is same as the intended magic string
        printf("ERROR: Magic string mismatch\n");
        return e_failure;
    }

    return e_success;
}

Status decode_secret_file_extn(char *extn,FILE *fptr_stego_image){

    decode_data_from_image(extn,4,fptr_stego_image);
    extn[4]='\0';
    //similar to above function

    return e_success;
}

Status decode_secret_file_size(long *size, FILE *fptr_stego_image)
{

    //long *size is a pointer to a variable that will store the file size
    //we type cast ot below because we treat memory of size as a stream of bytes
    decode_data_from_image((char *)size,
                           sizeof(long),
                           fptr_stego_image);

    return e_success;
}

Status decode_secret_file_data(FILE *fptr_stego_image, FILE *fptr_output, long file_size){

    char ch;

    for(long i=0;i<file_size;i++){

        decode_data_from_image(&ch,1,fptr_stego_image);/*
    read 8 image bytes
    extract LSB bits
    rebuild character*/

        fwrite(&ch,1,1,fptr_output); //writing back to the secret file output

        /*Example reconstruction:

        M
        My
        My 
        My p
        My pa
        ...
        My password is SECRET ;) */

    }
    return e_success;
}