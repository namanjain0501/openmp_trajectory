#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h> 
#include <omp.h> 

#define L  100.0 
#define W  200.0  
#define H  400.0  

#define TOTAL_BODIES 1000  
#define M  1.0 
#define R  0.5 
#define TIME_STEP  0.01 
#define TOTAL_STEPS 80000
// FINALLY TO BE CHANGED TO 720000

#define THREADS 8

typedef struct position{
     double x ; 
     double y ; 
     double z ; 
}Position ; 

typedef struct velocity{
    long double x ; 
    long double y ; 
    long double z ; 
}Velocity ; 

typedef struct Force{
    long double x ; 
    long double y ; 
    long double z ; 
}Force ; 

bool collide_wall(double box_dimension, double ball_position) {
    if((ball_position<=R) || ((box_dimension-ball_position)<=R))
        return 1;
    return 0;
}

void parse_coords( Position * pos , FILE * fp) {
    fscanf(fp,"%lf",&(pos->x)); 
    fscanf(fp,"%lf",&(pos->y));
    fscanf(fp,"%lf",&(pos->z));
}
void printPosition(Position * p , int i , FILE * fo) {
    fprintf(fo , "Position of object %d : %f %f %f\n" , i , p->x , p->y , p-> z ) ; 
}
double distance_sq(Position *p , Position *q ) {
    return (p->x - q->x) *(p->x - q->x) + (p->y - q->y) * (p->y - q->y) + (p->z - q->z) * (p->z - q->z) ;  
}
void calcForce(Position * p , Position * arr , Force * f , int size ) {
    f->x = f->y = f->z = 0 ; 
    for(int i = 0 ; i < size ; i++ ) {
        double d2 = distance_sq(p,&arr[i]) ;
        if(d2 > 0 ) {
            double total_force = (M*M) / d2 ;
            double d = sqrt(d2) ;   
            f->x += total_force * ((arr[i].x - p->x)/ d ) ; 
            f->y += total_force * ((arr[i].y - p->y)/ d ) ;
            f->z += total_force * ((arr[i].z - p->z )/ d ) ;
        }
    }
}
void  calcHalfVelocity(Velocity * v , Force * f , Velocity *ans ) {
    ans->x = v->x + ((f->x*TIME_STEP)/ (2*M)) ; 
    ans->y = v->y + ((f->y*TIME_STEP)/ (2*M)) ; 
    ans->z = v->z + ((f->z*TIME_STEP)/ (2*M)) ; 
}
void updatePos (Position * p , Velocity * hv) {
    p->x = p->x  + ((hv->x )* (TIME_STEP)) ; 
    p->y = p->y  + ((hv->y )* (TIME_STEP)) ; 
    p->z = p->z  + ((hv->z )* (TIME_STEP)) ; 
}
void  updateVelocity(Velocity * v , Force * f , Velocity *ans ) {
    ans->x = v->x + ((f->x*TIME_STEP)/ (2*M)) ; 
    ans->y = v->y + ((f->y*TIME_STEP)/ (2*M)) ; 
    ans->z = v->z + ((f->z*TIME_STEP)/ (2*M)) ; 
}



int normalize(Position * p) {
    double d = ((p->x)*(p->x)) + ((p->y)* (p->y)) + ((p->z)*(p->z)) ; 
    double dd =sqrt(d) ; 
    if(dd>0) {
        p->x /= dd ; 
        p->y /= dd ;
        p->z /= dd ; 
        return 1 ; 
    }
    else {
        return 0 ; 
    }
}


