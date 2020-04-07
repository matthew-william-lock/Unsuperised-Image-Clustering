#ifndef CLUSTER_H
#define CLUSTER_H

#include "PPM.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory> // C++11
#include <string>

#define PIXEL_INTENSITY_TAG 0

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
            

            bool findAndDelete(std::string fileName);
            bool findAndDeleteIterator(std::string fileName);
            void add(const std::string & fileName, const std::vector<int> & hisogram);
            int getSize();
            void calcCentroid();
            std::unordered_map<std::string,std::vector<int>> getS();
            void printSet();
            void printSetAndDistances(const std::vector<Cluster::PixelIntensityClusterSet> & setOfClusters, const int & clusterNo );

            double distancePI(const std::vector<int> & point, const Cluster::PixelIntensityClusterSet & set);
            
        };      

        //===========================================================================================

        // Perform unix terminal commands and pipe back to program
        std::string exec(std::string command);
    
        std::vector<Cluster::PixelIntensityClusterSet> PIclusters; // cluster data
        std::vector<double> distancesPI; // keep information of distances

        std::unordered_map<std::string,std::vector<int>> clusterData;
        std::vector<LCKMAT002::PPM> images; // image data
        int binSize; 

        // Find and delete file from all clusters, except for cluster x
        bool findAndDeletePI(std::string fileName, int x);    
        bool findAndDeletePIterator(std::string fileName, int x); 
        void addPI(std::string fileName, std::vector<int> hisogram);  
        

    public:
        Cluster(/* args */);
        ~Cluster();

        // Read PPM images to gather image data
        bool readImages(const std::string & directoryName);

        // Set binSize
        void setBinSize(const int & binSize);

        // Generate Histograms
        bool generateHistograms(int TAG);

        // Generate Cluster
        bool generateHistograms(int TAG, int noClusters);

        // Generate Cluster
        bool iterateClusters(int TAG);         

    };
        

}

#endif