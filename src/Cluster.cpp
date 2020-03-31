#include "Cluster.h"

namespace LCKMAT002{

    Cluster::Cluster(){
    };

    Cluster::~Cluster(){
    };

    // Perform unix terminal commands and pipe back to program
    std::string Cluster::exec(std::string command) {
        using namespace std;
        char buffer[256];
        string result = "";

        // Open pipe to file
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            return "popen failed!";
        }

        // read till end of process:
        while (!feof(pipe)) {
            // use buffer to read and add to result
            if (fgets(buffer, 256, pipe) != NULL)
                result += buffer;
        }

        pclose(pipe);
        return result;
    }

    bool Cluster::readImages(const std::string & directoryName){
        using namespace std;

        // Get string of fileNames
        string fileNamesString = exec("ls ../"+directoryName+"/*.ppm "); // Get full directory

        if (fileNamesString.length()==0){
            cout<<"No ppm files found in "<<directoryName<<endl;
            return false;
        }

        // Insert images into Vector
        int count =0;

        string fileName;
        stringstream stringStream(fileNamesString); 
        while (stringStream){
            getline(stringStream,fileName,'\n');
            if (fileName.length()>1){
                LCKMAT002::PPM imageFile(fileName);
                images.push_back(imageFile);                
                count++;
            }            
        }       

        return true;
        
    }


}