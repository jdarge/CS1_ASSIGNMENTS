#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "leak_detector_c.h"

#define MAXLEN 30 + 1

typedef struct itemNode {
    char name[MAXLEN];
    int count;
    struct itemNode *left;
    struct itemNode *right;
} itemNode;

typedef struct treeNameNode {
    char treeName[MAXLEN];
    itemNode *theTree;
    struct treeNameNode *left;
    struct treeNameNode *right;
} treeNameNode;

// MAIN
treeNameNode *buildNameTree(FILE *in, FILE *out, treeNameNode *root, int N, int I);//
treeNameNode *command(FILE *in, FILE *out, treeNameNode *root, int Q);//
treeNameNode *doCommand(FILE *in, FILE *out, treeNameNode *root, char command[MAXLEN]);//

// COMMANDS
void search(FILE *out, treeNameNode *root, char tree[MAXLEN], char branch[MAXLEN]);//
void count(FILE *out, treeNameNode *root, char tree[MAXLEN]);//
void reduce(FILE *out, treeNameNode *root, char tree[MAXLEN], char branch[MAXLEN], int n);//
void height_balance(FILE *out, treeNameNode *root, char tree[MAXLEN]);//
void item_before(FILE *out, treeNameNode *root, char tree[MAXLEN], char branch[MAXLEN]);//
void deleteBranch(FILE *out, treeNameNode *root, char tree[MAXLEN], char branch[MAXLEN]);//
treeNameNode *deleteTree(FILE *out, treeNameNode *root, char tree[MAXLEN]);//

// DISPLAY
void display(FILE *out, treeNameNode *root);//
void inorderTree(FILE *out, treeNameNode *current_ptr);//
void inorderItem(FILE *out, itemNode *current_ptr);//
void traverse_in_traverse(FILE *out, treeNameNode *root);//

// UTILITY GENERAL
int countTree(itemNode *node);//
int height(itemNode *node);//
int countBefore(itemNode *root, itemNode *select);//

// UTILITY TREE
treeNameNode *createTreeNameNode(FILE *in);//
treeNameNode *insertTreeNameNode(treeNameNode *root, treeNameNode *insert);//
treeNameNode *searchNameNode(treeNameNode *root, char treeName[MAXLEN]);//
treeNameNode *parentTree(treeNameNode *root, treeNameNode *node);//
int isLeaf(treeNameNode *node);//
int hasOnlyLeftChild(treeNameNode *node);//
int hasOnlyRightChild(treeNameNode *node);//
treeNameNode *minVal(treeNameNode *root);//

// UTILITY BRANCH
itemNode *createItemNode(char input[MAXLEN], int amount);//
itemNode *insertItemNode(itemNode *node_root, itemNode *insert);//
itemNode *searchBranchNode(itemNode *root, char branch[MAXLEN]);//
itemNode *deleteBranchFinal(itemNode *root, itemNode *selected);//
itemNode *parentBranch(itemNode *root, itemNode *node);//
int isLeafNode(itemNode *node);//
int hasOnlyLeftChildNode(itemNode *node);//
int hasOnlyRightChildNode(itemNode *node);//
itemNode *minValNode(itemNode *root);//

// FREE
treeNameNode *freeTree(treeNameNode *root, treeNameNode *selected);//
treeNameNode *freeTreeFinal(treeNameNode *root, treeNameNode *selected);//
void freeAllMem(treeNameNode *root);//
void freeTreeMem(itemNode *root);//

//------------------------------------------------------------------------------
//                                    MAIN                                    //
////////////////////////////////////////////////////////////////////////////////

int main(void) {
    atexit(report_mem_leak);

    FILE *in = fopen("in.txt", "r");
    FILE *out = fopen("out.txt", "w");

    int N;// NUMBER OF TREE NAMES
    int I;// TOTAL NUMBER OF ITEMS
    int Q;// NUMBER OF QUERIES

    treeNameNode *root = NULL;

    // TAKING IN INITIAL FILE INPUTS
    fscanf(in, "%d %d %d", &N, &I, &Q);

    // BUIDLING THE TREES
    root = buildNameTree(in, out, root, N, I);

    // EXECUTING COMMANDS
    root = command(in, out, root, Q);

    // CLOSING FILES
    fclose(out);
    fclose(in);

    // FREEING MEMORY
    freeAllMem(root);
}