// NOT USED 
double velAfterCollision(double v1 , double v2 , int m ) {
    if(abs(v1 > v2)) return v1 ;
    // EQUATION : 
    // v1 + m*v2 = X + m*V2 
    // 
    // (pow(v1,2) + m * pow(v2,2 ))  = pow(x,2) + (1/m) * (pow(x,2) - 2*(v1 + m*v2)*x + pow((v1 + m*v2),2))
    double a = 1 + (1.0 / (double) m );  
    double b = -2*((v1 + m*v2) / (double) m) ; 
    double c =  pow((v1 + m*v2),2) * (1.0/(double)m) - pow(v1,2) - (m * pow(v2,2)); 
    double root1 = (-b + sqrt(b*b-(4.0*a*c)) ) / (2.0*a);
    double root2 = (-b - sqrt(b*b-4.*a*c) ) / (2.*a);
    // printf("a = %lf b = %lf c = %lf \n" , a ,b, c ) ; 
    // printf("v1 = %lf v2 = %lf m = %lf Final V1 = %lf FINAL V2 = %lf\n" , v1, v2 , (double)m , root1 , root2 ) ; 
    if(b*b-(4.0*a*c) < 0 ) {
        printf("D < 0 \n") ; 
        
    }
    if(abs(root1 - v1) > 1e-6) {
        // printf("Collision happening taken root1\n") ;
        return root1 ; 
       
    }
    else {
        
        // printf("Collision happening taken root2 \n") ;
        return root2 ; 
    }
    return root1 ; 
    // return (root1 > 0) ? root1 : ((root2 > 0) ? (root2 : -1)) ; 
}
double dot(Velocity * v , Position * p) {
    return v->x*p->x + v->y*p->y + v->z * p->z  ; 
}
void change(Velocity * v , double dotV1 , double dotV2 , Position * p) {
    v->x = v->x - dotV1*p->x + dotV2*p->x ; 
    v->y = v->y - dotV1*p->y + dotV2*p->y ; 
    v->z = v->z - dotV1*p->z + dotV2*p->z ; 
    return ; 
}
void collision(Position * p , Position * all_pos  , Velocity * nv ,Velocity * all_velo , int size)  {
    int * collide = (int *)malloc(1 * sizeof(int)); 
    int count = 0 ; 
    for(int i = 0 ; i < size ; i++ ) {
        double d2 = distance_sq(p,&all_pos[i]) ;
        if(d2 < 2*R && d2 > 1e-10) {
            count++ ; 
            realloc(collide, count * sizeof(int));
            collide[count-1] = i ; 
        }
    }
    if(count > 0 ) {
        for(int i = 0 ; i < count ; i++ ) {
            Position collision_vector ; 
            collision_vector.x = (all_pos[collide[i]].x - p->x) ; 
            collision_vector.y = (all_pos[collide[i]].y - p->y) ; 
            collision_vector.z = (all_pos[collide[i]].z - p->z) ;
            normalize(&collision_vector) ; 
            double dotV1 = dot(nv, &collision_vector) ; 
            double dotV2 = dot(&all_velo[collide[i]], &collision_vector) ; 
            change(nv , dotV1 , dotV2 , &collision_vector) ; 
        }
    }
} 
int main() {
    FILE * fp;
    FILE * fo;
    fp = fopen("Trajectory.txt", "r");
    fo = fopen("output.bin", "wb");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    size_t len = 0;
    char line[75] = {0};
    Position pos[TOTAL_BODIES] ; 
    Velocity velocity[TOTAL_BODIES] ; 
    Velocity store_velocity[TOTAL_BODIES] ; 
    Force force[TOTAL_BODIES] ; 
    int i,j;
    Velocity half_velocity ;

    #pragma omp parallel num_threads(THREADS)
    {
        #pragma omp for private(i)
        for(i = 0 ;i < TOTAL_BODIES ; i++ ) {
            velocity[i].x = velocity[i].y = velocity[i].z =  0 ; 
        }

        #pragma omp master
        {
            // READING THE TOP NOT REQUIRED PART OF FILE 
            for(int i = 0 ; i < 8 ; i++ ) {
                fgets(line, 75 , fp) ; 
            }
            // reading initial coordinates serially
            for(i = 0 ; i < TOTAL_BODIES ; i++ ) {
                parse_coords( &pos[i] , fp) ;  
            }
        }

        #pragma omp barrier

        #pragma omp for
        for(i = 0 ; i < TOTAL_BODIES ; i++ ) {
            // calculating gravitional force (f at time step 0) on body i from all bodies
            calcForce(&pos[i] , pos , & force[i] , TOTAL_BODIES ) ;
        }
    }

    for(int st = 0 ; st < TOTAL_STEPS ; st++ )
    {
        #pragma omp parallel num_threads(THREADS)
        {
            #pragma omp for private(i)
            for(i = 0 ; i < TOTAL_BODIES ; i++ )
            {
                // v((st+1)/2)
                calcHalfVelocity(&velocity[i] , &force[i] , &velocity[i]) ; 
                // r(st+1)
                updatePos(&pos[i] , &velocity[i]) ; 
            }

            #pragma omp for private(i)
            for(i = 0 ; i < TOTAL_BODIES ; i++ )
            {
                // f(st+1)
                calcForce(&pos[i] , pos , &force[i] , TOTAL_BODIES ) ;
                // v(st+1)
                updateVelocity(&velocity[i] , &force[i] , &velocity[i]) ; 
            }

            //collision with wall
            #pragma omp for private(i)
            for(i = 0 ; i < TOTAL_BODIES ; i++ ) {
                if( collide_wall(W,pos[i].x)) {
                    //printf("collision of %d with wall",i);
                    velocity[i].x=-velocity[i].x;
                }   
                if( collide_wall(L,pos[i].y)) {
                    //printf("collision of %d with wall",i);
                    velocity[i].y=-velocity[i].y;
                }
                if( collide_wall(H,pos[i].z)) {
                    //printf("collision of %d with wall",i);
                    velocity[i].z=-velocity[i].z;
                }
            }

            #pragma omp for private(i) 
            for(i = 0 ; i < TOTAL_BODIES ; i++ ) {
                store_velocity[i] = velocity[i] ; 
                collision(&pos[i] , pos  , &store_velocity[i] , velocity , TOTAL_BODIES) ; 
            }

            #pragma omp for private(i)
            for(i = 0 ; i < TOTAL_BODIES ; i++ ) {
                velocity[i] = store_velocity[i] ; 
            }

            #pragma omp master
            {
                if(st%100 == 0)
                {
                    for(i = 0 ; i < TOTAL_BODIES ; i++ ) {
                        fwrite(&pos[i].x, sizeof(double), 1, fo);
                        fwrite(&pos[i].y, sizeof(double), 1, fo);
                        fwrite(&pos[i].z, sizeof(double), 1, fo);
                    }
                }
            }
        }
    }


    fclose(fp);
    fclose(fo);
    exit(EXIT_SUCCESS);
}