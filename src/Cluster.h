#ifndef CLUSTER_H
#define CLUSTER_H

#include "PPM.h"

#include <iostream>
#include <vector>

namespace LCKMAT002
{

    class Cluster
    {
    private:

        std::vector<LCKMAT002::PPM> images; // image data

        // Perform unix terminal commands and pipe back to program
        std::string exec(std::string command);


    public:
        Cluster(/* args */);
        ~Cluster();

        bool readImages(const std::string & directoryName);
    };
        

}

#endif