treeNameNode *buildNameTree(FILE *in, FILE *out, treeNameNode *root, int N, int I) {

    treeNameNode *store = NULL;
    char input[MAXLEN];
    int amount;

    // CREATING AND INSERTING TREE NAME NODES
    for (int i = 0; i < N; i++) {
        treeNameNode *temp = createTreeNameNode(in);
        root = insertTreeNameNode(root, temp);
    }

    // CREATING AND INSERTING ITEM NODES
    for (int i = 0; i < I; i++) {
        fscanf(in, "%s", input);
        store = searchNameNode(root, input);

        fscanf(in, "%s %d", input, &amount);
        itemNode *temp = createItemNode(input, amount);
        store->theTree = insertItemNode(store->theTree, temp);
    }

    // INITIAL DISPLAY
    display(out, root);

    return root;
}

treeNameNode *command(FILE *in, FILE *out, treeNameNode *root, int Q) {

    char input[MAXLEN];

    // READING EVERY COMMAND AND CALLING doCommand()
    for (int i = 0; i < Q; i++) {
        fscanf(in, "%s", input);
        root = doCommand(in, out, root, input);
    }

    return root;
}

treeNameNode *doCommand(FILE *in, FILE *out, treeNameNode *root, char command[MAXLEN]) {
    // CHECKS FOR THE FIRST COMMAND LINE
    // COMPARES W/ ALL POSSIBLE COMMANDS AND EXECUTES THE CORRECT ONE
    char first[MAXLEN];
    char second[MAXLEN];
    int n;

    if (strcmp(command, "search") == 0) {
        fscanf(in, "%s %s", first, second);
        search(out, root, first, second);
    }

    if (strcmp(command, "count") == 0) {
        fscanf(in, "%s", first);
        count(out, root, first);
    }

    if (strcmp(command, "reduce") == 0) {
        fscanf(in, "%s %s %d", first, second, &n);
        reduce(out, root, first, second, n);
    }

    if (strcmp(command, "height_balance") == 0) {
        fscanf(in, "%s", first);
        height_balance(out, root, first);
    }

    if (strcmp(command, "item_before") == 0) {
        fscanf(in, "%s %s", first, second);
        item_before(out, root, first, second);
    }

    if (strcmp(command, "delete") == 0) {
        fscanf(in, "%s %s", first, second);
        deleteBranch(out, root, first, second);
    }

    if (strcmp(command, "delete_name") == 0) {
        fscanf(in, "%s", first);
        root = deleteTree(out, root, first);
    }

    return root;
}

//------------------------------------------------------------------------------
//                                  COMMANDS                                  //
////////////////////////////////////////////////////////////////////////////////

void search(FILE *out, treeNameNode *root, char tree[MAXLEN], char branch[MAXLEN]) {
    // FIND TREE AND BRANCH
    treeNameNode *select = searchNameNode(root, tree);

    if (select) {// select WAS FOUND -> PRINT
        itemNode *selected_branch = searchBranchNode(select->theTree, branch);

        if (selected_branch) {// selected_branch WAS FOUND -> PRINT
            fprintf(out, "%d %s found in %s\n",
                    selected_branch->count, selected_branch->name, select->treeName
            );
            printf("%d %s found in %s\n",
                   selected_branch->count, selected_branch->name, select->treeName
            );
        } else {// selected_branch WAS NOT FOUND -> PRINT
            fprintf(out, "%s not found in %s\n", branch, select->treeName);
            printf("%s not found in %s\n", branch, select->treeName);
        }
    } else {// select WAS NOT FOUND -> PRINT
        printf("%s does not exist\n", tree);
        fprintf(out, "%s does not exist\n", tree);
    }
}

void count(FILE *out, treeNameNode *root, char tree[MAXLEN]) {
    // FIND A TREE
    treeNameNode *select = searchNameNode(root, tree);

    if (select) {// TREE WAS FOUND
        // COUNT THE TOTAL NUMBER OF ITEMS INSIDE THE TREE -> PRINT
        int count = countTree(select->theTree);
        fprintf(out, "%s count %d\n", select->treeName, count);
        printf("%s count %d\n", select->treeName, count);
    }
}

