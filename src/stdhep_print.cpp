#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "stdhep_util.hh"

// takes input stdhep file, merges a fixed number of events, and writes to a new stdhep file
int main(int argc,char** argv)
{
	stdhep_event new_event;
	int istream = 0;

	if (argc<2 || argc>3)
	{
		printf("<input stdhep filename> [number of events to read]\n");
		return 1;
	}

	int n_events = 0;
	int n_read = 0;

	n_events = open_read(argv[1],istream);

	if (argc==3)
	{
		n_read = atoi(argv[2]);
	}

  printf("Total_z_momentum, e+e-_z_momentum\n");
	for (int i=0;n_read==0||i<n_read;i++)
	{
		if (!read_next(istream)) {
			close_read(istream);
			return(0);
		}

		read_stdhep(&new_event);

    if(new_event.particles.size() != 6){
        printf("read event %d: nevhep = %d, nhep = %d\n",i+1,new_event.nevhep,new_event.particles.size());
          if (new_event.has_hepev4)
          {
    //          printf("HEPEV4 information: idruplh = %d, eventweightlh = %E\n",new_event.idruplh,new_event.eventweightlh);//
          }
      
        for (int j=0;j<new_event.particles.size();j++) {
          print_entry(new_event.particles[j]);
        }
      
      double p_sum_z=0;
      double p_epem_sum_z = 0;
      double p_elec = 0;
      double p_pos = 0;
      
      for(int i=0;i<new_event.particles.size();++i){
        if(new_event.particles[i].isthep == 1){
          p_sum_z += new_event.particles[i].phep[2];
          if(new_event.particles[i].idhep == -11){
            if(new_event.particles[i].phep[2]> p_elec){
              p_elec =new_event.particles[i].phep[2];
            }
          }else if(new_event.particles[i].idhep == 11){
            p_pos =new_event.particles[i].phep[2];
          }
        }
      }
      printf("%10.6f, %10.6f \n",p_sum_z,p_elec+p_pos);
    }
    
    new_event.particles.clear();
	}
}

