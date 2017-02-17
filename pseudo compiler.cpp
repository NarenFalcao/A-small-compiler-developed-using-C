#include "compiler.h"





#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>

#include <string.h>

#include <ctype.h>

#include <assert.h>

#define ALLOC(t) (t*) calloc(1, sizeof(t))



void printstatements(struct StatementNode *head);

//struct ValueNode *MasterList;

struct node *MasterList;

struct StatementNode *no_op;

void printvariables(struct node* head);

struct node{

    struct ValueNode *data;

    struct node *next;

};





char str[256];

int strindex=0;

char numiterator='a';

void printassign(struct AssignmentStatement *head);

struct StatementNode* parse_stmt();



struct StatementNode* parse_stmt_list();



struct node* insert_node(struct node *head,char *variable1,int value1)

{

    struct node *new_node;

    

    new_node = (struct node *)malloc(sizeof(struct node));

    struct ValueNode *new = (struct ValueNode*)malloc(sizeof(struct ValueNode));

  

    new->value= value1;

    new->name = malloc(strlen(variable1) + 1);

    strcpy(new->name,variable1);

    

    

    new_node->data = new;

    

    new_node->next = NULL;

    struct node *cur = head;

    if(head==NULL)

    {

        head = new_node;

        return head;

    }

    while(cur->next!=NULL)

    {

	    cur = cur->next;

    }



    cur->next = new_node;

    return head;

}





struct ValueNode* search_node(char *var)

{

    struct node *temp = MasterList;

    while(strcmp(temp->data->name,var)!=0){

        temp = temp->next;

    }

    

   return temp->data;

}



int search_value(struct ValueNode *curr)

{

    struct node *temp = MasterList;

    while(strcmp(temp->data->name,curr->name)!=0){

        temp = temp->next;

    }

    

   return temp->data->value;

}









struct AssignmentStatement* assign_stmt()

{

   struct AssignmentStatement * assignStmt;

   

    ttype = getToken();

    if (ttype == ID)

    {

        assignStmt = ALLOC(struct AssignmentStatement);
        
        
         struct ValueNode *temp =  search_node(strdup(token)); 
            struct ValueNode *new_node = (struct ValueNode *)malloc(1000);
            new_node->name = (char *)malloc(1000);
            strcpy(new_node->name,temp->name);
            new_node->value = temp->value;
        
        

        assignStmt->left_hand_side = new_node;

        ttype = getToken();

        if (ttype == EQUAL)

        {

            ttype = getToken();

            if (ttype == ID || ttype == NUM)

            {

            

                if(ttype == ID)

                {
                 struct ValueNode *temp =  search_node(strdup(token)); 
            struct ValueNode *new_node = (struct ValueNode *)malloc(1000);
            new_node->name = (char *)malloc(1000);
            strcpy(new_node->name,temp->name);
            new_node->value = temp->value;
            

                    assignStmt->operand1 = new_node;

                }

                else if (ttype == NUM)

                {

                    char *buff = (char *)malloc(1000);

                    char *var_name = "noname";

                    char *cat = &numiterator;

                    strcat(buff,var_name);

                    strcat(buff,cat);

                    numiterator++;

                    insert_node(MasterList,buff,atoi(token));
                    
                     struct ValueNode *temp =  search_node(buff); 
            struct ValueNode *new_node = (struct ValueNode *)malloc(1000);
            new_node->name = (char *)malloc(1000);
            strcpy(new_node->name,temp->name);
            new_node->value = temp->value;
                    

                    assignStmt->operand1 = new_node;

                }

            

               ttype = getToken();

                if (ttype == SEMICOLON)

                {

                    assignStmt->op = 0;

                }

                else if( ttype == PLUS || ttype == MINUS || ttype == MULT || ttype == DIV)

                {

                

                    if(ttype == PLUS)

                    {

                        assignStmt->op = PLUS;

                    }

                    else if(ttype == MINUS)

                    {

                        assignStmt->op = MINUS;

                    }

                    else if(ttype == MULT)

                    {

                        assignStmt->op = MULT;

                    }

                    else if(ttype == DIV)

                    {

                        assignStmt->op = DIV;

                    }  

                    

                    ttype = getToken();

                    if (ttype == ID || ttype == NUM)

                    {

                    

                         if(ttype == ID)

                        {
                         struct ValueNode *temp =  search_node(strdup(token)); 
            struct ValueNode *new_node = (struct ValueNode *)malloc(1000);
            new_node->name = (char *)malloc(1000);
            strcpy(new_node->name,temp->name);
            new_node->value = temp->value;
            
            

                             assignStmt->operand2 = new_node;

                        }

                        else if (ttype == NUM)

                        {

                            char *buff = (char *)malloc(1000);

                            char *var_name = "noname";

                            char *cat = &numiterator;

                            strcat(buff,var_name);

                            strcat(buff,cat);

                            numiterator++;

                            insert_node(MasterList,buff,atoi(token));
                            
                             struct ValueNode *temp =  search_node(buff); 
            struct ValueNode *new_node = (struct ValueNode *)malloc(1000);
            new_node->name = (char *)malloc(1000);
            strcpy(new_node->name,temp->name);
            new_node->value = temp->value;
                            

                            assignStmt->operand2 = new_node;

                        }

                    

                    ttype = getToken();

                if (ttype == SEMICOLON)

                {

                    

                }

                    

                    }

                    

                 }

            }

        }

   }

return assignStmt;

}