void reduce(FILE *out, treeNameNode *root, char tree[MAXLEN], char branch[MAXLEN], int n) {
    // FIND TREE AND BRANCH
    treeNameNode *select = searchNameNode(root, tree);

    if (select) {// TREE WAS FOUND
        itemNode *selected_branch = searchBranchNode(select->theTree, branch);

        if (selected_branch) {// BRANCH WAS FOUND
            // CREATE NEW TOTAL FOR ITEM NODE
            int count = selected_branch->count;
            int amount = count - n;

            char name[MAXLEN];
            strcpy(name, selected_branch->name);

            if (amount <= 0) {// NEW AMOUNT IS LESS/EQUAL TO ZERO -> DELETE NODE
                if (!parentBranch(select->theTree, selected_branch)) {// PARENT CHECK
                    selected_branch = deleteBranchFinal(select->theTree, selected_branch);
                    select->theTree = selected_branch;
                } else {
                    selected_branch = deleteBranchFinal(select->theTree, selected_branch);
                }
            } else {// NEW AMOUNT IS GREATER THAN ZERO -> DELETE NODE
                selected_branch->count = amount;
            }

            printf("%s reduced\n", name);
            fprintf(out, "%s reduced\n", name);
        }
    }
}

void height_balance(FILE *out, treeNameNode *root, char tree[MAXLEN]) {
    // FIND TREE AND BRANCH
    treeNameNode *select = searchNameNode(root, tree);

    if (select) {// TREE FOUND
        // GET HEIGHT VALUES
        int left = height(select->theTree->left) - 1;
        int right = height(select->theTree->right) - 1;
        int final = abs(left - right);

        // CHECK TO SEE IF HEIGHT IS BALANCED BETWEEN THE LEFT AND RIGHT SIDE
        if (final > 1) {// NOT BALANCED
            fprintf(out,
                    "%s: left height %d, right height %d, difference %d, not balanced\n",
                    select->treeName, left, right, final
            );
            printf(
                    "%s: left height %d, right height %d, difference %d, not balanced\n",
                    select->treeName, left, right, final
            );
        } else {// BALANCED
            fprintf(out,
                    "%s: left height %d, right height %d, difference %d, balanced\n",
                    select->treeName, left, right, final
            );
            printf("%s: left height %d, right height %d, difference %d, balanced\n",
                   select->treeName, left, right, final
            );
        }
    }
}

void item_before(FILE *out, treeNameNode *root, char tree[MAXLEN], char branch[MAXLEN]) {
    // FIND TREE AND BRANCH
    treeNameNode *select = searchNameNode(root, tree);

    if (select) {// TREE FOUND
        itemNode *selected_branch = searchBranchNode(select->theTree, branch);

        if (selected_branch) {// BRANCH FOUND
            // COUNT ITEMS BEFORE THE FOUND BRANCH AND DISPLAY
            int count = countBefore(root->theTree, selected_branch);
            fprintf(out, "item before %s: %d\n", selected_branch->name, count);
            printf("item before %s: %d\n", selected_branch->name, count);
        }
    }
}

void deleteBranch(FILE *out, treeNameNode *root, char tree[MAXLEN], char branch[MAXLEN]) {
    // FIND TREE AND BRANCH
    treeNameNode *select = searchNameNode(root, tree);

    if (select) {// TREE FOUND
        itemNode *selected_branch = searchBranchNode(select->theTree, branch);

        if (selected_branch) {// BRANCH FOUND
            // SAVE NAME FOR DELETE
            char name[MAXLEN];
            strcpy(name, selected_branch->name);

            // CHECK FOR PARENT AND THEN DELETE
            if (!parentBranch(select->theTree, selected_branch)) {// NO PARENT
                selected_branch = deleteBranchFinal(select->theTree, selected_branch);
                select->theTree = selected_branch;
            } else {// THERE IS A PARENT
                selected_branch = deleteBranchFinal(select->theTree, selected_branch);
            }

            fprintf(out, "%s deleted from %s\n", name, select->treeName);
            printf("%s deleted from %s\n", name, select->treeName);
        }
    }
}

treeNameNode *deleteTree(FILE *out, treeNameNode *root, char tree[MAXLEN]) {
    // FIND A TREE
    char name[MAXLEN];
    treeNameNode *select = searchNameNode(root, tree);

    if (select) {// TREE FOUND
        // SAVE TREE NAME FOR PRINT STATEMENTS
        strcpy(name, select->treeName);

        // REMOVE TREE AND POSSIBLY UPDATE ROOT
        root = freeTree(root, select);

        printf("%s deleted\n", name);
        fprintf(out, "%s deleted\n", name);
    }

    return root;
}

