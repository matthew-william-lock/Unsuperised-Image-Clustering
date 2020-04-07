#include "Cluster.h"
#include "PPM.h"

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int main(int argc, char* argv[]){
    using namespace std;

    std::string outputFile=std::string();
    int clusters = 10;
    int hgramWidth = 1;

    if (argc<2){
        cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] "<<endl; 
        cout<<"usage: clusterer: error: too few arguments"<<endl;
        return 1;
    }
    else if (argc>1 && argc < 9){

        for (size_t i = 2; i < argc; i++)
        {
            if (string(argv[i])!="-o" && string(argv[i])!="-k" && string(argv[i])!="-bin"){
                cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] "<<endl; 
                cout<<"usage: clusterer: error: invalid option "<<string(argv[i])<<endl;
                return 1;
            }
            else if (argc%2!=0){
                cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] "<<endl; 
                cout<<"usage: clusterer: missing input parameter"<<endl;
                return 1;
            }
            
            if (string(argv[i])=="-o") outputFile = string(argv[i+1]);
            else if (string(argv[i])=="-k") {
                if(is_number(string(argv[i+1]))) clusters= stoi(string(argv[i+1]));
                else{
                    cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] "<<endl; 
                    cout<<"usage: clusterer: invalid number of clusters : "<<string(argv[i+1])<<endl;
                    return 1;
                }                
            }
            else if (string(argv[i])=="-bin") {
                if(is_number(string(argv[i+1]))) hgramWidth= stoi(string(argv[i+1]));
                else{
                    cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] "<<endl; 
                    cout<<"usage: clusterer: invalid width of histogram feature : "<<string(argv[i+1])<<endl;
                    return 1;
                }
            }
                        
            i++;
        }
        
        cout<<"Output file: "<<outputFile<<endl;
        cout<<"Number of clusters: "<<clusters<<endl;
        cout<<"Width of histogram feature: "<<hgramWidth<<endl;
        
        // START OF PROGRAM -----------------------------------------------------------------------------------
        cout<<endl<<"Starting clusterer"<<endl;

        // LCKMAT002::PPM file("../Gradient_Numbers_PPMS/eight_10.ppm");
        LCKMAT002::Cluster cluster = LCKMAT002::Cluster();
        cout<<endl<<"===============Creating Cluster and Reading Images==============="<<endl;
        if(!cluster.readImages("Gradient_Numbers_PPMS")){
            cout<<"Error reading images"<<endl;
            return 1;
        }
        cluster.setBinSize(hgramWidth);
        cout<<"======================================================================="<<endl;

        cout<<endl<<"===============Creating Image Feature==============="<<endl;
        if (!cluster.generateHistograms(PIXEL_INTENSITY_TAG)){
            cout<<"Error generating image feature"<<endl;
            return 1;
        }      
        cout<<"========================================================="<<endl;

        cout<<endl<<"===============Initialising Clusters==============="<<endl;
        if (!cluster.generateHistograms(PIXEL_INTENSITY_TAG,clusters)){
            cout<<"Error generating clusters"<<endl;
            return 1;
        }      
        cout<<"================================================="<<endl;

        cout<<endl<<"===============Iterating Through Clusters==============="<<endl;
        cout<<endl;  
        int it = 0;

        cout<<"-----Iteration "<<it<<"-----"<<endl; 
        // The first iteration should incur any moves 
        if (!cluster.iterateClusters(PIXEL_INTENSITY_TAG)){
            cout<<"Error generating clusters"<<endl;
            return 1;
        }
        cout<<"----------"<<endl<<endl;
        
        it++;
        // Iterartions there after might move from one set to another  
        cout<<"-----Iteration "<<it<<"-----"<<endl; 
        while (cluster.iterateClusters(PIXEL_INTENSITY_TAG)){
            it++;
            cout<<"----------"<<endl<<endl;
            cout<<"-----Iteration "<<it<<"-----"<<endl; 
        }  
        cout<<"----------"<<endl<<endl;

        cout<<"Number of iterations:"<<it<<endl;

        cout<<"================================================="<<endl;       


        // END OF PROGRAM -------------------------------------------------------------------------------------
        
        
    }

    else if (argc>8){
        cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] "<<endl; 
        cout<<"usage: clusterer: error: too many arguments"<<endl;
        return 1;
    }

    return 0;
}