void parse_variables()

{

    ttype = getToken();

    while(ttype != SEMICOLON)

    {

        if(ttype!=COMMA){

            MasterList = insert_node(MasterList,strdup(token),0);  
          
              
              

         }

        ttype = getToken();

    }





}



struct StatementNode* parse_body()

{

    struct StatementNode* stl;

    stl = ALLOC(struct StatementNode);

    ttype = getToken();

    if(ttype == LBRACE)

    {

        stl = parse_stmt_list();

       // printf("hell:%s",stl->assign_stmt->left_hand_side->name);

       

       

    }

    ttype = getToken();

    if(ttype == LBRACE)

    {

    

    }

    return stl;

}



struct StatementNode* parse_stmt_list()

{

    struct StatementNode* st;   // statement

    struct StatementNode* stl;  // statement list



    st = ALLOC(struct StatementNode);

    stl = ALLOC(struct StatementNode);

    

    st = parse_stmt();

    

    ttype = getToken();

    // printf("\nst->type;%d",st->type);

    //printf("\nCHECK:%d",ttype);

    if (ttype == ID || ttype == PRINT || ttype == WHILE || ttype == IF || ttype == SWITCH)

    {  

   

        ungetToken();

       stl = parse_stmt_list();
      

    //  printf("\nAFTeR:%d",st->type);

        if(st->type == IF_STMT)

        {

       //    printf("\nIFCHK\n");

                

                struct StatementNode* temp = st;

                temp = temp->next; //= no_op; //this is for no_op movement
              //  printf("TY:%d",temp->type);

                temp->next = stl;

                //st->if_stmt->false_branch = stl; //not sure if correct
                 //printf("TY:%d",temp->next->type);
                 

        }

        else

        st->next = stl;            

        
        

        return st;

    }

    else 

    {

        ungetToken();

        //printf("%s",st->assign_stmt->left_hand_side->name);

    

        return st;

    }

}

















struct StatementNode* parse_stmt()

{

    struct StatementNode* stm;

    stm = ALLOC(struct StatementNode);

    

    ttype = getToken();

    if (ttype == ID) // assign_stmt

    {

        //printf("INSIDE ASSIGN");

        ungetToken();

        stm->assign_stmt = assign_stmt();

       // printf("%s",stm->assign_stmt->left_hand_side->name);

        stm->type = ASSIGN_STMT;

        

       

        return stm;

    }

    else if (ttype == PRINT) // print_stmt

    {

        

        struct PrintStatement *obj;

        obj = ALLOC(struct PrintStatement);

        ttype = getToken();

        if(ttype == ID)

        {

            struct ValueNode *temp =  search_node(strdup(token)); 
            struct ValueNode *new_node = (struct ValueNode *)malloc(1000);
            new_node->name = (char *)malloc(1000);
            strcpy(new_node->name,temp->name);
            new_node->value = temp->value;
            
           // printf("BACK:%s",temp->name);
           // printf("BACK1:%d",temp->value); 

           
            obj->id = new_node;

            stm->print_stmt = obj;

        }

         stm->type = PRINT_STMT;

         ttype = getToken();

         if(ttype == SEMICOLON)

         {

         

         }

     //    printf("INSIDE PRINT:%s",stm->print_stmt->id->name);

     //   printf("INSIDE PRINT: val%s",stm->print_stmt->id->value);

     //   printf("HELLO:%d",stm->type);

        return stm;

   }

    