//------------------------------------------------------------------------------
//                                  DISPLAY                                   //
////////////////////////////////////////////////////////////////////////////////

void display(FILE *out, treeNameNode *root) {
    // INORDER OPERATION FOR DISPLAYING TREE AND TREE ITEMS
    inorderTree(out, root);
    fprintf(out, "\n");
    printf("\n");
    traverse_in_traverse(out, root);
}

void inorderTree(FILE *out, treeNameNode *current_ptr) {
    // INORDER DISPLAY FOR TREE
    if (current_ptr) {
        inorderTree(out, current_ptr->left);
        fprintf(out, "%s ", current_ptr->treeName);
        printf("%s ", current_ptr->treeName);
        inorderTree(out, current_ptr->right);
    }
}

void inorderItem(FILE *out, itemNode *current_ptr) {
    // INORDER TREE BRANCH DISPLAY
    if (current_ptr) {
        inorderItem(out, current_ptr->left);

        fprintf(out, "%s ", current_ptr->name);
        printf("%s ", current_ptr->name);

        inorderItem(out, current_ptr->right);
    }
}

void traverse_in_traverse(FILE *out, treeNameNode *root) {
    // MAIN DISPLAY FUNCTION
    // DISPLAY TREE NAME AND ALL TREE ITEMS
    if (root) {
        traverse_in_traverse(out, root->left);

        fprintf(out, "===%s===\n", root->treeName);
        printf("===%s===\n", root->treeName);

        inorderItem(out, root->theTree);

        fprintf(out, "\n");
        printf("\n");

        traverse_in_traverse(out, root->right);
    }
}

//------------------------------------------------------------------------------
//                              UTILITY GENERAL                               //
////////////////////////////////////////////////////////////////////////////////

int countTree(itemNode *node) {
    // ADD TOTAL OF THE ROOT AND ALL ITEMS TO THE LEFT AND RIGHT
    if (node) {
        return node->count + countTree(node->left) + countTree(node->right);
    }

    return 0;
}

int height(itemNode *node) {
    // CALCULATE HEIGHT FROM THE NODE'S LEFT AND RIGHT SIDEE
    if (!node) {
        return 0;
    } else {
        // STORE HEIGHT OF LEFT AND HEIGHT OF RIGHT
        int l = height(node->left);
        int r = height(node->right);

        // CHECK TO SEE IF LEFT HEIGHT IS GREATER THAN THE RIGHT
        if (l > r) {
            return (l + 1);
        }

        return (r + 1);
    }
}

int countBefore(itemNode *root, itemNode *select) {
    // IF THERE'S NO ROOT, ROOT IS ALREADY SELECTED OR THE ROOT IS TO THE RIGHT OF selected
    // RETURN NOTHING
    if (!root || !select || root == select || strcmp(root->name, select->name) < 0) {
        return 0;
    }

    // RECURSIVELY CALL COUNTBEFORE ON BOTH SIDES
    return 1 + (countBefore(root->left, select)) + (countBefore(root->right, select));
}

//------------------------------------------------------------------------------
//                              UTILITY TREE                                  //
////////////////////////////////////////////////////////////////////////////////

treeNameNode *createTreeNameNode(FILE *in) {
    // CREATES A TREE NODE
    char name[MAXLEN];

    treeNameNode *temp = (treeNameNode *) malloc(sizeof(treeNameNode));

    // TAKE IN A NAME FROM THE FILE AND ADD IT
    fscanf(in, "%s", name);
    strcpy(temp->treeName, name);

    // INITIALIZES ALL OTHER PARAMATERS TO NULL
    temp->left = NULL;
    temp->right = NULL;
    temp->theTree = NULL;

    return temp;
}

