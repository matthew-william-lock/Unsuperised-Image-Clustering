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
                clusterData[imageFile.getFilename()]=vector<int>();                
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
        if (TAG==PIXEL_INTENSITY_TAG){

            // Initialise distances
            for (size_t i = 0; i < images.size(); i++){
                distancesPI.push_back(16581375); // Largest possible distance to act as infinity
            }
            

            // Prints out the range of intenseties, basis of logic for generating hisograms for each image
            cout<<"Range of pixel intensities:"<<endl;
            int binCount = -1;
            int numberOfEnteries=0;
            while (binCount<RGB_COMPONENT_COLOR)
            {
                int lower=++binCount;
                cout<<"["<<lower<<"-";
                binCount=binCount+binSize-1;
                int upper = binCount;
                if (binCount>=RGB_COMPONENT_COLOR) {
                    binCount=RGB_COMPONENT_COLOR;
                    upper=binCount;
                    cout<<upper<<"]";
                }
                else cout<<upper<<"], ";
                numberOfEnteries++;               
            }
            cout<<endl;
            cout<<"Number of entries: "<<numberOfEnteries<<endl;

            // Initialise clusterdata
            for (size_t i = 0; i < images.size(); i++){
                PPM image = images.at(i);
                auto it = clusterData.find(image.getFilename());
                if (it==clusterData.end()){
                    cout<<"Error initialising cluster data - The image "<<image.getFilename()<<" does not exist in cluster data"<<endl;
                    return false;
                }
                for (size_t j = 0; j < numberOfEnteries; j++){
                    it->second.push_back(0);
                }
                
            }
            

            binCount = -1;
            int entryNo=-1;
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

                // Generate histograms
                for (size_t i = 0; i < images.size(); i++){
                    PPM image = images.at(i);
                    auto it = clusterData.find(image.getFilename());
                    if (it==clusterData.end()){
                        cout<<"Error generating histograms"<<endl<<"The image "<<image.getFilename()<<" does not exist in cluster data"<<endl;
                        return false;
                    } else if(it->second.size()!=numberOfEnteries){
                        cout<<"Error generating histograms"<<endl<<"The image length of the vector in cluster data does not correspond to the bin size"<<endl;
                        return false;
                    }                    
                    vector<int> temp = it->second;

                    for (size_t y = 0; y < image.getNrows(); y++){
                        for (size_t x = 0; x < image.getNcols(); x++){

                            
                            if (image.getBWPixel(y,x) >= lower && image.getBWPixel(y,x) <= upper){
                                // cout<<"working"<<endl;
                                // cout<<y<<" "<<x<<" ("<<int(image.getBWPixel(y,x))<<") ["<<lower<<"-"<<upper<<"] "<<clusterData.find(image.getFilename())->second.at(entryNo)<<endl;
                                temp.at(entryNo)++; 
                                clusterData[image.getFilename()]=temp;
                                // cout<<"["<<lower<<"-"<<upper<<"] "<<clusterData.find(image.getFilename())->second.at(entryNo)<<endl<<endl;                               
                            }
                            // else
                            // {
                            //     cout<<y<<" "<<x << " NOT ["<<lower<<"-"<<upper<<"] "<<int(image.getBWPixel(y,x))<<endl;
                            // }
                            
                            

                        }
                    }
                    
                }
            }

            // Print results
            cout<<endl;
            for (size_t i = 0; i < images.size(); i++)
            {
                PPM image = images.at(i);
                cout<<image.getFilename()<<endl;

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
                    pixelCount=pixelCount+clusterData.find(image.getFilename())->second.at(entryNo);
                    cout<<"["<<lower<<"-"<<upper<<"] "<<clusterData.find(image.getFilename())->second.at(entryNo)<<endl;                    
                }
                cout<<"Pixel count: "<<pixelCount<<endl;
                cout<<endl;

                if (pixelCount!=image.getNcols()*image.getNrows())
                {
                    cout<<"Error generating histograms, not all pixels accounted for in "<<image.getFilename()<<endl;
                    return false;
                }               

            }
            

        }
        return true;
        
    }

    // Generate Cluster
    bool Cluster::generateHistograms(int TAG, int noClusters){
        using namespace std;

        if (TAG==PIXEL_INTENSITY_TAG){
            int min =0;
            int max = images.size()-1;
            unordered_map<int,int> initialCentroids = unordered_map<int,int>();

            // Initialise clusters
            cout<<"Initial clusters : "<<endl;
            for (size_t i = 0; i < noClusters; i++){

                // Initialise clusters
                PixelIntensityClusterSet cluster = PixelIntensityClusterSet();
                PIclusters.push_back(cluster);

                // Choose initial centroids
                srand(time(NULL)); // Seed the time
                int randNum = rand()%(max-min + 1) + min;

                // Make sure random cluster isn't pixed twice for centroid
                auto it = initialCentroids.find(randNum);
                while (it!=initialCentroids.end()){
                    randNum = rand()%(max-min + 1) + min;
                    it = initialCentroids.find(randNum);
                }
                initialCentroids[randNum]=0; // Assign once unique centroid is found
                PIclusters.at(i).setCentroid(images.at(randNum).getFilename(),clusterData.find(images.at(randNum).getFilename())->second); // Assign centroid to set

                cout<<randNum<<" "<<images.at(randNum).getFilename()<<","<<endl;              
            }
        }
        return true;      
    }

    bool Cluster::iterateClusters(int TAG){

        using namespace std;

        bool moved=false;

        if (TAG==PIXEL_INTENSITY_TAG){

            // Populate clusters on initial run
            if (PIclusters.begin()->getSize()==0)
            {
                // Work with each image
                for (size_t i = 0; i < clusterData.size(); i++){

                    // cout<<"File "<<images.at(i).getFilename()<<":"<<endl;

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

                        // cout<< tempDistance<<"\t"<<PIclusters.at(c).getCentroidName()<<endl;                        
                        

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
                    PIclusters.at(i).calcCentroid();
                    PIclusters.at(i).printSetAndDistances(PIclusters,i); 
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
                    PIclusters.at(i).calcCentroid();
                    PIclusters.at(i).printSetAndDistances(PIclusters,i); 
                    cout<<endl; 
                }

            }            
            

            int count = 0;
            for (size_t i = 0; i < PIclusters.size(); i++)
            {
                count += PIclusters.at(i).getSize();
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

    bool Cluster::findAndDeletePIterator(std::string fileName, int x){
        bool result = false;
        for (size_t i = 0; i < PIclusters.size(); i++){           
           if (i!=x){
               result = PIclusters.at(i).findAndDeleteIterator(fileName);
               if (result) return result;
               
           }          
        }
        return result;
        
    }

    // Nested class to hold cluster sets ========================================================

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
        if (it!=s.end() && s.size()!=1){
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

        cout<<"Cluster "<<centroidName<< endl;
        cout<<"Size "<<s.size()<<endl;

        // printSet();

        cout<<"old ";
        for (size_t i = 0; i < centroid.size(); i++){
            cout<<centroid.at(i)<<" ";
        }
        cout<<endl;
        
        cout<<"new ";        
        for (size_t i = 0; i < centroid.size(); i++){
            int avg = 0;

            for (auto it = s.begin(); it!=s.end(); it++){
                avg+=it->second.at(i);             
            }

            centroid.at(i)= (avg)/(s.size());
            cout<<centroid.at(i)<<" ";

        }
        cout<<endl;          
        
    }

    std::unordered_map<std::string,std::vector<int>> Cluster::PixelIntensityClusterSet::getS(){
        return this->s;
    }

    void Cluster::PixelIntensityClusterSet::printSet(){
        using namespace std;

        cout<<"Set space"<<endl;
        for (auto it = s.begin(); it!=s.end(); it++){
            cout<<it->first<<"\t:\t"<<distancePI(it->second,*this);
            cout<<endl;    
        }
        cout<<endl;

        
    }

    void Cluster::PixelIntensityClusterSet::printSetAndDistances(const std::vector<Cluster::PixelIntensityClusterSet> & setOfClusters,const int & clusterNo){
        using namespace std;    
        cout<<"Cluster ["<<clusterNo<<"]"<<endl;
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

    //==========================================================================================


}