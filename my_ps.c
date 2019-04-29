#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

float CPU(int pid){
    FILE *uptimeFile = fopen("/proc/uptime", "r");
    float uptime;
    fscanf(uptimeFile,"%f",&uptime);
    //printf("uptime:%f \n", uptime);
    fclose(uptimeFile);
    
    char stat[1000];
    sprintf(stat, "/proc/%d/stat", pid);
    FILE *f = fopen(stat, "r");
    
    unsigned long int usertime, systime;
    unsigned long long int starttime;
    
    fscanf(f,"%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu %*ld %*ld %*ld %*ld %*ld %*ld %llu %*lu", &usertime, &systime, &starttime);
    //printf("user:%lu \n", usertime);
    //printf("sys:%lu \n", systime);
    //printf("starttime:%llu \n", starttime);
    
    fclose(f);
    
    float total_time = (float)usertime + (float)systime;
    //printf("total time:%f \n", total_time);
    
    float seconds = uptime - ((float)starttime/sysconf(_SC_CLK_TCK));
    //printf("seconds:%f \n", seconds);
    
    float cpu_usage = 100*((total_time/sysconf(_SC_CLK_TCK))/seconds);
    //printf("cpus percent:%f \n", cpu_usage);
    return cpu_usage;
    
}

unsigned long int vmsize(int pid){
    char stat[1000];
    sprintf(stat, "/proc/%d/stat", pid);
    FILE *f = fopen(stat, "r");
    
    unsigned long int vsz;
    
    fscanf(f,"%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*ld %*llu %lu", &vsz);
    //printf("vsz:%lu \n", vsz);
    
    return vsz/1024;
}

long int findRSS(int pid){
    char stat[1000];
    sprintf(stat, "/proc/%d/stat", pid);
    FILE *f = fopen(stat, "r");
    
    unsigned long int rss;
    
    fscanf(f,"%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*ld %*llu %*lu %ld", &rss);
    //printf("vsz:%lu \n", vsz);
    
    return rss;
}

char findSTAT(int pid){
    char stat[1000];
    sprintf(stat, "/proc/%d/stat", pid);
    FILE *f = fopen(stat, "r");
    
    char state;
    
    fscanf(f,"%*d %*s %c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*ld %*llu %*lu %*ld", &state);
    //printf("vsz:%lu \n", vsz);
    
    return state;
}

/*unsigned long long int findSTART (int pid){
    FILE *uptimeFile = fopen("/proc/uptime", "r");
    float uptime;
    fscanf(uptimeFile,"%f",&uptime);
    //printf("uptime:%f \n", uptime);
    fclose(uptimeFile);
    
    char stat[1000];
    sprintf(stat, "/proc/%d/stat", pid);
    FILE *f = fopen(stat, "r");

    unsigned long long int starttime;
    
    fscanf(f,"%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*ld %llu %*lu", &starttime);
    
    float seconds = uptime - (float)(starttime/sysconf(_SC_CLK_TCK));
    
}*/

int main(void)
{
    printf("USER\tPID\t%%CPU\t%%MEM\tVSZ\tRSS\tTTY\tSTAT\tSTART\tTIME\tCOMMAND\n");
    struct dirent *de;  // Pointer for directory entry
    
    // opendir() returns a pointer of DIR type.
    DIR *dr = opendir("/proc");
    
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        return 0;
    }

    while ((de = readdir(dr)) != NULL){
        int pid = atoi(de->d_name);
        if(pid != 0){
            //printf("%d\n", pid);
            
            float percentCPU = CPU(pid);
            
            //printf("%%CPU:%.1f\n", percentCPU);
            
            unsigned long int vsize = vmsize(pid);
            
            long int rss = findRSS(pid);
            
            char stat = findSTAT(pid);
            
            printf("USER\t%d\t%.1f\t-\t%lu\t%ld\t?\t%c\n",pid, percentCPU, vsize, rss, stat);
        }
    }
    
    closedir(dr);
    return 0;
}
