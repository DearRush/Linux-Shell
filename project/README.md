# MyShell user manual（En-version）



## Program description:

The `myshell` program is an interactive command interpreter that bridges the gap between the Linux system and the user. As you type commands over and over again, it parses them and performs the appropriate actions.

## Program internal commands:

The internal commands that the Myshell program can use include:

**cd <directory>**

   + Jumps from the current default directory to the specified directory directory.
   + If directory is not specified, outputs the current directory.
   + If directory does not exist, outputs an error message.
   + This command also modifies the environment variable PWD.

**clr** 

+ Clear the display on the screen.

**dir <directory>** :

+ Outputs everything under the directory directory, including directories and files.

**environ** : 

+ Output all environment variables.

+ Output all environment variables.

**echo <comment>** : + Output the contents of comment and replace it with a new one.

+ Output the content of comment with newlines, when there are multiple spaces or tabs in comment, only one space will be output.

**help** : + Output user guide.

+ Output user guide.

**pause**.

+ stops shell execution until the user presses Enter

 **quit:**.

+ Exits the program.

## Program Environment Variables

The concept of environment variables is covered in the environ command. Environment variables are the equivalent of parameters that are set to the system or to the user's application program, and what they do is related to the specific environment variable. For example, path tells the program in which directories the system should look for the program in addition to the current directory when it is asked to run a program without being told the full path to where the program is located. There are also these environment variables:

| HOME     | Current User Home Directory                                  |
| -------- | ------------------------------------------------------------ |
| HISTSIZE | History                                                      |
| LOGNAME  | Current user's login name                                    |
| HOSTNAME | The name of the host                                         |
| SHELL    | Current user's shell type                                    |
| LANGUAGE | Language-related environment variables, multi-language can modify this environment variable |

## Redirection

The program provides a redirection function, which makes the input and output when executing commands to be determined according to the user's configuration. The redirection input uses the "<" symbol, and the redirection output uses the ">" and ">>" symbols. They can be used in the following ways:

| command < file             | use the specified file as the input device for the command   |
| -------------------------- | ------------------------------------------------------------ |
| Command > File             | Redirects the standard output of the command execution to the specified file. If the file already contains data, the original data will be cleared and new data will be written. If the file does not exist, a new file is created. |
| Command >> File            | Redirects the standard output of the command execution to the specified file. If the file already contains data, the new data will be written after the original content. If the file does not exist, creates a new file. |
| Command < File 1 > File 2  | Uses file 1 as the input device of the command, the execution result of the command is output to file 2, if the file 2 already contains data, the original data will be cleared and then new data will be written. If the file does not exist, a new file is created. |
| Command < File 1 >> File 2 | Uses file 1 as the input device of the command, and the execution result of this command is output to file 2. If file 2 already contains data, the new data will be written after the original content. If the file does not exist, a new file is created. |

## Background Execution

The myshell program can support background execution. When a command is followed by an & symbol, the shell does not need to wait for the command to finish before returning.

## Batch Processing

The program provides batch processing. When the user executes the program at the very beginning, if it is used with an argument, it can read the instructions from the file specified by the argument and execute them sequentially. For example, when we execute the following instruction in the Shell:

```
myshell batchfile (batch file)
```

then we would read each line of the batchfile file in turn and execute it. When the last line of the file is read, the Shell will exit.







# MyShell user manual



## 程序简介：

​    `myshell`程序是一个交互式命令解释器，它是在Linux系统和用户之间的桥梁。当您不断输入命令时，它会解析您输入的命令，并执行相应操作。

## 程序内部命令：

Myshell程序能够使用的内部命令包括了：

**cd <directory>**

   + 从当前的默认目录跳转到指定的目录 directory ，
   + 如果没有指定参数 directory ，输出当前目录
   + 如果 directory 目录不存在，输出错误信息。
   + 该指令同时会修改环境变量PWD。

**clr** 

+ 清空屏幕上的显示内容。

**dir <directory>** ：

+ 输出目录 directory 下的所有内容，包括目录和文件。

**environ**:

+ 输出所有的环境变量。

**echo <comment>** :

+ 输出 comment 的具体内容并换行，当 comment 的内容出现多个space（空格）或者tab时，只输出一个space。

**help**:

+ 输出用户指南

**pause**:

+ 停止Shell的执行，直到用户按下 Enter 键

 **quit:**

+ 退出程序。

## 程序环境变量

​	在environ命令中，涉及了环境变量这一概念。环境变量相当于给系统或用户应用程序设置的一些参数，具体起什么作用和具体的环境变量相关。比如path，是告诉该程序，当要求系统运行一个程序而没有告诉它程序所在的完整路径时，系统除了在当前目录下面寻找此程序外，还应到哪些目录下去寻找。此外还有这些环境变量：

| HOME     | 当前用户主目录                               |
| -------- | -------------------------------------------- |
| HISTSIZE | 历史记录数                                   |
| LOGNAME  | 当前用户的登录名                             |
| HOSTNAME | 主机的名称                                   |
| SHELL    | 当前用户Shell类型                            |
| LANGUAGE | 语言相关的环境变量，多语言可以修改此环境变量 |



## 重定向

​	该程序提供了重定向功能，该功能使得执行命令时的输入、输出可随用户配置而决定。重定向输入使用”<“符号，重定向输出使用”>“、”>>“符号。它们有以下几种使用方式：

| 命令 < 文件             | 将指定文件作为命令的输入设备                                 |
| ----------------------- | ------------------------------------------------------------ |
| 命令 > 文件             | 将命令执行的标准输出结果重定向输出到指定的文件中，如果该文件已包含数据，会清空原有数据，再写入新数据。如果文件不存在，则新建文件。 |
| 命令 >> 文件            | 将命令执行的标准输出结果重定向输出到指定的文件中，如果该文件已包含数据，新数据将写入到原有内容的后面。如果文件不存在，则新建文件。 |
| 命令 < 文件 1 > 文件 2  | 将文件 1 作为命令的输入设备，该命令的执行结果输出到文件 2 中，如果文件2已包含数据，会清空原有数据，再写入新数据。如果文件不存在，则新建文件。 |
| 命令 < 文件 1 >> 文件 2 | 将文件 1 作为命令的输入设备，该命令的执行结果输出到文件 2 中。如果文件2已包含数据，新数据将写入到原有内容的后面。如果文件不存在，则新建文件。 |

## 后台执行

​      myshell程序可以支持后台任务的执行。当一条指令后面有 & 符号时，Shell不需要等待该指令执行结束才能返回。

## 批处理

​     程序提供了批处理功能。当用户最开始执行程序时，如果在使用时带有参数，那么它可以从参数指定的文件中读取指令，并依次执行。例如，当我们在Shell中执行以下指令：

```
myshell batchfile(批处理文件)
```

那么，我们会依次读取 batchfile 文件的每一行并执行。当读取到文件的最后一行时，Shell会退出。


