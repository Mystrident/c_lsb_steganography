#ifndef TYPES_H
#define TYPES_H
/*
HEADER GUARD
Imagine:
encode.c
includes common.h

common.h
includes types.h

encode.h
also includes types.h

Without guards the compiler sees:

types.h
types.h
types.h

Multiple definitions → compilation error.

Header guards stop that.*/

typedef enum{
    /*enum means enumeration, suppose we have list of colors RED,BLUE,ORANGE, instead of using numbers like 0,1,2 ; enums make code more readable
    typedef creates a new data type
    so, we are creating a new datatype based on enum*/
    e_success,
    e_failure
    /*defines possible values, internally compiler assigns e_success=0 e_failure=1*/
}Status; //type name called Status

/*Many functions in project return either:
success or failure

Status open_files(EncodeInfo *encInfo); The function returns:

e_success or e_failure

That’s cleaner than returning random integers.

Inside the machine the compiler still stores numbers:

e_success → 0
e_failure → 1*/

typedef unsigned int uint;
/* usually int value ranges from -2,147,483,648  to  +2,147,483,647 , 32 bit integer range
but image size, file size, image width etc will never be negative, so we use unsigned int , since we now are only using positive numbers , range will be from 0  to  4,294,967,295

so now uint becomes a new data type name because of typedef

example : uint get_image_size_for_bmp(FILE *fptr_image);*/

#endif