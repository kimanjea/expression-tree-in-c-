//
// Implements the ExpressionTree Class
// Author: Max Benson
// Date: 10/27/2021
//

#include <iostream>
#include <sstream>
using std::cout;
using std::endl;
using std::string;
using std::stringstream;

#include "Stack.h"
#include "ExpressionTree.h"

// Token testing routines
bool IsNumber(string token);
bool IsVariable(string token);
bool IsOperator(string token);

/**
 * Default constructor
 * Creates an "null tree"
 */
ExpressionTree::ExpressionTree() {
    _root=nullptr;
}

/**
 * Destructor
 * Frees the dynamic memory allocated for the tree
 */
ExpressionTree::~ExpressionTree() {
    delete _root;
}

/**
 * Build an expression tree from its postfix representation
 * In case of error the stack is cleaned up.  Because it contains
 * pointers to TreeNodes, if any are left on the stack they must be
 * explicitly deleted
 * @param postfix string representation of tree
 * @return true if postfix valid and tree was built, false otherwise
 */
bool ExpressionTree::BuildExpressionTree(const string& postfix) {
    stringstream ss(postfix);
    string token;
    Stack <TreeNode*>TreeObjects;

    while(ss >> token) {
        if(IsNumber(token)|| IsVariable(token)){
            if(IsNumber(token)) {
                TreeNode *newObject = new TreeNode(NumberOperand, token);
                TreeObjects.Push(newObject);
            } else if  (IsVariable(token)){
                TreeNode *newObject = new TreeNode(VariableOperand, token);
                TreeObjects.Push(newObject);
            }
        } else if (IsOperator(token)){
            if(TreeObjects.Size()<2){
                cout << "Error" << endl;
                while(!TreeObjects.IsEmpty()){
                    TreeNode *A=TreeObjects.Pop();
                    delete A;
                }
                return false;
            }
            TreeNode *operand2=TreeObjects.Peek();
            TreeObjects.Pop();
            TreeNode *operand1=TreeObjects.Peek();
            TreeObjects.Pop();

            TreeNode *newObject = new TreeNode(Operator, token);
            newObject->SetLeft(operand1);
            newObject->SetRight(operand2);
            TreeObjects.Push(newObject);

        } else {
            cout << "Error" << endl;
            while(!TreeObjects.IsEmpty()){
                TreeNode *A =TreeObjects.Pop();
                delete A;
            }
            return false;
        }
    }
    if(TreeObjects.Size()!=1){
        cout << "Error" << endl;
        while(!TreeObjects.IsEmpty()){
           TreeNode *A= TreeObjects.Pop();
           delete A;
        }
        return false;
    }
    _root=TreeObjects.Pop();
    return true;
}

/**
 * Recursively simplify an expression stored in an expression tree.  THe following simplications are performed
 * - Addition, multiplication, and subtraction of constants is performed reducing the subtree to a leaf containing a number
 * - 0 + exp, exp + 0, exp - 0  will be reduced to exp, in general exp will a tree
 * - 1 * exp, exp * 1  will be reduced to exp, in general exp will a tree
 * - 0 * exp, exp * 0  will be reduce to a leaf containing 0
 * - exp - exp will be reduce to a leaf containing 0
 * - exp * number will be changed to number * exp
 * - (c1 * exp) + (c2 * exp) where c1, c2 are numbers  will be changed to (c1+c2) * exp
 * - (c1 * exp) - (c2 * exp) where c1, c2 are numbers will be changed to (c1-c2) * exp
 * pointers to TreeNodes, if any are left on the stack they must be
 * explicitly deleted
 * @param postfix string representation of tree
 * @return true if postfix valid and tree was built, false otherwise
 */
