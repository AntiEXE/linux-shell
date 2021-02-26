#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include"linkedlist.h"
#include<signal.h>

int sh_cd(char** args)
{
	if(args[1]==NULL)
	{
		fprintf(stderr,"sh: expected argument to \"cd\"\n");
	}
	else
	{
		if(chdir(args[1])!=0)
		{
			perror("Invalid Call");
		}
	}
	return 1;
}

int sh_exit(char **args)
{
	return 0;
}
//---
int sh_pwd()
{
    char cwd[1024];
    
    if (getcwd(cwd, sizeof(cwd)) != NULL){
        fprintf(stdout, "%s\n", cwd);
    }
    else {
        printf("getcwd() error!\n");
    }
    
    return 1;
}

int sh_help(char **args)
{
    int i = 0;
    printf("\n");
    
    printf("ACM shell bit modified by AntiEXE.\n");
    printf("refer to the https://github.com/AntiEXE/linux-shell");
    printf("These shell commands are defined internally.\n");
    printf("\n");
    
    printf("cd\nexit\nbg\nbglist\nkill\npwd\nhelp");
    
    printf("\n");
    
    return 1;
}

//--
process* headProcess=NULL;
int sh_bg(char **args)
{
//args -- bg echo "hello"
    ++args;
//args -- echo "hello"
    char *firstCmd= args[0];//echo
    int childpid=fork();
    if (childpid>=0)
    {
        if (childpid==0)
        {
            if (execvp(firstCmd,args)<0)
            {
                perror("Error on execvp\n");
                exit(0);
            }

        }
        else{
                if (headProcess==NULL)
                {
                    headProcess=create_list(childpid,firstCmd);
                }
                else{
                    add_to_list(childpid,firstCmd,true);
                }
        }
    }
    else{
        perror("fork() error");
    }
    return 1;
}
int sh_bglist(char **args)
{
    print_list();
    return 1;
}
int sh_kill(char **args)
{
    // kill 1575
    char* pidCmd=args[1];
    if(!pidCmd)//kill 
    {
        printf("Please specify a pid\n");
    }
    else
    {
        int pid=atoi(pidCmd);
        process* target= search_in_list(pid, &headProcess);
        if (target!=NULL)
        {
            if (kill(target->pid,SIGTERM)>=0)
            {
                delete_from_list(pid);
            }
            else{
                perror("Could not kill pid specified\n");
            }
        }
        else{
            printf("Specify a pid which is present in the list.\nType \"bglist\" to see active processes\n");
        }
    }
    return 1;
}
char* builtin_str[]={
	"cd",
	"exit",
    "bg",
    "bglist",
    "kill",
    "pwd",
    "help"
};
//bg echo "hello"
//bglist ->jobs
//kill 
int (*builtin_func[]) (char**)={
	&sh_cd,
	&sh_exit,
    &sh_bg,
    &sh_bglist,
    &sh_kill,
    &sh_pwd,
    &sh_help
};



char *sh_read_line()
{
	char*line=NULL;
	ssize_t bufsize =0;
	if(getline(&line,&bufsize,stdin)==-1)
	{
		if(feof(stdin))
			exit(EXIT_SUCCESS);
		else
		{
			perror("acm-sh: getline\n");
			exit(EXIT_FAILURE);
		}
	}
	return line;
} 

#define SH_TOKEN_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n"

char** sh_split_line(char* line)
{
	int bufsize=SH_TOKEN_BUFSIZE,position=0;
	char ** tokens=malloc(bufsize * sizeof(char*));
	char *token;

	if(!token)
	{
		fprintf(stderr,"acm-sh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	//strtok :- Library Function 
	token=strtok(line,SH_TOK_DELIM);
	while(token!=NULL)
	{
		tokens[position]=token;
		token=strtok(NULL,SH_TOK_DELIM);

		position++;
	}
	tokens[position]=NULL;
	return tokens;
}

int sh_launch(char **args)
{
	pid_t pid;
	int status;

	pid=fork();
	if(pid==0)
	{
		if(execvp(args[0],args)==-1)
		{
			printf("Invalid Command\n");
		}
		exit(EXIT_FAILURE);
	}
	else if(pid<0)
	{
		perror("acm-sh");
	}
	else
	{
		do
		{
			waitpid(pid,&status,WUNTRACED);
		}
		while(!WIFEXITED(status)&&!WIFSIGNALED(status));
	}
	return 1;
}

int sh_execute(char ** args)
{
	int i;
	if(args[0]==NULL)
	{
		return 1;
	}

	for(int i=0;i<5;i++)
	{
		if(strcmp(args[0],builtin_str[i])==0)
			return (*builtin_func[i])(args);
	}

	return sh_launch(args);

}

void broadcastTermination(int pid,int status){
    if (WIFEXITED(status)){
        printf("exited, status=%d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)){
        printf("killed by signal %d\n", WTERMSIG(status));
    }
    else if(WIFSTOPPED(status))
    {
        printf("stopped by signal %d\n", WSTOPSIG(status));
    }
    else if(WIFCONTINUED(status))
    {
        printf("continued\n");
    }
    delete_from_list(pid);
}

static void signalHandler(int sig){
    int pid;
    int status;
    pid=waitpid(-1,&status,WNOHANG);
    broadcastTermination(pid,status);
}


int main(int argc,char **argv)
{
	char *line;
	char **args;
	int status;
    signal(SIGCHLD,signalHandler);
	do{
		
		printf("ACM-SH > ");
		line=sh_read_line();
		args=sh_split_line(line);
		status=sh_execute(args);

		free(line);
		free(args);
	}
	while(status);

	return 0;
}
