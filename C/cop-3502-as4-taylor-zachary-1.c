#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct item_node_struct
{
    char name[32];
    int count;
    struct item_node_struct *left, *right;
};

typedef struct item_node_struct item_node;

struct tree_name_node_struct
{
    char treeName[32];
    struct tree_name_node_struct *left, *right;
    item_node *theTree;
};

typedef struct tree_name_node_struct tree_name_node;

tree_name_node *new_tree_node(char node_name[])
{
    tree_name_node *t = malloc(sizeof(tree_name_node));
    t->left = NULL;
    t->right = NULL;
    
    strcpy(t->treeName, node_name);
    
	  t->theTree = NULL;
    
    return t;
}

item_node *new_item_node(char item_name[], int count)
{
    item_node *t = malloc(sizeof(item_node));
    t->count = count;
    t->left = NULL;
    t->right = NULL;
    
    strcpy(t->name, item_name);

    return t;
}

tree_name_node* recursive_search_head_tree(tree_name_node *head_tree, char* tree_name)
{
    if(head_tree == NULL)
    {
		    return NULL;
    }
    else if(strcmp(head_tree->treeName, tree_name) == 0)
    {
        //printf("search %s\n", head_tree->treeName);
		    return head_tree;
    }
    else if(strcmp(head_tree->treeName, tree_name) > 0)
    {
	    	return recursive_search_head_tree(head_tree->left, tree_name);
    }
    else
	{
		return recursive_search_head_tree(head_tree->right, tree_name);
	}
}

void traverse_tree_node(tree_name_node* head_item, FILE* foutptr)
{
    if (head_item->left != NULL){
        traverse_tree_node (head_item->left, foutptr);
    }
  
    fprintf(foutptr,"%s ", head_item->treeName);
    
    if (head_item->right != NULL){
        traverse_tree_node (head_item->right, foutptr);
    }
}

void traverse_item_name_node(item_node* head_item, FILE* foutptr)
{
    if (head_item->left != NULL){
        //printf("\nleft %s \n", head_item->left->name);
        traverse_item_name_node (head_item->left,foutptr);
    }
  
    fprintf(foutptr,"%s ", head_item->name);
    
    if (head_item->right != NULL){
        //printf("\nright %s \n", head_item->right->name);
        traverse_item_name_node (head_item->right,foutptr);
        
    }
}

void traverse_tree_name_node(tree_name_node* head_tree, FILE* foutptr)
{
    if (head_tree->left != NULL){
        traverse_tree_name_node (head_tree->left,foutptr);
    }
    
    fprintf(foutptr,"===%s===\n", head_tree->treeName);
    traverse_item_name_node(head_tree->theTree,foutptr);
    fprintf(foutptr,"\n");
    
    if (head_tree->right != NULL){
        traverse_tree_name_node (head_tree->right,foutptr);
    }
}

tree_name_node* recursive_insert_tree_node(tree_name_node *head_tree, char* tree_name)
{
    if(head_tree == NULL)
    {
		return new_tree_node(tree_name);
    }
    if(strcmp(head_tree->treeName, tree_name) < 0)
    {
        head_tree->right = recursive_insert_tree_node(head_tree->right, tree_name);
    }
    else if(strcmp(head_tree->treeName, tree_name) > 0)
    {
        head_tree->left = recursive_insert_tree_node(head_tree->left, tree_name);
    } 	
	
	return head_tree;
}

int recursive_search_head_item(item_node *head_item, char* item_name)
{
    if(head_item == NULL)
    {
        return -1;
    }

    if(strcmp(head_item->name, item_name) == 0)
    {
        //printf("search %s\n", head_item->name);
        return head_item->count;
    }

    else if(strcmp(head_item->name, item_name) > 0)
    {
        return recursive_search_head_item(head_item->left, item_name);
    }

    else
    {
        return recursive_search_head_item(head_item->right, item_name);
    }
}

