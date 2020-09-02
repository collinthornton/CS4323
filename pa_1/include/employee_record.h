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

enum hireStatus{
    newHire,
    reHired,
    terminated
};

struct Day  { short day, month, year; };
struct Time { short hour, minute; };
typedef struct {
    struct Day day;
    struct Time time;
} Date;

typedef struct {
    char id[200];
    char full_name[100];
    char job_title[100];
    char dpt_id[20];
    char dpt_descr[100];

    Date hire_date;
    Date term_date;

    float net_salary;
    float grs_salary;

    enum hireStatus hire_status;
} Employee;

typedef struct {
    char dpt_id[20];
    char dpt_descr[100];

    float annual_salary_2019;
    float annual_salary_2018;
    float net_change;
} Department;

typedef struct {
    Employee* employee_list;
    Department* dpt_list;

    int num_employees, employees_allocated;
    int num_dpts, dpts_allocated;
} Database;

int main(int argc, char** argv);

void createDatabase(char input_files[2][100], int numFiles, Database* database);
void destroyDatabase(Database* database); 

unsigned int addEmployee(const Employee *employee, Database *database, int year);

const char* dateToString(const Date *date, char* str);
const char* employeeToString(const Employee *employee, char* str);
const char* dptToString(const Department *dpt, char *str);
char* removeWhitespace(const char* input, char* output);

int checkEmployee(const Database* database, const char* id);
int checkDpt(const Database* database, const char* dpt_descr);

unsigned short menuStateMachine(const Database* database);
unsigned short mainMenu(const Database* datbase, char input_files[2][100]);
unsigned short displayDepartments(const Database* database);
unsigned short displayEmployeesForDpt(const Database* database, const Department* dpt);

#endif // __EMPLOYEE_RECORD_H__