treeNameNode *insertTreeNameNode(treeNameNode *root, treeNameNode *element) {
    // CHECK TO SEE IF THERE ISN'T A ROOT YET
    if (!root) {
        return element;
    }

    // CHECK TO SEE IF THE NEW TREE NAME IS GREATER THAN THE ROOT'S
    if (strcmp(element->treeName, root->treeName) > 0) {
        // SOMETHING IS IN THE WAY, CONTINUE DOWN RIGHT
        if (root->right != NULL) {
            root->right = insertTreeNameNode(root->right, element);
        }
            // CLEAR TO SIMPLY INSERT NEW ELEMENT TO THE RIGHT
        else {
            root->right = element;
        }
    }

        // NEW TREE NAME IS LESS THAN THE ROOT'S
    else {
        // SOMETHING IS IN THE WAY, CONTINUE DOWN LEFT
        if (root->left != NULL) {
            root->left = insertTreeNameNode(root->left, element);
        }
            // CLEAR TO SIMPLY INSERT NEW ELEMENT TO THE LEFT
        else {
            root->left = element;
        }
    }

    return root;
}

treeNameNode *searchNameNode(treeNameNode *root, char treeName[MAXLEN]) {
    // CHECK TO SEE IF ROOT IS NULL
    if (root != NULL) {
        // SEE IF THE ROOT NAME AND NAME TO INSPECT ARE EQUIVALENT
        if (strcmp(root->treeName, treeName) == 0) {
            return root;
        }
        // SEE IF THE NAME TO INSPECT IS SMALLER THAN ROOT NAME
        if (strcmp(root->treeName, treeName) > 0) {
            return searchNameNode(root->left, treeName);
        }
        // NAME TO INSPECT IS EQUAL/GREATER THAN ROOT NAME
        return searchNameNode(root->right, treeName);
    } else
        return NULL;
}

treeNameNode *parentTree(treeNameNode *root, treeNameNode *node) {
    //  CHECK TO SEE IF THERE EVEN IS A PARENT OR IF SCOPE IS TOO SMALL
    if (!root || root == node) {
        return NULL;
    }
    // CHECK FOR EQUIVALENCE
    if (root->left == node || root->right == node) {
        return root;
    }
    // SEE IF THE NAME TO INSPECT IS SMALLER THAN ROOT NAME
    if (strcmp(root->treeName, node->treeName) > 0) {
        return parentTree(root->left, node);
    }
    // SEE IF THE NAME TO INSPECT IS GREATER THAN ROOT NAME
    if (strcmp(root->treeName, node->treeName) < 0) {
        return parentTree(root->right, node);
    }

    return NULL;
}

int isLeaf(treeNameNode *node) {
    // IS THERE ANY NODE TO THE LEFT OR RIGHT?
    return (!node->left && !node->right);
}

int hasOnlyLeftChild(treeNameNode *node) {
    // VERIFY THERES A NODE TO THE LEFT BUT NOT THE RIGHT
    return (node->left && !node->right);
}

int hasOnlyRightChild(treeNameNode *node) {
    // VERIFY THERES A NODE TO THE RIGHT BUT NOT THE LEFT
    return (!node->left && node->right);
}

treeNameNode *minVal(treeNameNode *root) {
    // GRABS THE SMALLEST TREE (ALL THE WAY LEFT)
    if (!root->left) {
        return root;
    }

    // KEEP RECALLING UNTIL WE FIND A NULL VALUE
    return minVal(root->left);
}

//------------------------------------------------------------------------------
//                              UTILITY BRANCH                                //
////////////////////////////////////////////////////////////////////////////////

itemNode *createItemNode(char input[MAXLEN], int amount) {
    // CREATE AN ITEM NODE
    itemNode *temp = (itemNode *) malloc(sizeof(itemNode));

    // SAVE ITEM NAME AND AMOUNT
    strcpy(temp->name, input);
    temp->count = amount;

    // SET EVERYTHING ELSE TO NULL
    temp->left = NULL;
    temp->right = NULL;

    return temp;
}

itemNode *insertItemNode(itemNode *root, itemNode *element) {
    // CHECK TO SEE IF THERE ISN'T A ROOT YET
    if (!root) {
        return element;
    }
    // CHECK TO SEE IF THE NEW TREE NAME IS GREATER THAN THE ROOT'S
    if (strcmp(element->name, root->name) > 0) {
        // SOMETHING IS IN THE WAY, CONTINUE DOWN RIGHT
        if (root->right) {
            root->right = insertItemNode(root->right, element);
        }
            // CLEAR TO SIMPLY INSERT NEW ELEMENT TO THE RIGHT
        else {
            root->right = element;
        }
    } else {
        // SOMETHING IS IN THE WAY, CONTINUE DOWN LEFT
        if (root->left) {
            root->left = insertItemNode(root->left, element);
        }
            // CLEAR TO SIMPLY INSERT NEW ELEMENT TO THE LEFT
        else {
            root->left = element;
        }
    }

    return root;
}

