//
//  lcio_info.cpp
//
//  Created by Maurik Holtrop on 7/7/18.
//
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <iostream>

#include "lcio.h"

//#include "IO/LCReader.h"
#include "EVENT/LCCollection.h"
#include "IMPL/LCTOOLS.h"
#include "EVENT/CalorimeterHit.h"
#include "EVENT/SimCalorimeterHit.h"
#include "EVENT/SimTrackerHit.h"

using namespace std;
using namespace lcio;

static int G_Debug;
static int G_verbose;

int main (int argc, char **argv)
{
  int c;
  G_verbose=1;
  int L_nomap=0;
  unsigned int event_number=0; // Wind to EVENT = event_number.
  unsigned int next_number=0;  // Skip n next events.
  unsigned int num_evt=1;      // Number of consequtive events to show.
  string count_name;
  
  while (1)
  {
    static struct option long_options[] =
    {
      /* These options set a flag. */
      {"verbose", no_argument,       &G_verbose, 2},
      {"brief",   no_argument,       &G_verbose, 0},
      {"nomap",   no_argument,       &L_nomap,   1},
      /* These options don’t set a flag.
       We distinguish them by their indices. */
      {"debug",  required_argument, 0, 'd'},
      {"event",  required_argument, 0, 'n'},
      {"next",   required_argument, 0, 'x'},
      {"num",    required_argument, 0, 'N'},
      {"count",  required_argument, 0, 'c'},
      {"help",   no_argument,       0, 'h'},
      {0, 0, 0, 0}
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;
    
    c = getopt_long (argc, argv, "bc:d:hn:N:vx:X",
                     long_options, &option_index);
    
    /* Detect the end of the options. */
    if (c == -1)
      break;
    
    switch (c)
    {
      case 0:
        // cout << "Case 0:" << endl;
        /* If this option set a flag, do nothing else now. */
//        if (long_options[option_index].flag != 0)
//          break;
//        printf ("option %s", long_options[option_index].name);
//        if (optarg)
//          printf (" with arg %s", optarg);
//        printf ("\n");
        break;
      case 'd':
        G_Debug=atoi(optarg);
        break;

      case 'n':
        event_number=atoi(optarg);
        break;

      case 'N':
        num_evt=atoi(optarg);
        break;

      case 'x':
        next_number=atoi(optarg);
        break;

      case 'c':
        count_name = optarg;
        break;
        
      case 'v':
        G_verbose = 2;
        
      case 'X':
        L_nomap = 1;
        break;
      case '?':
      case 'h':
      default:
        cout << "LCIO info: " << argv[0] << endl;
        cout << "This programs helps you investigate the contents of an LCIO file.\n";
        cout << "To fully dump the content, use dumpevent.\n\n";
        cout << "Options: \n";
        cout << "  -v   --verbose      : Add to the verbosity of the output. \n";
        cout << "  -d # --debug #      : Set the debug level to # \n";
        cout << "  -x # --next #       : Start at next #, skipping # next events.\n";
        cout << "  -n # --event #      : Start at event # , # is actual EVENT numner. \n";
        cout << "  -N # --num #        : Show N next events.\n";
        cout << "  -c s --count 'name' : Count the number of items in collection with name \n";
        cout << "  -X   --nomap        : Do not report number of events and runs. \n ";
        cout << "                      : Use for old files that do not contain a map. \n";
        exit(0);
    }
  }
  
  /* Remaining command line arguments (not options) needed. */
  if (optind == argc)
  {
    printf ("Please supply a file to analyze.\n");
    exit(1);
  }
  
  char *file= argv[optind++];
  if(G_Debug>0) printf ("Opening file %s \n",file);

  // If only one of -n or -x is given, the other should be infinite.
  // If neither is given, they should both remain zero.
  if(event_number==0 && next_number>0) event_number = -1;
  if(next_number==0 && event_number>0) next_number = -1;
  
  // Open the input file in an LCReader.
  LCReader *lc_reader=LCFactory::getInstance()->createLCReader(); // LCReader::directAccess
  lc_reader->open(file);

  if( !L_nomap){
    int nevt = lc_reader->getNumberOfEvents();
    EVENT::IntVec runs;
    lc_reader->getRuns(runs);
    printf("Number of runs: %lu,  first run: %d  number of events  %d\n",runs.size(),runs[0],nevt );
  }
  LCEvent  *evt=nullptr;
  // Initial loop over the events to find the event we want.
  // The loop stops either when EVENT number is >= event_number, or count >= next_number
  int count=0;
  int name_count=0;
  while(evt==nullptr || (evt->getEventNumber()<event_number && count < next_number)){
    count++;
    evt=lc_reader->readNextEvent();

    if( count_name.size() ){
      auto coll = evt->getCollection(count_name);
      name_count +=coll->getNumberOfElements();
    }
  }
  if( count_name.size() ){
    printf("Until event found %d of collection name %s \n\n",name_count,count_name.c_str());
  }
  
  for(int i=0;i<num_evt;++i){
    printf("\nNext count = %8d\n",count);
    if(evt){
      LCTOOLS::dumpEvent(evt);
      if(G_verbose>1){
        LCTOOLS::dumpEventDetailed(evt);
      }
    }
    else cout << "Problems encountered reading events " << event_number << endl;
    count++;
    evt=lc_reader->readNextEvent();
  }
  
  if( count_name.size() ){
    while( (evt=lc_reader->readNextEvent())!=nullptr ){
        auto coll = evt->getCollection(count_name);
        name_count +=coll->getNumberOfElements();
    }
    printf("Full scan   found %d of collection name %s \n",name_count,count_name.c_str());
  }

  
  exit (0);
}
