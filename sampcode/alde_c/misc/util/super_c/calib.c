/*                      Processor Speed Calibration Function
 */

/*      calib()

        Function: Return the speed of this processor, as a percent of
        the speed of a standard IBM-PC.

        Algorithm: Iterate for one system timer tic (as found in low
        memory location 46C). Multiply the result by 100 (conver to %)
        add in 50 (so the next divide will round rather than truncate),
        and divide by the number of iterations on a normal PC. Do all
        the calculations in long format to avoid overflow.
*/

calib()

{
        /* Pointer to system timer in low memory: */
        unsigned far *timerLow = {(unsigned far *) 0x46C};
        unsigned lastTime;      /* Last value read from *timerLow. */
        unsigned iter;          /* Iteration count. */

        /* Wait until timer has just ticked. */
        for (lastTime = *timerLow; lastTime == *timerLow;);
        /* Count iterations until the next tick. */
        for (iter = 0, lastTime = *timerLow; lastTime == *timerLow; iter++);
        /* Calculate and return the percent of a standard IBM-PC. */
        return ((100L*((long) iter) + 50L)/1878L);
}