item_node* recursive_insert_item_node(item_node *head_item, char* item_name, int count)
{
    if(head_item == NULL)
    {
		return new_item_node(item_name, count);
    }
    if(strcmp(head_item->name, item_name) < 0)
    {
        head_item->right = recursive_insert_item_node(head_item->right, item_name, count);
    }
    else if(strcmp(head_item->name, item_name) > 0)
    {
        head_item->left = recursive_insert_item_node(head_item->left, item_name, count);
    }
	
	return head_item;
}

tree_name_node* initialize_trees(FILE* fptr, tree_name_node *head_tree, int num_trees, int num_items)
{
    char line[32], tree_name[32], item_name[32];
    int i, count;
    
    for(i=0; i < num_trees; i++)
    {
        fgets(line, sizeof(line), fptr);
        sscanf(line, "%s", tree_name);

        head_tree = recursive_insert_tree_node(head_tree, tree_name);
    }
    
    for(i=0; i < num_items; i++)
    {
        fgets(line, sizeof(line), fptr);
        sscanf(line, "%s %s %d", tree_name, item_name, &count);
        
        tree_name_node* tree_node = recursive_search_head_tree(head_tree, tree_name);
        
        if(tree_node != NULL)
        {
           tree_node->theTree = recursive_insert_item_node(tree_node->theTree, item_name, count);
        }      
    }
    
    return head_tree;
}

int count_items_before(item_node *head_item, char* search_item)
{   
    int count = 0;

    if(head_item == NULL)
    {
        return 0;
    }
    else if (strcmp(head_item->name, search_item) < 0)
    {
        count++;
        count+=count_items_before(head_item->left, search_item);
        count+=count_items_before(head_item->right, search_item);
    }
    else if (strcmp(head_item->name, search_item) > 0)
    {
        count+=count_items_before(head_item->left, search_item);
    }
    
    return count;
}

int count_tree(item_node *head_item)
{
    if(head_item == NULL)
    {
        return 0;
    }
    
    return head_item->count + count_tree(head_item->left) + count_tree(head_item->right);
}

void delete_tree_helper(item_node* head_item)
{
    if(head_item->left != NULL)
    {
        delete_tree_helper(head_item->left);
        free(head_item->left);
    }
    if(head_item->right != NULL)
    {
        delete_tree_helper(head_item->right);
        free(head_item->right);
    }
}

void delete_tree(tree_name_node* head_tree, char* delete_tree_name)
{
    tree_name_node* tree_node = recursive_search_head_tree(head_tree, delete_tree_name);
    
    if(tree_node->theTree != NULL)
    {
        delete_tree_helper(tree_node->theTree);
        free(tree_node->theTree);
    }
}

item_node* delete_item_helper(item_node* head_item)
{
    if(head_item->left == NULL)
    {
        return head_item;
    }
    else
    {
        return delete_item_helper(head_item->left);
    }
}

item_node* delete_item(item_node* head_item, char* item)
{
    if(head_item == NULL)
    {
        return head_item;
    }
    if(strcmp(head_item->name, item) > 0)
    {
        head_item->left = delete_item(head_item->left, item);
    }
    else if(strcmp(head_item->name, item) < 0)
    {
        head_item->right = delete_item(head_item->right, item);
    }
    else
    {
        if(head_item->left == NULL)
        {
            return head_item->right;
        }
        else if(head_item->right == NULL)
        {
            return head_item->left;
        }
        
        item_node* temp = delete_item_helper(head_item->right);
        
        strcpy(head_item->name, temp->name);
        head_item->count = temp->count;
        
        head_item->right = delete_item(head_item->right, temp->name);
    }
    
    return head_item;
    
}

int height_subtree(item_node* head_item)
{
    if(head_item == NULL)
    {
        return -1;
    }
    
    int left_height = height_subtree(head_item->left);
    int right_height = height_subtree(head_item->right);
    
    int max;
    if(left_height > right_height)
    {
      max = left_height;
    }
    else 
    {
      max = right_height;
    }


    return 1 + max;
}

void read_input_count(FILE* fptr, int* num_trees, int* num_items, int* num_commands)
{
    char line[32];
    int trees, items, commands;
    
    fgets(line, sizeof(line), fptr);
    sscanf(line, "%d %d %d", &trees, &items, &commands);
    (*num_trees) = trees;
    (*num_items) = items;
    (*num_commands) = commands;
}

