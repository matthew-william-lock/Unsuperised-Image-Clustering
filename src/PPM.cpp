#include "PPM.h"

namespace LCKMAT002 {

    PPM::PPM(const std::string &filename){
        using namespace std;
        fileName=filename;

        cout<<"Opening file "<<filename<<endl;
        ifstream fileStream(filename, ios::binary|ios::in );

        // Check if file is open
        if (!fileStream.is_open()){
            cout<<"Error opening file "<<filename<<endl;
            exit(1);
        }


        // Check that the file is a PPM file
        string line;
        if (getline(fileStream,line)){
            if (line !="P6"){
                cout<<"The file "<<filename<<" is not a PPM file."<<endl;
                exit(1);
            }            
        } else {
            cout<<"Error reading file "<<filename<<endl;
            exit(1);
        }

        // Discard Comments
        if (getline(fileStream,line)){
            while (line.at(0)=='#'){
               getline(fileStream,line);
            }                     
        } else {
            cout<<"Error reading file "<<filename<<endl;
            exit(1);
        }

        // Get  Nrows and Ncols 
        stringstream sStream(line); 
        string NrowsString;
        string NcolsString;
        getline(sStream,NrowsString,' ');
        getline(sStream,NcolsString,' ');
        nrows = stoi(NrowsString);
        ncols = stoi(NcolsString);
        cout<<"Number of rows "<<nrows<<endl;
        cout<<"Number of columns "<<ncols<<endl;

        //Read 255
        if (getline(fileStream,line)){
            if (stoi(line)!=RGB_COMPONENT_COLOR){
                cout<<"Error reading file "<<filename<<", no 255 found"<<endl;
                exit(1);
            }
        } else {
            cout<<"Error reading file "<<filename<<endl;
            exit(1);
        }
        
        // Write PPM file
        // ofstream binaryFile ("../bin/test.ppm", ios::out | ios::binary);
        // binaryFile<<"P6"<<endl;
        // binaryFile<<nrows<<" "<<ncols<<endl;
        // binaryFile<<"255"<<endl;        

        // Read image data
        image=new PPMPixel *[nrows];
        for (size_t y = 0; y < nrows; y++){
            PPMPixel * row= new PPMPixel [ncols];
            // cout<<y<<" ";
            for (size_t x = 0; x < ncols; x++){
                // cout<<x<<" ";
                fileStream.read((char *) &row[x].red,sizeof(row[x].red));
                fileStream.read((char *) &row[x].green,sizeof(row[x].green));
                fileStream.read((char *) &row[x].blue,sizeof(row[x].blue));

                // Write PPM file
                // binaryFile.write((char*)&row[x].red, sizeof (row[x].red));
                // binaryFile.write((char *) &row[x].green,sizeof(row[x].green));
                // binaryFile.write((char *) &row[x].blue,sizeof(row[x].blue));
                
            }
            image[y]=row; 
        }   

        fileStream.close();
        cout<<"File closed"<<endl<<endl; 

    }

    PPM::~PPM(){
        // std::cout<<"Destruct "<<fileName<<std::endl;
        if (image!=nullptr){
            for (size_t y = 0; y < nrows; y++){
                if (image[y]!=nullptr){
                    delete [] image[y]; 
                }              
            }
            delete [] image;
        }
        
        
        // free(image); 
    }

    // Special Members ===========================================================================================================

    // Move Constructor
    PPM::PPM(PPM&& other) noexcept{
        *this = std::move(other);  // invoke move assignment operator
        other.fileName=fileName;
        other.image=image;
        other.ncols=ncols;
        other.nrows=nrows;

        this->image=new PPMPixel*[this->ncols];
        for (size_t y = 0; y < this->nrows; y++){
            PPMPixel * row = new PPMPixel[ncols];
            for (size_t x = 0; x < this->ncols; x++)
            {
                row[x].red=other.image[y][x].red;
                row[x].green=other.image[y][x].green;
                row[x].blue=other.image[y][x].blue;
            }
            this->image[y]=row;
            other.image[y]=nullptr;
        }
        other.image=nullptr;       

        // cout<<"Move Constrctor"<<endl;

    }

    // Copy Assignment Operator
    PPM PPM::operator=(const PPM &other ){
        this->fileName=other.fileName;
        this->image=other.image;
        this->ncols=other.ncols;
        this->nrows=other.nrows;

        for (size_t y = 0; y < this->nrows; y++){
            for (size_t x = 0; x < this->ncols; x++)
            delete [] this->image[y];
        } 
        delete [] this->image;


        this->image=new PPMPixel*[this->ncols];
        for (size_t y = 0; y < this->nrows; y++){
            PPMPixel * row = new PPMPixel[ncols];
            for (size_t x = 0; x < this->ncols; x++)
            {
            row[x].red=other.image[y][x].red;
            row[x].green=other.image[y][x].green;
            row[x].blue=other.image[y][x].blue;
            }
            this->image[y]=row;
        }       

        // cout<<"Copy assignment operator"<<endl;
    }

    // Move Assignment
    PPM& PPM::operator=(PPM&& other) noexcept{
        if (this != &other) {
            this->fileName=other.fileName;
            this->ncols=other.ncols;
            this->nrows=other.nrows;

            this->image=new PPMPixel*[this->ncols];
            for (size_t y = 0; y < this->nrows; y++){
                PPMPixel * row = new PPMPixel[ncols];
                for (size_t x = 0; x < this->ncols; x++)
                {
                row[x].red=other.image[y][x].red;
                row[x].green=other.image[y][x].green;
                row[x].blue=other.image[y][x].blue;
                }
                this->image[y]=row;
                other.image[y]=nullptr;
            }     
            other.image=nullptr;
        }

        // cout<<"Move assignment"<<endl;

        return *this;
    }

    // Copy Constructor
    PPM::PPM(const PPM& other){
        this->fileName=std::string(other.fileName);
        this->ncols=int(other.ncols);
        this->nrows=int(other.nrows);

        this->image=new PPMPixel*[this->ncols];
        for (size_t y = 0; y < this->nrows; y++){
            PPMPixel * row = new PPMPixel[ncols];
            for (size_t x = 0; x < this->ncols; x++)
            {
                row[x].red=other.image[y][x].red;
                row[x].green=other.image[y][x].green;
                row[x].blue=other.image[y][x].blue;
            }
            this->image[y]=row;
        }        

        // cout<<"Copy Constructor"<<endl;
    }

    //============================================================================================================================= 
}

    
        



