#include <stdio.h>
#include <stdlib.h>


struct diary_table{
	char title[11];
	char flag;
	int content_length;
	char* content;
};

struct diary_table* ptr[5];
int day;

void* safe_malloc(int size){
	void* ptr = malloc(size);
	if(!ptr){
		printf("error\n");
		exit(0);
	}
	return ptr;
}

int read_int(){
	char buffer[21];
	int size = read(0,buffer,20);
	buffer[size] = 0;
	if(atoi(buffer) < 0){
		return 0;
	}
	else{
		return atoi(buffer);
	}
}

void init(){
	setvbuf(stdout,0,_IONBF,0);
	setvbuf(stdin,0,_IONBF,0);
	setvbuf(stderr,0,_IONBF,0);
	day = 0;
}

void menu(){
	puts("----Welcome to Diary Management----");
	puts("*write your secret here*");
	puts("1:create new diary");
	puts("2.edit the diary");
	puts("3.show the diary");
	puts("4.delete the diary");
	puts("5.exit");
	puts("-----------------------------------");
	printf("Your Choice: ");
}

void show(){
	puts("todo...");
	return;
}

void new(){
	if(day >= 5){
		printf("no more space\n");
		return;
	}
	printf("The %d day:",day);
	ptr[day] = (struct diary_table*)safe_malloc(sizeof(struct diary_table));
	printf("content size(0-100000): ");
	unsigned int size = read_int();
	if(size > 100000){
		printf("size too big\n");
		return;
	}
	printf("title(max 11): ");
	read(0,ptr[day]->title,11);
	ptr[day]->content_length = size;
	ptr[day]->content = (char*)safe_malloc(size);
	printf("content: ");
	read(0,ptr[day]->content,size);
	ptr[day]->flag = 0;
	day++;
	puts("create success.");
}

int find_max_day(){
	int i;
	for(i=0;i<5&&ptr[i]!=0;i++){}
	return i-1;
}

void edit(){
	int day,offset,size,max_day;
	printf("day: ");
	day = read_int();
	max_day = find_max_day();
	if(day > max_day){
		puts("out of range");
		return;
	}
	if(ptr[day]->flag&1){
		puts("can not edit");
		return;
	}

	printf("content: ");
	read(0,ptr[day]->content,ptr[day]->content_length);

	ptr[day]->flag = 1;
	puts("edit success.");
}

void delete(){
	int day,max_day;
	printf("day: ");
	day = read_int();
	max_day = find_max_day();
	if(day > max_day){
		puts("out of range");
		return;
	}
	free(ptr[day]->content);
	free(ptr[day]);
	puts("delete success.");
}

int main(void){
	init();
	while(1){
		menu();
		printf("$");
		switch(read_int()){
		case 1:
			new();
			break;
		case 2:
			edit();
			break;
		case 3:
			show();
			break;
		case 4:
			delete();
			break;
		case 5:
			puts("bye");
			exit(0);
		default:
			puts("invalid choice");
		}
	}
}
