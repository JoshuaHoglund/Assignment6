#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "file_operations.h"
#include "quad.h"
#include <sys/time.h>
#include <time.h>
#include <omp.h>
#include <unistd.h>

int main(int argc, const char* argv[]) { 
 // read in N filename nsteps delta_t graphics
 // N number of stars/particles to simulate 
 // filename is the filename of the file to read the initial configuration from 
 // nsteps is the number of timesteps
 // theta_max is the threshold value to be used in the Barnes-Hut algorithm
 // graphics is 1 or 0 meaning graphics on/off

// check if the parameters in the command line are correct, otherwise error message with instructions.	
  	if(argc != 8) {
      printf("Please give in: N filename nsteps delta_t theta_max graphics n_threads.\n");
      return -1;
    }
 
// read in N, check if N is 1 or larger otherwise error message.
  	int N = atoi(argv[1]);
    printf("N = %d\n", N);
    if(N < 1) {
      printf("Error: (N < 1).\n");
      return -1;
    }
 // read in theta_max
   double theta_max = atof(argv[5]);
      printf("theta_max = %f\n", theta_max);
   
 // read in filename and open filename. 	
   FILE *ptr_file;
 

  	ptr_file = fopen(argv[2], "r");
 
  	if(!ptr_file){
  		printf("File does not exist." );
  		return 1;}
 	  // store data of opened file 
   //
 		
  	fclose(ptr_file);
   
 	int nsteps = atoi(argv[3]);
 	double delta_t = atof(argv[4]);
 	
   int graphics = atoi(argv[6]);
	int nThreads = atoi(argv[7]);
  
 double *values =(double*)malloc(5*N*sizeof(double));
 read_doubles_from_file(atoi(argv[1])*5, values, argv[2]);
 
 //Allocate memory for particles  
 particle_t *particles = (particle_t*)malloc(N*sizeof(particle_t));
 
 //Set the particle data  
 int i = 0;
 int j = 0;  
 while(j<N){
    particles[j].x_pos = values[i];
    particles[j].y_pos = values[i+1];
    particles[j].mass = values[i+2];
    particles[j].vel_x = values[i+3];
    particles[j].vel_y = values[i+4];
    j++;
    i=j*5;
 }
   
 // assignment 4//////////////////////////////////////////////////////////////////////////////////////////////////////
   //struct timespec t0, t1;// t2,t3,t4,t5;

 const double epsilon=0.001;
 const double G=100.0/N;
 //long elapsed_time_nsec=0;
// long elapsed_time_mass=0;
 //long elapsed_time_insert=0;
	
   
   if(graphics==0) {
      for(int t=0;t<nsteps;t++) {
        p_qtree * head=(p_qtree *) calloc(1,sizeof(p_qtree));
    (*head).width = 1;
    (*head).centerX = 0.5;
    (*head).centerY = 0.5;
    (*head).mass = 0.0;
    (*head).massCenterX = 0.5;
    (*head).massCenterY = 0.5;
        
    // gettimeofday(&t4,0); 
     for(int k=0;k<N;k++)
     {
       insert(&head, particles[k]);
     }
	//gettimeofday(&t5,0);
	//elapsed_time_insert += (t5.tv_sec-t4.tv_sec)*1e6 + t5.tv_sec-t4.tv_sec;
	      
       // gettimeofday(&t2,0);   
	     
	    
     massification(&head);

    //gettimeofday(&t3,0);
   //elapsed_time_mass += (t3.tv_sec-t2.tv_sec)*1e6 + t3.tv_sec-t2.tv_sec;
	// gettimeofday(&t1,0);        
 
	force_t * force = (force_t*)calloc(1,nThreads*sizeof(force_t));      
	      
#pragma omp parallel for num_threads(nThreads) schedule(guided)
	for(int i=0;i<N;i++){
	      
	      getForce(&head, particles[i],theta_max,G,epsilon, &force[omp_get_thread_num()]);
	      double m_i = 1/particles[i].mass;
	      particles[i].vel_x += delta_t*force[omp_get_thread_num()].x*m_i;
	      particles[i].vel_y += delta_t*force[omp_get_thread_num()].y*m_i;
	      particles[i].x_pos += delta_t*particles[i].vel_x;
	      particles[i].y_pos += delta_t*particles[i].vel_y;
		if(isnan(particles[i].y_pos)) {
			printf("22222222222222222222222222222222222222222222222 \n\n\n");
			       printf("m_i: %lf\n",m_i);
			printf("vel_y: %lf \n",particles[i].vel_y);
			printf("force[i].y: %lf \n",force[i].y);
			sleep(5);
		}
		printf("force_x: %lf \n", (*force).x);
		printf("force_y: %lf \n", (*force).y);
	      force[omp_get_thread_num()].x = 0;
	      force[omp_get_thread_num()].y = 0;
	   }
	      
	      
   
   delete(&head);
 //elapsed_time_nsec += (t1.tv_sec-t0.tv_sec)*1e9 + t1.tv_nsec-t0.tv_nsec;

      }
	  //printf("%ld force microsec\n", elapsed_time_nsec);
	  // printf("%ld mass microsec: ", elapsed_time_mass);
	  // printf("%ld insert microsec: ", elapsed_time_insert);
   }
	
	
  

 i = 0;
 j = 0;  
 while(j<N){
    values[i] = particles[j].x_pos;
    values[i+1] = particles[j].y_pos;
    values[i+2] = particles[j].mass;
    values[i+3] = particles[j].vel_x;
    values[i+4] = particles[j].vel_y;
    j++;
    i=j*5;
 }
   write_doubles_to_file(5*N,values,"result.gal");
   
  return 0;
 
}
