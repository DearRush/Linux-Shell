#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <time.h>



#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAX_BUFFER 1024    // max line buffer
#define MAX_ARGS 64        // max # args
#define SEPARATORS " \t\n" // token sparators
#define README "readme"    // help file name

struct shellstatus_st
{
    int foreground;  // foreground execution flag
    char *infile;    // input redirection flag & file
    char *outfile;   // output redirection flag & file
    char *outmode;   // output redirection mode
    char *shellpath; // full pathname of shell
};
typedef struct shellstatus_st shellstatus;

extern char **environ;

void check4redirection(char **, shellstatus *); // check command line for i/o redirection
void errmsg(char *, char *);                    // error message printout
void execute(char **, shellstatus);             // execute command from arg array
char *getcwdstr(char *, int);                   // get current work directory string
FILE *redirected_op(shellstatus);               // return required o/p stream
char *stripath(char *);                         // strip path from filename
void syserrmsg(char *, char *);                 // system error message printout

/*******************************************************************/

int main(int argc,char **argv)
{
    FILE *ostream = stdout;   // (redirected) o/p stream
    FILE *instream = stdin;   // batch/keyboard input
    char linebuf[MAX_BUFFER]; // line buffer
    char cwdbuf[MAX_BUFFER];  // cwd buffer
    char *args[MAX_ARGS];     // pointers to arg strings
    char **arg;               // working pointer thru args
    char *prompt = "==>";     // shell prompt
    char *readmepath;         // readme pathname
    shellstatus status;       // status structure

    // parse command line for batch input
    switch (argc)
    {
    case 1:
    {
        // keyboard input
        // TODO
        break;                                    //�������뼴�ɣ��޴���
    }
    case 2:
    {
        // possible batch/script
        // TODO           
        freopen(argv[1], "r", instream);//���������ļ��ж�ȡ�����������Ϊ�ļ�
        break;

    }
    default: // too many arguments
        fprintf(stderr, "%s command line error; max args exceeded\n"
                        "usage: %s [<scriptfile>]",
                stripath(argv[0]), stripath(argv[0]));
        exit(1);
    }

    // get starting cwd to add to readme pathname
    // TODO
    readmepath = (char*)malloc(200 * sizeof(char));
    getcwdstr(readmepath, MAX_BUFFER*sizeof(char));      //get cwd in ptr
    strcat(readmepath ,"/readme");                       //to assmble the readme path

    // get starting cwd to add to shell pathname
    // TODO
    memset(cwdbuf, '\0', 1024);                    //clear the array
    getcwdstr(cwdbuf, MAX_BUFFER * sizeof(char));  //get cwd
    strcat(cwdbuf, "/myshell");                    //to assmble the shell path


    // set SHELL= environment variable, malloc and store in environment
    // TODO
    char shellpa[1024] = "shell=";
    strcat(shellpa, cwdbuf);//assmble the environment variable
    putenv(shellpa);
    status.shellpath = cwdbuf;                   //save the path into the status


    // prevent ctrl-C and zombie children
    signal(SIGINT, SIG_IGN);  // prevent ^C interrupt
    signal(SIGCHLD, SIG_IGN); // prevent Zombie children

    // keep reading input until "quit" command or eof of redirected input
    while (!feof(instream))
    {
        // (re)initialise status structure
        status.foreground = TRUE;

        // set up prompt
        printf("%s", prompt);

        // get command line from input
        if (fgets(linebuf, MAX_BUFFER, instream))
        {
            // read a line
            // tokenize the input into args array
            arg = args;
            *arg++ = strtok(linebuf, SEPARATORS); // tokenize input
            while ((*arg++ = strtok(NULL, SEPARATORS)))
                ;

            // last entry will be NULL
            if (args[0])
            {
                // check for i/o redirection
                check4redirection(args, &status);

                // check for internal/external commands
                // "cd" command                            //ֱ��ʵ���ڲ�����
                if (!strcmp(args[0], "cd"))
                {
                    // TODO
                    if (status.outfile)//�������ض���
                    {
                        if (args[1]&&strcmp(args[1],">")&&strcmp(args[1],">>"))//����к��������Ҳ����ض������
                        {
                            if (!chdir(args[1]))
                            {
                                char path[100];
                                getcwd(path, sizeof(path));//to get the path 
                                char pathnow[200] = "PATH=";
                                strcat(pathnow, path);//��װ�ַ���ΪҪ��ĸ�ʽ
                                putenv(pathnow);
                            }
                            else   //�ı�·�����ɹ���˵��·��������
                            {
                                FILE* a = redirected_op(status);
                                fprintf(a, "%s", "Error! the directory does not exist!\n");
                                fclose(a);
                            }
                        }
                        else         //cd����û�к������������ļ���д�뵱ǰ·��
                        {
                            char path[100];
                            getcwdstr(path, sizeof(path));
                            FILE* a = redirected_op(status);
                            fprintf(a, "%s", path);
                            fclose(a);
                        }
                    }
                    else//���ض������
                    {
                        if (args[1])
                        {
                            if (!chdir(args[1]))
                            {
                                char path[100];
                                getcwd(path, sizeof(path));//to get the current path 
                                char pathnow[200] = "PWD=";
                                strcat(pathnow, path);//��װ�ַ���ΪҪ��ĸ�ʽ
                                putenv(pathnow);
                            }
                            else   //�ı�·�����ɹ���˵��·��������
                            {
                                printf("Error! the directory does not exist!\n");
                            }
                        }
                        else
                        {
                            char path[100];
                            printf("%s\n", getcwdstr(path, sizeof(path)));   //��װ����getcwdstr()
                        }

                    }
                }
                // "clr" command
                else if (!strcmp(args[0], "clr"))//��execute��������ʵ��
                {
                    // TODO
                    args[0] = "clear";//���ҵ�Linuxϵͳ�У�û��clr���ֻ��clear����
                    execute(args, status);
                }
                // "dir" command
                else if (!strcmp(args[0], "dir"))//��execute��������ʵ��
                {
                    // TODO
                    args[0] = "ls";
                    args[1] = "-al";
                    execute(args, status);
                }
                // "echo" command
                else if (!strcmp(args[0], "echo"))
                {
                    // TODO
                    if (status.outfile)//o redirection
                    {
                        int flag = 1;
                        FILE* a = redirected_op(status);
                        while (args[flag])
                        {
                            fprintf(a, "%s ", args[flag]);   //д�룬�ո���Ϊ������echo���������Ҫ��
                            flag++;                          //�����space����tabʱ��Ӧ��ѹ��Ϊһ��space����
                        }
                        fclose(a);
                    }
                    else   //û���ض���
                    {
                        int flag = 1;
                        while (args[flag])
                        {
                            printf("%s ", args[flag]);
                            flag++;
                        }
                        printf("\n");
                    }
                }
                // "environ" command
                else if (!strcmp(args[0], "environ"))//ֱ��ʵ��
                {
                    // TODO
                    char** ptr = environ;           //�ⲿ����
                    if (status.outfile)//o redirection
                    {
                        FILE* a = redirected_op(status);
                        for (; (*ptr) != NULL; ptr++)
                        {
                            fprintf(a, "%s\n", *ptr);
                        }
                    }
                    else
                    {
                        for (; (*ptr) != NULL; ptr++)
                        {
                            printf("%s\n", *ptr);
                        }
                    }
                }
                // "help" command
                else if (!strcmp(args[0], "help"))
                {
                    args[0] = "more";
                    args[1] = readmepath;
                    args[2] = NULL;
                    execute(args, status);
                }
                // "pause" command - note use of getpass - this is a made to measure way of turning off
                //  keyboard echo and returning when enter/return is pressed
                else if (!strcmp(args[0], "pause"))
                {
                    // TODO
                    char array[10];
                    gets(array);
                }
                // "quit" command
                else if (!strcmp(args[0], "quit"))
                {
                    break;
                }
                // else pass command on to OS shell
                else
                {
                    execute(args, status);//ʹ�ð�װ����
                }
                free(status.infile);
                free(status.outfile);
            }
        }
    }
    return 0;
}