TreeNode* ExpressionTree::SimplifyTree(TreeNode* tree) {

    if(IsOperator(tree->Data())) {

        if(tree->Data()=="*") {
            if (tree->Left()->IsNumber() && IsOperator(tree->Right()->Data())) {
                tree->SetRight(SimplifyTree(tree->Right()));
            }
            if(tree->Right()->IsNumber() && IsOperator(tree->Left()->Data())) {
                tree->SetLeft(SimplifyTree(tree->Left()));
            }
            if(IsVariable(tree->Left()->Data()) && IsOperator(tree->Right()->Data())) {
                tree->SetRight(SimplifyTree(tree->Right()));
            }
            if(IsVariable(tree->Right()->Data()) && IsOperator(tree->Left()->Data())) {
                tree->SetLeft(SimplifyTree(tree->Left()));
            }
            if(IsOperator(tree->Right()->Data()) && IsOperator(tree->Left()->Data())) {
                if(IsSameTree(tree->Right(),tree->Left())) {
                    string treeValue="0";
                    TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                    delete tree;
                    return returnTree;
                } else{
                    tree->SetLeft(SimplifyTree(tree->Left()));
                    tree->SetRight(SimplifyTree(tree->Right()));
                }
            }
            if (tree->Left() != nullptr && tree->Left()->IsNumber() && tree->Right() != nullptr && tree->Right()->IsNumber()) {
                stringstream ss;
                stringstream ss2;
                int num;
                int num2;
                int final=0;
                ss<<tree->Left()->Data();
                ss>>num;
                ss2<<tree->Right()->Data();
                ss2>>num2;

                final=num*num2;
                string treeValue= to_string(final);
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;

            } else if (tree->Left() != nullptr && tree->Left()->IsZero() && tree->Right() != nullptr &&
                       tree->Right()->IsNumber()) {
                string treeValue="0";
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;
            } else if (tree->Left() != nullptr && tree->Left()->IsNumber() && tree->Right() != nullptr &&
                       tree->Right()->IsZero()) {
                string treeValue="0";
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;
            } else if (tree->Left()!=nullptr && IsVariable(tree->Left()->Data()) && tree->Right()!=nullptr && IsVariable(tree->Right()->Data())) {
                return tree;
            } else if(tree->Left()!=nullptr && IsVariable(tree->Left()->Data()) && tree->Right()!=nullptr && tree->Right()->IsZero()) {
                string treeValue="0";
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;
            } else if(tree->Left()!=nullptr && IsVariable(tree->Right()->Data()) && tree->Right()!=nullptr && tree->Left()->IsZero()) {
                string treeValue="0";
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;
            }else if(tree->Left()!=nullptr && tree->Left()->IsNumber() && tree->Right()!=nullptr && IsVariable(tree->Right()->Data())) {
                return tree;
            } else if(tree->Left()!=nullptr && tree->Right()->IsNumber() && tree->Right()!=nullptr && IsVariable(tree->Left()->Data())) {
                return tree;
            }else if(tree->Left()!=nullptr && tree->Right()!=nullptr && tree->Left()->IsZero()) {
                string treeValue="0";
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;
            } else if(tree->Left()!=nullptr && tree->Right()!=nullptr && tree->Right()->IsZero()) {
                string treeValue="0";
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;
            } else if(tree->Left()!=nullptr && tree->Right()!=nullptr && tree->Left()->IsZero()) {
                string treeValue="0";
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;

            } else {

                return tree;
            }

        }
        else if(tree->Data()=="-"){
            if (tree->Left()->IsNumber() && IsOperator(tree->Right()->Data())) {
                tree->SetRight(SimplifyTree(tree->Right()));
            }
            if(tree->Right()->IsNumber() && IsOperator(tree->Left()->Data())) {
                tree->SetLeft(SimplifyTree(tree->Left()));
            }
            if(IsVariable(tree->Left()->Data()) && IsOperator(tree->Right()->Data())) {
                tree->SetRight(SimplifyTree(tree->Right()));
            }
            if(IsVariable(tree->Right()->Data()) && IsOperator(tree->Left()->Data())) {
                tree->SetLeft(SimplifyTree(tree->Left()));
            }
            if(IsOperator(tree->Right()->Data()) && IsOperator(tree->Left()->Data())) {
                if(IsSameTree(tree->Right(),tree->Left())) {
                    string treeValue="0";
                    TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                    delete tree;
                    return returnTree;
                } else{
                    tree->SetLeft(SimplifyTree(tree->Left()));
                    tree->SetRight(SimplifyTree(tree->Right()));
                }
            }
            if (tree->Left() != nullptr && tree->Left()->IsNumber() && tree->Right() != nullptr && tree->Right()->IsNumber()) {
                stringstream ss;
                stringstream ss2;
                int num;
                int num2;
                int final=0;
                ss<<tree->Left()->Data();
                ss>>num;
                ss2<<tree->Right()->Data();
                ss2>>num2;

                final=num-num2;
                string treeValue= to_string(final);
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;

            }
            else if (tree->Left() != nullptr && tree->Left()->IsZero() && tree->Right() != nullptr && tree->Right()->IsNumber()) {
                stringstream ss2;
                int num2;
                ss2<<tree->Right()->Data();
                ss2>>num2;
                string treeValue= to_string(num2);
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;

            }
            else if (tree->Left() != nullptr && tree->Left()->IsNumber() && tree->Right() != nullptr && tree->Right()->IsZero()) {
                stringstream ss;
                int num;
                ss<<tree->Left()->Data();
                ss>>num;
                string treeValue= to_string(num);
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;

            }
            else if (tree->Left()!=nullptr && tree->Right()!=nullptr && IsSameTree(tree->Left(),tree->Right())) {
                string treeValue="0";
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;
            }
            else if (tree->Left()!=nullptr && IsVariable(tree->Left()->Data()) && tree->Right()!=nullptr && IsVariable(tree->Right()->Data())) {
                return tree;
            }
            else if(tree->Left()!=nullptr && IsVariable(tree->Left()->Data()) && tree->Right()!=nullptr && tree->Right()->IsZero()) {
                TreeNode * returnTree= new TreeNode(VariableOperand,tree->Left()->Data());
                delete tree;
                return returnTree;
            }
            else if(tree->Left()!=nullptr && IsVariable(tree->Right()->Data()) && tree->Right()!=nullptr && tree->Left()->IsZero()) {
                   string r ="-";
                   string final;
                   final= r+tree->Right()->Data();
                TreeNode * returnTree= new TreeNode(VariableOperand,final);
                delete tree;
                return returnTree;
            }
            else if(tree->Left()!=nullptr && IsVariable(tree->Left()->Data()) && tree->Right()!=nullptr && tree->Right()->IsNumber()) {
                return tree;
            }
            else if(tree->Left()!=nullptr && IsVariable(tree->Right()->Data()) && tree->Right()!=nullptr && tree->Left()->IsNumber()) {
                return tree;
            }
            else if(tree->Left()!=nullptr && tree->Left()->IsNumber() && tree->Right()!=nullptr && IsVariable(tree->Right()->Data())) {
                return tree;
            }
            else if(tree->Left()!=nullptr && tree->Right()->IsNumber() && tree->Right()!=nullptr && IsVariable(tree->Left()->Data())) {
                return tree;
            }
            else {
                return tree;
            }
        }
        else if(tree->Data()=="+") {
            if (tree->Left()->IsNumber() && IsOperator(tree->Right()->Data())) {
                tree->SetRight(SimplifyTree(tree->Right()));
            }
            if(tree->Right()->IsNumber() && IsOperator(tree->Left()->Data())) {
                tree->SetLeft(SimplifyTree(tree->Left()));
            }
            if(IsVariable(tree->Left()->Data()) && IsOperator(tree->Right()->Data())) {
                tree->SetRight(SimplifyTree(tree->Right()));
            }
            if(IsVariable(tree->Right()->Data()) && IsOperator(tree->Left()->Data())) {
                tree->SetLeft(SimplifyTree(tree->Left()));
            }
            if(IsOperator(tree->Right()->Data()) && IsOperator(tree->Left()->Data())) {
                if(IsSameTree(tree->Right(),tree->Left())) {
                    string treeValue="0";
                    TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                    delete tree;
                    return returnTree;
                } else{
                    tree->SetLeft(SimplifyTree(tree->Left()));
                    tree->SetRight(SimplifyTree(tree->Right()));
                }
            }
            if (tree->Left() != nullptr && tree->Left()->IsNumber() && tree->Right() != nullptr && tree->Right()->IsNumber()) {
                stringstream ss;
                stringstream ss2;
                int num;
                int num2;
                int final=0;
                ss<<tree->Left()->Data();
                ss>>num;
                ss2<<tree->Right()->Data();
                ss2>>num2;

                final=num+num2;
                string treeValue= to_string(final);
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;

            } else if (tree->Left() != nullptr && tree->Left()->IsZero() && tree->Right() != nullptr &&
                       tree->Right()->IsNumber()) {
                stringstream ss2;
                int num2;
                ss2<<tree->Right()->Data();
                ss2>>num2;
                string treeValue= to_string(num2);
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;
            } else if (tree->Left() != nullptr && tree->Left()->IsNumber() && tree->Right() != nullptr &&
                       tree->Right()->IsZero()) {
                stringstream ss;
                int num;
                ss<<tree->Left()->Data();
                ss>>num;
                string treeValue= to_string(num);
                TreeNode *returnTree = new TreeNode(NumberOperand,treeValue);
                delete tree;
                return returnTree;
            } else if (tree->Left()!=nullptr && IsVariable(tree->Left()->Data()) && tree->Right()!=nullptr && IsVariable(tree->Right()->Data())) {
                return tree;
            } else if(tree->Left()!=nullptr && IsVariable(tree->Left()->Data()) && tree->Right()!=nullptr && tree->Right()->IsZero()) {

                 TreeNode * returnTree= new TreeNode(VariableOperand,tree->Left()->Data());
                delete tree;
                 return returnTree;
            } else if(tree->Left()!=nullptr && IsVariable(tree->Right()->Data()) && tree->Right()!=nullptr && tree->Left()->IsZero()) {

                TreeNode * returnTree= new TreeNode(VariableOperand,tree->Right()->Data());
                delete tree;
                return returnTree;
            }  else if(tree->Left()!=nullptr && IsVariable(tree->Left()->Data()) && tree->Right()!=nullptr && tree->Right()->IsNumber()) {
                return tree;
            }else if(tree->Left()!=nullptr && IsVariable(tree->Right()->Data()) && tree->Right()!=nullptr && tree->Left()->IsNumber()) {
                return tree;
            }else if(tree->Left()!=nullptr && tree->Left()->IsNumber() && tree->Right()!=nullptr && IsVariable(tree->Right()->Data())) {
                return tree;
            } else if(tree->Left()!=nullptr && tree->Right()->IsNumber() && tree->Right()!=nullptr && IsVariable(tree->Left()->Data())) {
                return tree;
            }else {
                return tree;
            }

        }

    } else {
            return tree;

    }

}

