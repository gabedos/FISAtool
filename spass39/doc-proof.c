/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *                 PROOF DOCUMENTATION                    * */
/* *                                                        * */
/* *  $Module:   DOCPROOF                                   * */ 
/* *                                                        * */
/* *  Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001      * */
/* *  MPI fuer Informatik                                   * */
/* *                                                        * */
/* *  This program is free software; you can redistribute   * */
/* *  it and/or modify it under the terms of the FreeBSD    * */
/* *  Licence.                                              * */
/* *                                                        * */
/* *  This program is distributed in the hope that it will  * */
/* *  be useful, but WITHOUT ANY WARRANTY; without even     * */
/* *  the implied warranty of MERCHANTABILITY or FITNESS    * */
/* *  FOR A PARTICULAR PURPOSE.  See the LICENCE file       * */
/* *  for more details.                                     * */
/* *                                                        * */
/* *                                                        * */
/* $Revision: 1.9 $                                         * */
/* $State: Exp $                                            * */
/* $Date: 2011-11-27 11:47:42 $                             * */
/* $Author: weidenb $                                       * */
/* *                                                        * */
/* *             Contact:                                   * */
/* *             Christoph Weidenbach                       * */
/* *             MPI fuer Informatik                        * */
/* *             Stuhlsatzenhausweg 85                      * */
/* *             66123 Saarbruecken                         * */
/* *             Email: spass@mpi-inf.mpg.de                * */
/* *             Germany                                    * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/


/* $RCSfile: doc-proof.c,v $ */

/**************************************************************/
/* Includes                                                   */
/**************************************************************/

#include "doc-proof.h"

/*******************************************************
*Previously inlined functions                          *  
********************************************************/
int dp_ProofDepth(void)
{
  return dp_DEPTH;
}

void dp_SetProofDepth(int Depth)
{
  dp_DEPTH = Depth;
}



/*******************************************************
*Previously inlined functions ends here                *  
********************************************************/


/**************************************************************/
/* Global Variables                                           */
/**************************************************************/

int dp_DEPTH;


/**************************************************************/
/* Functions                                                  */
/**************************************************************/

void dp_Init(void)
{
  dp_DEPTH = 0;
}


static void dp_FPrintDFGProof(LIST Clauses, const char *FilePrefix,
			      FLAGSTORE Flags, PRECEDENCE Precedence, HASHMAP ClauseToTermLabelList)
/*********************************************************
  INPUT:   A list of clauses representing a proof, a
           string indicating a file name prefix, a flag
	   store and a precedence.
  RETURNS: void.
  EFFECT:  Outputs the proof in DFG proof format to
           <FilePrefix>.prf
**********************************************************/
{
  FILE   *Output;
  CLAUSE Clause;
  LIST   AxClauses,ConClauses,ProofClauses,Scan;
  char   *name;

  AxClauses = ConClauses = ProofClauses = list_Nil();
  
  name = memory_Malloc(sizeof(char)*(strlen(FilePrefix)+5));
  sprintf(name,"%s.prf", FilePrefix);

  Output = misc_OpenFile(name,"w");

  fputs("begin_problem(Unknown).\n\n", Output);

  fputs("list_of_descriptions.\n", Output);
  fputs("name({*", Output);
  fputs(FilePrefix, Output);
  fputs("*}).\n", Output);
  fputs("author({*SPASS ", Output);
  fputs(vrs_VERSION, Output);
  fputs("*}).\n", Output);
  fputs("status(unsatisfiable).\n", Output);
  fputs("description({*File generated by SPASS containing a proof.*}).\n", Output);
  fputs("end_of_list.\n\n", Output);

  fputs("list_of_symbols.\n", Output);
  symbol_FPrintDFGSignature(Output);
  fputs("end_of_list.\n\n", Output);

  for (Scan=Clauses;!list_Empty(Scan);Scan=list_Cdr(Scan)) {
    Clause = (CLAUSE)list_Car(Scan);
    if (clause_IsFromInput(Clause)) {
      if (clause_GetFlag(Clause, CONCLAUSE))
	ConClauses = list_Cons(Clause, ConClauses);
      else
	AxClauses  = list_Cons(Clause, AxClauses);
    }
    else
      ProofClauses  = list_Cons(Clause, ProofClauses);
  }

  ConClauses   = list_NReverse(ConClauses);
  AxClauses    = list_NReverse(AxClauses);
  ProofClauses = list_NReverse(ProofClauses);
  
  clause_FPrintCnfDFGProof(Output, FALSE, AxClauses, ConClauses, Flags, Precedence);
  fputs("\nlist_of_proof(SPASS).\n", Output);
  for (Scan=ProofClauses; !list_Empty(Scan); Scan=list_Cdr(Scan)) {
    clause_FPrintDFGStep(Output,list_Car(Scan),TRUE);
  }
  fputs("end_of_list.\n\n", Output);

  clause_FPrintSettings(Output, AxClauses, ConClauses, Flags, Precedence, ClauseToTermLabelList);

  fputs("end_problem.\n\n", Output);

  misc_CloseFile(Output, name);
  fputs("\nDFG Proof printed to: ", stdout);
  puts(name);

  list_Delete(ConClauses);
  list_Delete(AxClauses);
  list_Delete(ProofClauses);
  memory_Free(name, sizeof(char)*(strlen(FilePrefix)+5));
}

