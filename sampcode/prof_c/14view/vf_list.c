/*
 *	vf_list -- linked list management functions
 */

#include <stdio.h>
#include <malloc.h>
#include "vf.h"

/*
 *	vf_mklst -- create a new list by allocating a node,
 *	making it point to itself, and setting its values
 *	to zero (appropriately cast)
 */

DNODE *
vf_mklst()
{
	DNODE *new;

	new = (DNODE *)malloc(sizeof (DNODE));
	if (new != NULL) {
		new->d_next = new;
		new->d_prev = new;
		new->d_lnum = new->d_flags = 0;
		new->d_line = (char *)NULL;
	}
	return (new);
} /* end vf_mklst() */


/*
 *	vf_alloc -- create a pool of available nodes
 */

DNODE *
vf_alloc(n)
int n;
{
	register DNODE *new;
	register DNODE *tmp;

	/* allocate a block of n nodes */
	new = (DNODE *)malloc(n * sizeof (DNODE));

	/* if allocation OK, string the nodes in one direction */
	if (new != NULL) {
		for (tmp = new; 1 + tmp - new < n; tmp = tmp->d_next)
			tmp->d_next = tmp + 1;
		tmp->d_next = (DNODE *)NULL;
	}

	return (new);	/* pointer to free list */
} /* end vf_alloc() */


/*
 *	vf_ins -- insert a node into a list after the specified node
 */

DNODE *
vf_ins(node, avail)
DNODE *node, *avail;
{
	DNODE *tmp;
	DNODE *vf_alloc(int);

	/*
	 *  check freelist -- get another block of nodes
	 *  if the list is almost empty
	 */
	if (avail->d_next == NULL)
		if ((avail->d_next = vf_alloc(N_NODES)) == NULL)
			/* not enough memory */
			return (DNODE *)NULL;

	/* get a node from the freelist */
	tmp = avail;
	avail = avail->d_next;

	/* insert the node into the list after node */
	tmp->d_prev = node;
	tmp->d_next = node->d_next;
	node->d_next->d_prev = tmp;
	node->d_next = tmp;

	/* point to next node in the freelist */
	return (avail);
} /* end vf_ins() */


/*
 *	vf_del -- delete a node from a list
 */

DNODE *
vf_del(node, avail)
DNODE *node, *avail;
{
	/* unlink the node from the list */
	node->d_prev->d_next = node->d_next;
	node->d_next->d_prev = node->d_prev;

	/* return the deleted node to the freelist */
	node->d_next = avail;
	avail = node;

	/* point to the new freelist node */
	return (avail);
} /* end vf_del() */
