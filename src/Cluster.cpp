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

    bool Cluster::pointsToRandomClusters(int TAG, int noClusters, unsigned int seed){
        using namespace std;

        int imagesAssigned =0;
        int numberOfImages=0;

        if (TAG==PIXEL_INTENSITY_TAG) numberOfImages = clusterData.size();
        else if (TAG==RGB_TAG) numberOfImages = RGBclusterData.size();        

        // Instantiate clusters
        for (size_t i = 0; i < noClusters; i++){
            if (TAG==PIXEL_INTENSITY_TAG) {
                PixelIntensityClusterSet cluster = PixelIntensityClusterSet();
                PIclusters.push_back(cluster);
            }
            else if (TAG==RGB_TAG) {
                RGBClusterSet RGBcluster = RGBClusterSet();
                RGBclusters.push_back(RGBcluster);
            }
        }        

        // Randomly assign images to clusters evenly
        int counter=0;
        unordered_map<int,int> randomSelections = unordered_map<int,int>();
        while (imagesAssigned<numberOfImages)
        {
            // Seed random generator 
            srand(seed);

            int randNum = rand()%((numberOfImages-1)-0 + 1) + 0;

            // Make sure random cluster isn't pixed twice for centroid
            auto it = randomSelections.find(randNum);
            while (it!=randomSelections.end()){
                randNum = rand()%((numberOfImages-1)-0 + 1) + 0;
                it = randomSelections.find(randNum);
            }
            randomSelections[randNum]=0; // Assign once random image is found

            // Add to cluster
            if (TAG==PIXEL_INTENSITY_TAG) {
                auto image = clusterData.find(images.at(randNum).getFilename());
                PIclusters.at(counter).add(image->first,image->second);
                PIclusters.at(counter).calcCentroid(); 
                distancesPI.at(randNum)=PIclusters.at(counter).distancePI(image->second,PIclusters.at(counter));
            } else if (TAG==RGB_TAG){
                auto image = RGBclusterData.find(images.at(randNum).getFilename());
                RGBclusters.at(counter).add(image->first,image->second);
                RGBclusters.at(counter).calcCentroid(); 
                distancesRGB.at(randNum)= RGBclusters.at(counter).distanceRGB(image->second,RGBclusters.at(counter));
            }   

            // Cycle cluster
            counter++;
            if (counter==noClusters) counter=0;
            imagesAssigned++;            
        }               
        
        // Print results        
        for (size_t i = 0; i < noClusters; i++){            
            if (TAG==PIXEL_INTENSITY_TAG) cout<<PIclusters.at(i).printSet();
            else if (TAG==RGB_TAG) RGBclusters.at(i).printSetAndDistances(RGBclusters,i);
        }
        return true;
    }

    bool Cluster::kMeansPlus(int TAG, int noClusters, unsigned int seed){
        using namespace std;

        int imagesAssigned =0;
        int numberOfImages=0;

        if (TAG==PIXEL_INTENSITY_TAG) numberOfImages = clusterData.size();
        else if (TAG==RGB_TAG) numberOfImages = RGBclusterData.size();

        /*

        Steps for K-means++ (https://en.wikipedia.org/wiki/K-means%2B%2B)

        1.1) Choose one center uniformly at random among the data points.
        1.2) For each data point x, compute D(x), the distance between x and the nearest center that has already been chosen.
        1.3) Choose one new data point at random as a new center, using a weighted probability distribution where a point x is chosen with probability proportional to D(x)2.
        1.4) Repeat Steps 2 and 3 until k centers have been chosen.
        1.5) Now that the initial centers have been chosen, proceed using standard k-means clustering.

        Steps for implementing weighted probability distribution (https://stats.stackexchange.com/questions/272114/using-kmeans-computing-weighted-probability-for-kmeans-initialization)

        2.1) Assume that you have an array D2 that contains the squared of the distances computed in step 2
        2.2) Normalize D2, that is, divide each element by the sum of D2. 
        2.3) Then, generate a uniform random number r in [0,1]. 
        2.4) Choose the first element in the list that the sum of the values in the normalized D2 up to that element is greater than or equal to r. 
        For example, if the normalized D2 is [0.2,0.1,0.4,0.3] and r is 0.55, the third element is selected.

        */

        // Instantiate clusters
        for (size_t i = 0; i < noClusters; i++){
            if (TAG==PIXEL_INTENSITY_TAG) {
                PixelIntensityClusterSet cluster = PixelIntensityClusterSet();
                PIclusters.push_back(cluster);
            }
            else if (TAG==RGB_TAG) {
                RGBClusterSet RGBcluster = RGBClusterSet();
                RGBclusters.push_back(RGBcluster);
            }
        } 

        // 2.1 Choose one center uniformly at random
        srand(seed); // Seed random number generator
        int randNum = rand()%((numberOfImages-1)-0 + 1) + 0;
        if (TAG==PIXEL_INTENSITY_TAG) {
            auto image = clusterData.find(images.at(randNum).getFilename());
            PIclusters.at(0).setCentroid(image->first,image->second);
        } else if (TAG==RGB_TAG){
            auto image = RGBclusterData.find(images.at(randNum).getFilename());
            RGBclusters.at(0).setCentroid(image->first,image->second);
        }

        // 1.2 - 1.3
        for (size_t k = 1; k < noClusters; k++){

            cout<<"k "<< k<<endl;

            // 1.2 ComputeD2
            vector<double> D2 = vector<double>();
            double sum =0;

            cout<<"Distances ";
            for (size_t i = 0; i < images.size(); i++){
                //Find closest cluster
                double distance;
                if (TAG == PIXEL_INTENSITY_TAG){
                    auto image = clusterData.find(images.at(i).getFilename());
                    distance=PIclusters.at(0).distancePI(image->second,PIclusters.at(0));
                    int clusterSet=0;          
                    // Find nearest cluster
                    for (size_t c = 0; c < k; c++){
                        double tempDistance=PIclusters.at(c).distancePI(image->second,PIclusters.at(c));
                        if (tempDistance<distance){
                            distance=tempDistance;
                            clusterSet=c;
                        }                                     
                    } 
                                
                }  else if (TAG == RGB_TAG){
                    auto image = RGBclusterData.find(images.at(i).getFilename());
                    distance=RGBclusters.at(0).distanceRGB(image->second,RGBclusters.at(0));
                    int clusterSet=0;          
                    // Find nearest cluster
                    for (size_t c = 0; c < k; c++){
                        double tempDistance=RGBclusters.at(c).distanceRGB(image->second,RGBclusters.at(c));
                        if (tempDistance<distance){
                            distance=tempDistance;
                            clusterSet=c;
                        }                                     
                    }           
                }     
                cout<<distance<<" ";
                D2.push_back(distance); 
                sum+=distance;                          
            }
            cout<<endl;
            cout<<"Size of D2 :"<<D2.size()<<endl;
            cout<<"Sum of distances "<<sum<<endl;

            // 2.2 Normalise D2
            cout<<"Normalised Distances ";
            for (size_t D2_it = 0; D2_it < D2.size(); D2_it++){
                D2.at(D2_it)=(D2.at(D2_it))/sum;
                cout<<D2.at(D2_it)<<" ";
            }
            cout<<endl;

            // 2.3 Uniform Random Number
            double randNum = (double)rand()/((double)RAND_MAX+1);
            cout<<"Random number "<<randNum<<endl;

            // 2.4 Choose new element
            double counter = D2.at(0);
            int position=0;
            while (counter<randNum){
                counter+=D2.at(position);
                position++;
            }
            if (position>(numberOfImages-1))position=numberOfImages-1; // Account for rang of vector
            cout<<"New centroid is image at "<<position<<endl<<endl;

            // 1.3 set centroid of cluster
            if (TAG==PIXEL_INTENSITY_TAG){
                auto image = clusterData.find(images.at(position).getFilename());
                PIclusters.at(k).setCentroid(image->first,image->second);
            } else if(TAG==RGB_TAG){
                auto image = RGBclusterData.find(images.at(position).getFilename());
                RGBclusters.at(k).setCentroid(image->first,image->second);
            }         
            
        }      
        
        // Print results        
        for (size_t i = 0; i < noClusters; i++){  
            cout<<"Cluster ["<<i<<"]"<<endl;          
            if (TAG==PIXEL_INTENSITY_TAG) cout<<PIclusters.at(i).printSet();
            else if (TAG==RGB_TAG) cout<<RGBclusters.at(i).printSet();
        }
        return true;
    }

    // Generate Cluster
    bool Cluster::randomCentroids(int TAG, int noClusters, unsigned int seed){
        using namespace std;

        int min =0;
        int max = images.size()-1;
        unordered_map<int,int> initialCentroids = unordered_map<int,int>();

        if (TAG==PIXEL_INTENSITY_TAG)cout<<"Initial centroids : "<<endl;
        else if (TAG==RGB_TAG) cout<<"Initial RGB clusters : "<<endl;

        for (size_t i = 0; i < noClusters; i++){

            // Initialise clusters
            if (TAG == PIXEL_INTENSITY_TAG){
                PixelIntensityClusterSet cluster = PixelIntensityClusterSet();
                PIclusters.push_back(cluster);
            } else if (TAG==RGB_TAG){
                RGBClusterSet RGBcluster = RGBClusterSet();
                RGBclusters.push_back(RGBcluster); 
            }         

            // Choose initial centroids
            srand(seed); // Seed the time
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

        bool moved = false;

        // Work with each image
        for (size_t i = 0; i < RGBclusterData.size(); i++){
            // Black and white images
            if (TAG == PIXEL_INTENSITY_TAG){
                auto image = clusterData.find(images.at(i).getFilename());
                double distance=PIclusters.at(0).distancePI(image->second,PIclusters.at(0));
                int newClusterSet=0;          
                // Find nearest cluster
                for (size_t c = 0; c < PIclusters.size(); c++){
                    double tempDistance=PIclusters.at(c).distancePI(image->second,PIclusters.at(c));
                    if (tempDistance<distance){
                        distance=tempDistance;
                        newClusterSet=c;
                    }                                     
                }
                distancesPI.at(i)=distance;
                if(findAndDeletePIterator(image->first,newClusterSet)) {
                    moved=true;
                    PIclusters.at(newClusterSet).add(image->first,image->second);
                } else if(!PIclusters.at(newClusterSet).contains(image->first)) {
                    PIclusters.at(newClusterSet).add(image->first,image->second); // Image has yet to be added to any of the clusters
                    moved = true;
                }
                
            }
            // RGB images
            else if (TAG==RGB_TAG){
                auto image = RGBclusterData.find(images.at(i).getFilename());
                double distance=RGBclusters.at(0).distanceRGB(image->second,RGBclusters.at(0));
                int newClusterSet=0;          
                // Find nearest cluster
                for (size_t c = 0; c < RGBclusters.size(); c++){
                    double tempDistance=RGBclusters.at(c).distanceRGB(image->second,RGBclusters.at(c));
                    if (tempDistance<distance){
                        distance=tempDistance;
                        newClusterSet=c;
                    }                                     
                }
                distancesRGB.at(i)=distance;
                if(findAndDeleteRGBIterator(image->first,newClusterSet)) {
                    moved=true;
                    RGBclusters.at(newClusterSet).add(image->first,image->second);
                } else if(!RGBclusters.at(newClusterSet).contains(image->first)) {
                    RGBclusters.at(newClusterSet).add(image->first,image->second); // Image has yet to be added to any of the clusters
                    moved = true;
                }
            }

        }

        // Move the centroids
        if (TAG==PIXEL_INTENSITY_TAG){
            for (size_t i = 0; i < PIclusters.size(); i++){
                PIclusters.at(i).printSetAndDistances(PIclusters,i);
                PIclusters.at(i).calcCentroid(); 
                cout<<endl; 
            }
        } else if (TAG==RGB_TAG){
            for (size_t i = 0; i < RGBclusters.size(); i++){
                RGBclusters.at(i).printSetAndDistances(RGBclusters,i);
                RGBclusters.at(i).calcCentroid(); 
                cout<<endl; 
            }
        }      
        

        //Display number of clusters left
        if (TAG==PIXEL_INTENSITY_TAG) cout<<"Number of cluster left at the end of iteration : "<<PIclusters.size()<<endl;
        else if (TAG==RGB_TAG) cout<<"Number of cluster left at the end of iteration : "<<RGBclusters.size()<<endl;
        
        // Check that no items where lost during iteration
        int count = 0;
          if (TAG==PIXEL_INTENSITY_TAG){
            for (size_t i = 0; i < PIclusters.size(); i++){
                count += PIclusters.at(i).getSize();
            }
        } else if (TAG==RGB_TAG){
            for (size_t i = 0; i < RGBclusters.size(); i++){
                count += RGBclusters.at(i).getSize();
            }
        }       
        cout<<"Number of items in sets "<<count<<endl;

    return moved;
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
                    //    PIclusters.erase(PIclusters.begin()+i); 
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
                        // RGBclusters.erase(RGBclusters.begin()+i); 
                    }                  
                    return result;
                }
            }              
                    
        }
        return result;
        
    }

    double Cluster::getSpread(int TAG){
        double spread = 0;
        if (TAG==PIXEL_INTENSITY_TAG){
            for (size_t i = 0; i < PIclusters.size(); i++){
               spread+=PIclusters.at(i).getSpread();
            }            
        }
        else if (TAG==RGB_TAG){
            for (size_t i = 0; i < RGBclusters.size(); i++){
               spread+=RGBclusters.at(i).getSpread();
            }
        }
        return spread;
    }

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

    bool Cluster::PixelIntensityClusterSet::contains(std::string name){
        auto it = s.find(name);
        if (it != s.end()) return true;
        return false;        
    }
    
    int Cluster::PixelIntensityClusterSet::getSize(){
        return this->s.size();
    }

    double Cluster::PixelIntensityClusterSet::getSpread(){
        double spread = 0;
        for (auto it =s.begin(); it !=s.end(); it++){
            spread+=distancePI(it->second,*this);
        }
        return spread;
        
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

        // Account for initalisation of centroid
        if (centroid.size()==0){
            for (size_t i = 0; i < s.begin()->second.size(); i++){ // First element will never be empty as call only made after element added
                centroid.push_back(0);
            }
        }        

        if (s.size()==0);// do nothing
        else{        
            // cout<<"new ";        
            for (size_t i = 0; i < centroid.size(); i++){
                int avg = 0;

                for (auto it = s.begin(); it!=s.end(); it++){
                    avg+=it->second.at(i);             
                }

                centroid.at(i)= (avg)/(s.size());
                // cout<<centroid.at(i)<<" ";

            }
        }
        // cout<<endl;          
        
    }

    std::unordered_map<std::string,std::vector<int>> Cluster::PixelIntensityClusterSet::getS(){
        return this->s;
    }

    std::string Cluster::PixelIntensityClusterSet::printSet(){
        using namespace std;

        stringstream ss;
        ss<<"Size "<<s.size()<<endl;
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

    bool Cluster::RGBClusterSet::contains(std::string name){
        auto it = s.find(name);
        if (it != s.end()) return true;
        return false;        
    }
    
    int Cluster::RGBClusterSet::getSize(){
        return this->s.size();
    }

    double Cluster::RGBClusterSet::getSpread(){
        double spread = 0;
        for (auto it =s.begin(); it !=s.end(); it++){
            spread+=distanceRGB(it->second,*this);
        }
        return spread;
        
    }

    // // Move the centroid to the mean of the data set
    void Cluster::RGBClusterSet::calcCentroid(){        
        

        using namespace std; 

        if (centroid.size()==0){
            for (size_t i = 0; i < s.begin()->second.size(); i++){ // First element will never be empty as call only made after element added
                centroid.push_back({0,0,0});
            }
        }        

        // cout<<"old \n";
        // for (size_t i = 0; i < centroid.size(); i++){
        //     cout<<centroid.at(i).at(0)<<"\t";
        //     cout<<centroid.at(i).at(1)<<"\t";
        //     cout<<centroid.at(i).at(2)<<"\n";
        // }
        // cout<<endl;
        
        // cout<<"new \n";  

        if (s.size()==0);// do nothing
        else{
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
        }
        // cout<<endl;          
        
    }

    // std::unordered_map<std::string,std::vector<int>> Cluster::PixelIntensityClusterSet::getS(){
    //     return this->s;
    // }

    std::string Cluster::RGBClusterSet::printSet(){
        using namespace std;

        stringstream ss;
        ss<<"Size "<<s.size()<<endl;
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

        // cout<<"Centroid"<<endl;
        // for (size_t i = 0; i < centroid.size(); i++){
        //     cout<<"{"<<centroid.at(i).at(0)<<" "<<centroid.at(i).at(1)<<" "<<centroid.at(i).at(2)<<"}"<<endl;    
        // }        

        cout<<endl;
        
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

        if (rgb.size()>0){
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