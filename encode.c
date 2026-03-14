#include<stdio.h> //provides fopen fread fwrite etc, ability to work with files
#include<string.h> //strlen strcmp strcpy etc
#include "encode.h"

Status open_files(EncodeInfo *encInfo){
    /* return type status, this function is used to open files, struct pointer is passed so that this fucntion can fill in the file pointers present inside the structure*/

    encInfo->fptr_src_image= fopen(encInfo->src_image_fname,"rb");
    /*encInfo-> means access a member of structure using pointer
    fptr_src_image stores the file pointer of source image
    encInfo->src_image_fname retrieves the filename stored in the struct
    
    now the program has access to the file image*/
    if(encInfo->fptr_src_image==NULL){
        printf("ERROR: unable to open file %s\n",encInfo->src_image_fname);
        return e_failure;
    }

    encInfo->fptr_secret=fopen(encInfo->secret_fname,"r");//storing file pointer of secret text
    if(encInfo->fptr_secret==NULL){
        printf("ERROR: unable to open file %s\n",encInfo->secret_fname);
        return e_failure;
    }

    encInfo->fptr_stego_image=fopen(encInfo->stego_image_fname,"wb");//program creates the stego image
    if(encInfo->fptr_stego_image==NULL){
        printf("ERROR: unable to open file %s\n",encInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image){
    //image size cant be negative so uint

    uint width, height;

    fseek(fptr_image,18,SEEK_SET);/*
    fseek(file_pointer, offset, position_reference)
    moves 18 bytes from start of file, SEEK_SET means from start of file, 
    In BMP 
    Byte offset 18 → image width
    Byte offset 22 → image height*/

    fread(&width,sizeof(int),1,fptr_image);
    /*
    fread(destination,size of each item, number of items, file)
    &width-address of variable width,tells c where to store data read from file
    sizeof(int)- we read 4 bytes, and BMP stores width as 4 byte integer
    1- we want to read one integer
    therefore this reads 4 bytes from byte 18 of BMP file and store it in width
    example width=1024
    
    after reading 4 bytes, file pointer is now moved to byte 22 from start, we now read height the same way
    */
   fread(&height, sizeof(int), 1, fptr_image);

   return width*height*3; //BMP uses RGB and each R G B has 1 byte each, so each pixel uses 3 bytes, so total pixel data is number of pixels = width *height, multiplied by 3
}

uint get_file_size(FILE *fptr){

    uint file_size;

    //Move file pointer to end of file
    fseek(fptr,0,SEEK_END);

    //get current position (which equals file size)
    file_size=ftell(fptr);

    //move pointer back to the beginning
    rewind(fptr);

    return file_size;
}

Status check_capacity(EncodeInfo *encInfo){
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image); //access member of structure encinfo and use a function to calculate the size

    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);

    if(encInfo->image_capacity>(strlen(MAGIC_STRING)+4+strlen(encInfo->extn_secret_file)+4 +encInfo->size_secret_file)*8){


        /*we must encode these things inside the image
        magic string
        extension size
        extension
        secret file size
        secret file data
        */

        /*strlen(MAGIC_STRING)->2 characters because magic string was '#*' 
        +4 -> because we store the extension size as integer and an integer uses 4 bytes
        strlen(encInfo->extn_secret_file) -> can be like .txt this would return length as 4 characters since '.txt' 
        next +4 -> stores the secret file size, file size would be an integer so 4 bytes
        encInfo->size_secret_file -> actual secret data 
        
        and totally everything is multiplied by 8, because to store one secret byte, we need 8 image byte, 
        */

        return e_success;
    }
    else{
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){

    /*
    the bmp header contains 54 bytes and even if we change the data of the image by encoding, the header of the bmp will remain the same and we should absolutely not tamper the header */
    char buffer[54];

    //move pointer to the beginning of the source image
    rewind(fptr_src_image);

    //read 54 bytes from source image
    fread(buffer,54,1,fptr_src_image);

    //write 54 bytes to the stego image
    fwrite(buffer,54,1,fptr_dest_image);

    return e_success;
    }

Status encode_magic_string(const char *magic_string,EncodeInfo *encodeInfo){
    /* magic string -> '#*' , const because the string cannot be modified
    and then the structure 
    */

    if(encode_data_to_image((char *)magic_string,strlen(magic_string),encodeInfo->fptr_src_image,encodeInfo->fptr_stego_image)==e_failure){

        // we are passing the magic string that needs to be encoded, size of the magic string, source and destination files to a function, that function will help us encode, then we check whether the encoding succeeded or failed
        
        return e_failure;

    }
    
    return e_success; //means magic string has been successfully encoded

}

Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo){

    /* .txt
ASCII values:
'.' = 46
't' = 116
'x' = 120
't' = 116
Binary:
.  = 00101110
t  = 01110100
x  = 01111000
t  = 01110100
Each of these bits is embedded into image bytes.
Example:
Image byte : 11001010
Secret bit : 1
Result     : 11001011
Only the least significant bit changes, so the image visually stays the same.*/

    if(encode_data_to_image((char *)file_extn,strlen(file_extn) ,encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure){
        return e_failure;
    }

    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    /*char data->one secret byte
    image_buffer->8 bytes from image
    
    we need to store 8 bits of data byte into 8 bytes of image
    
    example 'A'->ascii 65 binary 01000001-> we need to hide these 8 bits

    so the for loop runs 8 times
    */

    
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=(image_buffer[i]&0xFE)|((data>>(7-i))&1);
        /*image_buffer[i]->refers to current image byte and we are gonna be modifying its lsb 

        step 1: clearing the lsb
            image_buffer[i]&0xFE-> 0xFE means 11111110 in binary. Thus say image_buffer[i]=10101101
            When we perform AND:
                10101101
                        &
                11111110
                ---------
                10101100     The last bit becomes 0.

        step 2: extract secret bit from data
        (data>>(7-i))&1 -> we do right shift to move to the desired bit which we wanna insert in lsb and we isolate it using &1

        Example for 'A':
            01000001
        If i = 0:
            data >> 7
        Result:
            00000000
        If i = 7:
            data >> 0
        Result:
            01000001

        Step 3: inserting the secret bit
         we do or operation between step 1 and step 2 so that it inserts the secret bit

         Example:image byte  : 10101100
                secret bit  : 1
                -------------------
                result      : 10101101

                so clear->extract->insert
        */
    }
    return e_success;
    /*example

secret byte = 01000001

Image bytes before:

11001010
10101010
10011101
11100011
10101001
10010110
11110000
11011010

After encoding:

11001010
10101011
10011100
11100010
10101000
10010110
11110000
11011011*/
}

