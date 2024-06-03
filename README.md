



<br/>
<br/>

NOTE: 

1. The attached 'physics_particles.txt' and 'weather_measurements.csv' files are the two csv files I was using for testing in my program, and I wrote the functions for parsing these files as practice for making the standardized version of the parsing function to work with all csv files. Also, this is not for work or school but is just a freetime project.

2. The methods of this program are primarily designed for datasets where the first line in the CSV formatted file is the header, and the following lines contain data entries.

3. The vast vast vast majority of the time spent working on this coding exercise for me was spent on trying to standardize data preprocessing to ensure compatibility throughout the entire program, namely, the Utilities files. This is worth being aware of simply due to how under-developed the other parts of the program are(the integrators and statistical methods are mostly exempt, not much of a computational challenge, just math challenge).

<br/>

![Screenshot 2024-02-20 at 12 59 02 AM](https://github.com/DavidRichardson02/CSV-File-Data-Set-Analysis/assets/144840390/da25c0f9-ca64-4e64-96b1-d33eb89e974c)
<br/>

The screenshot attached directly above this text is a picture of the directory my XCode c program is contained in, the portion of the screen circled in red displays the directory and files that were automatically generated for the data set 'physics_particles'. The name of each file in the directory is a combination of the name of the data set file(extracted from pathname), the category these files belong to(Plottable_Field), the order in which they appear in the data set when all non-plottable fields have been omitted and the plottable fields are pushed back to take their place, and the final portion of the naming comes from the name of the data field itself. Lastly, the generated plottable data file with no number or name is a combination of all of the plottable data fields lined up in order and written to this file with a line skipped between each one. 

<br/>

The original file of the example data set used by me, 'physics_particles.csv', as well as the generated file of all of the formatted plottable data fields, 'physics_particles_Plottable_Field.txt', can be found on the main page of this repository, also there is a generated file for the unplottable data fields that I plan to incorporate into the MATLAB script creation later in development but it is incomplete as of yet(and even when it's finished it will only really be noticeable for it's cosmetic affect in providing labels and/or units to create the plots with).




