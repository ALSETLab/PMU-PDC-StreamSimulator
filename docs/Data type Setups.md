
To configure the data to be used as file input the file has to be saved as ‘.angcsv’, ‘.phscsv’, or ‘.digcsv’. For most files that have phasor measurements the data will most likely be structure into columns that separate the data based on type. Most of the test files were extracted from excel files that ordered the data in columns for their respective types, such as, voltage( magnitude, angle), current(magnitude, angle), digital and frequency. To use the data you’ll need to save in a txt file and change the name to have one of the three extensions. The data must also be separated by commas to be streamed by the simulator. An easy way to convert the data would be import the data needed into an excel spreadsheet and concatenate the numbers into a single line with each number separated by a comma. For phasor data, you’ll need to convert from polar to rectangular than concatenate.

  

Example:

![](https://lh3.googleusercontent.com/7AHthtevHX_hIDbt8ysh4h1QZsI_wXgF3TVe66cD46NXP1u2EuwbafCMOga9tCAdnlO_RE2QYZjnT8J6z18w7Rr65q52hE-0tQBMXFE829pxXwTvn-86kZhkktUMDHmqlrVY443D)

Columns A and B are the phasor data

Column D =ROUNDUP(A_*COS(B_*PI()/180),2)

Column E =ROUNDUP(A_*SIN(PI()*B_/180),2)

Column G =_xlfn.CONCAT(D_,";",E_,",")

Column J =CONCATENATE((Combined real and imaginary),(Row above))
