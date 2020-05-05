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
        PPM(const std::string & location,const std::string & filename);
        ~PPM();

        unsigned char getBWPixel(int y, int x);
        unsigned char getR(int y, int x);
        unsigned char getG(int y, int x);
        unsigned char getB(int y, int x);
        int getNrows();
        int getNcols();
        std::string const getFilename();

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