itemNode *searchBranchNode(itemNode *root, char branch[MAXLEN]) {
    // CHECK TO SEE IF ROOT IS NULL
    if (root != NULL) {
        // SEE IF THE ROOT NAME AND NAME TO INSPECT ARE EQUIVALENT
        if (strcmp(root->name, branch) == 0) {
            return root;
        }
            // SEE IF THE NAME TO INSPECT IS SMALLER THAN ROOT NAME
        else if (strcmp(root->name, branch) > 0) {
            return searchBranchNode(root->left, branch);
        }
        // NAME TO INSPECT IS EQUAL/GREATER THAN ROOT NAME
        return searchBranchNode(root->right, branch);
    }

    return NULL;
}

itemNode *deleteBranchFinal(itemNode *root, itemNode *selected) {

    itemNode *temp;
    itemNode *parent = parentBranch(root, selected);

    // LEAF NODE CASE
    if (isLeafNode(selected)) {
        // CHECK FOR PARENT NODE
        if (!parent) {
            free(root);
            return NULL;
        }
        // COMPARE PARENT NAME AND SELECTED NAME
        if (strcmp(parent->name, selected->name) > 0) {
            free(parent->left);
            parent->left = NULL;
        }
            // PARENT NAME IS LESS THAN SELECTED NAME
        else {
            free(parent->right);
            parent->right = NULL;
        }

        return root;
    }
    // LEFT CHILD ONLY CASE
    if (hasOnlyLeftChildNode(selected)) {
        // CHECK FOR PARENT
        if (!parent) {
            temp = selected->left;
            free(selected);
            return temp;
        }
        // COMPARE PARENT NAME AND SELECTED NAME
        if (strcmp(parent->name, selected->name) > 0) {
            temp = parent->left;
            parent->left = parent->left->left;
            free(temp);
        }
            // PARENT NAME IS LESS THAN SELECTED NAME
        else {
            temp = parent->right;
            parent->right = parent->right->left;
            free(temp);
        }

        return root;
    }
    // RIGHT CHILD ONLY CASE
    if (hasOnlyRightChildNode(selected)) {
        // CHECK FOR PARENT
        if (!parent) {
            temp = selected->right;
            free(selected);
            return temp;
        }
        // COMPARE PARENT NAME AND SELECTED NAME
        if (strcmp(parent->name, selected->name) > 0) {
            temp = parent->left;
            parent->left = parent->left->right;
            free(temp);
        }
            // PARENT NAME IS LESS THAN SELECTED NAME
        else {
            temp = parent->right;
            parent->right = parent->right->right;
            free(temp);
        }

        return root;
    }
    // TWO CHILDREN CASE
    // FIND MINIMUM ITEM NODE
    itemNode *new_del_node = minValNode(selected->right);
    // SAVE DATA
    int save_count = new_del_node->count;
    char save_name[MAXLEN];
    strcpy(save_name, new_del_node->name);
    // DELETE MINIMUM ITEM NODE
    deleteBranchFinal(root, new_del_node);
    // UPDATE SELECTED NODE W/ SAVED DATA
    selected->count = save_count;
    strcpy(selected->name, save_name);

    return root;
}

itemNode *parentBranch(itemNode *root, itemNode *node) {
    //  CHECK TO SEE IF THERE EVEN IS A PARENT OR IF SCOPE IS TOO SMALL
    if (!root || root == node) {
        return NULL;
    }
    // CHECK FOR EQUIVALENCE
    if (root->left == node || root->right == node) {
        return root;
    }
    // SEE IF THE NAME TO INSPECT IS SMALLER THAN ROOT NAME
    if (strcmp(root->name, node->name) > 0) {
        return parentBranch(root->left, node);
    }
    // SEE IF THE NAME TO INSPECT IS GREATER THAN ROOT NAME
    if (strcmp(root->name, node->name) < 0) {
        return parentBranch(root->right, node);
    }

    return NULL;
}

