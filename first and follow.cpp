#include "lexer.h"
#include <stdio.h>
#include<string.h>
#include<stdlib.h>

int terminal_count=0;
int terminalcount[1000];
int ignorefirsthash=0;
int L_iterator=0;

struct node{
    char *type;
    char *token1;
    int lineno;
    struct node *next;
    int count;
};

void swap1(struct node *a, struct node *b,struct node *terminal);

struct node* insert(struct node *head,char *typeval,char *tokenval,int lineval)
{
    struct node *new = (struct node*)malloc(sizeof(struct node));
  
    new->type = typeval;
    new->token1 = malloc(strlen(tokenval) + 1);
    strcpy(new->token1,tokenval);
    new->lineno = lineval;
   
    
    new->next = NULL;
    struct node *cur = head;
    if(head==NULL)
    {
        head = new;
        return head;
    }
    while(cur->next!=NULL)
    {
	    cur = cur->next;
    }

    cur->next = new;
    return head;
}



struct node* sorted_insert(struct node *head,char *typeval,char *tokenval,int lineval)
{
    struct node *new = (struct node*)malloc(sizeof(struct node));
 // struct node *temp = head;
    new->type = typeval;
    new->token1 = malloc(strlen(tokenval) + 1);
    strcpy(new->token1,tokenval);
    new->lineno = lineval;
   
    
    new->next = NULL;
    struct node *cur = head;
    if(head==NULL)
    {
        head = new;
        return head;
    }
    
     if (strcmp(head->token1,tokenval)<0)
    {
        new->next = head;
        head = new;
        return head;
    }
    
    while(cur->next!=NULL && strcmp(cur->next->token1,tokenval)>0)
    {
	    cur = cur->next;
	    //temp = temp->next;
    }
    new->next = cur->next;
    
    cur->next = new;
    return head;
}


void printlist(struct node *head)
{
    while(head!=NULL)
    {
        printf("%s ",head->token1);
        head = head->next;
    }
    printf("\n");
}


void printlistfirst(struct node *head,struct node *temp)
{

    if(head!=NULL)
    {
        
        printlistfirst(head->next,temp);
        if(temp==head)
        printf("%s",head->token1);
        else
        printf("%s, ",head->token1);
         
        
    }
  
   
}


void printlistnonterminal(struct node *head)
{
    if(head!=NULL)
    {
       printlist(head->next);
       printf("%d %s %s\n",head->lineno,head->type,head->token1);
        
    }

}


void printlistterminal(struct node *head,int arr[],int inc)
{
    if(head!=NULL)
    {
       printlistterminal(head->next,arr,inc+1);
       printf("%s : %d\n",head->token1,arr[inc]);
     //  printf("%dinc",inc);
       inc--; 
       
    }
//int i=0;
//while(head!=NULL && i<size){

//printf("%s %d\n",head->token1,arr[i]);
//i++;
//head = head->next;
//}
}



int Ifexistfunc(char *cur_token, struct node *non_terminal) //returns 0 if it exists
{

    while(non_terminal!=NULL)
    {
        if(strcmp(non_terminal->token1,cur_token)==0)
            return 0;
        non_terminal = non_terminal->next;
    }
    return 1;
}

int IfexistretIndex(char *cur_token, struct node *non_terminal)
{
    int index=0;
    while(non_terminal!=NULL)
    {
        if(strcmp(non_terminal->token1,cur_token)==0)
        return index;

        non_terminal = non_terminal->next;
        index++;
    }
    return -1;

}

int findcount(char *a,struct node *head){
    int count=0;
    while(head!=NULL)
    {
    //printf("%s",head->token1);
        if(strcmp(head->token1,a)==0){
            count++;
            
            }
        
        head=head->next;
    }
    return count;
}

void findterminalcount(struct node *terminal,struct node *terminal_nonfiltered)
{
    int index=0;
    while(terminal!=NULL)
    {
        // printf("%s",terminal->token1);
        int cnt  = findcount(terminal->token1,terminal_nonfiltered);
        // printf("%d",cnt);
        terminalcount[index] = cnt;
        index++;
        terminal = terminal->next;
    }

}

struct node * removeEpsilon(struct node *head){
  
  struct node *ret = (struct node *)malloc(10000);
  ret = NULL;
    while(head!=NULL)
    {   
   // printf("\nheadtoken:%s",head->token1);
     //  printf("loopchk: %s",head->token1);
           if(strcmp(head->token1,"#")!=0) 
           ret = insert(ret,"RET",head->token1,4); 
     
       head = head->next;
    }
   return ret;
    
}

