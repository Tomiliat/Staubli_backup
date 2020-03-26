#include <iostream>
#include <fstream>
#include "mySerial.h"
#include <math.h>
#include <cmath>
#include <sstream>
// Core ros functionality like ros::init
// #include <ros/ros.h>

// printer variables & parameters
float printspeed = 40;
float diambuse = 0.4;      // Buse from french = Layer
float hcouche = 0.2;
float diamfil = 1.75;
// F = feedrate = (60 * 4 * diambuse * hcouche * printspeed)/(diamfil^2 * pi)

// E = ((pi * diamfil^2) / (4)) * L1. 


int main(int argc, char **argv)
{
    // Here we calculate new parameters to F
    float feedrate = (60 * 4 * diambuse * hcouche * printspeed)/(std::pow(diamfil, 2) * M_PI);
    std::ostringstream ssF;
    ssF << feedrate;
    std::string feedrateString = ssF.str();

    // Initialize ROS
    // ros::init(argc, argv, "printertool");
    // ros::NodeHandle nh;

    std::string filename, line, path;
    std::ifstream read;

    // Here we define our port and baudrate, and make communication.
    mySerial serial = mySerial("/dev/ttyUSB0", 9600);

    // UNCOMMENT THIS, IF you want your file to be read from same directory with the script.
    /* 
    std::cout << "Enter G-Code filename: ";
    std::getline(std::cin, filename);
    read.open(filename);
    */
   
    // UNCOMMENT THIS, IF you want your file to be read from absolute path.
    //*
    std::cout << "Enter G-Code filename: ";
    std::getline(std::cin, filename);
    read.open("/home/tomiliatsereh/staubli_ws/src/manufacturing_6dof/gcode/" + filename); // "home/[USERNAME]/[PATH_TO_YOUR_FILE]" <- edit
    //

    //* UNCOMMENT THIS, IF you want to ask user for path and filename.
    /*
    std::cout << "Enter full path to G-code file folder: ";
    std::getline(std::cin, path);

    std::cout << "Enter G-Code filename: ";
    std::getline(std::cin, filename);
    
    read.open(path + filename);
    */


    if (read.is_open())
    {
        std::string oldE = "0";
        std::string actE;
        double newE;

        // Search F and change feedrate value. Overwrite with new parameters.
        while (getline(read, line))
        {
            if (isdigit(line[line.find("F") + 1]))
            {
                int fStartPos = line.find("F") + 1;
                int fSpacePos = line.find(" ", fStartPos);
                int feedrateLength = fSpacePos - fStartPos;
                line.replace(fStartPos, feedrateLength, feedrateString);
            }

            // Search E and change parameters
            if (isdigit(line[line.find("E") + 1]))
            {
                int eStartPos = line.find("E") + 1;
                int eSpacePos = line.find(" ", eStartPos);
                int eLength;
                if (eSpacePos == std::string::npos)
                {
                    // Space not found after "E", extruder value ends at the line end
                    eLength = line.length() - eStartPos;
                    actE = line.substr(eStartPos);
                }
                else
                {
                    // After "E" found space, extruder value is between "E" and " "
                    eLength = eSpacePos - eStartPos;
                    actE = line.substr(eStartPos, eLength);
                }
                newE = std::stod(actE) - std::stod(oldE);
                // Convert newE to string
                std::ostringstream ssE;
                ssE << newE;
                std::string extruderString = ssE.str();
                line.replace(eStartPos, eLength, extruderString);
                oldE = actE;
            }

            // cout just to see what is happening, not important and you can remove this if you don't need it in the future
            std::cout << line << "\n";

            // sending our gcode line by line
            serial.Send(line);
        }
        read.close();
    }

    else
        std::cout << "Unable to open file \n";

    return 0;
}