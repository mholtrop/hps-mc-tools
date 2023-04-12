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
    cxxopts::Options options(argv[0], "This program will send n"
                                      " particles of different types to a stdhep file. \n"
                                      " The combination of the --part  and --nmax option determines the mix of partices. \n"
                                      " --nmax n, determines the maximum number of particles in the event. \n"
                                      " --part p, determines the mix of particles: \n"
                                      "     p=1 through 7 : Only e-, e+, gamma, pi+, pi-, mu-, mu+ respectively. \n"
                                      "     p=8 : e- and e+ \n"
                                      "     p=9 : e-, e+, gamma. \n"
                                      "     p=10 : any particle. \n"
                                      "Examples:  \n"
                                      " particle-trainer -part 1 -n 1000 electron_1k.stdhep # create file with 1000 e- events.\n"
                                      " particle-trainer -part 9 -nmax 5 -n 1000 mix_1k.stdhep # 1000 e-, e+, gamma events, with up to 5 particles per event. \n");
    options
            .positional_help(" outfile - Output file.")
            .show_positional_help();

    bool apple = false;

    options
            .add_options()
                    ("d,debug", "Set debug level", cxxopts::value<int>()->default_value("0"))
                    ("p,part",  "Set particle type mix. See above.", cxxopts::value<int>()->default_value("1"))
                    ("nmax",  "Set max number of particles per event.", cxxopts::value<int>()->default_value("1"))
                    ("pmin",  "Set minimum momentum.", cxxopts::value<double>()->default_value("0.01"))
                    ("pmax",  "Set maximum momentum.", cxxopts::value<double>()->default_value("4.6"))
                    ("xmin",  "Set minimum x at 150cm (Ecal).", cxxopts::value<double>()->default_value("-45."))
                    ("xmax",  "Set maximum x at 150cm (Ecal).", cxxopts::value<double>()->default_value("45."))
                    ("ymin",  "Set minimum y at 150cm (Ecal).", cxxopts::value<double>()->default_value("1."))
                    ("ymax",  "Set maximum y at 150cm (Ecal).", cxxopts::value<double>()->default_value("10."))
                    ("rot",   "Set beam rotation.", cxxopts::value<double>()->default_value("32.5"))

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
    int debug = args["debug"].as<int>();
    string outputfile = args["outputfile"].as<std::string>();
    long nevent = args["num_evt"].as<long>();
    int  part_types = args["part"].as<int>();
    int  nmax = args["nmax"].as<int>();
    double momentum_low = args["pmin"].as<double>(); // GeV
    double momentum_max = args["pmax"].as<double>(); // GeV
    double x_min = args["xmin"].as<double>();     // cm @ 150 cm - a little inside the ECAL
    double x_max = args["xmax"].as<double>();     // cm @ 150 cm - a little inside the ECAL
    double y_min = args["ymin"].as<double>();      // cm @ 150 cm
    double y_max = args["ymax"].as<double>(); ;      // cm @ 150 cm
    double rot = args["rot"].as<double>(); ;      // cm @ 150 cm

    open_write((char *)outputfile.c_str(),ostream,nevent);

    vector<int> particle_types_available = { 11, -11, 22, 211, -211, 13, -13 }; // e- e+, gamma, pi+, pi-, mu-, mu+
    vector<double> particle_mass = {0.000510998946, 0.000510998946, 0. , 0.13957039, 0.13957039, 0.1056583745, 0.1056583745};

   vector<int> particle_indexes_to_choose_from;
    if(part_types >= 1 && part_types <= 7){
       particle_indexes_to_choose_from.push_back(part_types - 1);
    }else if(part_types == 8){
       particle_indexes_to_choose_from.push_back(0);
       particle_indexes_to_choose_from.push_back(1);
    }else if(part_types == 9){
       particle_indexes_to_choose_from.push_back(0);
       particle_indexes_to_choose_from.push_back(1);
       particle_indexes_to_choose_from.push_back(2);
    }else if(part_types == 10){
       for(size_t j=0; j<particle_types_available.size(); ++j) particle_indexes_to_choose_from.push_back(j);
    }else{
       printf("ERROR -- We do not know about your choise for --part. \n");
       return(1);
    }

    // Seed with a real random value, if available
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> momentum_dis(momentum_low, momentum_max);
    std::uniform_real_distribution<> x_dis(x_min,x_max);
    std::uniform_real_distribution<> y_dis(y_min,y_max);
    std::uniform_real_distribution<> one(0.,1.);
    std::uniform_int_distribution<>  choose_num(1,nmax);
    std::uniform_int_distribution<>  choose_part(0, (int)particle_indexes_to_choose_from.size());

    for(int i=0; i < nevent; ++i){

        evt.nevhep=i+1;
        int n_particles = choose_num(gen);
        for( int n=0; n< n_particles; ++n){
            int i_part = choose_part(gen);
            double part_mass = particle_mass[i_part];
            double momentum = momentum_dis(gen);
            double x        = x_dis(gen);
            double y        = y_dis(gen);
            if( one(gen) < 0.5) y = -y;
            double rxy        = sqrt(x*x+y*y);
            double theta = atan2(rxy, 150.);
            double phi   = atan2(y,x);
            double px = momentum*sin(theta)*cos(phi);
            double py = momentum*sin(theta)*sin(phi);
            double pz = momentum*cos(theta);
            px = px*cos(rot/1000.) + pz*sin(rot/1000.);
            pz = -px*sin(rot/1000.) + pz*cos(rot/1000.);
            double theta_x    = atan2(x,150.) + rot/1000.;  // theta=arccos(z/r) rotated 32.5 mrad to +x
            double theta_y    = atan2(y,150.);

            temp->isthep=1;  // Final state particle.
            temp->idhep=particle_indexes_to_choose_from[i_part];  // PID - electron = 11
            temp->jmohep[0]=0; // Position of mother particle in list.
            temp->jmohep[1]=0; // Position of second mother particle in list.
            temp->jdahep[0]=0; // Position of daughter particle in list.
            temp->jdahep[1]=0;
            temp->vhep[0]= 0; // Vertex-x;
            temp->vhep[1]= 0; // Vertex-y;
            temp->vhep[2]= 0; // Vertex-z;
            temp->vhep[3]= 0; // Production time;
            temp->phep[0]= px; // Momentum-x = momentum*sin(theta)*cos(phi);
            temp->phep[1]= py; // Momentum-y = momentum*sin(theta)*sin(phi);
            temp->phep[2]= pz; // Momentum-z = momentum*cos(theta);
            temp->phep[3]= sqrt(momentum*momentum+part_mass*part_mass); // Energy;
            temp->phep[4]= part_mass; // Mass;
            if(debug>0) printf("(x,y): (%7.2f, %7.2f)  (tht,phi): (%7.3f, %7.3f) p=(%7.3f, %7.3f, %7.3f)\n",
                               x,y,theta,phi,temp->phep[0],temp->phep[1],temp->phep[2]);

            evt.particles.push_back(*temp);
        }

        write_stdhep(&evt);
        write_file(ostream);

    }
    close_write(ostream);
    return 0;
}
