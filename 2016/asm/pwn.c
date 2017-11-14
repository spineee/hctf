#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
//define regist and datasegment
#define R0	0x1
#define R1	0x2
#define R2 	0x3
#define SP 	0x4
#define PC 	0x5
#define HEX 	0x6
#define	DATA	0x7
//define operation
#define PUSH 	0x1
#define POP 	0x2
#define MOV	0x3
#define LEA 	0x4
#define JMP 	0x5
#define CALL 	0x6
#define TEST 	0x7
#define JZ 	0x8
#define JG 	0x9
#define JB 	0xa
#define SUB 	0xb
#define ADD 	0xc
#define MUL 	0xd
#define DIV 	0xe
/*declear the stack and regs*/
int r0, r1, r2, proc_sp, offset;
unsigned char* sp, *pc;
unsigned char* stack, *binary_start, *binary_end, *data;
int flag;
void resolve_func(int count);
void end(void);

int inrange(char* pos, char* start, char* end) {
	return pos >= start&&pos <= end;
}
void err_msg(const char* msg) {
	fprintf(stderr, "Error: %s\n", msg);
	exit(EXIT_FAILURE);
}

void get_operand(int dft, int count, int** operand_list) {
	int i, j, *hex;
	if (dft != count) {
		err_msg("operand count unmatched");
	}
	hex = (int*)(pc + (count - 1) / 2 + 2);
	for (i = 0;i < count;i++) {
		switch (((*(pc + (i / 2) + 1))&((0xf) << (((i % 2) ^ 1) * 4))) >> (((i % 2) ^ 1) * 4)) {
		case R0:
			operand_list[i] = &r0;
			break;
		case R1:
			operand_list[i] = &r1;
			break;
		case R2:
			operand_list[i] = &r2;
			break;
		case SP:
			operand_list[i] = (int*)&sp;
			break;
		case PC:
			operand_list[i] = (int*)&pc;
			break;
		case HEX:
			operand_list[i] = hex;
			hex++;
			break;
		case DATA:
			if (offset != 0) {
				operand_list[i] = (int*)&(data);
			}
			else {
				err_msg("data segment undefined");
			}
			break;
		default:
			err_msg("undefined type\n");
		}
	}
	pc = (char*)hex;
}

void load_exe_file() {
	int len = 0x10000;
	/*FILE* fp = fopen(path,"r");
	fseek(fp,0,SEEK_END);
	len = ftell(fp);
	*/
	binary_start = (char *)malloc(len + 1);
	len = read(0, binary_start, len);
	/*fseek(fp,0,SEEK_SET);
	fread(binary_start,len,1,fp);
	fclose(fp);
	*/
	binary_end = binary_start + len - 1;

	//check magic head 
	//fk strcpy...
	if (!strncmp(binary_start, "hfex", 4)) {
		err_msg("Not supported file type");
	}

	//init pc
	offset = *(int*)(binary_start + 4);
	data = binary_start + 8;
	pc = binary_start + offset + 8;
}


void exe_single(void) {
	unsigned char operate, type;
	int count;
	int* operand_list[4] = { 0 };//max operand count is 3 
	 //check pc in code seg range
	if (!inrange(pc, binary_start + offset, binary_end)) {
		if (pc >= binary_end + 1) {
			end();
			printf("END\n");
			exit(0);
		}
		err_msg("pc illegal");
	}
	
	//get operate
	operate = *(pc) & 0xf;
	count = (*pc >> 4) & 0xf;
	switch (operate) {
	case PUSH:
		get_operand(1, count, operand_list);
		sp -= 4;
		if (sp <= stack - 4) {
			err_msg("sp out of range");
		}

		*(int *)sp = *operand_list[0];
		break;
	case POP:
		get_operand(1, count, operand_list);
		if (sp >= stack + 0x10000 - 4) {
			err_msg("sp out of range");
		}
		*operand_list[0] = *(int*)(sp);
		sp += 4;
		break;
	case MOV:
		get_operand(2, count, operand_list);
		*operand_list[0] = *operand_list[1];
		break;
	case LEA:
		get_operand(2, count, operand_list);
		*operand_list[0] = (int)operand_list[1];
		break;
	case JMP:
		get_operand(1, count, operand_list);
		pc += *operand_list[0];
		break;
	case CALL:
		resolve_func(count);
		break;
	case TEST:
		get_operand(2, count, operand_list);
		flag = *operand_list[0] - *operand_list[1];
		break;
	case JZ:
		get_operand(1, count, operand_list);
		if (flag == 0) {
			pc += *operand_list[0];
		}
		break;
	case JG:
		get_operand(1, count, operand_list);
		if (flag > 0) {
			pc += *operand_list[0];
		}
		break;
	case JB:
		get_operand(1, count, operand_list);
		if (flag < 0) {
			pc += *operand_list[0];
		}
		break;
	case SUB:
		get_operand(3, count, operand_list);
		*operand_list[0] = *operand_list[1] - *operand_list[2];
		break;
	case ADD:
		get_operand(3, count, operand_list);
		*operand_list[0] = *operand_list[1] + *operand_list[2];
		break;
	case MUL:
		get_operand(3, count, operand_list);
		*operand_list[0] = *operand_list[1] * *operand_list[2];
		break;
	case DIV:
		get_operand(3, count, operand_list);
		*operand_list[0] = *operand_list[1] / *operand_list[2];
		break;
	default:
		err_msg("operate type unknow");
	}


}


void exe() {
	while (1) {
		exe_single();
	}
}

void resolve_func(int count) {
	int i;
	char func_name[9];
	void(*func_ptr)();
	for (i = 0;i < count; i++) {
		pc += 1;
		func_name[i] = *pc;
	}
	func_name[i] = 0;
	pc++;
	/*
	if(strcmp(func_name,"puts")){
	err_msg("only puts supported :)");
	}
	*/
	func_ptr = dlsym(RTLD_DEFAULT, (const char*)func_name);
	if (!func_ptr) {
		err_msg(dlerror());
	}
	__asm __volatile(
	"movl %%esp, %0\n"
		"movl %2,%%esp\n"
		"movl %3,%%eax\n"
		"call %%eax\n"
		"movl %4,%%esp\n"
		"movl %%eax,%1\n" :"=m"(proc_sp), "=m"(r0) : "m"(sp), "m"(func_ptr), "m"(proc_sp)
		);
}


void init() {
	stack = (char*)malloc(0x10000);
	sp = stack + 0x10000 - 4;
	load_exe_file();
}


void end(void) {
	free(stack);
	free(binary_start);
}

int main(int argc, char* argv[]) {
	init();
	exe();
	end();
}
