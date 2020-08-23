#include "defaults.h"
#include "labelTableLinkedList.h"

void addToList(labelTableNodePtr *hptr, labelTableNode node) {
    labelTableNodePtr p1, p2;
    labelTableNodePtr t;

    t = (labelTableNodePtr) malloc(sizeof(labelTableNode));
    if(!t) {
        printf("Cannot Build List!\n");
        exit(0);
    }

    strcpy(t->label, node.label);
    t->address = node.address;
    t->location = node.location;
    t->type = node.type;
    t->next = NULL;

    p1 = *hptr;
    p2 = NULL;
    /* get to the last node */
    while(p1) {
        p2 = p1;
        p1 = p1->next;
    }

    /* if the list empty */
    if(p1 == *hptr) {
        *hptr = t;
    }
    else { /* add the new node to the end of the list */
        p2->next = t;
        t->next = p1;
    }
}

void freeLabelList(labelTableNodePtr *hptr) {
    labelTableNodePtr temp;

    while (*hptr) {
        temp = (*hptr)->next;
        free(*hptr);
        *hptr = temp;
    }
}

/* check if the label already exist in the label table*/
Bool labelInList(labelTableNodePtr hptr, char *label) {
    while(hptr) {
        if(!strcmp(label,hptr->label))
            return True;
        hptr = hptr->next;
    }
    return False;
}

/* return the type of the label, if not exist return None */
Type getLabelType(labelTableNodePtr hptr, char *label) {
    while(hptr) {
        if(!strcmp(label,hptr->label))
            return hptr->type;
        hptr = hptr->next;
    }
    return NoneEntOrExt;
}

/* get the adderss of the label */
int getLabelAddress(labelTableNodePtr hptr, char *label) {
    while(hptr) {
        if(!strcmp(label,hptr->label))
            return hptr->address;
        hptr = hptr->next;
    }
    return -1;
}

/* set the type of the label to type t */
void setType(labelTableNodePtr hptr, char *label, Type t) {
    while(hptr){
        if(!strcmp(label,hptr->label)) {
            hptr->type = t;
            return;
        }
        hptr = hptr->next;
    }
}