// we ll take this reference, the previous function was like an engine that encodes whatever data we give into the engine, now we build the crankshaft that gives to the engine repeatedly and that is 

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    //data -> secret data to hide, and size-> size of data

    char image_buffer[8];

    for(int i=0; i<size; i++){

        fread(image_buffer,1,8,fptr_src_image);

        //encode one data byte at a time to the image buffer
        encode_byte_to_lsb(data[i],image_buffer);

        fwrite(image_buffer,1,8,fptr_stego_image);
    }

    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo){

    /*file_size->number of bytes in secret file
    
    we send the file size and also ENCODE the file size just like we encode the SECRET DATA, data format, magic string because the decoder must know when to stop decoding
    */
    if(encode_data_to_image((char *)&file_size,sizeof(file_size),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_failure){
        return e_failure;
        /*
        we type cast file size ie (char *)&file_size because we pass in a long integer say 
        example:
        long file_size = 200; computer stores it like this
        00000000 00000000 00000000 11001000 assuming 4 byte integer representation
        
        in the encode_data_to_image(char *data, int size,..) we expect a char *data ie we are expecting a byte stream like this 11001000 etc not a long integer like 200
        
        so we convert the long integer to a byte stream and pass it as params for encode_data_to_image
        
        now
        Address   Data
        -------   ----
        1000      00000000
        1001      00000000
        1002      00000000
        1003      11001000
    file_size occupies those four bytes.

    When we write:
    (char *)&file_size
    we are basically saying:
    start reading bytes from address 1000*/
    }

    return e_success;
}

//now we will encode the actual secret data

Status encode_secret_file_data(EncodeInfo *encInfo){

    char buffer;

    while(fread(&buffer,1,1,encInfo->fptr_secret)==1){

        if(encode_data_to_image(&buffer,1,
                        encInfo->fptr_src_image,
                        encInfo->fptr_stego_image) == e_failure)
        {
            return e_failure;
        }
    }

    return e_success;

    /*example:
    My password is SECRET ;)
The first character read is M. ASCII value:77
Binary form: 01001101
encode_data_to_image takes that 'M' byte store it in 8 image bytes and other characters are respectively hidden this way

So the encoder hides data in this exact order:

#*          → magic signature
.txt        → extension
25          → file size
My password is SECRET ;)
    */
}

// IMPORTANT: sometimes the secretfile size may not match the whole image data size, that time, the secret data might be completely written inside the image and still some image bytes to be written into the image file for it to be properly displayed. so,

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;

    while(fread(&ch,1,1,fptr_src)==1){

        fwrite(&ch,1,1,fptr_dest);
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("INFO: Opening required files\n");

    if(open_files(encInfo) == e_failure)
    {
        printf("ERROR: Failed to open files\n");
        return e_failure;
    }

    printf("INFO: Checking capacity\n");

    if(check_capacity(encInfo) == e_failure)
    {
        printf("ERROR: Insufficient image capacity\n");
        return e_failure;
    }

    printf("INFO: Copying BMP header\n");

    copy_bmp_header(encInfo->fptr_src_image,
                    encInfo->fptr_stego_image);

    printf("INFO: Encoding magic string\n");

    if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Encoding file extension\n");

    if(encode_secret_file_extn(encInfo->extn_secret_file,
                               encInfo) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Encoding file size\n");

    if(encode_secret_file_size(encInfo->size_secret_file,
                               encInfo) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Encoding secret file data\n");

    if(encode_secret_file_data(encInfo) == e_failure)
    {
        return e_failure;
    }

    printf("INFO: Copying remaining image data\n");

    copy_remaining_img_data(encInfo->fptr_src_image,
                            encInfo->fptr_stego_image);

    printf("INFO: Encoding completed successfully\n");

    return e_success;
}