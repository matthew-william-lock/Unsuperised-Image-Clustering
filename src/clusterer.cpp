#include "Cluster.h"
#include "PPM.h"

#define RANDOM_CENTROIDS 0
#define IMAGES_TO_RANDOM_CLUSTERS 1
#define K_MEANS_PLUS 2

/**
 * Determine if string is a number
 * 
 * Only returns true for positive numbers 
 *
 * @param string String whose validity as a number is checked
 * @return boolean value of true if string is a number
 */

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int main(int argc, char* argv[]){
    using namespace std;

    std::string outputFile=std::string("");
    std::string datasetLocation=std::string("");
    int clusters = 10;
    int hgramWidth = 1;
    int tag = PIXEL_INTENSITY_TAG;
    int iterations = 10;
    bool shuffle = false;
    
    int init = K_MEANS_PLUS;

    /* Check to see if user has run the program with the correct arguments
    * 
    * Required Arguments:
    * <dataset> - specify the name of the dataset to be used
    * 
    * Optional Arguments
    * 
    * [-o ouput]    - 'output' specifies the name of the file to which the final k-means cluster is written to. 
    *                  The output is not written to a dile by default
    * 
    * [-k n]        - 'n' specifies the number of clusters (k) the k-means algorithm is to initialse 
    *                  k is set to 10 by default
    * 
    * [-bin b]      - 'b' specifies the size of the bin when generating histograms for each image
    *                  The bin size is set to 1 by default
    * 
    * [-color c]    - 'c' tells the k-means algorithm whether to consider RGB pixels, or only greyscale pixel. 
    *                  c ='rgb' for colour and c = 'g' for greyscale. 
    *                  Greyscale is selected by default      
    * 
    * [-it n]       - 'n' represents the number of time k-means will be run so that the spread can be minimised 
    *                  K-means will run 10 times by default
    * 
    * [-init i]     - 'i' represents the method of initalization for k-means
    *                  i= 'cent' for random centroid initalization, i='clust' for assigning images to random clusters, 'plus' for k-means+ method
    *                  K-means+ is default method (used weighted probabilsted funtion to select centroids, is more ideal than standard K-means)  
    * 
    * [-shuff b]    - 'b' represents a bool telling the k-means whether to shuffle the input data or not
    *                  b = 't' will cause the input data to shuffle, b = 'f' will keep the input data in it's original order
    *                  This allows the user to shuffle the input data and eliminate any biased caused by ordering of data
    *                  The input data will by default not be shuffled
    *   
    */

    if (argc<2){
        cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] [-color c] [-it n] [-shuff b]"<<endl; 
        cout<<"usage: clusterer: error: too few arguments"<<endl;
        return 1;
    }
    else if (argc>1 && argc < 17){

        // Set dataset location
        datasetLocation= string(argv[1]);

        // Check validity of input parameters
        for (size_t i = 2; i < argc; i++){
            if (string(argv[i])!="-o" && string(argv[i])!="-k" && string(argv[i])!="-bin" && string(argv[i])!="-color" && string(argv[i])!="-it"&& string(argv[i])!="-init"&& string(argv[i])!="-shuff"){
                cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] [-color c] [-it n] [-shuff b]"<<endl; 
                cout<<"usage: clusterer: error: invalid option "<<string(argv[i])<<endl;
                return 1;
            }
            else if (argc%2!=0){
                cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] [-color c] [-it n] [-shuff b]"<<endl; 
                cout<<"usage: clusterer: missing input parameter"<<endl;
                return 1;
            }
            
            if (string(argv[i])=="-o") outputFile = string(argv[i+1]);
            else if (string(argv[i])=="-k") {
                if(is_number(string(argv[i+1]))) clusters= stoi(string(argv[i+1]));
                else{
                    cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] [-color c] [-it n] [-shuff b]"<<endl; 
                    cout<<"usage: clusterer: invalid number of clusters : "<<string(argv[i+1])<<endl;
                    return 1;
                }                
            }
            else if (string(argv[i])=="-bin") {
                if(is_number(string(argv[i+1]))) {
                    if (stoi(string(argv[i+1]))>0 && stoi(string(argv[i+1]))< RGB_COMPONENT_COLOR+1) hgramWidth= stoi(string(argv[i+1]));
                    else{
                        cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] [-color c] [-it n] [-shuff b]"<<endl; 
                        cout<<"usage: clusterer: invalid width of histogram feature : "<<string(argv[i+1])<<endl;
                        return 1;
                    }
                }
                else{
                    cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] [-color c] [-it n] [-shuff b]"<<endl; 
                    cout<<"usage: clusterer: invalid width of histogram feature : "<<string(argv[i+1])<<endl;
                    return 1;
                }
            }
            else if (string(argv[i])=="-color") {
                if(string(argv[i+1])=="rgb") tag=RGB_TAG;
                else if(string(argv[i+1])=="hsv") tag=HSV_TAG   ;
                else{
                    cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] [-color c] [-it n] [-shuff b]"<<endl; 
                    cout<<"usage: clusterer: invalid feature selection : "<<(argv[i+1])<<endl;
                    return 1;
                }
            }
            else if (string(argv[i])=="-it") {
                if(is_number(string(argv[i+1])) && stoi(string(argv[i+1]))>0 ) iterations= stoi(string(argv[i+1]));
                else{
                    cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] [-color c] [-it n] [-shuff b]"<<endl; 
                    cout<<"usage: clusterer: invalid number of iterations : "<<string(argv[i+1])<<endl;
                    return 1;
                }
            }
            else if (string(argv[i])=="-init") {
                if(string(argv[i+1])=="cent") init=RANDOM_CENTROIDS;
                else if (string(argv[i+1])=="clust") init = IMAGES_TO_RANDOM_CLUSTERS;
                else if (string(argv[i+1])=="plus") init = K_MEANS_PLUS;
                else{
                    cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] [-color c] [-it n] [-shuff b]"<<endl; 
                    cout<<"usage: clusterer: invalid initialization of k-means : "<<string(argv[i+1])<<endl;
                    return 1;
                }
            }
            else if (string(argv[i])=="-shuff") {
                if(string(argv[i+1])=="t") shuffle=true;
                else if (string(argv[i+1])=="f") shuffle=false;
                else{
                    cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] [-color c] [-it n] [-shuff b] [-shuff b]"<<endl; 
                    cout<<"usage: clusterer: invalid shuffle argument : "<<string(argv[i+1])<<endl;
                    return 1;
                }
            }
                        
            i++;
        }

        cout<<"Output file: "<<outputFile<<endl;
        cout<<"Number of clusters: "<<clusters<<endl;
        cout<<"Width of histogram feature: "<<hgramWidth<<endl;
        
        // Store iterations of K-means
        std::vector<LCKMAT002::Cluster> iterationSet= std::vector<LCKMAT002::Cluster>(); 

        // Seeed the random number generator to be used when initialisng k-means
        srand(time(NULL));

        /* K-means implementation
        *
        * The K-means algorithm will now be run for a predetermined number of times.
        * Each iteration of K-means will be saved and the K-means with the smallest total spread will be selected as the output
        * 
        */

        for (size_t i = 0; i < iterations; i++){
            // // START OF PROGRAM -----------------------------------------------------------------------------------
            cout<<endl<<"Starting clusterer"<<endl;
            
            LCKMAT002::Cluster cluster = LCKMAT002::Cluster();

            cout<<endl<<"===============Creating Cluster and Reading Images==============="<<endl;

            /* Read images
            *
            * Finds all PPM images in given location
            * Shuffles images to make sure clustering is not favoured by initial ordering of images
            */
           
            if(!cluster.readImages(datasetLocation,shuffle)){
                cout<<"Error reading images"<<endl;
                return 1;
            }

            //Sets the bin size as specified by the user
            cluster.setBinSize(hgramWidth);

            cout<<"======================================================================="<<endl;

            cout<<endl<<"===============Creating Image Feature==============="<<endl;

            /* Histogram generation
            *
            * Histograms will be produced according the 'color' parameter as specified by the user
            * Selecting the RGB option will result in the generation of histograms taking longer than when compare to using greyscale
            */

            if (!cluster.generateHistograms(tag)){
                cout<<"Error generating image feature"<<endl;
                return 1;
            }     

            cout<<"========================================================="<<endl;

            // cout<<endl<<"===============Populating Clusters==============="<<endl;

            // /* Random Centroids
            // *
            // * If the user has selected to initialse using random centroids, random images will be selected as centroids for clusters.
            // * One unique image is selected as centroid for every cluster
            // */

            // if (init==RANDOM_CENTROIDS){
            //     if (!cluster.randomCentroids(tag,clusters,rand())){
            //         cout<<"Error generating clusters"<<endl;
            //         return 1;
            //     } 
            // } 
            
            // /* Images to random clusters
            // *
            // * If the user has selected to initialse k-means by randomly assigning images to clusters:
            // * Instead of selecting random images as centroids, images will be distributed randomly and evenly between clusters
            // * Centroids will then be computed using these newly created clusters
            // */

            // else if (init==IMAGES_TO_RANDOM_CLUSTERS){
            //     if (!cluster.pointsToRandomClusters(tag,clusters,rand())){
            //         cout<<"Error generating clusters"<<endl;
            //         return 1;
            //     } 
            // }

            // else if (init == K_MEANS_PLUS){
            //     if (!cluster.kMeansPlus(tag,clusters,rand())){
            //         cout<<"Error generating clusters"<<endl;
            //         return 1;
            //     } 
            // }
                 
            // cout<<"================================================="<<endl;

            // cout<<endl<<"===============Iterating Through Clusters==============="<<endl;
            // cout<<endl;  
            // int it = 0;

            // /* Iterate k-means
            // *
            // * Check every image to see which cluster they are most similar to
            // * If any images are moved between clusters, the algorithm will return true
            // * We will continue to iterate while images the algorithm returns true
            // */

            // cout<<"-----Iteration "<<++it<<"-----"<<endl; 
            // while (cluster.iterateClusters(tag)){
            //     it++;
            //     cout<<"----------"<<endl<<endl;
            //     cout<<"-----Iteration "<<it<<"-----"<<endl; 
            // }  
            // cout<<"----------"<<endl<<endl;

            // cout<<"Number of iterations:"<<it<<endl;

            // cout<<"================================================="<<endl; 

            // // Display the spread of the clusters 
            // cout<<endl<<"===============Spread of Cluster==============="<<endl;
            // cout<<"Spread of cluster : "<<cluster.getSpread(tag)<<endl;    
            // cout<<"================================================="<<endl;

            // iterationSet.push_back(cluster);
        }

        /* Select best K-means iteration
        *
        * Check the spread of all K-means iterations
        * Select the iteration with the lowest spread
        */

        // cout<<endl<<"===============Spread of All Cluster Iterations==============="<<endl;
        // double smallest_spread=iterationSet.at(0).getSpread(tag);
        // int bestPerformingIteration=0;
        // for (size_t i = 0; i < iterationSet.size(); i++){
        //     cout<<"K-means iteration "<<i+1<<" : "<<iterationSet.at(i).getSpread(tag)<<endl;  
        //     if(iterationSet.at(i).getSpread(tag)<smallest_spread){
        //         smallest_spread = iterationSet.at(i).getSpread(tag);
        //         bestPerformingIteration= i;
        //     }
        // }  
        // cout<<endl;
        // cout<<"Best performing iteration : "<<bestPerformingIteration+1<<" with " << smallest_spread<<" spread"<<endl;       
        // cout<<"================================================="<<endl;

        // cout<<endl<<"===============Final K-Means Clustering ==============="<<endl;
        // cout<<iterationSet.at(bestPerformingIteration);     
        // cout<<"================================================="<<endl;

        // END OF PROGRAM -------------------------------------------------------------------------------------
        
        
    }

    else if (argc>8){
        cout<<"usage: clusterer <dataset> [-o output] [-k n] [-bin b] "<<endl; 
        cout<<"usage: clusterer: error: too many arguments"<<endl;
        return 1;
    }

    return 0;
}