/***********************************************************************

void check4redirection(char ** args, shellstatus *sstatus);

check command line args for i/o redirection & background symbols
set flags etc in *sstatus as appropriate

***********************************************************************/

void check4redirection(char **args, shellstatus *sstatus)
{
    sstatus->infile = NULL; // set defaults
    sstatus->outfile = NULL;
    sstatus->outmode = NULL;

    while (*args)
    {
        // input redirection
        if (!strcmp(*args, "<"))                               //����ָ�������ض���������
        {
            // TODO
            *args++;//  <����֮��һ�����ļ�������
            char* cwd = (char*)malloc(200 * sizeof(char));     //��main������������ͷſռ�
            getcwdstr(cwd, 200 * sizeof(char));
            strcat(cwd, "/");
            strcat(cwd, *args);                                //����·���ַ����Ĵ���
            sstatus->infile = cwd;
        }
        // output direction
        else if ((!strcmp(*args, ">")) || (!strcmp(*args, ">>")))  //����ָ�������ض���������
        {
            // TODO
            if (!strcmp(*args,">"))
            {
                *args++;
                char* cwd = (char*)malloc(200 * sizeof(char));    //ͬ��
                getcwdstr(cwd, 200 * sizeof(char));
                strcat(cwd, "/");
                strcat(cwd, *args);
                sstatus->outfile = cwd;
                sstatus->outmode = "1";                 //���á�1���͡�2���ַ��������������ض������
            }
            else if (!strcmp(*args, ">>"))
            {
                *args++;
                char* cwd = (char*)malloc(200 * sizeof(char));
                getcwdstr(cwd, 200 * sizeof(char));
                strcat(cwd, "/");
                strcat(cwd, *args);
                sstatus->outfile = cwd;
                sstatus->outmode = "2";
            }
        }
        else if (!strcmp(*args, "&"))                   //����ָ�����к�ִ̨�б��
        {
            // TODO
            sstatus->foreground = FALSE;
        }
        *args++;
    }
}

