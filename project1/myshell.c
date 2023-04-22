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
        break;                                    //键盘输入即可，无处理
    }
    case 2:
    {
        // possible batch/script
        // TODO           
        freopen(argv[1], "r", instream);//批处理，从文件中读取命令，输入流改为文件
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
                // "cd" command                            //直接实现内部命令
                if (!strcmp(args[0], "cd"))
                {
                    // TODO
                    if (status.outfile)//如果输出重定向
                    {
                        if (args[1]&&strcmp(args[1],">")&&strcmp(args[1],">>"))//如果有后续参数且不是重定向符号
                        {
                            if (!chdir(args[1]))
                            {
                                char path[100];
                                getcwd(path, sizeof(path));//to get the path 
                                char pathnow[200] = "PATH=";
                                strcat(pathnow, path);//组装字符串为要求的格式
                                putenv(pathnow);
                            }
                            else   //改变路径不成功，说明路径不存在
                            {
                                FILE* a = redirected_op(status);
                                fprintf(a, "%s", "Error! the directory does not exist!\n");
                                fclose(a);
                            }
                        }
                        else         //cd命令没有后续参数，向文件中写入当前路径
                        {
                            char path[100];
                            getcwdstr(path, sizeof(path));
                            FILE* a = redirected_op(status);
                            fprintf(a, "%s", path);
                            fclose(a);
                        }
                    }
                    else//无重定向输出
                    {
                        if (args[1])
                        {
                            if (!chdir(args[1]))
                            {
                                char path[100];
                                getcwd(path, sizeof(path));//to get the current path 
                                char pathnow[200] = "PWD=";
                                strcat(pathnow, path);//组装字符串为要求的格式
                                putenv(pathnow);
                            }
                            else   //改变路径不成功，说明路径不存在
                            {
                                printf("Error! the directory does not exist!\n");
                            }
                        }
                        else
                        {
                            char path[100];
                            printf("%s\n", getcwdstr(path, sizeof(path)));   //包装函数getcwdstr()
                        }

                    }
                }
                // "clr" command
                else if (!strcmp(args[0], "clr"))//用execute（）函数实现
                {
                    // TODO
                    args[0] = "clear";//在我的Linux系统中，没有clr命令，只有clear命令
                    execute(args, status);
                }
                // "dir" command
                else if (!strcmp(args[0], "dir"))//用execute（）函数实现
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
                            fprintf(a, "%s ", args[flag]);   //写入，空格是为了满足echo命令的特殊要求
                            flag++;                          //（多个space或者tab时，应该压缩为一个space。）
                        }
                        fclose(a);
                    }
                    else   //没有重定向
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
                else if (!strcmp(args[0], "environ"))//直接实现
                {
                    // TODO
                    char** ptr = environ;           //外部引用
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
                    execute(args, status);//使用包装函数
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
        if (!strcmp(*args, "<"))                               //输入指令中有重定向输入标记
        {
            // TODO
            *args++;//  <符号之后一定是文件的名字
            char* cwd = (char*)malloc(200 * sizeof(char));     //在main（）函数最后释放空间
            getcwdstr(cwd, 200 * sizeof(char));
            strcat(cwd, "/");
            strcat(cwd, *args);                                //绝对路径字符串的创建
            sstatus->infile = cwd;
        }
        // output direction
        else if ((!strcmp(*args, ">")) || (!strcmp(*args, ">>")))  //输入指令中有重定向输出标记
        {
            // TODO
            if (!strcmp(*args,">"))
            {
                *args++;
                char* cwd = (char*)malloc(200 * sizeof(char));    //同上
                getcwdstr(cwd, 200 * sizeof(char));
                strcat(cwd, "/");
                strcat(cwd, *args);
                sstatus->outfile = cwd;
                sstatus->outmode = "1";                 //设置“1”和“2”字符串以区分两种重定向输出
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
        else if (!strcmp(*args, "&"))                   //输入指令中有后台执行标记
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

        // i/o redirection */                        //重定向文件与stdin/stdout,同时修改传递的参数（不能传递《、<等符号）
        // TODO
        if (sstatus.infile&&!(sstatus.outfile))      //只有重定向输入      
        {
            if (sstatus.foreground)                  //前台执行，没有“&”符号
            {
                freopen(sstatus.infile, "r", stdin);
                int i = 0;
                for (; ; i++)                   //修改向执行文件传递的参数,将<、&符号修改为NULL
                {
                    if (!strcmp(args[i], "<"))
                    {
                        args[i] = args[i + 1];
                        args[i + 1] = NULL;
                        break;
                    }
                }
            }
            else                                     //后台执行，有“&”符号
            {
                freopen(sstatus.infile, "r", stdin);
                int i = 0;
                for (; ; i++)                   //修改向执行文件传递的参数,将<、&符号修改为NULL
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
        else if (sstatus.outfile&&!(sstatus.infile))//只有重定向输出
        {
            if (*(sstatus.outmode)=='1')           //重定向输出符号是“>”
            {
                if (sstatus.foreground)           //前台执行，没有“&”符号
                {
                    freopen(sstatus.outfile, "w", stdout);
                    int i = 0;
                    for (; ; i++)             //修改向执行文件传递的参数,将》、>等符号修改为NULL
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
                    for (; ; i++)             //同上
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
            else if (*(sstatus.outmode)=='2')     //重定向输出符号是“>>”
            {
                freopen(sstatus.outfile, "a", stdout);
                if (sstatus.foreground)           //前台执行，没有“&”符号
                {
                    int i = 0;
                    for (; ; i++)             //修改向执行文件传递的参数,将》、>等符号修改为NULL
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
                    for (; ; i++)             //同上
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
        else if (sstatus.outfile && sstatus.infile)//重定向输出、输入均存在
        {
            freopen(sstatus.infile, "r", stdin);
            if (*(sstatus.outmode) == '1')           //重定向输出符号是“>”
            {
                if (sstatus.foreground)           //前台执行，没有“&”符号
                {
                    freopen(sstatus.outfile, "w", stdout);
                    int i = 0;
                    for (; ; i++)             //修改向执行文件传递的参数,将》、>等符号修改为NULL
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
                    for (; ; i++)             //同上
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
            else if (*(sstatus.outmode) == '2')     //重定向输出符号是“>>”
            {
                freopen(sstatus.outfile, "a", stdout);
                if (sstatus.foreground)           //前台执行，没有“&”符号
                {
                    int i = 0;
                    for (; ; i++)             //修改向执行文件传递的参数,将》、>等符号修改为NULL
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
                    for (; ; i++)             //同上
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
        else              //没有重定向
        {
           if (!sstatus.foreground)
           {
               int i = 0;
               for (; ; i++)             //找到&符号，将其清除
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
    getcwd(buffer, size);//找到当前路径装入buffer中
    return buffer;
}

/***********************************************************************

FILE * redirected_op(shellstatus ststus)

  return o/p stream (redirected if necessary)

***********************************************************************/

FILE *redirected_op(shellstatus status)   //将目标输出流返回
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
