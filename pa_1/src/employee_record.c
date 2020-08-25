// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Alg     -   Employe Record SOURCE
//   Date    -   8-24-20
//
// ########################################## 

#include "employee_record.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    printf("Hello, world!\r\n");

    struct Database database;

    char *input_files[1] = { "../resources/BaltimoreCityEmployeeSalariesFY2018.csv" };
    createDatabase(input_files, 1, &database);
    return 0;
}


void createDatabase(char* input_files[], int numFiles, struct Database *database) {
    for(int i=0; i<numFiles; ++i) {
        FILE *file = fopen(input_files[i], "r");

        if(file == NULL) {
            printf("File '%s' not found. Exiting. \r\n", input_files[i]);
            exit(-1);
        }

        char *name, *title, *dpt_id, *dpt_descr, *date;
        float gross_salary, net_salary;

        char buff[200];
        fgets(buff, 98, file);
        printf("%s\r\n", buff);

        while(fgets(buff, 98, file) != NULL) {
            char *token = strtok(buff, ",");
            while(token != NULL) {
                printf("%s\n", token);
                token = strtok(NULL, ",");
            }
            printf("\n\n");
        }
        

        //fscanf(file, "%s%s%s%s%s%f%f", 
        //   name, title, dpt_id, dpt_descr, date, &gross_salary, &net_salary);
        
        
        //printf("%s\r\n", buff);
        //printf("Name:\t%s\r\n", name);
        //printf("Job Title:\t%s\r\n",title);
        //printf("Dpt:\t%s\r\n", dpt_id);
        //printf("Descr:\t%s\r\n", dpt_descr);
        //printf("Hire Date:\t%s\r\n", date);
        //printf("An. Salary:\t%f\r\n", gross_salary);
        //printf("Net Salary:\t%f\r\n", net_salary);


        fclose(file);
    }

    return;
}