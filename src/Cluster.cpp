#include "Cluster.h"

namespace LCKMAT002{

    Cluster::Cluster(): binSize(0){
        this->clusterData=std::unordered_map<std::string,std::vector<int>>();
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
                clusterData[imageFile.getFilename()]=vector<int>();                 // Initialise PI Cluster Data   
                RGBclusterData[imageFile.getFilename()]=vector<vector<int>>();      // Initialise RGB Cluster Data      
                count++;
            }            
        }      
        return true;
    }

    // Set Bin Size
    void Cluster::setBinSize(const int & binSize){
        this->binSize=binSize;
    }


    // Generates all the histograms used to create cluster sets
    bool Cluster::generateHistograms(int TAG){
        using namespace std;

        // Initialise distances
        for (size_t i = 0; i < images.size(); i++){
            distancesPI.push_back(16581375); // Largest possible distance to act as infinity
            distancesRGB.push_back(99999999999999); // Largest possible distance to act as infinity
        }
        

        // Prints out the range of intenseties, basis of logic for generating hisograms for each image
        if (TAG==PIXEL_INTENSITY_TAG)cout<<"Range of pixel intensities:"<<endl;
        else if(TAG==RGB_TAG) cout<<"Range of RGB intensities:"<<endl;


        int binCount = -1;
        int numberOfEnteries=0;
        while (binCount<RGB_COMPONENT_COLOR)
        {
            int lower=++binCount;
            cout<<"["<<lower<<"-";
            binCount=binCount+binSize-1;
            if (binCount>=RGB_COMPONENT_COLOR) binCount=RGB_COMPONENT_COLOR;
            int upper = binCount;
            cout<<upper<<"], ";
            numberOfEnteries++;               
        }
        cout<<endl;
        cout<<"Number of entries: "<<numberOfEnteries<<endl;

        // Initialise clusterdata
        for (size_t i = 0; i < images.size(); i++){
            PPM image = images.at(i);
            auto P_it = clusterData.find(image.getFilename());
            auto RGB_it = RGBclusterData.find(image.getFilename());
            if (P_it==clusterData.end() || RGB_it ==RGBclusterData.end()){
                cout<<"Error initialising cluster data - The image "<<image.getFilename()<<" does not exist in cluster data"<<endl;
                return false;
            }
            for (size_t j = 0; j < numberOfEnteries; j++){
                P_it->second.push_back(0);
                RGB_it->second.push_back({0,0,0});
            }
            
        }            

        // binCount = -1;
        // int entryNo=-1;
        // while (binCount<RGB_COMPONENT_COLOR)
        // {
        //     int lower=++binCount;
        //     binCount=binCount+binSize-1;
        //     int upper = binCount;
        //     if (binCount>=RGB_COMPONENT_COLOR) {
        //         binCount=RGB_COMPONENT_COLOR;
        //         upper=binCount;
        //     }
        //     entryNo++; 

        //     cout<<"Generating "<<(entryNo+1)<<"/"<<numberOfEnteries<<endl;            

            // Generate histograms
            for (size_t i = 0; i < images.size(); i++){
                PPM image = images.at(i);
                cout<<"Generating "<<(i+1)<<"/"<<images.size()<<endl; 
                auto P_it = clusterData.find(image.getFilename());
                auto RGB_it = RGBclusterData.find(image.getFilename());
                if (P_it==clusterData.end() || RGB_it==RGBclusterData.end() ){
                    cout<<"Error generating histograms"<<endl<<"The image "<<image.getFilename()<<" does not exist in cluster data"<<endl;
                    return false;
                } else if(P_it->second.size()!=numberOfEnteries || RGB_it->second.size()!=numberOfEnteries){
                    cout<<"Error generating histograms"<<endl<<"The image length of the vector in cluster data does not correspond to the bin size"<<endl;
                    return false;
                } 
                vector<int> P_temp = P_it->second;
                vector<vector<int>> RGB_temp = RGB_it->second;

                
                    for (size_t y = 0; y < image.getNrows(); y++){
                        for (size_t x = 0; x < image.getNcols(); x++){  

                            bool R_found=false;
                            bool G_found=false;
                            bool B_found=false;
                            bool PI_found=false;

                            binCount = -1;
                            int entryNo=-1;
                            while (binCount<RGB_COMPONENT_COLOR && !( (R_found && G_found && B_found) || PI_found) )
                            {
                                int lower=++binCount;
                                binCount=binCount+binSize-1;
                                int upper = binCount;
                                if (binCount>=RGB_COMPONENT_COLOR) {
                                    binCount=RGB_COMPONENT_COLOR;
                                    upper=binCount;
                                }
                                entryNo++; 

                            if (TAG==PIXEL_INTENSITY_TAG){
                                if (image.getBWPixel(y,x) >= lower && image.getBWPixel(y,x) <= upper) {
                                    P_temp.at(entryNo)++; 
                                    PI_found=true;
                                    }
                                clusterData[image.getFilename()]=P_temp;
                                
                            } else if (TAG==RGB_TAG){
                                if (image.getR(y,x) >= lower && image.getR(y,x) <= upper) {
                                    RGB_temp.at(entryNo).at(0) ++; 
                                    R_found=true;
                                }
                                if (image.getG(y,x) >= lower && image.getG(y,x) <= upper) {
                                    RGB_temp.at(entryNo).at(1) ++; 
                                    G_found=true;
                                }
                                if (image.getB(y,x) >= lower && image.getB(y,x) <= upper) {
                                    RGB_temp.at(entryNo).at(2) ++; 
                                    B_found=true;
                                }
                                RGBclusterData[image.getFilename()]=RGB_temp; 
                                
                            }   
                            
                        }
                    }

                    
                }


                
                
            }
        // }

        // Print results
        cout<<endl;
        for (size_t i = 0; i < images.size(); i++){
            PPM image = images.at(i);
            cout<<image.getFilename()<<endl;
            if (TAG==RGB_TAG)cout<<"\t\tR\tG\tB"<<endl;

            int binCount = -1;
            int entryNo=-1;
            int pixelCount=0;
            while (binCount<RGB_COMPONENT_COLOR)
            {
                int lower=++binCount;
                binCount=binCount+binSize-1;
                int upper = binCount;
                if (binCount>=RGB_COMPONENT_COLOR) {
                    binCount=RGB_COMPONENT_COLOR;
                    upper=binCount;
                }

                entryNo++;
                if (TAG==PIXEL_INTENSITY_TAG){
                    pixelCount=pixelCount+clusterData.find(image.getFilename())->second.at(entryNo);

                    cout<<"["<<lower<<"-"<<upper<<"] "<<clusterData.find(image.getFilename())->second.at(entryNo)<<endl; 
                } else if (TAG==RGB_TAG){
                    pixelCount+=RGBclusterData.find(image.getFilename())->second.at(entryNo).at(0);
                    pixelCount+=RGBclusterData.find(image.getFilename())->second.at(entryNo).at(1);
                    pixelCount+=RGBclusterData.find(image.getFilename())->second.at(entryNo).at(2); 

                    stringstream ss;
                    ss<<"["<<lower<<"-"<<upper<<"]";
                    string limits = ss.str();
                    printf("%-*s ",9,limits.c_str());

                    cout<<"\t"<<RGBclusterData.find(image.getFilename())->second.at(entryNo).at(0)<<"\t";    
                    cout<<RGBclusterData.find(image.getFilename())->second.at(entryNo).at(1)<<"\t";    
                    cout<<RGBclusterData.find(image.getFilename())->second.at(entryNo).at(2)<<endl; 
                }

                                    
            }
            cout<<"Pixel count: "<<pixelCount<<endl;
            cout<<endl;

            if ( (pixelCount!=image.getNcols()*image.getNrows() && TAG==PIXEL_INTENSITY_TAG) || (pixelCount!=image.getNcols()*image.getNrows()*3 && TAG==RGB_COMPONENT_COLOR) )
            {
                cout<<"Error generating histograms, not all pixels accounted for in "<<image.getFilename()<<endl;
                return false;
            }               

        }            
        return true;  
    }

    // Generate Cluster
    bool Cluster::generateHistograms(int TAG, int noClusters){
        using namespace std;

        int min =0;
        int max = images.size()-1;
        unordered_map<int,int> initialCentroids = unordered_map<int,int>();

        if (TAG==PIXEL_INTENSITY_TAG)cout<<"Initial clusters : "<<endl;
        else if (TAG==RGB_TAG) cout<<"Initial RGB clusters : "<<endl;

        for (size_t i = 0; i < noClusters; i++){

            // Initialise clusters
            PixelIntensityClusterSet cluster = PixelIntensityClusterSet();
            PIclusters.push_back(cluster);

            RGBClusterSet RGBcluster = RGBClusterSet();
            RGBclusters.push_back(RGBcluster);

            // Choose initial centroids
            srand(time(NULL)); // Seed the time
            int randNum = rand()%(max-min + 1) + min;

            // Check similarity to other centroids
            bool identical = false;
            
            if (TAG==PIXEL_INTENSITY_TAG){
                for (size_t z = 0; z < PIclusters.size(); z++){
                    if (PIclusters.at(z).getCentroid()==clusterData.find(images.at(randNum).getFilename())->second) identical = true;                    
                }
            } else if (TAG==RGB_TAG){
                for (size_t z = 0; z < PIclusters.size(); z++){
                    if (RGBclusters.at(z).getCentroid()==RGBclusterData.find(images.at(randNum).getFilename())->second) identical = true;                    
                }
            }               
            
            // Make sure random cluster isn't pixed twice for centroid
            auto it = initialCentroids.find(randNum);
            while (it!=initialCentroids.end() || identical ){
                randNum = rand()%(max-min + 1) + min;

                // Check against previously picked random numbers
                it = initialCentroids.find(randNum);
                // Check to see if clusters are identical
                identical = false;
                for (size_t z = 0; z < PIclusters.size(); z++){
                    if (TAG==PIXEL_INTENSITY_TAG) {
                        if (PIclusters.at(z).getCentroid()==clusterData.find(images.at(randNum).getFilename())->second) identical = true;  
                    } else if (TAG==RGB_TAG){
                        if (RGBclusters.at(z).getCentroid()==RGBclusterData.find(images.at(randNum).getFilename())->second) identical = true;
                    }

                }
            }
            initialCentroids[randNum]=0; // Assign once unique centroid is found

            if (TAG==PIXEL_INTENSITY_TAG) {
                PIclusters.at(i).setCentroid(images.at(randNum).getFilename(),clusterData.find(images.at(randNum).getFilename())->second); // Assign centroid to set 
            } else if (TAG==RGB_TAG){
                RGBclusters.at(i).setCentroid(images.at(randNum).getFilename(),RGBclusterData.find(images.at(randNum).getFilename())->second); // Assign centroid to set
            }
            cout<<randNum<<" "<<images.at(randNum).getFilename()<<","<<endl;              
        }        
        return true;      
    }

    bool Cluster::iterateClusters(int TAG){

        using namespace std;

        bool moved=false;

        if (TAG==PIXEL_INTENSITY_TAG){

            // Populate clusters on initial run
            if (PIclusters.begin()->getSize()==0){
                // Work with each image
                for (size_t i = 0; i < clusterData.size(); i++){
                    // Largest distance possible initially set
                    double distance=distancesPI.at(i);
                    vector<int> image = clusterData.find(images.at(i).getFilename())->second;

                    // Work with each cluster
                    for (size_t c = 0; c < PIclusters.size(); c++){
                        double tempDistance=0;

                        // Determine the distance to centroid
                        for (size_t binLocation = 0; binLocation < PIclusters.at(c).getCentroid().size(); binLocation++){
                            double val = PIclusters.at(c).getCentroid().at(binLocation) - image.at(binLocation);
                            tempDistance = tempDistance + val*val;                       
                        }                   

                        if (images.at(i).getFilename()==PIclusters.at(c).getCentroidName())
                        {
                            distance = tempDistance;
                            distancesPI.at(i)=distance;
                            auto it = clusterData.find(images.at(i).getFilename());
                            PIclusters.at(c).add(images.at(i).getFilename(),image);
                            moved = findAndDeletePI(images.at(i).getFilename(),c);
                        }                    
                        
                        if (tempDistance<distance && images.at(i).getFilename()!=PIclusters.at(c).getCentroidName()) {
                            distance = tempDistance;
                            distancesPI.at(i)=distance;
                            auto it = clusterData.find(images.at(i).getFilename());
                            PIclusters.at(c).add(images.at(i).getFilename(),image);
                            moved = findAndDeletePI(images.at(i).getFilename(),c);
                        }    
                        
                    }
                    // cout<<endl;         
                }

                // Move the centroids
                for (size_t i = 0; i < PIclusters.size(); i++){
                    PIclusters.at(i).printSetAndDistances(PIclusters,i);
                    PIclusters.at(i).calcCentroid(); 
                    cout<<endl; 
                }

                return true;

            }

            else{

                // Work with each image
                for (size_t i = 0; i < clusterData.size(); i++){

                    // cout<<"File "<<images.at(i).getFilename()<<":"<<endl;

                    // Distance to current centroid
                    double distance=distancesPI.at(i);
                    vector<int> image = clusterData.find(images.at(i).getFilename())->second;

                    // Work with each cluster
                    for (size_t c = 0; c < PIclusters.size(); c++){

                        double tempDistance=0;

                        // Determine the distance to centroid
                        for (size_t binLocation = 0; binLocation < PIclusters.at(c).getCentroid().size(); binLocation++){
                            double val = PIclusters.at(c).getCentroid().at(binLocation) - image.at(binLocation);
                            tempDistance = tempDistance + val*val;                       
                        }

                        // cout<< tempDistance<<"\t"<<PIclusters.at(c).getCentroidName()<<endl;                                        
                        
                        if (tempDistance<distance && images.at(i).getFilename()!=PIclusters.at(c).getCentroidName()) {
                            auto it = clusterData.find(images.at(i).getFilename());              

                            // Deletes cluster if it is emptied              
                            if(findAndDeletePIterator(images.at(i).getFilename(),c)) {
                                moved=true;
                                PIclusters.at(c).add(images.at(i).getFilename(),image);
                                distance = tempDistance;
                                distancesPI.at(i)=distance;
                            }
                        }    
                        
                    }
                    // cout<<endl;         
                }

                // Move the centroids
                for (size_t i = 0; i < PIclusters.size(); i++){
                    PIclusters.at(i).printSetAndDistances(PIclusters,i);
                    PIclusters.at(i).calcCentroid(); 
                    cout<<endl; 
                }

                //Display number of clusters left
                cout<<"Number of cluster left at the end of iteration : "<<PIclusters.size()<<endl;

            }            
            

            int count = 0;
            for (size_t i = 0; i < PIclusters.size(); i++)
            {
                count += PIclusters.at(i).getSize();
            }
            cout<<"Number of items in sets "<<count<<endl;
            
            
            return moved;

        }

        else if (TAG==RGB_TAG){

            // Populate clusters on initial run

            if (RGBclusters.begin()->getSize()==0){
                // Work with each image
                for (size_t i = 0; i < RGBclusterData.size(); i++){


                    // Distance to current centroid
                    vector<vector<int>> image = RGBclusterData.find(images.at(i).getFilename())->second;

                    int newClusterSet=0;
                    double distance=RGBclusters.at(0).distanceRGB(image,RGBclusters.at(0));

                    // Work with each cluster
                    for (size_t c = 0; c < RGBclusters.size(); c++){

                        // Determine the distance to centroid  
                        double tempDistance=RGBclusters.at(c).distanceRGB(image,RGBclusters.at(c));
                        if (tempDistance<distance){
                            distance=tempDistance;
                            newClusterSet=c;
                        }                      
                        
                    }

                    distancesRGB.at(i)=distance;
                    if(findAndDeleteRGBIterator(images.at(i).getFilename(),newClusterSet)) {
                        moved=true;
                        RGBclusters.at(newClusterSet).add(images.at(i).getFilename(),image);
                    } else {
                        moved=true;
                        RGBclusters.at(newClusterSet).add(images.at(i).getFilename(),image);
                    }

                    // cout<<endl;         
                }

                // Move the centroids
                for (size_t i = 0; i < RGBclusters.size(); i++){
                    RGBclusters.at(i).printSetAndDistances(RGBclusters,i);
                    RGBclusters.at(i).calcCentroid(); 
                    cout<<endl; 
                }
            
            
                return true;

            }

            else{

                // Work with each image
                for (size_t i = 0; i < RGBclusterData.size(); i++){


                    // Distance to current centroid
                    vector<vector<int>> image = RGBclusterData.find(images.at(i).getFilename())->second;

                    int newClusterSet=0;
                    double distance=RGBclusters.at(0).distanceRGB(image,RGBclusters.at(0));

                    // Work with each cluster
                    for (size_t c = 0; c < RGBclusters.size(); c++){

                        // Determine the distance to centroid  
                        double tempDistance=RGBclusters.at(c).distanceRGB(image,RGBclusters.at(c));
                        if (tempDistance<distance){
                            distance=tempDistance;
                            newClusterSet=c;
                        }                      
                        
                    }
                    distancesRGB.at(i)=distance;
                    if(findAndDeleteRGBIterator(images.at(i).getFilename(),newClusterSet)) {
                        moved=true;
                        RGBclusters.at(newClusterSet).add(images.at(i).getFilename(),image);
                    }

                    // cout<<endl;         
                }

                // Move the centroids
                for (size_t i = 0; i < RGBclusters.size(); i++){
                    RGBclusters.at(i).printSetAndDistances(RGBclusters,i);
                    RGBclusters.at(i).calcCentroid(); 
                    cout<<endl; 
                }

                
                

                //Display number of clusters left
                cout<<"Number of cluster left at the end of iteration : "<<PIclusters.size()<<endl;

            }            
            

            int count = 0;
            for (size_t i = 0; i < RGBclusters.size(); i++)
            {
                count += RGBclusters.at(i).getSize();
            }
            cout<<"Number of items in sets "<<count<<endl;
            
            
            return moved;

        }           

        return true;
    }

    bool Cluster::findAndDeletePI(std::string fileName, int x){
        bool result = false;
        for (size_t i = 0; i < PIclusters.size(); i++){           
           if (i!=x){
               result = PIclusters.at(i).findAndDelete(fileName);
               if (result) return result;
               
           }          
        }
        return result;        
    } 

    bool Cluster::findAndDeleteRGB(std::string fileName, int x){
        bool result = false;
        for (size_t i = 0; i < RGBclusters.size(); i++){           
           if (i!=x){
               result = RGBclusters.at(i).findAndDelete(fileName);
               if (result) return result;
               
           }          
        }
        return result;        
    } 

    bool Cluster::findAndDeletePIterator(std::string fileName, int x){
        bool result = false;
        for (size_t i = 0; i < PIclusters.size(); i++){           
           if (i!=x){
               result = PIclusters.at(i).findAndDeleteIterator(fileName);
               if (result) {

                   if (PIclusters.at(i).getSize()==0) {
                       PIclusters.erase(PIclusters.begin()+i); 
                   }                  
                   return result;
               }
               
           }          
        }
        return result;
        
    }

    bool Cluster::findAndDeleteRGBIterator(std::string fileName, int x){
        bool result = false;
        for (size_t i = 0; i < RGBclusters.size(); i++){         
            if (i!=x){
                result = RGBclusters.at(i).findAndDeleteIterator(fileName);
                if (result) {

                    if (RGBclusters.at(i).getSize()==0) {
                        RGBclusters.erase(RGBclusters.begin()+i); 
                    }                  
                    return result;
                }
            }              
                    
        }
        return result;
        
    }

    bool const Cluster::isEmptyPI(){
        if (RGBclusters.at(0).getSize()) return true;
        return false;
        
    };

    // Nested class to hold cluster sets =====================================================================================================================

    Cluster::PixelIntensityClusterSet::PixelIntensityClusterSet(){
        this->centroid=std::vector<int>();
        this->s=std::unordered_map<std::string,std::vector<int>>();
    }

    Cluster::PixelIntensityClusterSet::~PixelIntensityClusterSet(){
        
    }

    void Cluster::PixelIntensityClusterSet::setCentroid(const std::string & name, const std::vector<int> & centroid){
        this->centroid = centroid;
        this->centroidName= name;
    }

    std::vector<int> Cluster::PixelIntensityClusterSet::getCentroid(){
        return centroid;
    }

    std::string Cluster::PixelIntensityClusterSet::getCentroidName(){
        return this->centroidName;
    }

    // Search cluster to see if it contains file, delete if it does
    bool Cluster::PixelIntensityClusterSet::findAndDelete(std::string fileName){
        auto it = s.find(fileName);
        if (it!=s.end()){
            s.erase(it);
            // std::cout<<"\ndelete\n";
            return true;
        }
        return false;
    }

    // Search cluster to see if it contains file, delete if it does
    bool Cluster::PixelIntensityClusterSet::findAndDeleteIterator(std::string fileName){
        auto it = s.find(fileName);
        if (it!=s.end()){
            s.erase(it);
            // std::cout<<"\ndelete\n";
            return true;
        }
        return false;
    }

    // Add to the cluster set
    void Cluster::PixelIntensityClusterSet::add(const std::string & fileName, const std::vector<int> & hisogram){
        this->s[fileName]=hisogram;
    }
    
    int Cluster::PixelIntensityClusterSet::getSize(){
        return this->s.size();
    }

    // Move the centroid to the mean of the data set
    void Cluster::PixelIntensityClusterSet::calcCentroid(){        

        using namespace std; 

        // cout<<"old ";
        // for (size_t i = 0; i < centroid.size(); i++){
        //     cout<<centroid.at(i)<<" ";
        // }
        // cout<<endl;

        

        // cout<<"new "; 
        // for (size_t i = 0; i < centroid.size(); i++)
        // {
        //     int min = 999999999;
        //     int max = 0;
        //     for (auto it = s.begin(); it!=s.end(); it++){
        //         if(it->second.at(i)<min) min = it->second.at(i);
        //         if(it->second.at(i)>max) max = it->second.at(i);        
        //     }

        //     centroid.at(i)=(min+max)/2;
        //     cout<<centroid.at(i)<<" ";
        // }
        // cout<<endl;
        
        // cout<<"new ";        
        for (size_t i = 0; i < centroid.size(); i++){
            int avg = 0;

            for (auto it = s.begin(); it!=s.end(); it++){
                avg+=it->second.at(i);             
            }

            centroid.at(i)= (avg)/(s.size());
            // cout<<centroid.at(i)<<" ";

        }
        // cout<<endl;          
        
    }

    std::unordered_map<std::string,std::vector<int>> Cluster::PixelIntensityClusterSet::getS(){
        return this->s;
    }

    std::string Cluster::PixelIntensityClusterSet::printSet(){
        using namespace std;

        stringstream ss;
        ss<<"Initial centroid : " <<centroidName <<endl;
        for (auto it = s.begin(); it!=s.end(); it++){
            ss<<it->first;
            ss<<endl;    
        }
        ss<<endl;

        return ss.str();

        
    }

    void Cluster::PixelIntensityClusterSet::printSetAndDistances(const std::vector<Cluster::PixelIntensityClusterSet> & setOfClusters,const int & clusterNo){
        using namespace std;    
        cout<<"Cluster ["<<clusterNo<<"] "<<centroidName<<endl;
        cout<<"Size "<<s.size()<<endl;
        cout<<"Set space\t\t\t\t\t\t";
        for (size_t i = 0; i < setOfClusters.size(); i++)
        {
           cout<<i<<"\t";
        }
        cout<<endl;

        for (auto it = s.begin(); it!=s.end(); it++){
            cout<<it->first<<"\t:\t"<<distancePI(it->second,*this)<<"\t";
            for (size_t i = 0; i < setOfClusters.size(); i++)
            {
                cout<<distancePI(it->second, setOfClusters.at(i))<<"\t";
            }
            cout<<endl;
            
        }

        
    }

    double Cluster::PixelIntensityClusterSet::distancePI(const std::vector<int> & point, const  Cluster::PixelIntensityClusterSet & set){
        double distance =0;
        for (size_t i = 0; i < point.size(); i++)
        {
            double temp = set.centroid.at(i)-point.at(i);
            temp = temp*temp;
            distance+=temp;
        }
        return distance;
        
    }

    //==========================================================================================================================================================

    // Nested class to hold RGB cluster sets =====================================================================================================================

    Cluster::RGBClusterSet::RGBClusterSet(){
        this->centroid=std::vector<std::vector<int>>();
        this->s=std::unordered_map<std::string,std::vector<std::vector<int>>>();
    }

    Cluster::RGBClusterSet::~RGBClusterSet(){        
    }

    void Cluster::RGBClusterSet::setCentroid(const std::string & name, const std::vector<std::vector<int>> & centroid){
        this->centroid = centroid;
        this->centroidName= name;
    }

    std::vector<std::vector<int>> Cluster::RGBClusterSet::getCentroid(){
        return centroid;
    }

    std::string const Cluster::RGBClusterSet::getCentroidName(){
        return this->centroidName;
    }

    // Search cluster to see if it contains file, delete if it does
    bool Cluster::RGBClusterSet::findAndDelete(std::string fileName){
        auto it = s.find(fileName);
        if (it!=s.end()){
            s.erase(it);
            // std::cout<<"\ndelete\n";
            return true;
        }
        return false;
    }

    // Search cluster to see if it contains file, delete if it does
    bool Cluster::RGBClusterSet::findAndDeleteIterator(std::string fileName){
        auto it = s.find(fileName);
        if (it!=s.end()){
            s.erase(it);
            // std::cout<<"\ndelete\n";
            return true;
        }
        return false;
    }

    // // Add to the cluster set
    void Cluster::RGBClusterSet::add(const std::string & fileName, const std::vector<std::vector<int>> & hisogram){
        this->s[fileName]=hisogram;
    }
    
    int Cluster::RGBClusterSet::getSize(){
        return this->s.size();
    }

    // // Move the centroid to the mean of the data set
    void Cluster::RGBClusterSet::calcCentroid(){        
        

        using namespace std; 

        // cout<<"old \n";
        // for (size_t i = 0; i < centroid.size(); i++){
        //     cout<<centroid.at(i).at(0)<<"\t";
        //     cout<<centroid.at(i).at(1)<<"\t";
        //     cout<<centroid.at(i).at(2)<<"\n";
        // }
        // cout<<endl;
        
        // cout<<"new \n";        
        for (size_t i = 0; i < centroid.size(); i++){
            int avg_R = 0;
            int avg_G = 0;
            int avg_B = 0;

            for (auto it = s.begin(); it!=s.end(); it++){
                avg_R+=it->second.at(i).at(0);
                avg_G+=it->second.at(i).at(1);
                avg_B+=it->second.at(i).at(2);             
            }

            centroid.at(i).at(0)= (avg_R)/(s.size());
            centroid.at(i).at(1)= (avg_G)/(s.size());
            centroid.at(i).at(2)= (avg_B)/(s.size());

            // cout<<centroid.at(i).at(0)<<"\t";
            // cout<<centroid.at(i).at(1)<<"\t";
            // cout<<centroid.at(i).at(2)<<"\n";

        }
        // cout<<endl;          
        
    }

    // std::unordered_map<std::string,std::vector<int>> Cluster::PixelIntensityClusterSet::getS(){
    //     return this->s;
    // }

    std::string Cluster::RGBClusterSet::printSet(){
        using namespace std;

        stringstream ss;
        ss<<"Initial centroid : " <<centroidName <<endl;
        for (auto it = s.begin(); it!=s.end(); it++){
            ss<<it->first;
            ss<<endl;    
        }
        ss<<endl;

        return ss.str();

    }

    void Cluster::RGBClusterSet::printSetAndDistances(const std::vector<Cluster::RGBClusterSet> & setOfClusters,const int & clusterNo){
        using namespace std;    
        cout<<"Cluster ["<<clusterNo<<"] "<<centroidName<<endl;
        cout<<"Size "<<s.size()<<endl;
        cout<<"Set space\t\t\t\t\t\t";
        for (size_t i = 0; i < setOfClusters.size(); i++)
        {
           cout<<i<<"\t";
        }
        cout<<endl;

        for (auto it = s.begin(); it!=s.end(); it++){
            cout<<it->first<<"\t:\t"<<distanceRGB(it->second,*this)<<"\t";
            for (size_t i = 0; i < setOfClusters.size(); i++)
            {
                cout<<distanceRGB(it->second, setOfClusters.at(i))<<"\t";
            }
            cout<<endl;
            
        }

        
    }

    double Cluster::RGBClusterSet::distanceRGB(const std::vector<std::vector<int>> & point, const Cluster::RGBClusterSet & set){
        double distance =0;
        for (size_t i = 0; i < point.size(); i++)
        {
            double R_temp = set.centroid.at(i).at(0)-point.at(i).at(0);
            double G_temp = set.centroid.at(i).at(1)-point.at(i).at(1);
            double B_temp = set.centroid.at(i).at(2)-point.at(i).at(2);
            distance+=R_temp*R_temp +G_temp*G_temp + B_temp*B_temp;
        }
        return distance;
        
    }

    //==========================================================================================================================================================


    // Operator overloads
    std::ostream& operator<<(std::ostream& os, const Cluster& dt){

        std::vector<Cluster::RGBClusterSet>  rgb = dt.RGBclusters;
        std::vector<Cluster::PixelIntensityClusterSet>  pi = dt.PIclusters;

        if (rgb.at(0).getSize()>0){
            for (size_t i = 0; i < rgb.size(); i++){
                os<<"Set "<<i+1<<std::endl;
                os<<rgb.at(i).printSet();   
            }
            
        } else {
            for (size_t i = 0; i < pi.size(); i++){
                os<<"Set "<<i+1<<std::endl;
                os<<pi.at(i).printSet();   
            }
        }

        return os;
    }   

}