#include "semantics.h"

static int scope = 0;
int loc = 0;
int loopCount = 0;
int variablesOnStack = 0;
int place = 0;
string variableValueTracker[100];
string variablesOnTheStack[100];
int numOfVariablesInUse = 0;

FILE *file;

token *temp;

void codeGeneration(Node *root, char *name) {

  //printf("\nEntered Code Generation.\n");
  //printf("\nEntered Code Generation.\nSemantics Position\tStack Information\n\n");

  /* Create the .asm output file w/ the input filename */
	string tempFile(name);
	string fileName = tempFile.substr(0, tempFile.find("."));
	fileName.append(".asm");

	file = fopen(fileName.c_str(), "w");

  /* If unable to open the output file = Error */
	if(file == NULL) {
		printf("SEMANTICS ERROR : Could not write output file.\n Aborted.\n");
		exit(EXIT_FAILURE);
	}

  /* Begin code generation w/ the root node */
	semantics(root);

  /* At the end of code generation, add final statements to output file */
	//popFromRealStack(scope, file, variablesOnStack);
  fprintf(file, "STOP\n");
  int temp = numOfVariablesInUse;
  numOfVariablesInUse = 0;
  place++;

  /* Print the variable declaractions */
  for (int i = 0; i < temp; i++) {
		fprintf(file, "T%i %s\n", numOfVariablesInUse, variableValueTracker[numOfVariablesInUse].c_str());
    numOfVariablesInUse++;
	}

  printf("\n\nCode Generation completed - please see output file '%s'.\n\n", fileName.c_str());

}

