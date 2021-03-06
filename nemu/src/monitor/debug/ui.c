#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args); 
static int cmd_w(char *args);
static int cmd_d(char *args);
static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si","Single instruction", cmd_si},
	{ "info","Information",cmd_info},
	{ "x","Scan memory",cmd_x},
	{ "p", "Expression evaluation",cmd_p},
	{ "w", "Watchpoints", cmd_w},
	{ "d", "Delete watchpoints",cmd_d},
	/* TODO: Add more commands */
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))
static int cmd_d(char *args){
	int num;
	sscanf(args,"%d",&num);
	delete_wp(num);
	return 0;
}
static int cmd_w(char *args){
	WP *f;
	bool success;
	f = new_wp();
	f->val = expr(args,&success);
	if(!success) Assert(1,"Wrong\n");
	strcpy(f->expr,args);
	printf("Watchpoint%d: %s\nValue is %d\n",f->NO,f->expr,f->val);
	return 0;
}
static int cmd_p(char *args){
	uint32_t num;
	bool success;
	num = expr(args, &success);
	if(success) 
		printf("0x%x:\t%d\n",num,num);
	else
		assert(0);
	return 0;
}
static int cmd_x(char *args){
	int n;
	int i;
	bool success;
	swaddr_t addr;
	char *num = strtok(NULL," ");
	sscanf(num,"%d",&n);
	//args = num + strlen(num) + 1;
	char *address = strtok(NULL," ");
	sscanf(address,"%x",&addr);
	//addr = expr(address,&success);
	if(!success) assert(1); 
	for(i=1;i<=n;i++){
		printf("0x%08x: ",addr);
		printf("0x%08x\n",swaddr_read(addr,4));
		addr += 4;
	}
	return 0;
}
static int cmd_info(char *args){
	char *arg = strtok(NULL," ");
	int i;
	if(arg[0]=='r'){
		for(i=R_EAX;i<=R_EDI;i++){
			printf("%s\t0x%08x\n",regsl[i],reg_l(i));
		}
		printf("eip\t0x%08x\n",cpu.eip);
	}else if(arg[0]=='w'){
			info_wp();
		
	}else{
		assert(0);
	}
	return 0;
}
static int cmd_si(char *args){
	int n=1;
	if(args != NULL){
		sscanf(args,"%d",&n);
	}
	cpu_exec(n);
	return 0;
}
static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
