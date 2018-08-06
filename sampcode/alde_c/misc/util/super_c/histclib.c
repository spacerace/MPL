/*              Histogram Library; C portion.
 */

/*      prHist(hist)

        Function: Given a pointer to a table of 1025 histogram counters,
        print out the address that each counter covers, the count for that
        counter, and the percentage of the total both with and without
        the "other" counter included. Don't print counters that are zero.

        Algorithm: Add up all the counters. Print out the heading. Then
        cycle through all the counters, printing out the information.
*/

prHist(hist)

long *hist;

{
        float total, totalWo;
        long *hptr;
        unsigned addr;

        /* Alert the user that we're done running, and are now computing
           the histogram. */
        printf("Program done. Compiling histogram data.\n\n");
        /* Add up all the counters. */
        for (total = 0, hptr = hist+1; hptr < hist+1025; total += *hptr++);
        /* Compute the total without the "other" category. */
        totalWo = total + ((float) *hist);
        /* Print the total count without "other." */
        printf("Total recorded hits: %15.0f.\n\n",totalWo);
        /* Print the header for the main list. */
        printf(" Address     Count    %% with Other  %% without Other\n");
        printf("---------  ---------  ------------  ---------------\n");
        /* Print the "other" counter. */
        printf("  Other    %9ld      %7.3f%%\n",*hist,
               100.0*(((float) *hist)/total));
        /* Loop through all of the counters... */
        for (hptr = hist+1, addr = 0; hptr < hist+1025; hptr++, addr += 64)
                /* If it isn't zero, print it. */
                if (*hptr != 0L)
                        printf("%4x-%4x  %9ld      %7.3f%%         %7.3f%%\n",
                               addr,addr+63,*hptr,
                               100.0*(((float) *hptr)/totalWo),
                               100.0*(((float) *hptr)/total));
}

