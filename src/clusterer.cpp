#include <iostream>

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int main(int argc, char* argv[]){
    using namespace std;

    string outputFile;
    int clusters = 10;
    int hgramWidth = 1;

    if (argc<2){
        cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] "<<endl; 
        cout<<"usage: clusterer: error: too few arguments"<<endl;
        return 1;
    }
    else if (argc>2 && argc < 9){

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
        
        
    }

    else if (argc>8){
        cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] "<<endl; 
        cout<<"usage: clusterer: error: too many arguments"<<endl;
        return 1;
    }

    return 0;
}