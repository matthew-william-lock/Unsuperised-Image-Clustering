#ifndef PPM_h
#define PPM_h

#include<stdio.h>
#include<stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

namespace LCKMAT002{

    typedef struct {
     unsigned char red,green,blue;
    } PPMPixel;

    #define RGB_COMPONENT_COLOR 255
    
    class PPM
    {
    private:
        int nrows;
        int ncols;

        PPMPixel ** image;
        std::string fileName;

    public:
        PPM(const std::string & filename);
        ~PPM();

        // Special Members ===========================================================================================================

        // Move Constructor
        PPM(PPM&& other) noexcept;

        // Assignment Operator
        PPM operator=(const PPM &M );

        // Move Assignment Operator
        PPM& operator=(PPM&& other) noexcept;

        // Copy Constructor
        PPM(const PPM& other);  


        //============================================================================================================================= 

    };
    

    

}


#endif