LIST dp_PrintProof(PROOFSEARCH Search, LIST Clauses, const char *FilePrefix, HASHMAP  ClauseToTermLabelList)
/*********************************************************
  INPUT:   A proofsearch object, a list of empty clauses and
           the prefix of the output file name.
  RETURNS: The list of clauses required for the proof.
  MEMORY:  The returned list must be freed.
  EFFECT:  The proof is printed both to standard output and
           to the file <FilePrefix>.prf.
**********************************************************/
{
  LIST ProofClauses,Scan,EmptyClauses,AllClauses, ReducedProof;
  LIST Missing, Incomplete, SplitClauses;

  FLAGSTORE Flags;

  Flags = prfs_Store(Search);

  Missing = pcheck_ConvertParentsInSPASSProof(Search, Clauses);
  
  if (!list_Empty(Missing)) {
    puts("\nNOTE: clauses with following numbers have not been found:");
    for (; !list_Empty(Missing); Missing = list_Pop(Missing))
      printf("%zd ", (intptr_t)list_Car(Missing)); 
    putchar('\n');
  }

  EmptyClauses = list_Copy(Clauses); 
  ProofClauses = list_Nil();
  AllClauses   = list_Nconc(list_Copy(prfs_DocProofClauses(Search)),
			    list_Nconc(list_Copy(prfs_UsableClauses(Search)),
				       list_Copy(prfs_WorkedOffClauses(Search))));

  /*
   *  collect proof clauses by noodling upward in the 
   *  proof tree, starting from <EmptyClauses>.
   *  Before, add all splitting clauses to avoid gaps in split tree 
   */

 /*  printf("\n Empty Clauses: "); */
/*   for (Scan = EmptyClauses; !list_Empty(Scan); Scan = list_Cdr(Scan)) { */
/*     printf("\n"); */
/*     clause_PrintSpecial(list_Car(Scan)); */
/*   } */
/*   for (Scan = prfs_DocProofClauses(Search); !list_Empty(Scan); Scan = list_Cdr(Scan))  */
/*     if (clause_IsEmptyClause(list_Car(Scan))) { */
/*       printf("\n"); */
/*       clause_PrintSpecial(list_Car(Scan)); */
/*     } */
    

  SplitClauses = list_Nil();
  for (Scan = AllClauses; !list_Empty(Scan); Scan = list_Cdr(Scan)) 
    if (clause_IsFromSplitting(list_Car(Scan))) 
      SplitClauses = list_Cons(list_Car(Scan), SplitClauses);

  /* mark all needed clauses */
  pcheck_ClauseListRemoveFlag(EmptyClauses, MARKED);
  pcheck_ClauseListRemoveFlag(AllClauses, MARKED);
  pcheck_MarkRecursive(EmptyClauses);
  pcheck_MarkRecursive(SplitClauses);
  
  /* collect all marked clauses */
  ProofClauses = list_Nil();
/*   printf("\n\n All Clauses:"); */
  for (Scan = AllClauses; !list_Empty(Scan); Scan = list_Cdr(Scan)) {
/*     printf("\n");clause_PrintSpecial(list_Car(Scan)); */
    if (clause_GetFlag(list_Car(Scan), MARKED))
      ProofClauses = list_Cons(list_Car(Scan), ProofClauses); 
  }

  /* build reduced proof  */
  ProofClauses = list_Nconc(ProofClauses, list_Copy(EmptyClauses));
  ProofClauses = pcheck_ClauseNumberMergeSort(ProofClauses);
  ReducedProof = pcheck_ReduceSPASSProof(ProofClauses); 

  dp_SetProofDepth(pcheck_SeqProofDepth(ReducedProof));
  
  pcheck_ParentPointersToParentNumbers(AllClauses);
  pcheck_ParentPointersToParentNumbers(Clauses);

  /* check reduced proof for clauses whose parents have been marked as
     incomplete (HIDDEN flag) by ConvertParentsInSPASSProof    */

  Incomplete = list_Nil();
  for (Scan = ReducedProof; !list_Empty(Scan); Scan = list_Cdr(Scan)) {
    if (clause_GetFlag(list_Car(Scan), HIDDEN))
      Incomplete = list_Cons(list_Car(Scan), Incomplete);
  }
  if (!list_Empty(Incomplete)) {
    puts("NOTE: Following clauses in reduced proof have incomplete parent sets:");
    for (Scan = Incomplete; !list_Empty(Scan); Scan = list_Cdr(Scan))
      printf("%zd", clause_Number(list_Car(Scan)));
    putchar('\n');
  }

  if (flag_GetFlagIntValue(Flags, flag_TPTP) == flag_TPTPALL) {
    printf("\n Geoff Proof ");
  }
  else {
    printf("\n\nHere is a proof with depth %d, length %zd :\n",
	   dp_ProofDepth(), list_Length(ReducedProof));
    clause_ListPrint(ReducedProof);

    if (flag_GetFlagIntValue(Flags, flag_FPDFGPROOF))
      dp_FPrintDFGProof(ReducedProof, FilePrefix, Flags, prfs_Precedence(Search), ClauseToTermLabelList);
  }

  fflush(stdout);

  list_Delete(EmptyClauses);
  list_Delete(AllClauses);
  list_Delete(ProofClauses);
  list_Delete(SplitClauses);
  list_Delete(Incomplete); 

  return ReducedProof;
}



