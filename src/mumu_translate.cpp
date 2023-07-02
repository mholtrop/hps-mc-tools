//
// Created by Maurik Holtrop on 6/22/23.
//
// Translator for CalcHEP output. See Andrea's email 6/22/23 10am.
#include <math.h>
#include "stdhep_util.hh"
#include "cxxopts.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <map>
#include <random>
#include <locale>
#include <cmath>
using namespace std;

void rotate_beam(double tht, double &x, double &z){
   // Rotate the beam vector x and z component around the y-axis by theta.
   double xp = cos(tht)*x + sin(tht)*z;
   double zp = -sin(tht)*x + cos(tht)*z;
   x = xp;
   z = zp;
}

int main(int argc, char **argv) {

   setlocale(LC_NUMERIC, "");
   cxxopts::Options options(argv[0], ". This program will translate the mu+mu- events from CalcHEP from text"
                                     " to stdhep format.");
   options
         .positional_help(" infile - input CalcHEP file.")
         .show_positional_help();

   bool apple = false;

   options
         .add_options()
               ("d,debug", "Set debug level", cxxopts::value<int>()->default_value("0"))
               ("o,outputfile", "Output file.", cxxopts::value<std::string>()->default_value("mumu.stdhep"))
               ("i,inputfile", "Input text file, -i is optional.", cxxopts::value<std::string>()->default_value("mumu.stdhep"))
               ("n,nevent","Number of events per output file.", cxxopts::value<long>()->default_value("0"))
               ("r,rotate","Rotate all vectors around the y-axis in the x-y plane by given angle in mrad. [30.5]",
                     cxxopts::value<double>()->default_value("30.5"))
               ("h,help", "Print help");

   options.parse_positional({"inputfile"});
   auto args = options.parse(argc, argv);

   if (args.count("help"))
   {
      std::cout << options.help() << std::endl;
      exit(0);
   }

   int debug = args["debug"].as<int>();
   auto beam_rot = args["rotate"].as<double>()/1000.;

   string inputfile = args["inputfile"].as<std::string>();
   ifstream in_file(inputfile);
   if( !in_file.is_open()){
      cout << "Could not open input file " << inputfile << endl;
      return(1);
   }

   long num_event_in_file;
   // We need to know the number of events in the input file for StdHEP output file.
   // Line 10 of input file has "#Number_of_events   10000".
   string line;
   bool done = false;
   while(!done){
      if( !getline(in_file, line)){
         cout << "Reached end of file without finding #Events tag. \n";
         return(1);
      };
      if( debug >1) cout << "::" << line << endl;
      size_t n_loc;
      if( (n_loc = line.find("Number_of_events") ) != std::string::npos){
         num_event_in_file = atoi(line.substr(18).c_str());
         if(num_event_in_file < 1){
            cout << "Input file parsing failed to find Number_of_events.\n";
            return(1);
         }
         if( debug ) cout << "The file has " << num_event_in_file << " events.\n";
      }
      if ( (n_loc = line.find("#Events") ) != std::string::npos){
         done = true;
      }
   }

   auto nevent = args["nevent"].as<long>();
   int n_out_files = 1;
   if(nevent>0) {
      int over_flow = (num_event_in_file%nevent > 0);
      n_out_files = int(num_event_in_file / nevent) + over_flow;
   }else{
      nevent = ((1UL)<<63)-1;
   }

   int lines_read = 0;
   for(int i_out_file=0;i_out_file < n_out_files; ++i_out_file) {
      // Open the stdhep file.
      int ostream = 0;
      string outputfile;
      if(n_out_files == 1){
         outputfile = args["outputfile"].as<std::string>();
      }else{
         string tmpf = args["outputfile"].as<std::string>();
         size_t loc = tmpf.rfind(".stdhep");
         char buffer[256];
         snprintf(buffer, 256, "%s_%03d.stdhep", tmpf.substr(0, loc).c_str(), i_out_file);
         outputfile = buffer;
      }
      open_write((char *) outputfile.c_str(), ostream, num_event_in_file);

      // Read and parse each line from the file.

      stringstream ss;
      stdhep_event evt;
      struct stdhep_entry *temp = new struct stdhep_entry;
      double xtmp, px, py, pz;
      double electron_mass = 0.51099895e-3; // e- mass in GeV.
      double muon_mass = 0.1056583755;      // muon mass in GeV.

      // For all particles all events:
      temp->jmohep[0] = 0; // Position of mother particle in list.
      temp->jmohep[1] = 0; // Position of second mother particle in list.
      temp->jdahep[0] = 0; // Position of daughter particle in list.
      temp->jdahep[1] = 0;
      temp->vhep[0] = 0; // Vertex-x;
      temp->vhep[1] = 0; // Vertex-y;
      temp->vhep[2] = 0; // Vertex-z;
      temp->vhep[3] = 0; // Production time;
      temp->isthep = 1;  // Final state particle.

      int n_evt_this_file = 0;
      while(n_evt_this_file < nevent && getline(in_file, line)) {
         ++lines_read;
         ++n_evt_this_file;
         ss.str(line);
         evt.nevhep = lines_read;
         ss >> xtmp; // Event Weight, always = 1.
         ss >> xtmp; // Momentum of incoming electron.
         ss >> xtmp; // Momentun of nucleus ~ 0.

         temp->idhep = 11;  // PID - electron = 11
         ss >> px;
         ss >> py;
         ss >> pz;

         rotate_beam(beam_rot, px, pz);
         //
         temp->phep[0] = px; // Momentum-x;
         temp->phep[1] = py; // Momentum-y;
         temp->phep[2] = pz; // Momentum-z;
         temp->phep[3] = sqrt(px * px + py * py + pz * pz + electron_mass * electron_mass); // Energy;
         temp->phep[4] = electron_mass; // Mass;
         evt.particles.push_back(*temp);

         temp->idhep = 13;  // PID - mu- = 13
         ss >> px;
         ss >> py;
         ss >> pz;
         rotate_beam(beam_rot, px, pz);

         temp->phep[0] = px; // Momentum-x;
         temp->phep[1] = py; // Momentum-y;
         temp->phep[2] = pz; // Momentum-z;
         temp->phep[3] = sqrt(px * px + py * py + pz * pz + muon_mass * muon_mass); // Energy;
         temp->phep[4] = muon_mass; // Mass;
         evt.particles.push_back(*temp);

         temp->idhep = -13;  // PID - mu+ = -13
         ss >> px;
         ss >> py;
         ss >> pz;
         rotate_beam(beam_rot, px, pz);

         temp->phep[0] = px; // Momentum-x;
         temp->phep[1] = py; // Momentum-y;
         temp->phep[2] = pz; // Momentum-z;
         temp->phep[3] = sqrt(px * px + py * py + pz * pz + muon_mass * muon_mass); // Energy;
         temp->phep[4] = muon_mass; // Mass;
         evt.particles.push_back(*temp);

         write_stdhep(&evt);
         write_file(ostream);
      }
      close_write(ostream);
   }

   if(debug ) cout << "Read " << lines_read << " lines from file.\n";
   if(lines_read != num_event_in_file){
      cout << "WARNING!! -- Different number of lines read than expected " << num_event_in_file << " != " << lines_read << " \n";
   }

   return 0;
}