int removeredundant(struct node *t,char *current_token,int line){
    while(t!=NULL)
    {
        if(strcmp(t->token1,current_token)==0 && t->lineno == line)
        return 0;
    
        t = t->next;
    }
    return 1;
}

char *getnonterminal(struct node *non_terminal,int q){
    if(q==0)
        return non_terminal->token1;
    else{
        while(q>0){
            non_terminal = non_terminal->next;
            
        q--;
        
        }
        return non_terminal->token1;
    
    }
    


}

int main(int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        printf("Error: missing argument\n");
        return 1;
    }

     task = atoi(argv[1]);
     
    int a = getToken();
    int flag=0;
    int non_terminal_count=0;
    memset(terminalcount,0,1000);
    int j;
    for(j=0;j<1000;j++)
    terminalcount[j] = 0;
    int i;
    struct node *head[1000]; //considering 10 production rules, change it later
    
    int ite;
    for(ite = 0;ite<1000;ite++)
    {
        head[ite] = (struct node*)malloc(sizeof(struct node)*100);
        head[ite] = NULL;
    }
    
   
    struct node *non_terminal = NULL;
    struct node *terminal = NULL;
    struct node *terminal_nonfiltered = NULL;
    while(a!= END_OF_FILE)
    {
	    //if(t_type == NUM)
	    
	    if(t_type==HASH)
	    {
	        flag=1;
	    }
	    
	    if(flag==0)
	    {
	       non_terminal = insert(non_terminal,"NON_TERMINAL",current_token,line); //in reverse way, need to chk from back
	       non_terminal_count++;
	    }
	    else
	    {
	      if(ignorefirsthash==1) {
	       
	            if(t_type==HASH){
	            L_iterator++;
	            }
	            else{
	           // printf(current_token);
	            head[L_iterator] = insert(head[L_iterator],"RULES",current_token,line);
	            }
	            
	        }
	        ignorefirsthash=1;
	        if(t_type!=HASH && t_type!=DOUBLEHASH && t_type!=ARROW )
	        {
	            if(Ifexistfunc(current_token,non_terminal))
	            { //compares with non terminal and filters only the terminals
	            
	                if(removeredundant(terminal_nonfiltered,current_token,line)) // find if the terminal with that lineno is already there, returns 0 if already there
                    terminal_nonfiltered= sorted_insert(terminal_nonfiltered,"TERMINAL",current_token,line); 
                    if(Ifexistfunc(current_token,terminal))
	                { //filters the unredundant terminals
	                    //printlist(terminal);
	                    //printf("ct:%s",current_token);
	                     terminal= sorted_insert(terminal,"TERMINAL",current_token,line); //make it to sorted insert - done; straight way - not in reverse
	                    //printlist(terminal);
	
	                    terminal_count++;
	                }
	                //printf("%d",terminal_count);
	
	                /*int index = IfexistretIndex(current_token,terminal);
	                if(index==-1)
	                printf("ERROR");
	                else{
	                terminalcount[index] = terminalcount[index]+1; // put memset to terminalcount arr
	
	                //printf("%d\n",terminalcount[index]);
	                }*/
	
	            }
	        }
	                      //  printf("%d",line);
	                      // printf("%d",a);
	                       // printf("%s\n",current_token);
	        
        }
	    a = getToken();
	}
	
	
	
    
    
    
    int iterator;
    findterminalcount(terminal,terminal_nonfiltered); //function to calculate the count of terminal 
    
    
   struct node *first[non_terminal_count];
    for(iterator = 0;iterator<non_terminal_count;iterator++)
    {
        first[iterator] = (struct node*)malloc(10000);
        first[iterator] = NULL;
    }
    
  /*   int q;
            for(q=0;q<terminal_count;q++)
            printf("%d",terminalcount[q]);*/
           // printf("%d",non_terminal_count);
          // printf("%d",L_iterator);
          
          
    //printlist(non_terminal);      
          
    int q;
 //   for(q=0;q<L_iterator;q++)
  //  {    
       // printf("\n");
       // printlist(head[q]);  //List of production rules
  //  }
    int changesmade=1;
    while(changesmade){
    //  printf("WHILE");
    changesmade=0;
    for(q=0;q<L_iterator;q++)  //Apply first rules
    {  
   
           struct node *runTemp = head[q]; //points to head
           struct node *before = runTemp; //points to left of ->
           struct node *after = NULL;
            
           if(runTemp->next->next!=NULL)
                after = runTemp->next->next; // points to right of ->
       
            
             
             //printf("\n BEFORE:%s",before->token1);
            //if(after!=NULL) 
            //printf("\n AFTER:%s",after->token1);
             
            //CALCULATING FIRST SET 
            
                 
            //apply rules one by one
            //RULE1
            if(after!=NULL&& !Ifexistfunc(after->token1,terminal) && Ifexistfunc(after->token1,first[IfexistretIndex(before->token1,non_terminal)])){   //returns 0 if it exists , second condition is to check if the value is already there in the first set, if not already there go inside --  RULE1
            
                    first[IfexistretIndex(before->token1,non_terminal)] = sorted_insert(first[IfexistretIndex(before->token1,non_terminal)],"FIRST",after->token1,q+1); //get the index of the non_terminal and add the first set to that index of first
                    changesmade=1;
            }
            
            //RULE2
            
            if(after==NULL && Ifexistfunc("#",first[IfexistretIndex(before->token1,non_terminal)])){
                    first[IfexistretIndex(before->token1,non_terminal)] = sorted_insert(first[IfexistretIndex(before->token1,non_terminal)],"FIRST","#",q+1);  //RULE2 if its null (epsilon) , add null to first
                    changesmade=1;
            }
          
          //RULE3
          
           if(after!=NULL && !Ifexistfunc(after->token1,non_terminal) && strcmp(before->token1,after->token1)!=0)
            {
            
           // printf("RULE3");
            
          //  int val1 = IfexistretIndex(after->token1,non_terminal);
           // printf("INSIDE: %d",val1);
            //printlist(first[0]);
           //first[IfexistretIndex(before->token1,non_terminal)]
           
               struct node *retvalue = NULL;
               if(first[IfexistretIndex(after->token1,non_terminal)]!=NULL ) 
               {// if first set of B in A-B is not null go inside
                    int val1 = IfexistretIndex(after->token1,non_terminal);
               //    printf("\nval1:%d",val1);
                    //printf("HOLLA");
                  //  printlist(first[val1]);
                    if(!Ifexistfunc("#",first[val1]))
                    {
              //        printf("IFBLOCK");
                        retvalue = removeEpsilon(first[val1]); 
                    }//RULE3  A-Bx add FIRST(B)-epsilon
                    else
                    {
                        retvalue = first[val1];
             //         printf("ELSE");
                    }
               }
              //      struct node *a = retvalue;
                //            while(a!=NULL){
                 //             printf("ret:%s",a->token1);
                  //          a=a->next;
                   //         }
                while(retvalue!=NULL)
                { // goes inside only when the retvalue token is not already there in the first set
               
                    //  printf("\nINSERT:%s",retvalue->token1);
                    if(Ifexistfunc(retvalue->token1,first[IfexistretIndex(before->token1,non_terminal)]))
                    {
                        first[IfexistretIndex(before->token1,non_terminal)] = sorted_insert(first[IfexistretIndex(before->token1,non_terminal)],"FIRST",retvalue->token1,q+1);
                        changesmade=1;
                    }
                        //printf("\n%s retval:",retvalue->token1);
                        retvalue = retvalue->next;
                }
            }
            
        //RULE 4
         if(after!=NULL && !Ifexistfunc(after->token1,non_terminal) && !Ifexistfunc("#",first[IfexistretIndex(after->token1,non_terminal)])){
         struct node *aftertmp = after;
            //RULE 4 if epsilon is present in after, add first(after-next)-epsilon and goes on
           int val1 = IfexistretIndex(aftertmp->token1,non_terminal);
           
               while(aftertmp->next!=NULL && !Ifexistfunc(aftertmp->token1,non_terminal) && !Ifexistfunc("#",first[IfexistretIndex(aftertmp->token1,non_terminal)]) ) //get the index of the non terminal and chk the first set if epsilon(#) exists
                {
             
                     struct node *retvalue = NULL;
                    if(!Ifexistfunc(aftertmp->next->token1,non_terminal) && first[IfexistretIndex(aftertmp->next->token1,non_terminal)]!=NULL){ // new condition added to rule out the terminals being in aftertmp->next
                        retvalue = removeEpsilon(first[IfexistretIndex(aftertmp->next->token1,non_terminal)]);  
                    }
                    else if(!Ifexistfunc(aftertmp->next->token1,terminal) && Ifexistfunc(aftertmp->next->token1,first[IfexistretIndex(before->token1,non_terminal)])) 
                    {
                    
                     //printf("\nTERMI:%s",aftertmp->next->token1);
                    // int bfr = IfexistretIndex(before->token1,non_terminal);
                    // printf("\nbfr: %d",bfr);
                     
                        first[IfexistretIndex(before->token1,non_terminal)] = sorted_insert(first[IfexistretIndex(before->token1,non_terminal)],"FIRST",aftertmp->next->token1,q+1);
                        changesmade=1;
                   //printlist(first[IfexistretIndex(before->token1,non_terminal)]);
                    }
                   
                     
                    while(retvalue!=NULL && Ifexistfunc(retvalue->token1,first[IfexistretIndex(before->token1,non_terminal)])){ // goes inside only when the retvalue token is not already there in the first set
                    first[IfexistretIndex(before->token1,non_terminal)] = sorted_insert(first[IfexistretIndex(before->token1,non_terminal)],"FIRST",retvalue->token1,q+1);
                    changesmade=1;
                    //printf("\n%s retval:",retvalue->token1);
                     retvalue = retvalue->next;
                     }
                      //first[IfexistretIndex(before->token1,non_terminal)]
                     aftertmp = aftertmp->next;    
                }
            }
            
           //RULE 5
            
          if(after!=NULL && !Ifexistfunc(after->token1,non_terminal) && !Ifexistfunc("#",first[IfexistretIndex(after->token1,non_terminal)])){
            struct node *aftertmp = after;
            //RULE 5 if epsilon is present in all the after non terminals, add epsilon(#) to the set
                while(aftertmp!=NULL &&  !Ifexistfunc(aftertmp->token1,non_terminal) && !Ifexistfunc("#",first[IfexistretIndex(aftertmp->token1,non_terminal)]) ) //get the index of the non terminal and chk the first set if epsilon(#) exists
                {
                     aftertmp = aftertmp->next; 
                     if(aftertmp == NULL && Ifexistfunc("#",first[IfexistretIndex(before->token1,non_terminal)]))
                     {
                     //printf("CASE5");
                     first[IfexistretIndex(before->token1,non_terminal)] =     sorted_insert(first[IfexistretIndex(before->token1,non_terminal)],"FIRST","#",q+1);
                     changesmade=1;
                     }   
                }
            
            }
            
            
            
            
            

    
    }
         
    
       
   }
   
   struct node *follow[non_terminal_count];
    for(iterator = 0;iterator<non_terminal_count;iterator++)
    {
        follow[iterator] = (struct node*)malloc(10000);
        follow[iterator] = NULL;
    }
   
   
   //FOLLOW SETS BEGINS
   int followchange = 1;
  while(followchange){
    //  printf("WHILE");
    followchange=0;
    struct node *non_t = non_terminal;
    
    while(non_t != NULL){
    for(q=0;q<L_iterator;q++)  //Apply first rules
    {  
    
           struct node *runTemp1 = head[q]; //points to head
           struct node *before1 = runTemp1; //points to left of ->
           struct node *after1 = NULL;
            
           if(runTemp1->next->next!=NULL)
                after1 = runTemp1->next->next; // points to right of ->
                
           if(!Ifexistfunc(non_t->token1,runTemp1->next)){   
                //printf("NONTERM: %s", non_t->token1);
                //printlist(head[q]);
    
                    //RULE1
                    
                    if(strcmp(before1->token1,non_terminal->token1) == 0 && Ifexistfunc("$",follow[IfexistretIndex(before1->token1,non_terminal)])) //if $ is not there in follow of Starting
                    {
                        follow[IfexistretIndex(before1->token1,non_terminal)] = sorted_insert(follow[IfexistretIndex(before1->token1,non_terminal)],"FOLLOW","$",q+1); //get the index of the non_terminal and add the first set to that index of first
                                    followchange=1;   
                    }
                    
                    //RULE 2
                    struct node *last_value = head[q];
                    while(last_value->next!=NULL)
                    {
                    last_value = last_value->next;
                    }
                    
                    if(!Ifexistfunc(last_value->token1,non_terminal)) //if all values exists, return 0
                    {
                        //printf("EXISTS: %s",last_value->token1);
                        struct node *retvalue = follow[IfexistretIndex(before1->token1,non_terminal)];
                        while(retvalue!=NULL)
                                { // goes inside only when the retvalue token is not already there in the first set
                               
                                    //  printf("\nINSERT:%s",retvalue->token1);
                                    if(Ifexistfunc(retvalue->token1,follow[IfexistretIndex(last_value->token1,non_terminal)]))
                                    {
                                        follow[IfexistretIndex(last_value->token1,non_terminal)] = sorted_insert(follow[IfexistretIndex(last_value->token1,non_terminal)],"FOLLOW",retvalue->token1,q+1);
                                        followchange=1;
                                    }
                                        //printf("\n%s retval:",retvalue->token1);
                                        retvalue = retvalue->next;
                                }
                        
                        
                    }
                    
                    
                    //RULE 3
                    
                    struct node *Aptr = head[q]->next;
                    while(strcmp(Aptr->token1,non_t->token1)!=0)
                    {
                        Aptr = Aptr->next;
                    }
                    struct node *Addfollow = Aptr; // The ptr to which follow needs to be added
                    //printf("NONTERM:%s \t APTR:%s",non_t->token1,Aptr->token1);
                    Aptr=Aptr->next; //moving next to the match  C0 of B->@AC0C1
                    while(Aptr!=NULL && !Ifexistfunc(Aptr->token1,non_terminal) && !Ifexistfunc("#",first[IfexistretIndex(Aptr->token1,non_terminal)])) //
                    {
                            if(Aptr->next == NULL)
                            {
                                 struct node *retvalue = follow[IfexistretIndex(before1->token1,non_terminal)];
                                while(retvalue!=NULL)
                                { // goes inside only when the retvalue token is not already there in the first set
                               
                                    //  printf("\nINSERT:%s",retvalue->token1);
                                    if(Ifexistfunc(retvalue->token1,follow[IfexistretIndex(Addfollow->token1,non_terminal)]))
                                    {
                                        follow[IfexistretIndex(Addfollow->token1,non_terminal)] = sorted_insert(follow[IfexistretIndex(Addfollow->token1,non_terminal)],"FOLLOW",retvalue->token1,q+1);
                                        followchange=1;
                                    }
                                        //printf("\n%s retval:",retvalue->token1);
                                        retvalue = retvalue->next;
                                }   
                            }
                            Aptr = Aptr->next;
                       
                    }
                    
                    
                    //RULE 4
                    
                    struct node *Aptrloop = head[q]->next;
                     struct node *Aptr1 = NULL;
                      struct node *Addf;
                    while(Aptrloop!=NULL){
                    if(strcmp(Aptrloop->token1,non_t->token1)==0)
                    {
                       
                        Addf = Aptrloop; 
                    Aptr1 = Aptrloop->next;
                    }
                    
                    
                 //  printf("\n TERM: %s \t APTR: %s", non_t->token1, Aptr1->token1);
                    
                 
                 
                  
                 struct node *retvalue = NULL;
                    if(Aptr1!= NULL && !Ifexistfunc(Aptr1->token1,non_terminal) && first[IfexistretIndex(Aptr1->token1,non_terminal)]!=NULL){ // new condition added to rule out the terminals being in aftertmp->next
                        retvalue = removeEpsilon(first[IfexistretIndex(Aptr1->token1,non_terminal)]);  
                    }
                    else if(Aptr1!= NULL && !Ifexistfunc(Aptr1->token1,terminal) && Ifexistfunc(Aptr1->token1,follow[IfexistretIndex(Addf->token1,non_terminal)]))  // if Aptr1 is terminal and Aptr1's token not exists in follow of A in B->@AC0C1
                    {
                    
                     //printf("\nTERMI:%s",aftertmp->next->token1);
                    // int bfr = IfexistretIndex(before->token1,non_terminal);
                  //  printf("\nbfr: %s",Addf->token1);
                     
                        follow[IfexistretIndex(Addf->token1,non_terminal)] = sorted_insert(follow[IfexistretIndex(Addf->token1,non_terminal)],"FOLLOW",Aptr1->token1,q+1);
                        followchange=1;
                   //printlist(first[IfexistretIndex(before->token1,non_terminal)]);
                    }
                   
                     
                    while(retvalue!=NULL && Ifexistfunc(retvalue->token1,follow[IfexistretIndex(Addf->token1,non_terminal)])){ // goes inside only when the retvalue token is not already there in the first set
                    follow[IfexistretIndex(Addf->token1,non_terminal)] = sorted_insert(follow[IfexistretIndex(Addf->token1,non_terminal)],"FOLLOW",retvalue->token1,q+1);
                    followchange=1;
                    //printf("\n%s retval:",retvalue->token1);
                     retvalue = retvalue->next;
                     }
                 
                 Aptrloop = Aptrloop->next;   
                }    
                    
                    
                    //RULE 5
                    {
                     struct node *Aptr1  = NULL;
                     Aptr1 = head[q]->next;
                    while(strcmp(Aptr1->token1,non_t->token1)!=0)
                    {
                        Aptr1 = Aptr1->next;
                    }
                    struct node *Addf = NULL;
                    Addf = Aptr1;
                    
                    Aptr1 = Aptr1->next;
                    
                    
                    
                    while(Aptr1!=NULL && Aptr1->next!=NULL && !Ifexistfunc(Aptr1->token1,non_terminal) && !Ifexistfunc("#",first[IfexistretIndex(Aptr1->token1,non_terminal)])) //
                    {
                            
                            struct node *retvalue = NULL;
                    if(!Ifexistfunc(Aptr1->next->token1,non_terminal) && first[IfexistretIndex(Aptr1->next->token1,non_terminal)]!=NULL){ // new condition added to rule out the terminals being in aftertmp->next
                        retvalue = removeEpsilon(first[IfexistretIndex(Aptr1->next->token1,non_terminal)]);  
                    }
                    else if(!Ifexistfunc(Aptr1->next->token1,terminal) && Ifexistfunc(Aptr1->next->token1,follow[IfexistretIndex(Addf->token1,non_terminal)]))  // if Aptr1 is terminal and Aptr1's token not exists in follow of A in B->@AC0C1
                    {
                    
                     //printf("\nTERMI:%s",aftertmp->next->token1);
                    // int bfr = IfexistretIndex(before->token1,non_terminal);
                  //  printf("\nbfr: %s",Addf->token1);
                     
                        follow[IfexistretIndex(Addf->token1,non_terminal)] = sorted_insert(follow[IfexistretIndex(Addf->token1,non_terminal)],"FOLLOW",Aptr1->next->token1,q+1);
                        followchange=1;
                   //printlist(first[IfexistretIndex(before->token1,non_terminal)]);
                    }
                   
                     
                    while(retvalue!=NULL && Ifexistfunc(retvalue->token1,follow[IfexistretIndex(Addf->token1,non_terminal)])){ // goes inside only when the retvalue token is not already there in the first set
                    follow[IfexistretIndex(Addf->token1,non_terminal)] = sorted_insert(follow[IfexistretIndex(Addf->token1,non_terminal)],"FOLLOW",retvalue->token1,q+1);
                    followchange=1;
                    //printf("\n%s retval:",retvalue->token1);
                     retvalue = retvalue->next;
                     }
                    Aptr1 = Aptr1->next;
                    }
                    
                   } 
                    
                    
                    
              }
                    
    }
    non_t = non_t->next;
    }
    
    
    
    }
   
   
   switch (task) {
        case 0:
            // TODO: Output information about the input grammar
            {
            printf("\n");
            printlist(non_terminal);
            printlistterminal(terminal,terminalcount,0);	 //prints the task 0
     
            break;}

        case 1:{
        
         for(q=0;q<non_terminal_count;q++)
    {    
       // printlist(non_terminal);
       char *a = getnonterminal(non_terminal,q);
       printf("\nFIRST(%s) = ",a);
        printf("{ ");
        printlistfirst(first[q],first[q]);  //List of production rules
        printf(" }");
    }     
        
        
           
            break;}

        case 2:{
        
         for(q=0;q<non_terminal_count;q++)
    {    
       // printlist(non_terminal);
       char *a = getnonterminal(non_terminal,q);
       printf("\nFOLLOW(%s) = ",a);
        printf("{ ");
        printlistfirst(follow[q],follow[q]);  //List of production rules
        printf(" }");
    }     
        
        
           
            break;}

        default:
            printf("Error: unrecognized task number %d\n", task);
            break;
    }
    
    
    
   
      
   
   
   
   
    
  //  printlist(terminal_nonfiltered);
  //  printlist(terminal);
    
   //printlistterminal(terminal,terminalcount,0);	 //prints the task 0
    return 0;
}
