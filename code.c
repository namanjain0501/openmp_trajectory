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
#define TOTAL_STEPS 100
// FINALLY TO BE CHANGED TO 720000

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

bool collide_wall(double box_dimension,double ball_position) {
    if(ball_position<=R || box_dimension-ball_position<=R)
        return 1;
    return 0;
}

void parse_coords( Position * pos , FILE * fp) {
    fscanf(fp,"%lf",&(pos->y)); 
    fscanf(fp,"%lf",&(pos->x));
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

double velAfterCollision(double v1 , double v2 , int m ) {
    // EQUATION : 
    // (pow(v1,2) + m * pow(v2,2 ))/2.0  = pow(x,2)/2 + (0.5/m) * (pow(x,2) - 2*(v1 + m*v2)*x + pow((v1 + m*v2),2))
    double a = 0.5 + (0.5 / (double) m );  
    double b = -((v1 + m*v2) / (double) m) ; 
    double c =  pow((v1 + m*v2),2) * (0.5/(double)m) - pow(v1,2) + (m * pow(v2,2))*0.5; 
    double root1 = (-b + sqrt(b*b-4.*a*c) ) / (2.*a);
    // double root2 = (-b - sqrt(b*b-4.*a*c) ) / (2.*a);
    if(root1 < 0) {
        printf("ERROR\n") ; 
        exit(0) ; 
    }
    else {
        printf("Collision happening\n") ;
    }
    return root1 ; 
    // return (root1 > 0) ? root1 : ((root2 > 0) ? (root2 : -1)) ; 
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
        Position mean ; 
        mean.x = mean.y = mean.z = 0 ; 
        Velocity mean_velocity ; 
        mean_velocity.x =  mean_velocity.y =  mean_velocity.z =  0 ; 
        for(int i = 0 ; i < count ; i++ ) {
            mean.x+=all_pos[collide[i]].x ; 
            mean.y+=all_pos[collide[i]].y ;
            mean.z+=all_pos[collide[i]].z ;
        }
        mean.x /= (double)count ; 
        mean.y /= (double)count ; 
        mean.z /= (double)count ; 

        Position collision_vector ; 
        collision_vector.x = (mean.x - p->x) ; 
        collision_vector.y = (mean.y - p->y) ; 
        collision_vector.z = (mean.z - p->z) ;
        normalize(&collision_vector) ; 
       
        Velocity new_velocity ; 
        new_velocity.x = new_velocity.y = new_velocity.z = 0 ; 
        for(int i = 0 ; i < count ; i++ ) {
            new_velocity.x += all_velo[collide[i]].x ; 
            new_velocity.y += all_velo[collide[i]].y ; 
            new_velocity.z += all_velo[collide[i]].z ; 
        } 
        new_velocity.x/=(double)count ; 
        new_velocity.y/=(double)count ; 
        new_velocity.z/=(double)count ; 
        
        nv->x =  velAfterCollision(collision_vector.x * nv->x , collision_vector.x*new_velocity.x , count) ;  
        nv->y =  velAfterCollision(collision_vector.y * nv->y , collision_vector.y*new_velocity.y , count) ;  
        nv->z =  velAfterCollision(collision_vector.z * nv->z , collision_vector.z*new_velocity.z , count) ;  
    }
} 
int main() {
    FILE * fp;
    FILE * fo;
    size_t len = 0;
    char line[75] = {0};
    Position pos[TOTAL_BODIES] ; 
    Velocity velocity[TOTAL_BODIES] ; 
    Velocity store_velocity[TOTAL_BODIES] ; 
    Force force[TOTAL_BODIES] ; 
    for(int i = 0 ;i < TOTAL_BODIES ; i++ ) {
        velocity[i].x = velocity[i].y = velocity[i].z =  0 ; 
    }
    fp = fopen("Trajectory.txt", "r");
    fo = fopen("output.txt", "w");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    // READING THE TOP NOT REQUIRED PART OF FILE 
    for(int i = 0 ; i < 8 ; i++ ) {
        fgets(line, 75 , fp) ; 
    }
    for(int i = 0 ; i < TOTAL_BODIES ; i++ ) {
        parse_coords( &pos[i] , fp) ;  
    }
    Velocity half_velocity ; 
    for(int  i = 0 ; i < TOTAL_BODIES ; i++ ) {
        calcForce(&pos[i] , pos , & force[i] , TOTAL_BODIES ) ;
    }
    for(int st = 0 ; st < TOTAL_STEPS ; st++ ) {
        for(int i = 0 ; i < TOTAL_BODIES ; i++ ) {
            calcHalfVelocity(&velocity[i] , &force[i] , &half_velocity) ; 
            updatePos(&pos[i] , &half_velocity) ; 
            calcForce(&pos[i] , pos , & force[i] , TOTAL_BODIES ) ;
            updateVelocity(&half_velocity , &force[i] , &velocity[i]) ; 
        }
        //collision with wall
        for(int i = 0 ; i < TOTAL_BODIES ; i++ ) {
            if( collide_wall(L,pos[i].x)) {
                //printf("collision of %d with wall",i);
                velocity[i].x=-velocity[i].x;
            }   
            if( collide_wall(W,pos[i].y)) {
                //printf("collision of %d with wall",i);
                velocity[i].y=-velocity[i].y;
            }
            if( collide_wall(H,pos[i].z)) {
                //printf("collision of %d with wall",i);
                velocity[i].z=-velocity[i].z;
            }
        }

        
        for(int i = 0 ; i < TOTAL_BODIES ; i++ ) {
            store_velocity[i] = velocity[i] ; 
            collision(&pos[i] , pos  , &store_velocity[i] , velocity , TOTAL_BODIES) ; 
        }
        for(int i = 0 ; i < TOTAL_BODIES ; i++ ) {
            velocity[i] = store_velocity[i] ; 
        }
    }
    for(int i = 0 ; i < TOTAL_BODIES ; i++ ) {
        printPosition( &pos[i] , i+1 , fo) ;
    }


    fclose(fp);
    
    exit(EXIT_SUCCESS);
}