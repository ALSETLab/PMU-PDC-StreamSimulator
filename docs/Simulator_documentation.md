# PDC Simulator Version 30.04.2013

  

## Overview

The PDC simulator can be configured to simulate multiple PMUs and generate different values and errors. The simulated data is stream based on the IEEE C37.118.2 protocol which dictates that data be streamed in four message types of data, configuration, header, and command frames. The simulator can produce three different types of values; phasor, analog and digital as well as having options for frequency and delta frequency. The connection to the PMU/PDC server is setup through similar means to PMU applications which needs IP address, port number and ID code/ PMU Id.

  

##  Navigating

Navigation through the simulator is done by the sidebar which breaks down the PDC into PMU and the PMUs into value types.

![](https://lh4.googleusercontent.com/Hb2LodTRqRaZg-Mjok6c5DEzYX56VW8E2W-84KvrsgecXYYFz-aui_ccIQqof9pzBBNqJaVNbVq5wQimxd4xhNxitBAEsaYoOa0IwKj2N0CRgqlq0vLfm7sztkcqysyeKc3-sTe4)

  

##  PDC Configuration

![](https://lh6.googleusercontent.com/D7nZEAAx2eJw8AGY52qNWx-wFVu6RP3H-9hrtV0hi8Xiz08uhDUa_W24CShdcLN8CqC7zwZONNRvBEvr60gt2C86eZgwBGpa61zVHwUG3hhKyRDkZsjdoa0WM0SAfF9TPdEYAiDs)

-   PDC Project name: Sets the name that will be used for current simulated PDC.
    
-   Listen port/TCP: Represents the port number the PDC will connect too to transmit data.
    
-   PDC ID: An ID created by the user for the purpose of distinguishing a specific PDC
    
-   Time: The rate for sampling in milliseconds
    
-   Data rate: The speed for data transfer
    

**To change listen port in source code so it starts on a specific port on startup /StrongridSimulatorGUI_MFC/ManagedPdcSimulator.cpp approximately line 63 change listen port

![](https://lh3.googleusercontent.com/w9kBapSo8HWolLZygy7GnU7qXLgosm-08aqUitn-KsjTHmdRW94_hzAmS17wrvy7OayUtd0bvyU9C1AOPYrkW-2dISsWJfegDzIRJZbCWNw7kr7vqUkd0z1pVGPXpDXDXqiofbhp)

Above image shows the PMUs overseen by the PDC. In this window the user can create more PMUs or delete any created. The user can start or stop the data stream as well as send the configuration of that PMU to the receiver. The file bottom below the next to the x will create the PMU and the x will delete them. The “Start simulator” button starts the transmission of data for every PMU.

  

## PMU Settings

In the PMU Station setting, the user can customize setting for the different simulated PMUs. The information available for the PMUs can be set to identify the type of data that PMU will stream, for example, the PMU format doesn’t affect actual values (in this version) however will determine in what form the data will be in from either the file or the constant. Real Time data can simulate any errors or frequency for the PMU.

  

## Station UI

  
  
  
  
  
  
  

### Basic information:

![](https://lh3.googleusercontent.com/WPD7r5fF4kCuHaKIsJwARsvRGMKJ1nrxuQIKpOD4v4Wiz4JWp0JC8x0QdKhqXYWZrjVGvsZaNfUSLAZosjqFLlVPSj1MaWC89UxmypqJCuMmNE_8zVTJGcDuUbJxB5NSOEP4zrXY)

-   Station name: allows the user to name the current PMU, since the application can run multiple PMUs naming can easily distinguish them. This option also can help identify where the data is coming from. The examples used for documentation are based on readings from grids in Oklahoma, having distinct names for the sources will help understand the results.
    
-   Id code: configuration code to distinguish the sources of data, and can be used to help data identification when connected with applications.
    
-   The nominal frequency is either 60 or 50 hertz and further characterizes the data information. Depending on the type of frequency can determine where the data was taken from.
    
-   Config change count: keeps track of what version of testing the user is on; completely optional.
    

  

### PMU Format:

![](https://lh4.googleusercontent.com/fmkXebB3u0u5ZLx9AfazTGsnnKpPEa7qg-q_Kqf19xF-X9fUOFlUidy2YxkKJ0Iq16tZWDHtdyv4I7LYIgypisICM5fU-F1LrHQfAnzI-Tv3IvNClU2UbB_tV4giVCWGpNtKoeG4)

-   The information that can be configured in the PMU format is only useful for understanding the data coming in and does not affect the actual data values.
    
-   For phasor data to be analyzed properly, the data must be in rectangular form and not polar (in this version). The PDC will read the angle as the imaginary value and mess the polar and rectangular conversions.
    

  
  
  
  
  

### Real Time Data:

![](https://lh6.googleusercontent.com/gz_NVxg2dngbdTI4eAM5mSEB5SO6rIQhj_BiHneblQe-1VN-TPz-5nkkMHLcHdjeahtyaQzGiSXbZuvJiaE-2N1S0K5lJ_Jun-UHH6CmJ-5SJrtRy3UfGdwDKYY013YBJoT-QyVz)

  The real time data allows causing error codes, setting frequency values, and clock options. These setting can be used to test the reaction of an application to errors.
    
   There are 4 Data error options:
    

-   Good measurement data - No error
    
-   PMU error- No information about data
    
-   PMU in test mode- Doesn’t use certain user selected options for the PMU such as setting constants for the values, but run normal without those included
    
-   PMU error - Doesn’t use certain user selected options for the PMU such as setting constants for the values.
    

Trigger:
    

-   Manual-Transmits a trigger from the start
    
-   magnitude low-When a data reading is lower than the rest it will send a trigger
    
-   magnitude high-When a data reading is higher than the rest it will send a trigger
    
-   Phase angle diff-When a phase angle difference is past a certain range
    
-   Frequency high or low- If the is too far from 50 or 60
    
-   df/dt High- if the change in frequency is to high
    
-   reserved- set by the user
    
-   Digital- can set a trigger to be based on the digital value change. If digital value is normally 0 then switches to 1 then a trigger is sent.
    

   Clock quality: sets time error
    

-   Not used (indicates code from previous version of profile)
    
-   Estimated maximum time error < 100 ns
    
-   Estimated maximum time error < 1 us
    
-   Estimated maximum time error < 10 us
    
-   Estimated maximum time error < 100 us
    
-   Estimated maximum time error < 1 ms
    
-   Estimated maximum time error < 10 ms
    
-   Estimated maximum time error > 10 ms or time error unknown
    

   Clock unlocked: Range of difference between actual time and read time
    

-   sync locked or unlocked < 10 s (best quality)
    
-   10 s <= unlocked time < 100 s
    
-   100s < unlock time <= 1000s
    
-   unlocked time > 1000s
    

Check box options:

-   Invalid clock sync: PMU time not accurate
    
-   Data sort (0=chronological, 1=by arrival): data either arrives sorted by timestamp or when the system reads it
    
-   Trigger detected
    
-   Config change pending: change to the PMU/PDC in queue and has not been applied
    
-   Data modified by post processing: a change to the data after a process has occurred
    

  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  

## Data Types

### Phasor

![](https://lh5.googleusercontent.com/KbP3rF-tApLk0OWD2huoHLU8U4zmtgBx2Ppx-nbE1ZjshwOW-1HDPnTd3b-UlkWs-K4piCGyanci_QRfZycDib7YsVq-ShS7bVVjF-hzcomK5_NyX6RnIF68EOYkw9EBct1LHNgm)

-   Name of the data stream helps organize the data being streamed, can be used for specific sources of the data.
    
-   Phasor data types can only be inputted as rectangular form and streamed out in rectangular form in this version of the PDC. The current and voltage options can be used to set the type of data being streamed, doesn’t affect actual data. Scalar doesn’t affect data values.
    
-   Two types of inputs are constant and file-based, file must be in “.phcsv” file type. Meaning a comma separated file based only for phasor values in rectangular form.
    
-   Constant values are determined and set by the user
    

  
  
  
  
  

### Analog

![](https://lh6.googleusercontent.com/3JxjJWQaB2BaVaGo_kARsK_wjiX4rNuKxBP5grKw_78jWhMzz836yRuaHbLyUlm8xpmBzimUuTwU77ThBUvaa_uRmSSdYKnGKzGUp4Wa1CB5e3mkTh3jVdtxmLCgJXbgQz8b0gKv)

   Name of the data stream helps organize the data being streamed, can be used for specific sources of the data.
    
  Type
    

-   Single point on wave
    
-   RMS of analog input
    
-   Peak of analog input
    

-   Two types of inputs are constant and file-based, file must be in “.angcsv” file type. Meaning a comma separated file based only for analog values.
    
-   Constant values are determined and set by the user
    

  
  
  
  
  
  

### Digital

![](https://lh3.googleusercontent.com/ZIj5elf6mTW8HbDKsdh63IATwGSPk0lp6hqOMxfvemWHug8HFTQsp1_sd26kpckgPrZm_U8wDYBnOp_Z7F0GwKAedPcGPjZgPTcI8Nuks7xCjJK3fJmmF3QOuNfpJ27MRM2VdETI)

-   Name of the data stream helps organize the data being streamed, can be used for specific sources of the data, such as streaming outputs to determine if an error occurred when running other values.
    
-   Nominal represents whether the bit starts off on or off does nothing for actual data determination.
    
-   Two types of inputs are constant and file-based, file must be in “.digcsv” file type. Meaning a comma separated file based only for digital values.
    
-   Constant values are determined and set by the user
    

  
  
  
  
  
  
  
  

## Example

*Done with labview and S3DK add-on

![](https://lh4.googleusercontent.com/TsNybShoXa2T-sQfmkS2L-Bspl5crGWppHLRf6rd9ggKBucSwogLM2CGgtkTXtFkCCWASFn2_8eoQLLQe7_3pnbTMsh0GIh-xDaIGEYpLau10z32jamPTToVH_TvvTHbjF4-Xbnh)

-   With this version analog isn’t streaming
    
-   Even though the phasor value is only processed as rectangular, polar form can be converted
