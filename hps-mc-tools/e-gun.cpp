//
//  main.cpp
//  hps-mc-tools
//
//  Created by Maurik Holtrop on 7/5/18.
//  Copyright © 2018 Jefferson Lab. All rights reserved.
//
// For PID info see: https://cdcvs.fnal.gov/redmine/projects/heppdt/wiki/Notes_about_StdHep
// Not much information about the details of the StdHep data format available from the official sites.
// Documentation for STDhep: http://cd-docdb.fnal.gov/0009/000903/015/stdhep_50601_manual.ps
#include <iostream>
#include <math.h>
#include "stdhep_util.hh"

int main(int argc, const char * argv[]) {
    stdhep_event evt;
    struct stdhep_entry *temp = new struct stdhep_entry;
    const double pi=3.141592653589793;
    
    std::cout << "This program will send 1000 electrons to a stdhep file called electrons.stdhep.\n";
    int ostream = 0;
    
    int nevent=10000;
    open_write((char *)"electrons.stdhep",ostream,nevent);
    
    double momentum=2.3; // GeV
    double theta = 0.03; // rad
    double phi   = pi/2.;      // rad
    
    for(int i=0;i<nevent;++i){
        evt.nevhep=i+1;
        temp->isthep=1;  // Final state particle.
        temp->idhep=11;  // PID - electron
        temp->jmohep[0]=0; // Position of mother particle in list.
        temp->jmohep[1]=0; // Position of second mother particle in list.
        temp->jdahep[0]=0; // Position of daughter particle in list.
        temp->jdahep[1]=0;
        temp->vhep[0]= 0; // Vertex-x;
        temp->vhep[1]= 0; // Vertex-y;
        temp->vhep[2]= 0; // Vertex-z;
        temp->vhep[3]= 0; // Production time;
        temp->phep[0]= momentum*sin(theta)*cos(phi); // Momentum-x;
        temp->phep[1]= momentum*sin(theta)*sin(phi); // Momentum-y;
        temp->phep[2]= momentum*cos(theta); // Momentum-z;
        temp->phep[3]= sqrt(momentum*momentum+0.000511*0.000511); // Energy;
        temp->phep[4]= 0.000511; // Mass;
        evt.particles.push_back(*temp);
        write_stdhep(&evt);
        write_file(ostream);
        
    }
    close_write(ostream);
    return 0;
}