/***********************************************************************

  void execute(char ** args, shellstatus sstatus);

  fork and exec the program and command line arguments in args
  if foreground flag is TRUE, wait until pgm completes before
    returning

***********************************************************************/

void execute(char **args, shellstatus sstatus)
{
    int status;
    pid_t child_pid;
    char tempbuf[MAX_BUFFER];

    switch (child_pid=fork())
    {
    case -1:
        syserrmsg("fork", NULL);
        break;
    case 0:
        // execution in child process
        // reset ctrl-C and child process signal trap
        signal(SIGINT, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        // i/o redirection */                        //�ض����ļ���stdin/stdout,ͬʱ�޸Ĵ��ݵĲ��������ܴ��ݡ���<�ȷ��ţ�
        // TODO
        if (sstatus.infile&&!(sstatus.outfile))      //ֻ���ض�������      
        {
            if (sstatus.foreground)                  //ǰִ̨�У�û�С�&������
            {
                freopen(sstatus.infile, "r", stdin);
                int i = 0;
                for (; ; i++)                   //�޸���ִ���ļ����ݵĲ���,��<��&�����޸�ΪNULL
                {
                    if (!strcmp(args[i], "<"))
                    {
                        args[i] = args[i + 1];
                        args[i + 1] = NULL;
                        break;
                    }
                }
            }
            else                                     //��ִ̨�У��С�&������
            {
                freopen(sstatus.infile, "r", stdin);
                int i = 0;
                for (; ; i++)                   //�޸���ִ���ļ����ݵĲ���,��<��&�����޸�ΪNULL
                {
                    if (!strcmp(args[i], "<"))
                    {
                        args[i] = args[i + 1];
                        args[i + 1] = NULL;
                    }
                    if (!strcmp(args[i], "&"))
                    {
                        args[i] = NULL;
                        break;
                    }
                }
            }
        }
        else if (sstatus.outfile&&!(sstatus.infile))//ֻ���ض������
        {
            if (*(sstatus.outmode)=='1')           //�ض�����������ǡ�>��
            {
                if (sstatus.foreground)           //ǰִ̨�У�û�С�&������
                {
                    freopen(sstatus.outfile, "w", stdout);
                    int i = 0;
                    for (; ; i++)             //�޸���ִ���ļ����ݵĲ���,������>�ȷ����޸�ΪNULL
                    {
                        if (!strcmp(args[i], ">"))
                        {
                            args[i] = NULL;
                            args[i + 1] = NULL;
                            break;
                        }
                    }
                }
                else
                {
                    freopen(sstatus.outfile, "w", stdout);
                    int i = 0;
                    for (; ; i++)             //ͬ��
                    {
                        if (!strcmp(args[i], ">"))
                        {
                            args[i] = NULL;
                            args[i + 1] = NULL;
                        }
                        if (!strcmp(args[i], "&"))
                        {
                            args[i] = NULL;
                            break;
                        }
                    }
                }
            }
            else if (*(sstatus.outmode)=='2')     //�ض�����������ǡ�>>��
            {
                freopen(sstatus.outfile, "a", stdout);
                if (sstatus.foreground)           //ǰִ̨�У�û�С�&������
                {
                    int i = 0;
                    for (; ; i++)             //�޸���ִ���ļ����ݵĲ���,������>�ȷ����޸�ΪNULL
                    {
                        if (!strcmp(args[i], ">>"))
                        {
                            args[i] = NULL;
                            args[i + 1] = NULL;
                            break;
                        }
                    }
                }
                else
                {
                    int i = 0;
                    for (; ; i++)             //ͬ��
                    {
                        if (!strcmp(args[i], ">>"))
                        {
                            args[i] = NULL;
                            args[i + 1] = NULL;
                        }
                        if (!strcmp(args[i], "&"))
                        {
                            args[i] = NULL;
                            break;
                        }
                    }
                }
            }
        }
        else if (sstatus.outfile && sstatus.infile)//�ض�����������������
        {
            freopen(sstatus.infile, "r", stdin);
            if (*(sstatus.outmode) == '1')           //�ض�����������ǡ�>��
            {
                if (sstatus.foreground)           //ǰִ̨�У�û�С�&������
                {
                    freopen(sstatus.outfile, "w", stdout);
                    int i = 0;
                    for (; ; i++)             //�޸���ִ���ļ����ݵĲ���,������>�ȷ����޸�ΪNULL
                    {
                        if (!strcmp(args[i], "<"))
                        {
                            args[i] = args[i + 1];
                            args[i + 1] = NULL;
                            i++;
                            continue;
                        }
                        if (!strcmp(args[i], ">"))
                        {
                            args[i] = NULL;
                            args[i + 1] = NULL;
                            break;
                        }
                    }
                }
                else
                {
                    freopen(sstatus.outfile, "w", stdout);
                    int i = 0;
                    for (; ; i++)             //ͬ��
                    {
                        if (!strcmp(args[i], "<"))
                        {
                            args[i] = args[i + 1];
                            args[i + 1] = NULL;
                            i++;
                            continue;
                        }
                        if (!strcmp(args[i], ">"))
                        {
                            args[i] = NULL;
                            args[i + 1] = NULL;
                        }
                        if (!strcmp(args[i], "&"))
                        {
                            args[i] = NULL;
                            break;
                        }
                    }
                }
            }
            else if (*(sstatus.outmode) == '2')     //�ض�����������ǡ�>>��
            {
                freopen(sstatus.outfile, "a", stdout);
                if (sstatus.foreground)           //ǰִ̨�У�û�С�&������
                {
                    int i = 0;
                    for (; ; i++)             //�޸���ִ���ļ����ݵĲ���,������>�ȷ����޸�ΪNULL
                    {
                        if (!strcmp(args[i],"<"))
                        {
                            args[i] = args[i + 1];
                            args[i + 1]=NULL;
                            i++;
                            continue;
                        }
                        if (!strcmp(args[i], ">>"))
                        {
                            args[i] = NULL;
                            args[i + 1] = NULL;
                            break;
                        }
                    }
                }
                else
                {
                    int i = 0;
                    for (; ; i++)             //ͬ��
                    {
                        if (!strcmp(args[i], "<"))
                        {
                            args[i] = args[i + 1];
                            args[i + 1] = NULL;
                            i++;
                            continue;
                        }
                        if (!strcmp(args[i], ">>"))
                        {
                            args[i] = NULL;
                            args[i + 1] = NULL;
                        }
                        if (!strcmp(args[i], "&"))
                        {
                            args[i] = NULL;
                            break;
                        }
                    }
                }
            }
        }
        else              //û���ض���
        {
           if (!sstatus.foreground)
           {
               int i = 0;
               for (; ; i++)             //�ҵ�&���ţ��������
               {
                   if (!strcmp(args[i], "&"))
                   {
                       args[i] = NULL;
                       break;
                   }
               }
           }
        }

        
       

        // set PARENT = environment variable, malloc and put in nenvironment
        // TODO
        strcpy(tempbuf, "parent=");
        strcat(tempbuf, sstatus.shellpath);                   //to withdraw the path in the shell struct
        putenv(tempbuf);


        // final exec of program
        execvp(args[0], args);
        syserrmsg("exec failed -", args[0]);
        exit(127);
    }

    // continued execution in parent process
    // TODO
    if (sstatus.foreground==TRUE)
    {
        waitpid(child_pid, &status, 0);
        printf("\n");
    }
    else
    {
        waitpid(child_pid, &status, WNOHANG);
    }
    
}

/***********************************************************************

 char * getcwdstr(char * buffer, int size);

return start of buffer containing current working directory pathname

***********************************************************************/

char *getcwdstr(char *buffer, int size)
{
    // TODO
    getcwd(buffer, size);//�ҵ���ǰ·��װ��buffer��
    return buffer;
}

/***********************************************************************

FILE * redirected_op(shellstatus ststus)

  return o/p stream (redirected if necessary)

***********************************************************************/

FILE *redirected_op(shellstatus status)   //��Ŀ�����������
{
    FILE *ostream = stdout;

    // TODO
    if (status.outfile)
    {
        if (*(status.outmode)=='1')
        {
            ostream = fopen(status.outfile, "w");
        }
        else if (*(status.outmode)=='2')
        {
            ostream = fopen(status.outfile, "a");
        }
    }

    return ostream;
}

/*******************************************************************

  char * stripath(char * pathname);

  strip path from file name

  pathname - file name, with or without leading path

  returns pointer to file name part of pathname
            if NULL or pathname is a directory ending in a '/'
                returns NULL
*******************************************************************/

char *stripath(char *pathname)
{
    char *filename = pathname;

    if (filename && *filename)
    {                                      // non-zero length string
        filename = strrchr(filename, '/'); // look for last '/'
        if (filename)                      // found it
            if (*(++filename))             //  AND file name exists
                return filename;
            else
                return NULL;
        else
            return pathname; // no '/' but non-zero length string
    }                        // original must be file name only
    return NULL;
}

/********************************************************************

void errmsg(char * msg1, char * msg2);

print an error message (or two) on stderr

if msg2 is NULL only msg1 is printed
if msg1 is NULL only "ERROR: " is printed
*******************************************************************/

void errmsg(char *msg1, char *msg2)
{
    fprintf(stderr, "ERROR: ");
    if (msg1)
        fprintf(stderr, "%s; ", msg1);
    if (msg2)
        fprintf(stderr, "%s; ", msg2);
    return;
    fprintf(stderr, "\n");
}

/********************************************************************

  void syserrmsg(char * msg1, char * msg2);

  print an error message (or two) on stderr followed by system error
  message.

  if msg2 is NULL only msg1 and system message is printed
  if msg1 is NULL only the system message is printed
 *******************************************************************/

void syserrmsg(char *msg1, char *msg2)
{
    errmsg(msg1, msg2);
    perror(NULL);
    return;
}