/* Recursive semantics() - calls itself and reacts based on the node*/
void semantics(Node *root) {

  //printf(":::%s:::\n", root->name.c_str());

  /* If the node is empty, explicit return */
  if(root == NULL) {
      return;
  }

  /*if (root->child1 != NULL) {
    printf("%s\n", root->child1->name.c_str());
    if (root->child2 != NULL) {
      printf("%s\n", root->child2->name.c_str());
      if (root->child3 != NULL)
        printf("%s\n", root->child3->name.c_str());
      }
    }*/

	/* <program> → void <vars> <block> */
	if(root->name == "<program>") {

    //printf(":::PROGRAM:::\n");
  		semantics(root->child1);
  		semantics(root->child2);
	}

	/* <block> → begin <vars> <stats> end */
	if(root->name == "<block>") {

    //printf(":::BLOCK:::\n");
		scope++;

		semantics(root->child1);
		semantics(root->child2);
		popFromRealStack(scope, file, variablesOnStack);
		scope--;

	}

	/* <vars> → empty | let Identifier = integer <vars> */
	if(root->name == "<vars>") {
//printf(":::VARS:::\n");
		if(!root->TK.empty()) {
			temp = root->TK.front();
			root->TK.erase(root->TK.begin());

			stack test;
			test.tokenWord = temp->tokenWord;
			test.scope = scope;
			test.lineNum = temp->line;

      //printf("Declared the variable %s\n", temp->tokenWord.c_str());
      //variablesOnTheStack[numOfVariablesInUse] = temp->tokenWord;
      variableValueTracker[numOfVariablesInUse] = temp->tokenWord;

      //numOfVariablesInUse++;

      string local = "T" + std::to_string(place);
			//place++;
      //printf("\t\t\tTOKEN: %s\tLINE: %i\tSCOPE: %i\n", test.tokenWord.c_str(), test.lineNum, test.scope);

			if(temp->tokenID == 1001) {
        check(temp);
				pushToRealStack(test, file);
        fprintf(file, "LOAD %s\n", local.c_str());
        fprintf(file, "STACKW 0\n");
        //printf("VARS 1 : %s\n", local.c_str());

        temp = root->TK.front();
  			root->TK.erase(root->TK.begin());
        //printf("2 Declared the variable %s\n", temp->tokenWord.c_str());
        variableValueTracker[numOfVariablesInUse] = temp->tokenWord.c_str();



				semantics(root->child1);
				variablesOnStack++;
			} else if (temp->tokenID == 1002) {

        //printf("VARS 2 : %s\n", temp->tokenWord.c_str());
        variablesOnTheStack[numOfVariablesInUse] = temp->tokenWord.c_str();
      }
		} else {

		}
	}

	/* <expr> → <A> / <expr> | <A> * <expr> | <A> */
	if(root->name == "<expr>") {
//printf(":::EXPR:::\n");
		if(root->child2 != NULL) {

      string local = "T" + std::to_string(place);
			place++;

      temp = root->TK.front();
			root->TK.erase(root->TK.begin());

			/* <A> / <expr> */
			if(temp->tokenID == 1026) {
				semantics(root->child1);

            //printf("EXPR 1 : %s\n", temp->tokenWord.c_str());

        fprintf(file, "STORE %s\n", local.c_str());
				semantics(root->child2);
        fprintf(file, "DIVI %s\n", local.c_str());

				/* <A> * <expr> */
			} else if(temp->tokenID == 1013) {
				semantics(root->child2);

            //printf("EXPR 2 : %s\n", temp->tokenWord.c_str());

				fprintf(file, "STORE %s\n", local.c_str());
				semantics(root->child1);
				fprintf(file, "MULTI %s\n", local.c_str());

			}

		} else {
      semantics(root->child1);
      semantics(root->child2);

	}}

	/* <A> → <M> + <A> | <M> - <A> | <M> */
	if(root->name == "<A>") {
//printf(":::A:::\n");
		if(!root->TK.empty()) {
      string local = "T" + std::to_string(place);
			//place++;

			temp = root->TK.front();
			root->TK.erase(root->TK.begin());

			/* <M> + <A> */
			if(temp->tokenID == 1011) {
				semantics(root->child1);
        //printf("A + : %s\n", temp->tokenWord.c_str());

        fprintf(file, "STORE %s\n", local.c_str());
				semantics(root->child2);
        fprintf(file, "ADD %s\n", local.c_str());


				/* <M> - <A> */
			} else if(temp->tokenID == 1012) {
				fprintf(file, "STORE %s\n", local.c_str());
				semantics(root->child1);
				fprintf(file, "SUB %s\n", local.c_str());

			}
		} else {
      semantics(root->child1);
      semantics(root->child2);

		}
	}

	/* <M> → - <M> | <R> */
	if(root->name == "<M>") {
//printf(":::M:::\n");
		if(root->child1 == NULL) {
			semantics(root->child2);

		} else {
			semantics(root->child1);
			fprintf(file, "MULT -1\n");

		}
	}

	/* <R> → ( <expr> ) | Identifier | Number */
	if(root->name == "<R>") {
//printf(":::R:::\n");

		if(!root->TK.empty()) {
			temp = root->TK.front();
			root->TK.erase(root->TK.begin());
			stack stk;

			stk.tokenWord = temp->tokenWord;
			stk.lineNum = temp->line;
			stk.scope = scope;

    /*  variableValueTracker[place] = stk.tokenWord;
      printf("R2 %s\n", variableValueTracker[place].c_str());
      string local = "T" + std::to_string(place);
			//place++;*/

			if(temp->tokenID == 1001) {
				fprintf(file, "STACKR %i\n", find(stk));
        variableValueTracker[numOfVariablesInUse] = temp->tokenWord;
        //printf("R1 %s\n", variableValueTracker[numOfVariablesInUse].c_str());

			} else if(temp->tokenID == 1002) {
        variableValueTracker[numOfVariablesInUse] = temp->tokenWord;
        //printf("R2 %s\n", variableValueTracker[numOfVariablesInUse].c_str());
        numOfVariablesInUse++;
        string local = "T" + std::to_string(place);
  			//place++;
				fprintf(file, "LOAD %s\n", local.c_str());

			}
		} else {
      semantics(root->child1);

		}
	}

	/* <stats> → <stat> <mStat> */
	if (root->name == "<stats>") {

    /* Call the child node(s) */
      semantics(root->child1);
      semantics(root->child2);
	}

	/* <mStat> → empty | <stat> <mStat> */
	if (root->name == "<mStat>") {

    /* Call the child node(s) */
      semantics(root->child1);
      semantics(root->child2);

	}

	/* <stat> → <in> | <out> | <block> | <if> | <loop> | <assign> */
	if(root->name == "<stat>") {
//printf(":::STAT:::\n");
    /* Call the child node(s) */
    semantics(root->child1);

	}

	/* <in> → read ( Identifier ) : */
	if(root->name == "<in>") {
//printf(":::IN:::\n");
		if(!root->TK.empty()) {
			temp = root->TK.front();
			root->TK.erase(root->TK.begin());

			stack stk;
			stk.tokenWord = temp->tokenWord;
			stk.lineNum = temp->line;
			stk.scope = scope;

      //printf("IN 1 : %s\n", variableValueTracker[numOfVariablesInUse].c_str());

      string local = "T" + std::to_string(place);
      numOfVariablesInUse++;
      //place++;

			fprintf(file, "READ %s\n", local.c_str());
			fprintf(file, "LOAD %s\n", local.c_str());
			fprintf(file, "STACKW %i\n", find(stk));
		}

	}

	/* <out> → print ( <expr> ) : */
	if(root->name == "<out>") {
//printf(":::OUT:::\n");
    string local = "T" + std::to_string(place);
    //place++;

    //printf("OUT 1 : %s\n", local.c_str());

    /* Call the child node(s) */
		semantics(root->child1);
		fprintf(file, "STORE %s\n", local.c_str());
		fprintf(file, "WRITE %s\n", local.c_str());

	}

	/* <if> → cond ( <expr> <RO> <expr> ) <block> */
	if(root->name == "<if>") {
//printf(":::IF:::\n");
		int thisLoop = ++loopCount;
		int local = place;
		//place++;

    /* Call the child node(s) */
		//fprintf(file, "L%i: \n", loopCount);
		/* Child 1 is <expr> */
		semantics(root->child1);
		fprintf(file, "STORE T%d\n", local);
		/* Child 2 is <RO> */
		semantics(root->child3);
  	fprintf(file, "SUB %d\n", local);
		/* Child 3 is <expr> */
		semantics(root->child2);
		/* Child 4 is <block> */
		semantics(root->child4);
		fprintf(file, "L%d: NOOP\n", thisLoop);


	}

	/* <loop> → iter ( <expr> <RO> <expr> ) <stat> */
	if(root->name == "<loop>") {
//printf(":::LOOP:::\n");
		int thisLoop = ++loopCount;
		int local = place;
		//place++;

    /* Call the child node(s) */
		fprintf(file, "L%i: \n", loopCount);
		/* Child 1 is <expr> */
		semantics(root->child1);
		fprintf(file, "STORE %i\n", local);
		/* Child 2 is <RO> */
		semantics(root->child3);
    fprintf(file, "SUB %i\n", local);
		/* Child 3 is <expr> */
		semantics(root->child2);
		/* Child 4 is <block> */
		semantics(root->child4);
		fprintf(file, "BR loop%i\n", thisLoop);

		fprintf(file, "L%i: NOOP\n", thisLoop);


	}

	/* <assign> → Identifier = <expr> : */
	if(root->name == "<assign>") {
//printf(":::ASSIGN:::\n");
    semantics(root->child1);

		if(!root->TK.empty()) {
			temp = root->TK.front();
			root->TK.erase(root->TK.begin());

			stack stk;
			stk.tokenWord = temp->tokenWord;
			stk.lineNum = temp->line;
			stk.scope = scope;

      //printf("ASSIGN 1 %s\n", temp->tokenWord.c_str());
      //printf("variable %s = %s\n", temp->tokenWord.c_str(), variableValueTracker[place - 1].c_str());
			fprintf(file, "STACKW %i\n", find(stk));

		}
	}

	/* <RO> → < | < = | > | > = | = = | = */
	if(root->name == "<RO>") {
//printf(":::RO:::\n");
		if(!root->TK.empty()) {
			temp = root->TK.front();
			root->TK.erase(root->TK.begin());

			if(temp->tokenWord == "<") {
				if(!root->TK.empty()) {
					token *temp2 = root->TK.front();
					root->TK.erase(root->TK.begin());

					if(temp2->tokenWord == "=") {
						fprintf(file, "BRZPOS L%d\n", loopCount);

					} /*else {
            fprintf(file, "BRPOS L%d\n", loopCount);
          }*/
				}

				fprintf(file, "BRZPOS L%d\n", loopCount);

			}

			if(temp->tokenWord == ">") {
				if(!root->TK.empty()) {
					token *temp2 = root->TK.front();
					root->TK.erase(root->TK.begin());

					if(temp2->tokenWord == "=") {
						fprintf(file, "BRNEG L%d\n", loopCount);

					} else {
            fprintf(file, "BRZNEG L%d\n", loopCount);
            place++;
          }
				}

				fprintf(file, "BRZNEG L%d\n", loopCount);
        place++;
			}

			if(temp->tokenWord == "=") {
				fprintf(file, "BRPOS L%d\n", loopCount);
				fprintf(file, "BRNEG L%d\n", loopCount);

			}
		}
	}
} 


/* Function to check if a variable was already decalred */
void check(token *TK) {
	stack stk;
	stk.tokenWord = TK->tokenWord;
	stk.lineNum = TK->line;
	stk.scope = scope;

	int result = searchStack(stk);

  /* If variable was already declared - error. Else add it to the stack */
	if(result > 0) {
		cout << "SEMANTICS ERROR : Variable " << stk.tokenWord << " has been previously declared on line " << result << "\n";
		exit(EXIT_FAILURE);
	} else {
		push(stk);
	}
}