    else if (ttype == IF || ttype == WHILE || ttype == SWITCH) 

    {

           if(ttype == IF)

           {
            struct StatementNode* stm;

    stm = ALLOC(struct StatementNode);

               // printf("IF");

                 stm->type = IF_STMT;

                struct IfStatement *ifobj;

                ifobj = ALLOC(struct IfStatement);

           

                //condition

                

                //operand1 check

                ttype = getToken();

                if(ttype == ID || ttype == NUM)

                {

                    if(ttype == ID)

                    {

                   // printf("TOKEN:%s",token);
                   struct ValueNode *temp =  search_node(strdup(token)); 
            struct ValueNode *new_node = (struct ValueNode *)malloc(1000);
            new_node->name = (char *)malloc(1000);
            strcpy(new_node->name,temp->name);
            new_node->value = temp->value;
                   
                   

                       

                        ifobj->condition_operand1 = new_node;

                    }

                    else if(ttype == NUM)

                    {

                            char *buff = (char *)malloc(1000);

                            char *var_name = "noname";

                            char *cat = &numiterator;

                            strcat(buff,var_name);

                            strcat(buff,cat);

                            numiterator++;

                            insert_node(MasterList,buff,atoi(token));

                           // struct ValueNode *temp = search_node(buff);
                            
                             struct ValueNode *temp =  search_node(buff); 
            struct ValueNode *new_node = (struct ValueNode *)malloc(1000);
            new_node->name = (char *)malloc(1000);
            strcpy(new_node->name,temp->name);
            new_node->value = temp->value;
                            
                            

                            ifobj->condition_operand1 = new_node;

                        

                    

                    }    

                

                

                }

                

                //relop ccheck

                

                

                ttype = getToken();

                if(ttype == GREATER)

                {

                    ifobj->condition_op = GREATER;

                }

                else if(ttype == LESS)

                {

                    ifobj->condition_op = LESS;

                

                }

                else if(ttype == NOTEQUAL)

                {

                    ifobj->condition_op = NOTEQUAL;

                

                }

                

                

                //operand2 check

                ttype = getToken();

                if(ttype == ID || ttype == NUM)

                {

                    if(ttype == ID)

                    {
                     struct ValueNode *temp =  search_node(strdup(token)); 
            struct ValueNode *new_node = (struct ValueNode *)malloc(1000);
            new_node->name = (char *)malloc(1000);
            strcpy(new_node->name,temp->name);
            new_node->value = temp->value;
            

                      

                        ifobj->condition_operand2 = new_node;

                    }

                    else if(ttype == NUM)

                    {

                            char *buff = (char *)malloc(1000);

                            char *var_name = "noname";

                            char *cat = &numiterator;

                            strcat(buff,var_name);

                            strcat(buff,cat);

                            numiterator++;

                            insert_node(MasterList,buff,atoi(token));
                            
                             struct ValueNode *temp =  search_node(buff); 
            struct ValueNode *new_node = (struct ValueNode *)malloc(1000);
            new_node->name = (char *)malloc(1000);
            strcpy(new_node->name,temp->name);
            new_node->value = temp->value;
                            

                         //   struct ValueNode *temp = search_node(buff);

                            ifobj->condition_operand2 = new_node;

                        

                    

                    }    

                

                

                }

                

               

            //checking the condition

            

            int left_value = search_value(ifobj->condition_operand1);

            int right_value = search_value(ifobj->condition_operand2);

            bool result;

            if(ifobj->condition_op == GREATER)

            {

                if(left_value>right_value)

                    result = true;

                else

                    result = false;

            }  

            else if(ifobj->condition_op == LESS)

            {

                   if(left_value<right_value)

                    result = true;

                   else

                    result = false;

            }

            else if(ifobj->condition_op == NOTEQUAL)

            {

                   if(left_value!=right_value)

                        result = true;

                   else

                        result = false;

            }

            

            

            //true case

            

                ifobj->true_branch = parse_body();

       //   printf("INSIDE PRINT:%s",ifobj->true_branch->print_stmt->id->name);

        //  printf("INSIDE PRINT: val%s",ifobj->true_branch->print_stmt->id->value);

            

            //no - op 

            

          struct StatementNode *temp = ifobj->true_branch;

          

          //no_op->type = NOOP_STMT; it changes true type to noop

           while(temp->next!=NULL)

                {

                    temp = temp->next;

                }

          

         // printf("HOLA");

          struct StatementNode* stm1;

    stm1 = ALLOC(struct StatementNode);

          stm1->type = NOOP_STMT;

               // no_op = (struct StatementNode *)malloc(1000);
               // stm1->next=NULL;

            //  temp->next= stm1; //it is correct
            // temp = temp->next;

               

             //   ifobj->false_branch = temp;

            //   no_op = temp;

             
             temp->next= stm1;

             
             temp = temp->next;

             //   printf("TYPE:%d",ifobj->false_branch->type);

                

             ifobj->false_branch =temp;
              

            stm->if_stmt = ifobj;
            

              stm->next = temp; //removing
             
          
        //  printf("\n1:%d",s->next->type);     
        
               

         //   printf("LEFTVALUE:%s", stm->if_stmt->condition_operand1->name);

         //   printf("RightVALUE:%s", stm->if_stmt->condition_operand2->name);

         return stm;  

           }

           

           else if(ttype == WHILE)

           {

           

           return stm;

           }

           else if(ttype == SWITCH)

           {

           

           

           return stm;

           }

           else

           {

           ungetToken();

           }

           

    }

    

