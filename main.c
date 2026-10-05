// libraries for in/output characters memory typestring and boolean values.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// limit the max and min length of line and read the file name 
#define MAX_LINE_LEN 256
#define MAX_NAME_LEN 100
#define FILE_NAME "course.txt"

// show the course name and the grade and the next pointer to the next node in the linked list
typedef struct CourseRecord {
    char course_name[MAX_NAME_LEN];
    int grade;
    struct CourseRecord *next;
} CourseRecord;

// function to remove leading and trailing whitespace from a string.
// if the string is empty return the function 
void trim_whitespace(char *str) {
    if (str == NULL || *str == '\0') return;
    char *start = str;
    while (isspace((unsigned char)*start)) start++;
    if (*start == '\0') {
        *str = '\0';
        return;
    }
    // find the end of the line and move to the last 
    char *end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    // if the start pointer is not the same as the original string copy the trimmed string to original string
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

// add a new record to the linked list 
bool add_record(CourseRecord **head, const char *course, int grade) {
    CourseRecord *new_node = (CourseRecord *)malloc(sizeof(CourseRecord));
    // if the memory fails to allocate the new node
    if (new_node == NULL) {
        // print the error
        fprintf(stderr, "Error: Memory allocation failed.\n");
        // return false
        return false;
    }
    // copy the course name to the new node and set the next pointer to the head of list
    strncpy(new_node->course_name, course, MAX_NAME_LEN - 1);
    new_node->course_name[MAX_NAME_LEN - 1] = '\0';
    new_node->grade = grade;
    new_node->next = *head;
    *head = new_node;
    return true;
}

// the function to print the transcript 
void print_transcript(const char *student_name, CourseRecord *head) {
    if (head == NULL) {
        printf("\nNo records found for student: %s\n\n", student_name);
        return;
    }
    // print a formatted transcript header with student name and the course and the grade.
    printf("\n==========================================\n");
    printf(" Transcript for: %s\n", student_name);
    printf("==========================================\n");
    printf("%-30s | %-5s\n", "Course Name", "Grade");
    printf("------------------------------------------\n");
    // get the total grades and count for calculating the average
    int total_grades = 0;
    int count = 0;
    CourseRecord *curr = head;

    while (curr != NULL) {
        printf("%-30s | %-5d\n", curr->course_name, curr->grade);
        // total grades and count of the grades from the linked list
        total_grades += curr->grade;
        // count the grades for calculating
        count++;
        // continue to the next node in the linked list
        curr = curr->next;
    }

    printf("------------------------------------------\n");
    // if there are any grades counted
    if (count > 0) {
        // calculate the average
        double average = (double)total_grades / count;
        // print the average grade with 2 decimal places
        printf("%-30s | %-5.2f\n", "Average Grade", average);
    }
    printf("==========================================\n\n");
}

// memory cleanup for the linked list
void free_list(CourseRecord *head) {
    CourseRecord *temp;
    // loop through the list 
    while (head != NULL) {
        temp = head;
        head = head->next;
        // free the memory for the node
        free(temp);
    }
}

// loading the student records from the file and matching the target in the file
CourseRecord *load_student_records(const char *filename, const char *target_student) {
    // open the file 
    FILE *file = fopen(filename, "r");
    // if file is not open shows error
    if (file == NULL) {
        // print the error if not successful opening file
        fprintf(stderr, "Error: Could not open file '%s'.\n", filename);
        return NULL;
    }
    // hold the current course name and the line read from the file
    char line[MAX_LINE_LEN];
    char current_course[MAX_NAME_LEN] = "";
    CourseRecord *head = NULL;
    // read the file line by line
    while (fgets(line, sizeof(line), file) != NULL) {
        trim_whitespace(line);

        // check if the line is empty after trimming 
        if (strlen(line) == 0) continue;

        // if the line starts with the opening bracket and ends with the closing bracket, then it is a course title.
        if (line[0] == '[' && line[strlen(line) - 1] == ']') {
            strncpy(current_course, line + 1, strlen(line) - 2);
            current_course[strlen(line) - 2] = '\0';
            trim_whitespace(current_course);
            continue;
        }

        // separate the student name and grade by looking for the colon in the line
        char *colon_pos = strchr(line, ':');
        if (colon_pos != NULL) {
            if (strlen(current_course) == 0) {
                // print a warning if name entered without course, and continue to next line
                fprintf(stderr, "Warning: Ignored entry before any course title: %s\n", line);
                continue;
            }

            // separate the student and the grade by replacing the colon with null terminator
            *colon_pos = '\0';
            char student_name[MAX_NAME_LEN];
            strncpy(student_name, line, MAX_NAME_LEN - 1);
            student_name[MAX_NAME_LEN - 1] = '\0';
            trim_whitespace(student_name);

            int grade = atoi(colon_pos + 1);

            // if the student name matches the target student add the record to list
            if (strcasecmp(student_name, target_student) == 0) {
                add_record(&head, current_course, grade);
            }
        }
    }
    // close the file after reading
    fclose(file);
    return head;
}

// the main for running program
int main(void) {
    char student_name[MAX_NAME_LEN];
    
    // welcome message with formatted text
    printf("\n==========================================\n");
    printf("  WELCOME TO TRANSCRIPT PRINTER\n");
    printf("==========================================\n\n");
    
    // the main loop to ask for student names and print their transcript
    while (1) {
        printf("Enter student name (or 'stop' to exit): ");
        if (fgets(student_name, sizeof(student_name), stdin) == NULL) {
            break;
        }
        // remove the trailing newline character / whitespace
        trim_whitespace(student_name);
        
        // check if the user entered an empty name or just new line
        if (strlen(student_name) == 0) {
            // print the error message and continue the loop to ask for the name again
            printf("Error: Name cannot be empty. Please try again.\n\n");
            continue;
        }
        
        // check if the user wants to stop the program
        if (strcasecmp(student_name, "stop") == 0) {
            // exit the program
            printf("End of the program. Goodbye!\n");
            break;
        }
        
        // load student records for the given name 
        CourseRecord *records = load_student_records(FILE_NAME, student_name);
        // print student transcript
        print_transcript(student_name, records);
        // free the memory allocated for the linked list
        free_list(records); 
    } // close loop while

    return 0;
}