/**
 * Determine whether two tree structures represent the same expression
 * @param tree1 first tree structure
 * @param tree2 second tree structure
 * @return true if same, false otherwise
 */
bool ExpressionTree::IsSameTree(TreeNode* tree1, TreeNode* tree2) const {
    bool same;
    same =false;
    if( tree1->Data()==tree2->Data() ){
        if(tree1->Left()!=nullptr && tree2->Left()!=nullptr && tree2->Right()!=nullptr &&tree1->Right()!=nullptr) {
            if( IsSameTree(tree1->Right(), tree2->Right()) && IsSameTree(tree1->Left(), tree2->Left())) {
            same = true;
            }
        } else {
            same=true;
        }

    }
    return  same;
}

/**
 * Produce an infix representation of the tree structure
 * @param tree
 * @param fNeedOuterParen - caller will generatlly pass false to eliminate outer set of paraentheses, recursive calls pass true
 * @return string representation
 */
string ExpressionTree::ToString(TreeNode* tree, bool fNeedOuterParen) const {
    string s;

    if (Operator == tree->Type()) {
        if (fNeedOuterParen) {
            s += "(";
        }
        s += ToString(tree->Left(), true);
        s += tree->Data();
        s += ToString(tree->Right(), true);
        if (fNeedOuterParen) {
            s += ")";
        }
    } else {
        s += tree->Data();
    }
    return s;
}

/**
 * Scans token to see if all characters are digits
 * @param token a string
 * @return true if nonempty and contains all digits, otherwise false
 */
bool IsNumber(string token) {
    if (token.length() == 0) {
        return false;
    }
    for (size_t i = 0; i < token.length(); i ++) {
        if (!isdigit(token[i]) ) {
            return false;
        }
    }
    return true;
}

/**
 * Scans token to see if it starts with letter and rest are letter or digits
 * @param token a string
 * @return true if nonempty,starts with letter, and rest are letters or digits
 */
bool IsVariable(string token)  {
    if (token.length() == 0 || !isalpha(token[0])) {
        return false;
    }
    for (size_t i = 0; i < token.length(); i ++) {
        if (!isalnum(token[i]) ) {
            return false;
        }
    }
    return true;
}

/**
 * CHecks if the stirng is a +, -, or *
 * @param token a string
 * @return true if its an operator
 */
bool IsOperator(string token)  {
    return (token.length() == 1 && (token[0] == '+' || token[0] == '-' || token[0] == '*'));
}

