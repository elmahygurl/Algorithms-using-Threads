#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

struct t_arguments1
{
    int row, col;      // Row and column indices of the element in the output matrix
    int size;  //for multiplication
    int **matrix1;
    int **matrix2;
    int **output1;
};
struct t_arguments2
{
    int row;  //current row im at
    int tot_col2;
    int tot_rows2; //for multiplication
    int **matrix1;
    int **matrix2;
    int *output2;
};
void *multiplyy1(void *arguments)   //calculating element per thread
{
    struct t_arguments1 *args = (struct t_arguments1 *)arguments;
    int r= args ->row;
    int c=args->col;
    int s=args->size;
    int output_here=0;

    for (int temp=0; temp<s; temp++)
    {
        output_here+= args->matrix1[r][temp] * args->matrix2[temp][c];
    }
    args->output1[r][c]=output_here;
    // pthread_exit(NULL);

}
void *multiplyy2(void *arguments)
{
    struct t_arguments2 *args2= (struct t_arguments2 *) arguments;
    int r= args2->row;
    int columns = args2->tot_col2;
    int size= args2->tot_rows2 ;
    int outputt;


    for (int j=0 ; j< columns; j++ )
    {
        outputt=0;
        for (int temp=0; temp<size; temp++)
        {
            outputt+= args2->matrix1[r][temp]* args2->matrix2[temp][j];
        }

        // values for debugging
        //printf("\n r: %d, j: %d\t", r, j);
        //printf("output: %d",outputt);

        args2->output2[j]=outputt;

    }

    //printf("\nThread %d completeddddd.\n", r);
    //pthread_exit(NULL);

}

//helping functions
void readMatrix(FILE *file,int **matrix, int rows,int columns)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            fscanf(file, "%d", &matrix[i][j]);
        }
    }

}

