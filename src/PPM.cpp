#include "PPM.h"

namespace LCKMAT002 {

    using namespace std;

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

    int PPM::getNrows(){
        return nrows;
    };

    int PPM::getNcols(){
        return ncols;
    };

    unsigned char PPM::getR(int y, int x){
        return image[y][x].red;
    }

    unsigned char PPM::getG(int y, int x){
        return image[y][x].green;
    }

    unsigned char PPM::getB(int y, int x){
        return image[y][x].blue;
    }

    // Return black and white pixel at value X and Y of image
    unsigned char PPM::getBWPixel(int y, int x){
        return 0.21 * image[y][x].red + 0.72 * image[y][x].green + 0.07 * image[y][x].blue;
    }

    std::string const PPM::getFilename(){
        return fileName;
    }

    // Special Members ===========================================================================================================

    // Destructor   
    PPM::~PPM(){
        // std::cout<<"Destruct "<<fileName<<std::endl;
        // cout<<"address desrtuct " <<this->image<<endl;
        // cout<<"check"<<endl;
        if (image==nullptr){
            // cout<<"Null"<<endl;
            delete[] image;
        }
        else{
            // cout<<"Not null"<<endl;
            for (size_t y = 0; y < nrows; y++){
                // if (image[y]!=nullptr){
                    // cout<<"inner address "<<image[y]<<endl;
                    delete [] image[y]; 
                // }              
            }
            delete [] image;
        }
        // }
    }

    // Move Constructor
    PPM::PPM(PPM&& other) noexcept{
        // cout<<"Move Constrctor start"<<endl;
        // *this = std::move(other);  // invoke move assignment operator
        this->fileName=other.fileName;
        this->ncols=other.ncols;
        this->nrows=other.nrows;

        // cout<<"move construvtor "<<this->nrows<<endl;

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
        // other.image=nullptr;
        // cout<<"Move construct done"<<endl;

    }

    // Copy Assignment Operator
    PPM PPM::operator=(const PPM &other ){
        // cout<<"Copy assignment operator"<<endl;
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
    }

    // Move Assignment
    PPM& PPM::operator=(PPM&& other) noexcept{
        // cout<<"Move assignment"<<endl;
        if (this != &other) {
            this->fileName=other.fileName;
            this->ncols=other.ncols;
            this->nrows=other.nrows;

            // this->image = other.image;
            // cout<<"Filename "<<this->fileName<<endl;
            // cout<<"old lhs "<<&*this->image<<" old rhs "<<&*other.image<<endl;
            // std::swap(this->image, other.image);
             for (size_t y = 0; y < this->nrows; y++){
                this->image[y]=other.image[y];     
                other.image[y]=nullptr;                          
            }
            this->image=other.image;
            other.image=nullptr;

            // cout<<"new lhs "<<&*this->image<<" new rhs "<<&*other.image<<endl;
            // other.image = nullptr;


            // cout<<"cleared address "<<other.image<<endl;
            // this->image=new PPMPixel*[this->ncols];
            // for (size_t y = 0; y < this->nrows; y++){
            // // //     PPMPixel * row = new PPMPixel[ncols];
            // //     for (size_t x = 0; x < this->ncols; x++)
            // // //     {
            // // //         row[x].red=other.image[y][x].red;
            // // //         row[x].green=other.image[y][x].green;
            // // //         row[x].blue=other.image[y][x].blue;
            // // //     }
            // // //     this->image[y]=row;
            //     // delete [] other.image[y];
            //     other.image[y]=nullptr;
            // }     
            // other.image=nullptr;
            // // delete [] other.image;
            // cout<<"Cleared address "<<other.image<<endl;

        }       

        return *this;
    }

    // Copy Constructor
    PPM::PPM(const PPM& other){
        // cout<<"Copy Constructor"<<endl;
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
    }

    //============================================================================================================================= 
}

    
        



