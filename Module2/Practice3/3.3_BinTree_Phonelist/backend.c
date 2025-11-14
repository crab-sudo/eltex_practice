#include "backend.h"

static int max(int a, int b) {
    return (a > b) ? a : b;
}

static int height(Person* N) {
    if (N == NULL) return 0;
    return N->height;
}

static Person* rightRotate(Person* y) {
    Person* x = y->left;
    Person* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

static Person* leftRotate(Person* x) {
    Person* y = x->right;
    Person* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

static int getBalance(Person* N) {
    if (N == NULL) return 0;
    return height(N->left) - height(N->right);
}

static Person* minValueNode(Person* node) {
    Person* current = node;
    while (current->left != NULL) current = current->left;
    return current;
}

void Init(Person** head) {
    *head = NULL;
}

static void freePostOrder(Person* node) {
    if (!node) return;
    freePostOrder(node->left);
    freePostOrder(node->right);
    free(node);
}

void FreeAll(Person** head) {
    if (!head || !*head) return;
    freePostOrder(*head);
    *head = NULL;
}

void StatusToString(STATUS status, char new_status[15]) {
    if (status == CODE_0)               strcpy(new_status, "CODE_0"             );
    if (status == ERR_FULL_ARRAY)       strcpy(new_status, "ERR_FULL_ARRAY"     );
    if (status == ERR_NO_FIRST_NAME)    strcpy(new_status, "ERR_NO_FIRST_NAME"  );
    if (status == ERR_NO_SECOND_NAME)   strcpy(new_status, "ERR_NO_SECOND_NAME" );
    if (status == ERR_INCORECT_ID)      strcpy(new_status, "ERR_INCORECT_ID"    );
}

static bool existsInTree(Person* root, unsigned int id) {
    Person* cur = root;
    while (cur) {
        if (id == cur->id) return true;
        else if (id < cur->id) cur = cur->left;
        else cur = cur->right;
    }
    return false;
}

unsigned int GeneratorID(Person* head) {
    unsigned int id;
    bool exists;
    srand((unsigned int)time(NULL));
    do {
        exists = false;
        id = (rand() % 9000) + 1000;
        if (existsInTree(head, id)) exists = true;
    } while (exists);
    return id;
}

/* Internal recursive insert for AVL by id */
static Person* insertNode(Person* node, Person* new_person) {
    if (node == NULL) return new_person;

    if (new_person->id < node->id)
        node->left = insertNode(node->left, new_person);
    else if (new_person->id > node->id)
        node->right = insertNode(node->right, new_person);
    else
        return node; /* duplicate ids shouldn't happen */

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    /* Left Left */
    if (balance > 1 && new_person->id < node->left->id)
        return rightRotate(node);

    /* Right Right */
    if (balance < -1 && new_person->id > node->right->id)
        return leftRotate(node);

    /* Left Right */
    if (balance > 1 && new_person->id > node->left->id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    /* Right Left */
    if (balance < -1 && new_person->id < node->right->id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

STATUS Add(Person** head,   char* fname, 
                            char* sname, 
                            char* pname, 
                            char* job, 
                            char* vacancy, 
                            char* nomb, 
                            char* email, 
                            char* mess) {

    if (!strcmp(fname, "-")) return ERR_NO_FIRST_NAME;
    if (!strcmp(sname, "-")) return ERR_NO_SECOND_NAME;

    Person* new_person = (Person*)malloc(sizeof(Person));
    if (!new_person) return ERR_FULL_ARRAY;

    new_person->id = GeneratorID(*head);
    strcpy( new_person->fname,   fname  );
    strcpy( new_person->sname,   sname  );
    strcpy( new_person->pname,   pname  );
    strcpy( new_person->job,     job    );
    strcpy( new_person->vacancy, vacancy);
    strcpy( new_person->nomb,    nomb   );
    strcpy( new_person->email,   email  );
    strcpy( new_person->mess,    mess   );

    new_person->left = NULL;
    new_person->right = NULL;
    new_person->height = 1;

    if (!*head) {
        *head = new_person;
        return CODE_0;
    }

    *head = insertNode(*head, new_person);
    return CODE_0;
}

STATUS Edit(Person* head, unsigned int id,  char* fname, 
                                            char* sname, 
                                            char* pname, 
                                            char* job, 
                                            char* vacancy, 
                                            char* nomb, 
                                            char* email, 
                                            char* mess) {

    Person* tmp = head;
    while (tmp && tmp->id != id) {
        if (id < tmp->id) tmp = tmp->left;
        else tmp = tmp->right;
    }
    if (!tmp) return ERR_INCORECT_ID;

    if (strcmp( fname,      "-"))   strcpy( tmp->fname,     fname  );
    if (strcmp( sname,      "-"))   strcpy( tmp->sname,     sname  );
    if (strcmp( pname,      "-"))   strcpy( tmp->pname,     pname  );
    if (strcmp( job,        "-"))   strcpy( tmp->job,       job    );
    if (strcmp( vacancy,    "-"))   strcpy( tmp->vacancy,   vacancy);
    if (strcmp( nomb,       "-"))   strcpy( tmp->nomb,      nomb   );
    if (strcmp( email,      "-"))   strcpy( tmp->email,     email  );
    if (strcmp( mess,       "-"))   strcpy( tmp->mess,      mess   );

    return CODE_0;
}

/* delete node and rebalance */
static Person* deleteNode(Person* root, unsigned int id, STATUS* out_status) {
    if (root == NULL) {
        if (out_status) *out_status = ERR_INCORECT_ID;
        return root;
    }

    if (id < root->id)
        root->left = deleteNode(root->left, id, out_status);
    else if (id > root->id)
        root->right = deleteNode(root->right, id, out_status);
    else {
        /* node with only one child or no child */
        if ((root->left == NULL) || (root->right == NULL)) {
            Person* temp = root->left ? root->left : root->right;

            if (temp == NULL) {
                /* no child */
                temp = root;
                root = NULL;
            } else {
                /* one child */
                *root = *temp;
            }
            free(temp);
        } else {
            /* node with two children */
            Person* temp = minValueNode(root->right);
            root->id = temp->id;
            strcpy(root->fname, temp->fname);
            strcpy(root->sname, temp->sname);
            strcpy(root->pname, temp->pname);
            strcpy(root->job, temp->job);
            strcpy(root->vacancy, temp->vacancy);
            strcpy(root->nomb, temp->nomb);
            strcpy(root->email, temp->email);
            strcpy(root->mess, temp->mess);

            root->right = deleteNode(root->right, temp->id, out_status);
        }
    }

    if (root == NULL) return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    /* Left Left */
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    /* Left Right */
    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    /* Right Right */
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    /* Right Left */
    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

STATUS Delete(Person** head, unsigned int id) {
    if (!*head) return ERR_INCORECT_ID;
    STATUS st = CODE_0;
    *head = deleteNode(*head, id, &st);
    return st;
}

STATUS CommandParser(Person** head, char* format, ...) {
    STATUS status = CODE_0;
    unsigned int id = 0;
    char fname  [20]    = "-";
    char sname  [20]    = "-";
    char pname  [20]    = "-";
    char job    [30]    = "-";
    char vacancy[20]    = "-";
    char nomb   [17]    = "-";
    char email  [30]    = "-";
    char mess   [30]    = "-";

    va_list args;
    va_start(args, format);

    for (char* c = format; *c; c++) {
        if (*c != '%') continue;
        switch (*++c) {
            case 'a':
                status = Add(   head,   fname, 
                                        sname, 
                                        pname, 
                                        job, 
                                        vacancy, 
                                        nomb, 
                                        email, 
                                        mess);                  break;

            case 'x':
                status = Edit(  *head,  id, 
                                        fname, 
                                        sname, 
                                        pname, 
                                        job, 
                                        vacancy, 
                                        nomb, 
                                        email, 
                                        mess);                  break;

            case 'd':
                status = Delete(head, id);                      break;


            case 'i':
                id = (unsigned int)atoi(va_arg(args, char*));   break;

            case 'f':
                strcpy(fname, va_arg(args, char*));             break;

            case 's':
                strcpy(sname, va_arg(args, char*));             break;

            case 'p':
                strcpy(pname, va_arg(args, char*));             break;

            case 'j':
                strcpy(job, va_arg(args, char*));               break;

            case 'v':
                strcpy(vacancy, va_arg(args, char*));           break;

            case 'n':
                strcpy(nomb, va_arg(args, char*));              break;

            case 'e':
                strcpy(email, va_arg(args, char*));             break;
                
            case 'm':
                strcpy(mess, va_arg(args, char*));              break;
        }
    }

    va_end(args);
    return status;
}

/* In-order traversal to build table rows (ascending id) */
static void inorder_table(Person* node, char str_persons[][200], unsigned int* idx) {
    if (!node) return;
    inorder_table(node->left, str_persons, idx);
    sprintf(str_persons[(*idx)++], "| %-4u | %-15s | %-15s | %-15s | %-20s | %-20s | %-15s | %-15s | %-15s |",
                                node->id, 
                                node->fname, 
                                node->sname, 
                                node->pname, 
                                node->job, 
                                node->vacancy, 
                                node->nomb, 
                                node->email, 
                                node->mess);
    inorder_table(node->right, str_persons, idx);
}

/* Print tree rotated: right on top, node, left below.
   Each level adds indentation. This produces an ASCII "drawing" of tree.
   It writes lines into the same str_persons array starting at *idx. */
static void printTreeRotated(Person* node, int space, char lines[][200], unsigned int* idx) {
    if (!node) return;

    const int COUNT = 6; /* spacing increment */
    space += COUNT;

    printTreeRotated(node->right, space, lines, idx);

    char buf[200];
    int i;
    for (i = 0; i < space - COUNT; ++i) buf[i] = ' ';
    buf[i] = '\0';

    char nodeinfo[160];
    /* show only id in tree drawing as requested */
    sprintf(nodeinfo, "%u", node->id);
    snprintf(buf + i, sizeof(buf) - i, "%s", nodeinfo);

    strncpy(lines[(*idx)++], buf, 200);

    if (node->left || node->right) {
        char branch[200];
        int j;

        /* indentation */
        for (j = 0; j < space - COUNT; ++j) branch[j] = ' ';
        branch[j] = '\0';

        /* add / and \ depending on children */
        if (node->right && node->left)
            snprintf(branch + j, sizeof(branch) - j, "/ \\");
        else if (node->right)
            snprintf(branch + j, sizeof(branch) - j, "\\");
        else if (node->left)
            snprintf(branch + j, sizeof(branch) - j, "/");

    strncpy(lines[(*idx)++], branch, 200);
}

    printTreeRotated(node->left, space, lines, idx);
}

void PersonToString(Person* head, char str_persons[][200], unsigned int* count) {
    unsigned int i = 0;

    /* 1) build table rows (in-order traversal -> ascending ids) */
    if (head) {
        inorder_table(head, str_persons, &i);
    }

    /* 2) append an empty separator line before tree for readability */
    if (head) {
        if (i < 100) strncpy(str_persons[i++], "", 200);
    } else {
        /* tree empty -> show message */
        snprintf(str_persons[i++], 200, "(empty tree)");
        *count = i;
        return;
    }

    /* 3) append ASCII tree (rotated) showing only ids */
    printTreeRotated(head, 0, str_persons, &i);

    *count = i;
}
