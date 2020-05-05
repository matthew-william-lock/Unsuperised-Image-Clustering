# K-Means Clustering Tool
*Disclaimer : This is a markdown document and should be read using a  [markdown editor](https://stackedit.io/app#)*

This program makes use of the K-Means clustering to group any number of PPM images provided by the user.
<br>Functionality of features of this K-Means implementation include:
- Ability to work on dataset of any size
- Ability to manipulate basic K-means parameters
- Ability run K-Means a specified number of times to find best clustering of data
- Ability to pick how inital centroids / clusters are initialised (K-Means++ included)
- Ability to shuffle input data to make sure clustering is not affected by initial order of dataset

## Extra Credit Work

### RGB Hisograms
The ability to use RGB pixels for the image feature has been implemented.
Add the follow argument when running program to use RGB image space:
```
- color rgb
```

### HSV Hisograms
One can convert the RGB space to an HSV (Hue, Saturation and Value Intensity) space. This will allow the program to more finely distinguish between images as there is less overall variations in parameters such as hue as compared to simply counting the number of RGB pixels. In testing the given dataset, K-Means was found to best cluster the 100 images of various digits using the HSV image feature with a bin size of roughly 70. We can run the program with these parameters as follows:
```
./clusterer Gradient_Numbers_PPMS -bin 70 -color hsv    
```

The inspiration for the HSV image feature was taken from [Image Clustering using Color, Texture and Shape Features](https://www.researchgate.net/publication/220595166_Image_Clustering_using_Color_Texture_and_Shape_Features), while the algorithm to convert from RGB to HSV can be found [here](https://www.geeksforgeeks.org/program-change-rgb-color-model-hsv-color-model/)

## Source files
**Cluster.cpp:** : Acts as a driver to implement K-Means <br>
**clusterer.h**: Defines the cluster class which handles all functions related to K-Means<br>
**volimage.cpp**: Implements the clusterer class<br>
**PPM.h**: Defines PPM class used to read PPM image files<br>
**PPM.cpp**: Implements the PPM class<br>

## Installation
**Step 1:** Compile Cluster
```
cd src
make
```

**Step 2:** Run Cluster
```
cd src
./clusterer <dataset> [-o output] [-k n] [-bin b] [-color c] [-init i] [-it n] [-shuff b]
```

# Usage

**Default Parameters:**
- output = null
- k = 10
- bin size = 10
- color = black and white
- initialisation = K-Means++
- shuffle = false

**General Notes:**
- Decreasing the bin size will result in a slower runtime for the RGB and HSV image features
- Deacreasing the bin size will generally decrease accuracy of clustering

Optional arguments:
| Parameter                 | Description   |	
| :------------------------ | :-------------|
| -i output_file_name | Indicate the name of the file to write the final clustering to. This file will be located in /bin. If no output file is set, the output will be displayed on the terminal
| -k k_value | Set the number of clusters for K-means.
| -bin bin_size | Set a valid bin size in the range (1-255)
| -color image_feature | Set image feature to be used. 'pi' indicates black and white feature space, 'rgb' indicates RGB feature space, 'hsv' indicates HSV feature space
| -init initialisation_method | Set K-means initialisation method. 'cent' will pick centroids at random, 'clust' will randomly assign images to initial clusters, and 'plus' will implement K-Means++ initialisation method
| -it number_of_iterations | Number of time K-Means will be run to find best spread
| -shuff shuffle | Indicates whethere or not to shuffle input data. 't' will shuffle input data while 'f' will read the input data in order

**Example Usages:**
```
# bin size set to 50
# initialisation method set to random centroids
./clusterer Gradient_Numbers_PPMS -bin 50 -init cent

# feature space will be RGB
# initialisation method set to K-means+
./clusterer Gradient_Numbers_PPMS -color rgb -init plus -shuff t

# bin size set to 70
# feature space will be HSV
./clusterer Gradient_Numbers_PPMS -bin 70 -color hsv 
```
