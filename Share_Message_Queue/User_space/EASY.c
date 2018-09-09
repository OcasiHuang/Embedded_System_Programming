#include <stdio.h>

#define SIZE 10
//int front = -1, rear =-1;
//item[10] = front, item[11] = rear
 double items2[SIZE+2];
 double items1[SIZE+2];


int isFull(double* it)
{
    if( (it[10] == it[11] + 1) || (it[10] == 0 && it[11] == SIZE-1)) return 1;
    return 0;
}

int isEmpty(double* it)
{
    if(it[10] == -1) return 1;
    return 0;
}

/*void sq_create1()
{
	double items1[SIZE];
	int it[10] = -1, it[11] =-1;
}
void sq_create2()
{
	 double items2[SIZE];
     int it[10] = -1, it[11] =-1;
}*/

void sq_write(double* it, double element)
{
    if(isFull(it)) printf("\n Queue is full!! \n");
    else
    {
        if(it[10] == -1) it[10] = 0;
        it[11] = (int)(it[11] + 1) % SIZE;
        it[(int)it[11]] = element;
        printf("\n Inserted -> %lf, real -> %lf", element, it[(int)it[11]]);
    }
}


double sq_read(double* it)
{
    double element;
    printf("\nit[10] = %lf, it[11] = %lf\n", it[10], it[11]);
    if(isEmpty(it)) {
        printf("\n Queue is empty !! \n");
        return(-1);
    } else {
    	printf("it2[0] = %lf\n", it[0]);
    	printf("it2[5] = %lf\n", it[5]);
        element = it[(int)it[10]];
        if (it[10] == it[11]){
            it[10] = -1;
            it[11] = -1;
        } 
        else {
            it[10] = (int)(it[10] + 1) % SIZE;
            
        }
        printf("\n Deleted element -> %lf \n", element);
    	printf("\nit[10] = %lf, it[11] = %lf\n", it[10], it[11]);
        return(element);
    }
}




void sq_delete()
{}


int main()
{
    // Fails because it[10] = -1
//    sq_create1();
//sq_create2()	;	
 	items1[10] = -1.0;
 	items2[10] = -1.0;
 	items1[11] = -1.0;
 	items2[11] = -1.0;
    
    sq_write(&items1[0],1);
    sq_write(&items1[0],2);
    sq_write(&items2[0],3);
    sq_write(&items2[0],3);
    sq_write(&items2[0],3);
    sq_write(&items2[0],3);
    sq_write(&items2[0],3);
    sq_read(&items2[0]);
	printf("items2 = %lf:\n",items2[1]);
	printf("items2 = %lf:\n",items2[2]);
	printf("items2 = %lf:\n",items2[3]);
	printf("items2 = %lf:\n",items2[4]);
	                
	printf("items1 = %lf:\n",items1[1]);
	printf("items1 = %lf:\n",items1[2]);
	printf("items1 = %lf:\n",items1[3]);
 	printf("items1 = %lf:\n",items1[4]);


    
    // Fails to enqueue because it[10] == 0 && it[11] == SIZE - 1
   /* enQueue(6);
    
    display();
    deQueue();
    
    display();
    
    enQueue(7);
   */
    
    return 0;
}