    return stm;

}



void printvariables(struct node* head)

{

    struct node *cur = head;

    while(cur!=NULL)

    {

        printf("%s\n",cur->data->name);

        printf("%d\n",cur->data->value);

        cur = cur->next;

    }



}







//PRINT ALL STATEMENTS



void printassign(struct AssignmentStatement *head)

{
  

   // printf("%s",head->left_hand_side->name);

   //   printf("=");

  //  printf("%d", head->left_hand_side->value);

  

  //  printf("%d",head->op);

    if(head->op==0)

    {

       // printf("%s",  head->operand1->name);

       // printf("%d",  head->operand1->value);

        

        //updating
     //    printf("\nHELL:%s", head->left_hand_side->name);

     //    printf("\nHELL1:%d", head->operand1->value);

      

     //  head->left_hand_side = new_node;
    
struct ValueNode *temp1 = search_node(head->operand1->name);
  // printf("VAL:%s,%d",temp1->name,temp1->value);
      //  printf("HELLO1");

    //  head->left_hand_side->value = temp1->value;
    //  printf("STR:%d",head->left_hand_side->value);
    //  printf("\nNEWSTR:%d",head->operand1->value);

     struct ValueNode *temp = search_node(head->left_hand_side->name);
   //   printf("VAL1:%s,%d",temp->name,temp->value);
 //     printf("ACTUAL:%d",temp->value);

      temp->value = temp1->value;  

   //   printvariables(MasterList);

      

    }

    else

    {

       // printf("%s",  head->operand1->name);

       // printf("%d",  head->operand1->value);

         

        

       // printf("%s",  head->operand2->name);

       // printf("%d",  head->operand2->value);

        

        //updating

        if(head->op==PLUS)

        {
          //  printf("HELLOa");
        struct ValueNode *temp1 = search_node(head->operand1->name);
        head->operand1->value = temp1->value;
        struct ValueNode *temp2 = search_node(head->operand2->name);
         head->operand2->value = temp2->value;
         struct ValueNode *temp = search_node(head->left_hand_side->name);
        
            temp->value = temp1->value + temp2->value;  
   //         printf("VVAL:%d",temp->value);

        }

        

        else if(head->op==MINUS){
           // printf("HELLOb");
        struct ValueNode *temp1 = search_node(head->operand1->name);
         head->operand1->value = temp1->value;
        struct ValueNode *temp2 = search_node(head->operand2->name);
         head->operand2->value = temp2->value;
         
         
         struct ValueNode *temp = search_node(head->left_hand_side->name);
        
            temp->value = temp1->value - temp2->value;  
         }

        else if(head->op==MULT){
           // printf("HELLOc");
        struct ValueNode *temp1 = search_node(head->operand1->name);
         head->operand1->value = temp1->value;
        struct ValueNode *temp2 = search_node(head->operand2->name);
         head->operand2->value = temp2->value;
        
         struct ValueNode *temp = search_node(head->left_hand_side->name);
        
            temp->value = temp1->value * temp2->value;  
         }

        else if(head->op==DIV) {
         //  printf("HELLOd");
       struct ValueNode *temp1 = search_node(head->operand1->name);
        head->operand1->value = temp1->value;
        struct ValueNode *temp2 = search_node(head->operand2->name);
         head->operand2->value = temp2->value;
        
        struct ValueNode *temp = search_node(head->left_hand_side->name);
     
       // printf("%d,%d",temp1->value,temp2->value);
           temp->value = temp1->value / temp2->value;  
        //   printf("ANS:%d",temp->value);
            
         }

    }

    

   

}