void execute_commands(FILE* fptr, FILE* foutptr, tree_name_node* head_tree, int num_commands)
{
    fprintf(foutptr,"=====Processing Commands=====\n");
    char line[32], parse1[32], parse2[32], parse3[32];

    int i = 0;
    for(i=0; i < num_commands; i++)
    {
        fgets(line, sizeof(line), fptr);
        if(strstr(line, "search") != NULL)
        {
            sscanf(line, "%s %s %s", parse1, parse2, parse3);
            tree_name_node* tree_node = recursive_search_head_tree(head_tree, parse2);
            
            

            if(tree_node == NULL)
            {
                fprintf(foutptr,"%s does not exist\n", parse2);
                continue;
            }
            
            int count = recursive_search_head_item(tree_node->theTree, parse3);
            
            if(count == -1)
            {
                fprintf(foutptr,"%s not found in %s\n", parse3, parse2);
            }
            else
            {
                fprintf(foutptr,"%d %s found in %s\n", count, parse3, parse2);
            }
        }
        else if(strstr(line, "item_before") != NULL)
        {
            sscanf(line, "%s %s %s", parse1, parse2, parse3);
            tree_name_node* tree_node = recursive_search_head_tree(head_tree, parse2);
            
            int items_before = 1 + count_items_before(tree_node->theTree, parse3);
            
            fprintf(foutptr,"item before %s: %d\n", parse3, items_before);
            
        }
        else if(strstr(line, "height_balance") != NULL)
        {
        
            sscanf(line, "%s %s", parse1, parse2);
            
            tree_name_node* tree_node = recursive_search_head_tree(head_tree, parse2);
            
            int left_height = height_subtree(tree_node->theTree->left);
            int right_height = height_subtree(tree_node->theTree->right);
            
            int difference = right_height - left_height;
            
            if(difference == 0 || difference == 1)
            {
                fprintf(foutptr,"%s: left height %d, right height %d, difference %d, balanced\n", tree_node->theTree->name, left_height, right_height, difference);
            }
            else
            {
                fprintf(foutptr,"%s: left height %d, right height %d, difference %d, not balanced\n", tree_node->theTree->name, left_height, right_height, difference);
            }
        
        }
        else if(strstr(line, "count") != NULL)
        {
            sscanf(line, "%s %s", parse1, parse2);
            tree_name_node* tree_node = recursive_search_head_tree(head_tree, parse2);
            
            int tree_count = count_tree(tree_node->theTree);
            fprintf(foutptr,"%s count %d\n", parse2, tree_count);            
        }
        else if(strstr(line, "delete_tree") != NULL)
        {
            sscanf(line, "%s %s", parse1, parse2);
            delete_tree(head_tree, parse2);
            fprintf(foutptr,"%s deleted\n", parse2);
        }
        else 
        {
            sscanf(line, "%s %s %s", parse1, parse2, parse3);
            tree_name_node* tree_node = recursive_search_head_tree(head_tree, parse2);
            
            delete_item(tree_node->theTree, parse3);
            
            fprintf(foutptr,"%s deleted from %s\n", parse3, parse2);
        }
    }
}

int main() {
    int num_trees, num_items, num_commands;
    
    FILE* finptr = fopen("cop3502-as4-input.txt", "r");
    FILE* foutptr = fopen("cop3502-as4-output-taylor-zachary.txt", "w");
    read_input_count(finptr, &num_trees, &num_items, &num_commands);
    
    //printf("%d %d %d", num_items, num_items, num_commands);

    tree_name_node *head_tree = NULL;
    head_tree = initialize_trees(finptr, head_tree, num_trees, num_items);

    traverse_tree_node(head_tree,foutptr);
    fprintf(foutptr,"\n");
    traverse_tree_name_node(head_tree,foutptr);
    execute_commands(finptr, foutptr, head_tree, num_commands);
    
    fclose(finptr);
    fclose(foutptr);
    return 0;
}
