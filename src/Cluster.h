#ifndef CLUSTER_H
#define CLUSTER_H

#include "PPM.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <memory> // C++11
#include <string>
#include <cstdio>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <algorithm>    // std::random_shuffle 


#define PIXEL_INTENSITY_TAG 0
#define RGB_TAG 1
#define HSV_TAG 2

namespace LCKMAT002
{

    class Cluster
    {
    private:

        // Nested class to hold cluster sets ========================================================

        class PixelIntensityClusterSet
        {
        private:
            std::unordered_map<std::string,std::vector<int>> s;
            std::vector<int> centroid;
            std::string centroidName;
        public:
            PixelIntensityClusterSet(/* args */);
            ~PixelIntensityClusterSet();

            void setCentroid(const std::string & name, const std::vector<int> & centroid);
            std::vector<int> getCentroid();
            std::string getCentroidName();

            void add(const std::string & fileName, const std::vector<int> & hisogram);
            bool contains(std::string name);            

            bool findAndDelete(std::string fileName);
            bool findAndDeleteIterator(std::string fileName);
            
            int getSize();
            double getSpread();

            void calcCentroid();
            std::unordered_map<std::string,std::vector<int>> getS();
            std::string printSet();
            void printSetAndDistances(const std::vector<Cluster::PixelIntensityClusterSet> & setOfClusters, const int & clusterNo );

            double distancePI(const std::vector<int> & point, const Cluster::PixelIntensityClusterSet & set);
            
        };      

        //===========================================================================================

         // Nested class to hold RGB cluster sets ========================================================

        class RGBClusterSet
        {
        private:
            std::unordered_map<std::string,std::vector<std::vector<int>>> s;
            std::vector<std::vector<int>> centroid;
            std::string centroidName;
        public:
            RGBClusterSet(/* args */);
            ~RGBClusterSet();

            void setCentroid(const std::string & name, const std::vector<std::vector<int>> & centroid);
            std::vector<std::vector<int>> getCentroid();
            std::string const getCentroidName();
            void calcCentroid();

            int getSize();
            double getSpread();
            void add(const std::string & fileName, const std::vector<std::vector<int>> & hisogram);
            bool contains(std::string name);

            bool findAndDelete(std::string fileName);
            bool findAndDeleteIterator(std::string fileName);            
            
            std::string printSet();
            void printSetAndDistances(const std::vector<Cluster::RGBClusterSet> & setOfClusters, const int & clusterNo );
            double distanceRGB(const std::vector<std::vector<int>> & point, const Cluster::RGBClusterSet & set);
            
        };      

        //===========================================================================================

        // Nested class to hold HSV cluster sets ========================================================

        class HSVClusterSet
        {
        private:
            std::unordered_map<std::string,std::vector<std::vector<int>>> s;
            std::vector<std::vector<int>> centroid;
            std::string centroidName;
        public:
            HSVClusterSet(/* args */);
            ~HSVClusterSet();

            void setCentroid(const std::string & name, const std::vector<std::vector<int>> & centroid);
            std::vector<std::vector<int>> getCentroid();
            std::string const getCentroidName();
            void calcCentroid();

            int getSize();
            double getSpread();
            void add(const std::string & fileName, const std::vector<std::vector<int>> & hisogram);
            bool contains(std::string name);

            bool findAndDelete(std::string fileName);
            bool findAndDeleteIterator(std::string fileName);            
            
            std::string printSet();
            void printSetAndDistances(const std::vector<Cluster::HSVClusterSet> & setOfClusters, const int & clusterNo );
            double distanceHSV(const std::vector<std::vector<int>> & point, const Cluster::HSVClusterSet & set);
            
        };      

        //===========================================================================================

        // Perform unix terminal commands and pipe back to program
        std::string exec(std::string command);
    
        std::vector<Cluster::PixelIntensityClusterSet> PIclusters; // PI cluster data
        std::vector<Cluster::RGBClusterSet> RGBclusters; // PI cluster data


        std::vector<double> distancesPI; // keep information of distances for PI
        std::vector<double> distancesRGB; // keep information of distances for RGB  

        std::unordered_map<std::string,std::vector<int>> clusterData; // PI Cluster data
        std::unordered_map<std::string,std::vector<std::vector<int>>> RGBclusterData; // RGB Cluster data
        std::unordered_map<std::string,std::vector<std::vector<int>>> HSVclusterData; // HSV Cluster data

        std::vector<LCKMAT002::PPM> images; // PPM image data
        int binSize; 

        // Find and delete file from all clusters, except for cluster x
        bool findAndDeletePI(std::string fileName, int x);    
        bool findAndDeletePIterator(std::string fileName, int x); 
        void addPI(std::string fileName, std::vector<int> hisogram);  

        bool findAndDeleteRGB(std::string fileName, int x);    
        bool findAndDeleteRGBIterator(std::string fileName, int x); 
        // void addPI(std::string fileName, std::vector<int> hisogram);
        

    public:
        Cluster(/* args */);
        ~Cluster();

        // Read PPM images to gather image data
        bool readImages(const std::string & directoryName,const bool & shuffle);

        // Set binSize
        void setBinSize(const int & binSize);

        // Generate Histograms
        bool generateHistograms(int TAG);

        // Initialise k-means
        bool randomCentroids(int TAG, int noClusters,unsigned int seed);
        bool pointsToRandomClusters(int TAG, int noClusters,unsigned int seed);
        bool kMeansPlus(int TAG, int noClusters,unsigned int seed);

        // Iterare Cluster
        bool iterateClusters(int TAG);

        // Operator Overloading
        friend std::ostream& operator<<(std::ostream& os, const Cluster& dt);

        // Check PIClusters
        bool const isEmptyPI();

        // Geat spread of cluster
        double getSpread(int TAG);

    };
    
    // Operator Overloading
    std::ostream& operator<<(std::ostream& os, const Cluster& dt);
        

}

#endif