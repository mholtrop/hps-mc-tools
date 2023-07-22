//
// Created by Maurik Holtrop on 7/22/23.
//
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

void rotate_entry(stdhep_entry *entry, double theta_x, double theta_y)
{
   double px = entry->phep[0];
   double py = entry->phep[1];
   double pz = entry->phep[2];
   double vx = entry->vhep[0];
   double vy = entry->vhep[1];
   double vz = entry->vhep[2];

   // rotate about x-axis (rotation in y-z)
   entry->phep[1] = py*cos(theta_x) - pz*sin(theta_x);
   entry->phep[2] = py*sin(theta_x) + pz*cos(theta_x);

   py = entry->phep[1];
   pz = entry->phep[2];

   // rotate about y-axis (rotation in z-x)
   entry->phep[0] = px*cos(theta_y) + pz*sin(theta_y);
   entry->phep[2] = pz*cos(theta_y) - px*sin(theta_y);

   // rotate about x-axis (rotation in y-z)
   entry->vhep[1] = vy*cos(theta_x) - vz*sin(theta_x);
   entry->vhep[2] = vy*sin(theta_x) + vz*cos(theta_x);

   vy = entry->vhep[1];
   vz = entry->vhep[2];

   // rotate about y-axis (rotation in z-x)
   entry->vhep[0] = vx*cos(theta_y) + vz*sin(theta_y);
   entry->vhep[2] = vz*cos(theta_y) - vx*sin(theta_y);
}


int main(int argc, char **argv) {

   int istream = 0;
   int ostream = 1;
   stdhep_event evt_in;
   stdhep_event evt_out;
   struct stdhep_entry *temp = new struct stdhep_entry;

   setlocale(LC_NUMERIC, "");
   cxxopts::Options options(argv[0], ". This program will translate the mu+mu- events from CalcHEP from text"
                                     " to stdhep format.");
   options
         .positional_help(" infile - input CalcHEP file.")
         .show_positional_help();

   bool apple = false;

   options
         .add_options()
               ("d,debug", "Set debug level")
               ("o,output", "Output file stub.", cxxopts::value<std::string>()->default_value(""))
               ("i,input", "Input stdhep file, -i is optional.",
                cxxopts::value<std::string>()->default_value(""))
               ("n,nevent", "Number of events per output file.", cxxopts::value<long>()->default_value("-1"))
               ("r,rotate", "Rotate all vectors around the y-axis in the x-y plane by given angle in mrad. [30.5]",
                cxxopts::value<double>()->default_value("30.5"))
               ("h,help", "Print help");

   options.parse_positional({"input"});
   auto args = options.parse(argc, argv);

   auto inputfile = args["input"].as<std::string>();
   auto outputstub = args["output"].as<std::string>();
   double theta_x = 0;
   double theta_y = args["rotate"].as<double>()/1000.; //!< convert to radian
   auto max_event_per_file = args["nevent"].as<long>();
   if(max_event_per_file < 0){
      max_event_per_file = 1L<<60;
   }
   if (args.count("help") || inputfile == "") {
      std::cout << options.help() << std::endl;
      exit(0);
   }

   if( outputstub == ""){
      int i = inputfile.rfind(".");
      outputstub = inputfile.substr(0, i);
   }

   int debug = args["debug"].count();

   int n_events = open_read((char *)inputfile.c_str(), istream);
   long nevt=0;
   int n_files = 0;
   long nevt_this_file = 1L<<62; // Make sure to open the output file on first event.

   if( debug ){
      cout << "Maximum events per file " << max_event_per_file << endl;
      printf("Expecting %d output files with names %s_x.stdhep \n",
             n_events/max_event_per_file+1, outputstub.c_str());
   }

   bool still_reading = true;
   while (still_reading) {
      still_reading = read_next(istream);
      if(!still_reading){
         if(debug) cout << "End of file.\n";
         break;
      }
      if ((++nevt) % 10000 == 0) printf("evt: %9ld \n", nevt);

      if(nevt_this_file >= max_event_per_file){

         if(n_files>0) close_write(ostream);
         string out_filename = outputstub+"_"+to_string(n_files)+".stdhep";
         int n_out_tmp=max_event_per_file;
         if( nevt + n_out_tmp > n_events ){
            n_out_tmp = n_events - nevt;
         }
         cout << "Opening " << out_filename << " with " << n_out_tmp << " events.\n";
         open_write((char *)out_filename.c_str(),ostream,n_out_tmp);
         ++n_files;
         nevt_this_file = 0;
      }

      read_stdhep(&evt_in);

      for (int j=0; j < evt_in.particles.size(); j++){
         rotate_entry( &(evt_in.particles[j]), theta_x, theta_y);
      }

      write_stdhep(&evt_in);
      write_file(ostream);
      ++nevt_this_file;
    }

   close_read(istream);
   close_write(ostream);
   return 0;
}