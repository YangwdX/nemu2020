#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}
WP* new_wp(){
	WP *f, *p;
	f = free_;

	if(f==NULL) assert(0);

	free_ = free_->next;
	f->next = NULL;
	p = head;
	if(p!=NULL){
		while(p->next!=NULL){
			p=p->next;
		}
		p->next=f;
	}else{
		head=f;
	}
	return f;
}
void free_wp(WP *wp){
	WP *f,*h;
	f = free_;
	if(f!=NULL){	
		while(f->next!=NULL){
			f = f->next;
		}
		f->next=wp;
	}else{
		free_=wp;
		f=free_;
	}
	h = head;
	if(h==NULL) assert(0);
	if(head->NO==wp->NO){
		head = head->next;
	}else{
		while(h->next!=NULL&&head->next->NO!=wp->NO)
			h = h->next;
		if(h->next->NO==wp->NO){
			h->next=h->next->next;
		}else{
			assert(0);
		}
	}
	wp->next=NULL;
	wp->val=0;
	wp->b=0;
	wp->expr[0]='\0';
}
bool check_wp(){
	WP *f;
	f = head;
	bool key = true;
	while(f!=NULL){
		bool success;
		uint32_t new_val = expr(f->expr,&success);
		if(!success) assert(1);
		if(new_val != f->val){
			key = false;
			/*if(f->b){
				printf("Hit breakpoint %d at 0x%08x\n",f-b,cpu.eip);
				f = f->next;
				continue;
			}*/
			printf("Watchpoint%d: %s has changed\n",f->NO,f->expr);
			printf("Old value is %d\nNew value is %d\n",f->val,new_val);
			f->val = new_val;
		}
		f = f->next;
	}
	return key;
}
void delete_wp(int num){
	WP *f;
	f = &wp_pool[num];
	free_wp(f);
}
void info_wp(){
	WP *f;
	f = head;
	while(f!=NULL){
		printf("Watchpoint%d: %s = %d\n",f->NO,f->expr,f->val);
		f = f->next;
	}
}
/* TODO: Implement the functionality of watchpoint */


