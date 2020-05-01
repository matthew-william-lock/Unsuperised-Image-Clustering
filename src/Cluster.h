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

#define PIXEL_INTENSITY_TAG 0
#define RGB_TAG 1

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
            std::string getCentroidName();

            int getSize();
            void add(const std::string & fileName, const std::vector<std::vector<int>> & hisogram);

            bool findAndDelete(std::string fileName);
            bool findAndDeleteIterator(std::string fileName);
            // 
            // 
            void calcCentroid();
            // std::unordered_map<std::string,std::vector<int>> getS();
            // void printSet();

            void printSetAndDistances(const std::vector<Cluster::RGBClusterSet> & setOfClusters, const int & clusterNo );
            double distanceRGB(const std::vector<std::vector<int>> & point, const Cluster::RGBClusterSet & set);
            
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