//free allocated memory
void freeMatrix(int **matrix, int rows)
{
    for (int i = 0; i < rows; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

void freeEverything(int **matrix1, int **matrix2, int **output1,int **output2, int rows1, int rows2)
{
    freeMatrix(matrix1, rows1);
    freeMatrix(matrix2, rows2);
    freeMatrix(output1, rows1);
    freeMatrix(output2, rows1);
}
double now(void)
{

    struct timeval tv;
    gettimeofday(&tv, NULL);

    double retval = (double)tv.tv_sec * 1000.0;  // Convert seconds to milliseconds
    retval += (double)tv.tv_usec / 1000.0;       // Convert microseconds to milliseconds

    return retval;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");

    //FILE *file = fopen("input1-matrix.txt", "r");

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file.\n");
        return 1;
    }
    //printf("me here 1\n");

    int rows1,rows2,col1,col2;

    // first matrix
    fscanf(file, "%d %d", &rows1, &col1);


    int **matrix1 = malloc(rows1 * sizeof(int *));
    for (int i = 0; i < rows1; i++)
    {
        matrix1[i] = malloc(col1 * sizeof(int));
    }

    readMatrix(file, matrix1, rows1, col1);

    // second matrix
    fscanf(file, "%d %d", &rows2, &col2);



    int **matrix2 = malloc(rows2 * sizeof(int *));
    for (int i = 0; i < rows2; i++)
    {
        matrix2[i] = malloc(col2 * sizeof(int));
    }

    readMatrix(file, matrix2, rows2, col2);
    fclose(file);

    if (col1 != rows2)
    {
        printf("Sizes are not compatible to be multiplied \n");
        freeMatrix(matrix1, rows1);
    freeMatrix(matrix2, rows2);
    
       
        return(0);

    }

    //allocating for output
    int **output1 = malloc(rows1 * sizeof(int *));
    for (int i = 0; i < rows1; i++)
    {
        output1[i] = malloc(col2 * sizeof(int));
    }


    clock_t start_time1 = clock();   //start clock
    /// double start=now();

    printf("Procedure 1 - Element per thread\n");
    ///calculating element per thread  - Procedure 1
    pthread_t *threads = (pthread_t *)malloc(rows1 * col2 * sizeof(pthread_t));
    struct t_arguments1 *threadArgs = (struct t_arguments1 *)malloc(rows1 * col2 * sizeof(struct t_arguments1)); //will be same size as output matrix

    int threadIndex = 0;  //i+j
    for (int i = 0; i < rows1; i++)
    {
        for (int j = 0; j < col2; j++)
        {
            threadArgs[threadIndex].row = i;
            threadArgs[threadIndex].col = j;
            threadArgs[threadIndex].size=col1;   //can be either col1 or row2 - to know halef kam mara fel multiplyy1
            threadArgs[threadIndex].matrix1 = matrix1;
            threadArgs[threadIndex].matrix2 = matrix2;
            threadArgs[threadIndex].output1 = output1;

            if (pthread_create(&threads[threadIndex], NULL, multiplyy1, &threadArgs[threadIndex]) != 0)
            {
                printf("Error creating thread.\n");
                //freeEverything(matrix1, matrix2, output1, rows1, rows2);
                free(threads);
                free(threadArgs);
                printf("Uh-oh!\n");
                return -1;
            }
           
            threadIndex++;   //da hywsal to size of output matrix at the end 3x2=6 - row1xcol2
        }
    }

    clock_t end_time1 = clock();

 for (int i = 0; i < rows1; i++)
    {
        for (int j = 0; j < col2; j++)
        {
 if (pthread_join(threads[threadIndex], NULL) != 0)
            {
                printf("Error joining thread.\n");
                //freeEverything(matrix1, matrix2, output1, rows1, rows2);
                free(threads);
                free(threadArgs);
                return -1;
            }
        }
    }

    
    ///printf("elapsed time = %.6f\n", now() - start);
//printing output matrix 1
    for (int i = 0; i < rows1; i++)
    {
        for (int j = 0; j < col2; j++)
        {
            printf("%d ",output1[i][j]);

        }
        printf("\n");
    }

    double elapsed_time1 = ((double)(end_time1 - start_time1))/ CLOCKS_PER_SEC;  //clocks per sec to convert to seconds

    printf("Time    %f\n", elapsed_time1);



    ///The computation of each row of the output matrix happens in a thread. -Procedure 2
clock_t start_time2 = clock();   //start clock
    printf("\nProcedure 2 - Row per thread\n");

    int **output2 = malloc(rows1 * sizeof(int *));
    for (int i = 0; i < rows1; i++)
    {
        output2[i] = malloc(col2 * sizeof(int));
    }


    pthread_t *threads2 = (pthread_t *)malloc(rows1* sizeof(pthread_t));
    struct t_arguments2 *threadArgs2 = (struct t_arguments2 *)malloc(rows1 * sizeof(struct t_arguments2));

    for (int i=0; i<rows1; i++)
    {
        threadArgs2[i].row=i;
        threadArgs2[i].tot_col2=col2; //rep
        threadArgs2[i].tot_rows2=rows2;  //rep
        threadArgs2[i].matrix1 = matrix1;
        threadArgs2[i].matrix2 = matrix2;
        threadArgs2[i].output2 = output2[i];


        if (pthread_create(&threads2[i], NULL, multiplyy2, &threadArgs2[i]) != 0)
        {
            printf("OOPS..ERROR CREATING THREAD!!\n");
            //free(threads2);
            //free(threadArgs2);
            return -1;
        }
        /*else
        {
            printf("Thread %d created.\n", i);
        }*/

       

    }

 for (int i=0; i<rows1; i++)
    {
         if (pthread_join(threads2[i], NULL) != 0)  //waiting for threads to finish
        {
            printf("Error joining thread.\n");
            //freeEverything(matrix1, matrix2, output1, rows1, rows2);
            //free(threads2);
            //free(threadArgs2);
            return -1;
        }
    }
    
    
clock_t end_time2 = clock();   //start clock
    //printing output matrix 2
    for (int i = 0; i < rows1; i++)
    {
        for (int j = 0; j < col2; j++)
        {
            printf("%d ", output2[i][j]);
        }
        printf("\n");
    }
  double elapsed_time2 = ((double)(end_time2 - start_time2))/ CLOCKS_PER_SEC;  //clocks per sec to convert to seconds

    printf("Time    %f\n", elapsed_time2);

//free all allocated memory
    freeEverything(matrix1, matrix2, output1,output2, rows1, rows2);

}