int isLeafNode(itemNode *node) {
    // IS THERE ANY NODE TO THE LEFT OR RIGHT?
    return (node->left == NULL && node->right == NULL);
}

int hasOnlyLeftChildNode(itemNode *node) {
    // VERIFY THERES A NODE TO THE LEFT BUT NOT THE RIGHT
    return (node->left != NULL && node->right == NULL);
}

int hasOnlyRightChildNode(itemNode *node) {
    // VERIFY THERES A NODE TO THE RIGHT BUT NOT THE LEFT
    return (node->left == NULL && node->right != NULL);
}

itemNode *minValNode(itemNode *root) {
    // GRABS THE SMALLEST TREE (ALL THE WAY LEFT)
    if (!root->left) {
        return root;
    }

    // KEEP RECALLING UNTIL WE FIND A NULL VALUE
    return minValNode(root->left);
}

//------------------------------------------------------------------------------
//                                    FREE                                    //
////////////////////////////////////////////////////////////////////////////////

treeNameNode *freeTree(treeNameNode *root, treeNameNode *selected) {
    // FREE BRANCH ENTIRELY
    while (selected->theTree) {
        selected->theTree = deleteBranchFinal(selected->theTree, selected->theTree);
    }
    // DELETE TREE ENTIRELY
    root = freeTreeFinal(root, selected);

    return root;
}

treeNameNode *freeTreeFinal(treeNameNode *root, treeNameNode *selected) {

    treeNameNode *temp;
    treeNameNode *parent = parentTree(root, selected);
    // LEAF NODE CASE
    if (isLeaf(selected)) {
        // CHECK FOR PARENT NODE
        if (!parent) {
            free(root);
            return NULL;
        }
        // COMPARE PARENT NAME AND SELECTED NAME
        if (strcmp(parent->treeName, selected->treeName) > 0) {
            free(parent->left);
            parent->left = NULL;
        }
            // PARENT NAME IS LESS THAN SELECTED NAME
        else {
            free(parent->right);
            parent->right = NULL;
        }

        return root;
    }
    // LEFT CHILD ONLY CASE
    if (hasOnlyLeftChild(selected)) {
        // CHECK FOR PARENT
        if (!parent) {
            temp = selected->left;
            free(selected);
            return temp;
        }
        // COMPARE PARENT NAME AND SELECTED NAME
        if (strcmp(parent->treeName, selected->treeName) > 0) {
            temp = parent->left;
            parent->left = parent->left->left;
            free(temp);
        }
            // PARENT NAME IS LESS THAN SELECTED NAME
        else {
            temp = parent->right;
            parent->right = parent->right->left;
            free(temp);
        }

        return root;
    }
    // RIGHT CHILD ONLY CASE
    if (hasOnlyRightChild(selected)) {
        // CHECK FOR PARENT
        if (!parent) {
            temp = selected->right;
            free(selected);
            return temp;
        }
        // COMPARE PARENT NAME AND SELECTED NAME
        if (strcmp(parent->treeName, selected->treeName) > 0) {
            temp = parent->left;
            parent->left = parent->left->right;
            free(temp);
        }
            // PARENT NAME IS LESS THAN SELECTED NAME
        else {
            temp = parent->right;
            parent->right = parent->right->right;
            free(temp);
        }

        return root;
    }
    // TWO CHILDREN CASE
    // FIND MINIMUM TREE NODE
    treeNameNode *new_del_node = minVal(selected->right);
    // SAVE DATA
    char treeName[MAXLEN];
    strcpy(treeName, new_del_node->treeName);
    itemNode *theTree = new_del_node->theTree;
    // DELETE MINIMUM TREE NODE
    freeTreeFinal(root, new_del_node);
    // UPDATE SELECTED NODE W/ SAVED DATA
    strcpy(selected->treeName, treeName);
    selected->theTree = theTree;

    return root;
}

void freeAllMem(treeNameNode *root) {
    // POST ORDER STYLE RECURSIVE FREE
    if (root) {
        freeAllMem(root->left);
        freeAllMem(root->right);
        freeTreeMem(root->theTree);
        free(root);
    }
}

void freeTreeMem(itemNode *root) {
    // POST ORDER STYLE RECURSIVE TREE FREE 
    if (root) {
        freeTreeMem(root->left);
        freeTreeMem(root->right);
        free(root);
    }
}

