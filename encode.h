#include "types.h"// brings in Status and uint
#include "common.h" //brings in MAGIC_STRING

typedef struct _EncodeInfo // structure is being created and typedef gives a convenient name
{
    /*we do this struct based approach because instead of passing 10 different variables as params to every function, we just pass a struct*/

    char *src_image_fname;//stores the image filename, beautiful.bmp, char * because filenames are strings
    char *secret_fname; //stores the secret file name , secret.txt
    char *stego_image_fname; //store the output filename, output.txt

    FILE *fptr_src_image; /*
    this variable stores the file pointer of the source image, 
    c interact with files using file pointers 
    when we do fopen("image.bmp",r) c returns a pointer that represents the open file stream, so we store that returned poitner here*/
    FILE *fptr_secret;//pointer to the secret file
    FILE *fptr_stego_image;// pointer to the output image

    /*file extension*/
    char extn_secret_file[10];/*this stores .txt or .pdf, we store this because we must reconstruct the original file, if the file was say secret.pdf, the decoder must recreate decoded.pdf after decoding, so if we dont store that info, we wont be able to reconstruct */

    int size_secret_file; //this stores how many bytes are in the secret file, secret.txt - 120 bytes, without this the decoder may not know when the secret file ends

    int image_capacity;/*how many bytes we can hide inside the image
    bmp images store RGB, so each pixel uses 3 bytes, so total image capacity is calculated from (width*height*3) 3 for that 3 bytes, 
    we know that each image byte can hide 1 secret bit, therefore total hiding capacity is derived from total image capacity*/ 
}EncodeInfo;

/* Function prototypes */

Status open_files(EncodeInfo *encInfo);

uint get_image_size_for_bmp(FILE *fptr_image);

uint get_file_size(FILE *fptr);

Status check_capacity(EncodeInfo *encInfo);

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

Status encode_byte_to_lsb(char data, char *image_buffer);

Status encode_data_to_image(char *data, int size,
                            FILE *fptr_src_image,
                            FILE *fptr_stego_image);

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo);

Status encode_secret_file_data(EncodeInfo *encInfo);

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

Status do_encoding(EncodeInfo *encInfo);

