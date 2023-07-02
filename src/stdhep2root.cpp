//
// Created by Maurik Holtrop on 7/2/23.
//
#include "stdhep_util.hh"
#include "cxxopts.hpp"
#include <iostream>
#include "TFile.h"
#include "TTree.h"

using namespace std;

int main(int argc, char **argv) {

   // Define the Tuple variables.
   vector<int> pdg;
   vector<double> px,py,pz;
   vector<double> vx,vy,vz;
   vector<double> energy;
   vector<double> mass;

   stdhep_event event;

   setlocale(LC_NUMERIC, "");
   cxxopts::Options options(argv[0], ". This program will translate StdHep file to root tuples.");
   options
         .positional_help(" infiles - input stdhep files.")
         .show_positional_help();

   bool apple = false;

   options
         .add_options()
               ("d,debug", "Set debug level", cxxopts::value<int>()->default_value("0"))
               ("o,outputfile", "Output file.", cxxopts::value<std::string>()->default_value("stdhep.root"))
               ("i,inputfiles", "Input stdhep files, -i is optional.",
                cxxopts::value<std::vector<std::string>>())
               ("h,help", "Print help");

   options.parse_positional({"inputfiles"});
   auto args = options.parse(argc, argv);

   if (args.count("help")) {
      std::cout << options.help() << std::endl;
      exit(0);
   }

   int debug = args["debug"].as<int>();
   auto inputfiles = args["inputfiles"].as<std::vector<std::string>>();
   auto outputfile = args["outputfile"].as<std::string>();

   if( debug) {
      cout << "Output file: " << outputfile << endl;
      cout << "Input files: ";
      for (auto s: inputfiles) cout << s << " ";
      cout << endl;
   }

   auto root_file = new TFile(outputfile.c_str(),"RECREATE");
   auto output_tree = new TTree("StdHep", "StdHep particles tree");
   output_tree->Branch("pdg", &pdg);
   output_tree->Branch("px", &px);
   output_tree->Branch("py", &py);
   output_tree->Branch("pz", &pz);
   output_tree->Branch("energy", &energy);
   output_tree->Branch("mass", &mass);
   output_tree->Branch("vx", &vx);
   output_tree->Branch("vy", &vy);
   output_tree->Branch("vz", &vz);

   long nevt=0;

   for(auto file: inputfiles) {
      int istream = 0;
      if(debug) cout << "Opening file " << file << endl;
      int n_events = open_read((char *)file.c_str(), istream);

      while (read_next(istream)) {
         if( (++nevt)%10000 == 0 ) printf("evt: %9ld \n", nevt);
         pdg.clear();
         px.clear();
         py.clear();
         pz.clear();
         vx.clear();
         vy.clear();
         vz.clear();
         energy.clear();
         mass.clear();

         read_stdhep(&event);

         for (int j=0;j<event.particles.size();j++){
            pdg.push_back(event.particles[j].idhep);
            px.push_back(event.particles[j].phep[0]);
            py.push_back(event.particles[j].phep[1]);
            pz.push_back(event.particles[j].phep[2]);
            energy.push_back(event.particles[j].phep[3]);
            mass.push_back(event.particles[j].phep[4]);
            vx.push_back(event.particles[j].vhep[0]);
            vy.push_back(event.particles[j].vhep[1]);
            vz.push_back(event.particles[j].vhep[2]);
         }

         output_tree->Fill();
         event.particles.clear();
      }
      close_read(istream);
   }

   root_file->Write();
   root_file->Close();
}