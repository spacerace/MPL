/*
 * Copyright (c) 1978 Charles H. Forsyth
 *
 * Modified 02-Dec-80 Bob Denny -- Conditionalize debug code for reduced size
 * Modified 29-May-81 Bob Denny -- Clean up overlay stuff for RSX.
 * More     19-Mar-82 Bob Denny -- New C library & compiler
 * More     03-May-82 Bob Denny -- Final touches, remove unreferenced autos
 * More     29-Aug-82 Bob Denny -- Clean up -d printouts
 * More     29-Aug-82 Bob Denny -- Reformat for readability and comment
 *                                 while learning about LEX.
 * More		20-Nov-83 Scott Guthery -- Adapt for IBM PC & DeSmet C
 */

/*
 *    *********
 *    * DFA.C *
 *    *********
 *
 * LEX -- DFA construction routines
 */

#include <stdio.h>
#include "lexlex.h"

extern struct set *eclosure();          /* Used only by DFA */
extern struct dfa *defalt();            /* Used only by DFA */
extern struct move *stbase();           /* Used only by DFA */

/*
 * Build the DFA from the NFA
 */
dfabuild()
   {
   struct trans *trp;
   struct nfa **vec, **tp, *np, *temp[MAXNFA+1];
   int a, i;
   struct set **sp, *stack[MAXDFA], **spp, *xp;  /* DFA set stack */
   struct dfa *state;                  /* --> current DFA state */
   struct xset *xs, *xse;

  /*
   * Simulate an epsilon transition from nfa state 0 to
   * the initial states of the machines for each
   * translation.
   */
   nfa[0].n_char = EPSILON;            /* Set NFA state 0 transition EPSILON */
   /*
    * Allocate a state vector, each node of which will
    * point to an NFA starting state.  Each translation
    * generates an NFA, so the number of translations
    * equals the number of NFA start states.
    */
   vec = lalloc(i = (transp-trans)+1, sizeof(*vec), "dfabuild");
   /*
    * Fill in the start state vector
    */
   vec[0] = nfa;                       /* vec[0] always --> NFA state 0 */
   for (a = 1, trp = trans; trp < transp; trp++)  /* For each translation */
      vec[a++] = trp->t_start;         /* Pick up the NFA start state */
   /*
    * Now build the set sp --> e-CLOSURE(vec)
    */
   sp = eclosure(newset(vec, i, 1));
   free(vec);                          /* Deallocate the start state vector */

   /*
    * At this point state 0 of the DFA is constructed.
    * This is the start state of the DFA.
    * Mark it "added" and push it on to the stack.
    */
   sp->s_flag |= ADDED;
   spp = stack;
   *spp++ = sp;
   /*
    * From state 0, which is now stacked, all further DFA
    * states will be derived.
    */
   while (spp > stack)
      {
      sp = *--spp;
      for (a = 0; a < NCHARS; a++)
         insets[a] = 0;
      xse = sets;
      for (i = 0; i < sp->s_len; i++)
         xse = addset(sp->s_els[i], xse);
      state = newdfa();
      sp->s_state = state;
      state->df_name = sp;
#ifdef DEBUG
      if (lldebug)
         {
         fprintf(lexlog, "build state %d ", state-dfa);
         pset(sp, 1);
         fprintf(lexlog, "\n");
         }
#endif
      state->df_ntrans = xse-sets;
      for (xs = sets; xs < xse; xs++)
         {
         a = xs->x_char&0377;
         tp = temp;
         for (i = 0; i < sp->s_len; i++)
            if ((np = sp->s_els[i])->n_char==a ||
                np->n_char==CCL &&
                np->n_ccl[a/NBPC]&(1<<(a%NBPC)))
               add(temp, &tp, np->n_succ[0]);
         xp = newset(temp, tp-temp, 1);
         xp = eclosure(xp);
#ifdef DEBUG
         if (lldebug)
            {
            putc('\t', lexlog);
            chprint(a);
            putc('\t', lexlog);
            pset(xp, 1);
            fprintf(lexlog, "\n");
            }
#endif
         xs->x_set = xp;
         if (xp->s_state==0 && (xp->s_flag&ADDED)==0)
            {
            xp->s_flag |= ADDED;
            if (spp >= stack+MAXDFA)
               {
               error("dfabuild: stack overflow");
               exit(1);
               }
            *spp++ = xp;
            }
         }
      state->df_default = defalt(state, &xse);
      setbase(state, stbase(xse), xse);
      }
   }

/*
 * If an nfa state is not already a member of the vector `base', add it.
 */
add(base, tpp, np)
struct nfa ***tpp, **base, *np;
   {
   register struct nfa **tp;

   for (tp = base; tp < *tpp; tp++)
      if (*tp == np)
         return;
   *(*tpp)++ = np;
   }

/*
 * Add the character(s) on which state `np' branches to the transition vector.
 */
addset(np, xse)
register struct nfa *np;
struct xset *xse;
   {
   register a;
   register char *ccl;

   if ((a = np->n_char) < NCHARS)
      xse = addxset(a, xse);
   if (a != CCL)
      return(xse);
   ccl = np->n_ccl;
   for (a = 0; a < NCHARS; a++)
      if (ccl[a/NBPC]&(1<<(a%NBPC)))
         xse = addxset(a, xse);
   return(xse);
   }

/*
 * Add a character to the transition vector, if it isn't there already.
 */
addxset(a, xse)
register a;
struct xset *xse;
   {
   register struct xset *xs;
   register int temp;
   /*
   * VMS native doesn't do this correctly:
   *    if (insets[a]++)
   */
   temp = insets[a];
   insets[a] += 1;
   if (temp)
      return(xse);
   xs = xse++;
   xs->x_char = a;
   xs->x_set = NULL;
   xs->x_defsame = 0;
   return(xse);
   }
