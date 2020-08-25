// ##########################################
// 
//   Author  -   Collin Thornton
//   Email   -   collin.thornton@okstate.edu
//   Alg     -   Employe Record INCLUDE
//   Date    -   8-24-20
//
// ########################################## 

#ifndef __EMPLOYEE_RECORD_H__
#define __EMPLOYEE_RECORD_H__

struct Day  { short day, year, time; };
struct Time { short hour, minute; };
struct Date {
    struct Day day;
    struct Time time;
};

struct Employee {
    char *first_name, *last_name;
    char *job_title;
    char *dpt_id;
    char *dpt_name;

    struct Date hire_date;
    struct Date term_date;

    float net_salary;
    float gross_salary;
};

struct Database {
    struct Employee* employee_list;
    char** dpt_list; 
};

int main(int argc, char** argv);

void createDatabase(char* input_files[], int numFiles, struct Database* database);
void destroyDatabase(struct Database* database); 

unsigned int addEmployee(const struct Employee *employee, struct Database *database);
unsigned int delEmployee(const struct Employee *employee, struct Database *database);
unsigned int increaseListSize(unsigned int, struct Database *database);
unsigned int decreaseListSize(unsigned int, struct Database *database);

void menuStateMachine(void);


#endif // __EMPLOYEE_RECORD_H__