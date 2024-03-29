//
//  main.cpp
//  hps-mc-tools
//
//  Created by Maurik Holtrop on 7/5/18.
//
// For PID info see: https://cdcvs.fnal.gov/redmine/projects/heppdt/wiki/Notes_about_StdHep
// Not much information about the details of the StdHep data format available from the official sites.
// Documentation for STDhep: http://cd-docdb.fnal.gov/0009/000903/015/stdhep_50601_manual.ps
//
// Throw a bunch of electrons or positrons in a uniform distribution into a square area.
//
#include <iostream>
#include <math.h>
#include "stdhep_util.hh"
#include "cxxopts.hpp"
#include <iostream>
#include <iomanip>
#include <map>
#include <random>
#include <locale>
#include <cmath>
using namespace std;

int main(int argc, char **argv) {
    stdhep_event evt;
    struct stdhep_entry *temp = new struct stdhep_entry;
//  const double pi=3.141592653589793;

    setlocale(LC_NUMERIC, "");
    cxxopts::Options options(argv[0], ". This program will send n"
                                      " particles of type p to a stdhep file.");
    options
            .positional_help(" outfile - Output file.")
            .show_positional_help();

    bool apple = false;

    options
            .add_options()
                    ("d,debug", "Set debug level", cxxopts::value<int>()->default_value("0"))
                    ("p,part",  "Set particle type [default: 11]", cxxopts::value<int>()->default_value("11"))
                    ("m,mass",  "Set particle mass [default: 0.000511]", cxxopts::value<double>()->default_value("0.000511"))
                    ("min",  "Set minimum momentum [default: 0.1]", cxxopts::value<double>()->default_value("0.1"))
                    ("max",  "Set maximum momentum [default: 2.3]", cxxopts::value<double>()->default_value("2.3"))
                    ("xmin",  "Set minimum x at 150cm (Ecal) [default: -42.]", cxxopts::value<double>()->default_value("-42."))
                    ("xmax",  "Set maximum x at 150cm (Ecal) [default: 42.]", cxxopts::value<double>()->default_value("42."))
                    ("ymin",  "Set minimum y at 150cm (Ecal) [default: 1.]", cxxopts::value<double>()->default_value("1."))
                    ("ymax",  "Set maximum y at 150cm (Ecal) [default: 10.]", cxxopts::value<double>()->default_value("10."))

                    ("o,outputfile", "Output files (-o is optional) ", cxxopts::value<std::string>()->default_value("gun.stdhep"))
                    ("n,num_evt","Only process num_evt event [default:100000]", cxxopts::value<long>()->default_value("100000"))
                    ("h,help", "Print help");

    options.parse_positional({"outputfile"});
    auto args = options.parse(argc, argv);

    if (args.count("help"))
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    int ostream = 0;
    string outputfile = args["outputfile"].as<std::string>();
    long nevent = args["num_evt"].as<long>();
    int  part_type = args["part"].as<int>();
    double part_mass = args["mass"].as<double>();
    double momentum_min = args["min"].as<double>(); // GeV
    double momentum_max = args["max"].as<double>(); // GeV
    double x_min = args["xmin"].as<double>();     // cm @ 150 cm - a little inside the ECAL
    double x_max = args["xmax"].as<double>();     // cm @ 150 cm - a little inside the ECAL
    double y_min = args["ymin"].as<double>();      // cm @ 150 cm
    double y_max = args["ymax"].as<double>(); ;      // cm @ 150 cm

    open_write((char *)outputfile.c_str(),ostream,nevent);


    // Seed with a real random value, if available
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> momentum_dis(momentum_min, momentum_max);
    std::uniform_real_distribution<> x_dis(x_min,x_max);
    std::uniform_real_distribution<> y_dis(y_min,y_max);
    std::uniform_real_distribution<> one(0.,1.);

    for(int i=0;i<nevent;++i){

        double momentum = momentum_dis(gen);
        double x        = x_dis(gen);
        double y        = y_dis(gen);
        if( one(gen) < 0.5) y = -y;
        double r        = sqrt(x*x+y*y+150.*150.);
        double theta    = acos(150./r);   // theta=arccos(z/r) rotated 32.5 mrad to +x
        double phi      = atan2(y,x);

        evt.nevhep=i+1;
        temp->isthep=1;  // Final state particle.
        temp->idhep=part_type;  // PID - electron = 11
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
        temp->phep[3]= sqrt(momentum*momentum+part_mass*part_mass); // Energy;
        temp->phep[4]= part_mass; // Mass;
        evt.particles.push_back(*temp);
        write_stdhep(&evt);
        write_file(ostream);

    }
    close_write(ostream);
    return 0;
}