void printprint(struct PrintStatement *head)

{

   //printf("%s",  head->id->name);

    
  
     struct ValueNode *temp = search_node(head->id->name);
   

       head->id->value = temp->value;  
    //    printf("\nHEADVAL: %d\n",  head->id->value);

 // printvariables(MasterList);

}



void printif(struct IfStatement *head)

{

   //printf("%s",  head->condition_operand1->name);

   // printf("%d",  head->condition_operand1->value);

    

    

    

   // printf("%s",  head->condition_operand2->name);

   // printf("%d",  head->condition_operand2->value);

    

    //printf("\n%d",head->condition_op);

    

   // printf("{");

    

    

    

     bool result;

            if(head->condition_op == GREATER)

            {

                if(head->condition_operand1->value > head->condition_operand2->value)

                    result = true;

                else

                    result = false;

            }  

            else if(head->condition_op == LESS)

            {

                   if(head->condition_operand1->value < head->condition_operand2->value)

                    result = true;

                   else

                    result = false;

            }

            else if(head->condition_op == NOTEQUAL)

            {

                   if(head->condition_operand1->value != head->condition_operand2->value)

                        result = true;

                   else

                        result = false;

            }

   //printf("RESU:%d",result);

    if(result){

   // printf("INSIDE");

    printstatements(head->true_branch);

    

    }

    

    else

    printstatements(head->false_branch);

     

    //printf("}");

    

    

}



void printstatements(struct StatementNode *head)

{
    

    while(head!=NULL)

    {

  // printf("\nT:%d\n",head->type);

        if(head->type == ASSIGN_STMT)

        {

       // printf("ASSIGNPRINT");

        //printf("in:%s",head->assign_stmt->left_hand_side->name);

          printassign(head->assign_stmt);

         // printvariables(MasterList);
          head = head->next;

        }

        else if(head->type == PRINT_STMT)

        {
       //  printvariables(MasterList);
            
            

     //       printf("PRINTPRINT");

            printprint(head->print_stmt);
             head = head->next;

        }

        else if(head->type == IF_STMT)

        {

       // printf("IIFFSTMT");

           // printif(head->if_stmt);
            
             struct ValueNode *temp = search_node(head->if_stmt->condition_operand1->name);
   

       head->if_stmt->condition_operand1->value = temp->value;  
       
       
             struct ValueNode *temp1 = search_node(head->if_stmt->condition_operand2->name);
   

       head->if_stmt->condition_operand2->value = temp1->value;  
       
            
            

    

     bool result;

            if(head->if_stmt->condition_op == GREATER)

            {

                if(head->if_stmt->condition_operand1->value > head->if_stmt->condition_operand2->value)

                    result = true;

                else

                    result = false;

            }  

            else if(head->if_stmt->condition_op == LESS)

            {

                   if(head->if_stmt->condition_operand1->value < head->if_stmt->condition_operand2->value)

                    result = true;

                   else

                    result = false;

            }

            else if(head->if_stmt->condition_op == NOTEQUAL)

            {

                   if(head->if_stmt->condition_operand1->value != head->if_stmt->condition_operand2->value)

                        result = true;

                   else

                        result = false;

            }
         //   printf("RESU:%d",result);
            
            if(result){
             head = head->if_stmt->true_branch;
             }
             else
              head = head->if_stmt->false_branch;
            
            

        }

        else if(head->type == NOOP_STMT)

        {
            head = head->next;

        }

        else if(head->type == GOTO_STMT)

        {

        

        }

       

       

    }



}







struct StatementNode* parse_generate_intermediate_representation(){



   struct StatementNode * Head;

   

   parse_variables();

   

   Head = parse_body();

   printstatements(Head);

   

 

  //printvariables(MasterList);



    return Head;

}
