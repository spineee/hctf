#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE_CONT 0X100
#define MAX_RAND_CONT(oprand_list,num) if (i > num){\
	err_msg(*line,"oprand cont not matched\n");\
}


void err_msg(int line,char* msg){
	printf("error at line %d: %s\n",line+1,msg);
	exit(0);
}

char* get_line(void){
	char* buffer = malloc(MAX_LINE_CONT);
	char ch;
	int count = 0;
	while((ch=getchar()) != '\n'){
		buffer[count] = ch;
		count++;
		if(count >= MAX_LINE_CONT-1){
			break;
		}
	}
	*(buffer+count) = 0;
	return buffer;
}


int data_length;

void generate_byte_code_line(char* buffer,char* opcode,int* size,int* line){
	int i,j,len,arg_count = 0,opcode_size = 0;
	int k, format_pos = 0,ret;
	char* pos;
	char* operate;
	char* operand_list[4];
	char* data_list[100];
	char* b[] = {buffer,0};
	int format_number[3]={0};
	int tmp;
	data_length = 0;
/*
	test if the data segment
*/
	len = strlen(buffer);
	strsep(b," ");
	operate = buffer;
	//if true,get the data with loop
	if(!strcmp(operate,"data:")){
		if(*line!=0){
			err_msg(*line,"data should declear at line 1");
		}
		i = 0;
		while(1){
			buffer = get_line();
			*line += 1;
			pos = buffer;
			len = strlen(buffer);
			if(!strcmp(buffer,"end")){
				break;
			}
			b[0] = buffer;
			while(1){
				ret = strsep(b,",");
				if(ret == 0){
					break;
				}
			}

			do{
				data_list[i++] = pos;
			}while((pos += strlen(pos)+1) < buffer+len);

			data_list[i] = 0;

			for(j = 0;j < i;j++){
				if(sscanf(data_list[j],"%x",&tmp)==-1){
					err_msg(*line,"data format error");
				}
				*(int*)(opcode+4*j) = tmp;
				opcode_size+=4;
			}

			free(buffer);
		}
		data_length = opcode_size;
		*(size) = opcode_size;
		return;
	}
/*
	end
*/

/*
	read operands
*/
	pos = operate;
	while(1){
		ret = strsep(b,",");
		if(ret == 0){
			break;
		}
	}
	i = 0;
	while((pos += strlen(pos)+1) < buffer+len){
		operand_list[i++] = pos;
	}

	operand_list[i] = 0;
/*
	end
*/

/*
	if the asmcode is call funcname
*/
	if(!strcmp(operate,"call")){
		MAX_RAND_CONT(oprand_list,1);
		if(strlen(operand_list[0]) >= 16){
			err_msg(*line,"func name max length 15");
		}

		opcode[0] = 0x6^(strlen(operand_list[0])<<4);
		strncpy(opcode+1,operand_list[0],strlen(operand_list[0]));
		*size = strlen(operand_list[0])+1;
		return;
	}
/*	
*/
	for(j=0;j<i;j++){
		if(!strcmp(operand_list[j],"r0")){
			opcode[(j/2)+1] ^= 1<<(((j%2)^1)*4);
		}
		else if(!strcmp(operand_list[j],"r1")){
			opcode[(j/2)+1] ^= 2<<(((j%2)^1)*4);
		}
		else if(!strcmp(operand_list[j],"r2")){
			opcode[(j/2)+1] ^= 3<<(((j%2)^1)*4);
		}
		else if(!strcmp(operand_list[j],"sp")){
			opcode[(j/2)+1] ^= 4<<(((j%2)^1)*4);
		}
		else if(!strcmp(operand_list[j],"pc")){
			opcode[(j/2)+1] ^= 5<<(((j%2)^1)*4);
		}
		else if(!strncmp(operand_list[j],"0x",2)&&sscanf(operand_list[j],"%x",&tmp)){
			opcode[(j/2)+1] ^= 6<<(((j%2)^1)*4);
			format_number[format_pos] = tmp;
			format_pos++;
			opcode_size+=4;
		}
		else if(!strcmp(operand_list[j],"data")){
			opcode[(j/2)+1] ^= 7<<(((j%2)^1)*4);
		}
		else{
			err_msg(*line,"operand unlegal");
		}
	}

	opcode_size += ((j-1)/2)+2;

	for(k=0;k < format_pos;k++){
		*(int*)(opcode+((j+1)/2)+1+4*k) = format_number[k];
	}

	if(!strcmp(operate,"push")){
		MAX_RAND_CONT(oprand_list,1);
		opcode[0] = 0x1^(i<<4);
	}

	else if(!strcmp(operate,"pop")){
		MAX_RAND_CONT(oprand_list,1);
		opcode[0] = 0x2^(i<<4);
	}

	else if(!strcmp(operate,"mov")){
		MAX_RAND_CONT(oprand_list,2);
		opcode[0] = 0x3^(i<<4);
	}

	else if(!strcmp(operate,"lea")){
		MAX_RAND_CONT(oprand_list,2);
		opcode[0] = 0x4^(i<<4);
	}

	else if(!strcmp(operate,"jmp")){
		MAX_RAND_CONT(oprand_list,1);
		opcode[0] = 0x5^(i<<4);
	}

	else if(!strcmp(operate,"cmp")){
		MAX_RAND_CONT(oprand_list,2);
		opcode[0] = 0x7^(i<<4);
	}

	else if(!strcmp(operate,"jz")){
		MAX_RAND_CONT(oprand_list,1);
		opcode[0] = 0x8^(i<<4);
	}

	else if(!strcmp(operate,"jg")){
		MAX_RAND_CONT(oprand_list,1);
		opcode[0] = 0x9^(i<<4);
	}

	else if(!strcmp(operate,"jb")){
		MAX_RAND_CONT(oprand_list,1);
		opcode[0] = 0xa^(i<<4);
	}

	else if(!strcmp(operate,"sub")){
		MAX_RAND_CONT(oprand_list,3);
		opcode[0] = 0xb^(i<<4);
	}

	else if(!strcmp(operate,"add")){
		MAX_RAND_CONT(oprand_list,3);
		opcode[0] = 0xc^(i<<4);
	}

	else if(!strcmp(operate,"mul")){
		MAX_RAND_CONT(oprand_list,3);
		opcode[0] = 0xd^(i<<4);
	}

	else if(!strcmp(operate,"div")){
		MAX_RAND_CONT(oprand_list,3);
		opcode[0] = 0xe^(i<<4);
	}

	else {
		err_msg(*line,"no operate matched");
	}
	*size = opcode_size;
}

void get_opcode(void){
	char* b[0x100] = {0};
	unsigned char opcode[0x100];
	unsigned char result[0x10000];
	int i,j,size,total_size = 0,line = 0,entry = 0;
	result[0] = 'h';
	result[1] = 'f';
	result[2] = 'e';
	result[3] = 'x';
	*(int*)(result+4) = 0;
	total_size += 8;
	while(1){
		memset(opcode,0,0x100);
		b[i] = get_line();
		if(!strcmp(b[i],"$")){
			free(b[i]);
			break;
		}
		generate_byte_code_line(b[i],opcode,&size,&line);
		entry += data_length;
		*(int*)(result+4) = entry;

		for(j=0;j<size;j++){
			result[j+total_size] = *(opcode+j);
		}
		total_size += size;
		free(b[i]);
		line++;
	}
	for(j=0;j<total_size;j++){
		printf("%c",*(result+j));
	}
}


int main(void){
	printf("give me your code, end with a single line of '$'\n");
	fflush(stdout);
	get_opcode();
	return 0;
}
