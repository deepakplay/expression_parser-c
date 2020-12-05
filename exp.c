#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef enum types{
	VAL=1, ADD, SUB, MUL, DIV, OPEN_B, CLOSE_B, PWR
}n_type;

typedef struct nodes{
	n_type type;
	double data;
	struct nodes *prev;
	struct nodes *next;
}node;

typedef struct s_node{
	double data;
	struct s_node *next;
}i_node;

node *create(){
	node *new=(node *)calloc(1,sizeof(node));
	if(new==NULL){
		printf("Memory Error!!\n");
		exit(1);
	}
	new->prev=NULL;
	new->next=NULL;
	return new;
}

i_node *new_node(double data){
	i_node *stack=(i_node *)calloc(1,sizeof(i_node));
	if(stack==NULL){
		printf("Memory Error!!\n");
		exit(1);
	}
	stack->data=data;
	stack->next=NULL;
	return stack;
}

/*long int toint(char *str){
	long int num=0;
	for(int i=0;str[i]!='\0';i++)num=(str[i]-'0')+num*10;
	return num;
}*/

node *lexer(char *expn){
	int j=0,fcnt=0;
	node *first;
	node *current;
	char tmp[50];
	first = create();
	current = first;
	
	for(int i=0;expn[i]!='\0';i++){
		if(isdigit(expn[i])){
			current->type=VAL;
			for(j=0,fcnt=0;isdigit(expn[i])||expn[i]=='.';i++,j++) {
				if(expn[i]=='.')fcnt++;
				if(fcnt>1) return NULL;
				tmp[j]=expn[i];
			}
			tmp[j]='\0';
			i--;
			current->data=atof(tmp);
		}else if(expn[i]=='+')
			current->type=ADD;
		else if(expn[i]=='-')
			current->type=SUB;
		else if(expn[i]=='*')
			current->type=MUL;
		else if(expn[i]=='/')
			current->type=DIV;
		else if(expn[i]=='^')
			current->type=PWR;
		else if(expn[i]=='(')
			current->type=OPEN_B;
		else if(expn[i]==')')
			current->type=CLOSE_B;

		if(expn[i+1]!='\0'){
			if(expn[i]!=' '){
				current->next=create();
				current->next->prev=current;
				current=current->next;
			}
		}
	}
	return first;
}

int priority(n_type type){
	if(type==ADD||type==SUB)
		return 1;
	else if(type==MUL||type==DIV)
		return 2;
	else if(type==PWR)
		return 3;
	else 
		return -1;
}

int isoperator(n_type type){
	return	type==ADD||
			type==SUB||
			type==MUL||
			type==DIV||
			type==PWR;
}

void pushf(i_node **root,double data){
	i_node *stack=new_node(data);
	stack->next= *root;
	*root=stack;
}

double popf(i_node **root){
	double popped=0;
	if(!(*root))
		return 0;
	i_node *temp= *root;
	*root = (*root)->next;
	popped=temp->data;
	free(temp);
	return popped;
}

void pushlist(node **n_node,node *d_node){
	(*n_node)->type=d_node->type;
	if((*n_node)->type==VAL)(*n_node)->data=d_node->data;
	(*n_node)->next=create();
	(*n_node)->next->prev=*n_node;
	*n_node=(*n_node)->next;
}

node *poplist(node **n_node){
	if((*n_node)->prev==NULL) return NULL;
	node *popped=(*n_node)->prev;
	if((*n_node)->prev->prev!=NULL){
		(*n_node)->prev->prev->next=*n_node;
		(*n_node)->prev=(*n_node)->prev->prev;
	}else{
		(*n_node)->prev=NULL;
		(*n_node)->next=create();
	}
	return popped;
}

node *postfix(node *lexed){ //Shunting Yard Algorithm
	node *sstk=create();
	node *first;
	node *ostk=create();
	first=ostk;
	while(lexed){
		if(lexed->type==OPEN_B){
			pushlist(&sstk,lexed);
		}else if(lexed->type==CLOSE_B){
			while(sstk->prev!=NULL&&sstk->prev->type!=OPEN_B){
				pushlist(&ostk,poplist(&sstk));
			}
			if(sstk->prev==NULL){
				printf("  Mismatched Parenthesis\n");
				return NULL;
			}else poplist(&sstk);
		}else if(isoperator(lexed->type)){
			while(sstk->prev!=NULL && isoperator(sstk->prev->type)&&
				((lexed->type!=PWR &&
				priority(lexed->type)<=priority(sstk->prev->type))||
				(lexed->type==PWR  &&
				priority(lexed->type)<priority(sstk->prev->type)))){
					pushlist(&ostk,poplist(&sstk));
			}
			pushlist(&sstk,lexed);
		}else{
			pushlist(&ostk,lexed);
		}
		lexed=lexed->next;
	}
	
	while(sstk->prev!=NULL){
		if(!isoperator(sstk->prev->type)){
			printf("  Mismatched Parenthesis\n");
			return NULL;
		}
		pushlist(&ostk,poplist(&sstk));
	}
	return first;
}

double pwr(double x,double y){
	double num=1;
	for(int c=1;c<=y;c++)num*=x;
	return num;
}

int isvalid(char *str){
	for(int i=0;str[i]!='\0';i++){
		if( str[i]=='+'||str[i]=='-'||
			str[i]=='*'||str[i]=='/'||
			str[i]=='('||str[i]==')'||
			str[i]=='^'||str[i]==' '||
			str[i]=='\n'||str[i]=='.'||
			isdigit(str[i])){
				continue;
		}else{
			return 0;
		}
	}
	return 1;
}

double oper(double num1,double num2,n_type type){
	if(type==ADD)
		return num1+num2;
	else if(type==SUB)
		return num1-num2;
	else if(type==MUL)
		return num1*num2;
	else if(type==DIV)
		return num1/num2;
	else if(type==PWR)
		return pwr(num1,num2);
}

double evaluate(node *pstfx){
	i_node *stack=NULL;
	while(pstfx){
		if(isoperator(pstfx->type)||pstfx->type==VAL){
			if(!isoperator(pstfx->type)) {
				pushf(&stack, pstfx->data);
			}else{
				pushf(&stack, oper(popf(&stack), popf(&stack), pstfx->type));
			}
		}
		pstfx=pstfx->next;
	}
	return popf(&stack);
}

int main(){
	char str[100];
	node *expn=NULL;
	system("title Deepak Play Expression Solver");
	while(1){
		system("cls");
		printf("\n  Math Interpreter by Deepak Play :-)\n\n");
		while(1){
			printf(">> ");
			fgets(str,100,stdin);
			if(!strcmp(str,"cls\n")||!strcmp(str,"CLS\n"))
				break;
			else if(!strcmp(str,"exit\n")||!strcmp(str,"EXIT\n"))
				exit(0);
			else if(!strcmp(str,"help\n")){
				printf("\n  CLS  - Clear the Screen");
				printf("\n  EXIT - Quit the Program\n");
			}else if(!strcmp(str,"\n"));
			else{
				if(isvalid(str)){
					expn=lexer(str);
					if(!expn) printf("  Syntax Error\n\n");
					else {
						expn=postfix(expn);
						if(expn) printf(" %.2f\n\n",evaluate(expn));
					}
				}else{
					printf("  Invalid Expression\n");
				}
			}
		}
	}
	return 0;
}