/*              Pong Game Main Program
 */

/* Copying modes for rect: */

#define COPY 0
#define XOR 1
#define BITSET 2
#define BITCLEAR 3
#define blankPattern 0

/* Gray pattern for the wall: */
wallPattern[] = {
    0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555,
    0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555, 0xAAAA, 0x5555
};

/* Diamond pattern for the ball: */
ballPattern[] = {
    0x0180, 0x03C0, 0x07E0, 0x0FF0, 0x1FF8, 0x3FFC, 0x7FFE, 0xFFFF,
    0xFFFF, 0x7FFE, 0x3FFC, 0x1FF8, 0x0FF0, 0x07E0, 0x03C0, 0x0180
};

#define FALSE 0
#define TRUE 1

/*      main()

        Function: Play pong. Simulate a ball moving around the display and
        bouncing off the walls and paddle. If the users hits the up or
        down arrow keys, move the paddle.

        Algorithm: Initialize the display and draw the walls, paddle, and
        ball. Then iterate checking the keyboard for up/down arrow hits,
        and simulating the moving ball.
*/

main()

{
        int ballX, ballY;       /* Current location of ball. */
        int newX, newY;         /* Next ball location. */
        int ballXV, ballYV;     /* Current velocity of ball. */
        int padHeight;          /* Paddle height. */
        int padY;               /* Current paddle location. */
        int ch;                 /* Input character from user. */
        int goodHits;           /* Number of successfull rebounds. */

        /* Set the display to graphics mode. */
        setMod(6);

        /* Set the initial positions and velocities of everything. */
        ballX = 431;
        ballY = 97;
        ballXV = ballYV = 1;
        padHeight = 60;
        padY = 10;
        goodHits = 0;

        /* Draw the wall, paddle, and ball. */
        rect(0,0,640,200,blankPattern,1,COPY);          /* Clear screen. */
        rect(0,0,640,10,wallPattern,1,COPY);            /* Top wall. */
        rect(0,190,640,200,wallPattern,1,COPY);         /* Bottom wall. */
        rect(630,10,640,190,wallPattern,1,COPY);        /* Right wall. */
        rect(10,padY,20,padY+padHeight,0,1,XOR);        /* Paddle. */
        rect(ballX,ballY,ballX+16,ballY+16,ballPattern,0,XOR); /* Ball. */

        /* Loop, simulating movement and waiting for commands from the user. */
        while (TRUE) {
                /* Check if there's a keypress waiting. */
                if (keyChk(&ch)) {
                        /* If so, read it from the keystroke queue. */
                        keyRd();
                        /* If it was an ESC, exit the main loop. */
                        if ((ch & 0xFF) == 0x1B) break;
                        /* If it was an extended key, switch on that key. */
                        if ((ch & 0xFF) == 0)
                                switch ((ch>>8) & 0xFF) {
                                        /* Up arrow: move paddle up. */
                                        case 72:
                                                /* Check for at top. */
                                                if (padY <= 20) break;
                                                /* Draw new paddle. */
                                                rect(10,padY-20,20,
                                                     padY+padHeight-20,0,1,1);
                                                /* Erase old paddle. */
                                                rect(10,padY,20,
                                                     padY+padHeight,0,1,1);
                                                /* Update position. */
                                                padY -= 20;
                                                break;
                                        /* Down arrow: move paddle down. */
                                        case 80:
                                                /* Check for at bottom. */
                                                if ((padY+padHeight) >= 180) 
                                                        break;
                                                /* Draw new paddle. */
                                                rect(10,padY+20,20,
                                                     padY+padHeight+20,0,1,1);
                                                /* Erase old paddle. */
                                                rect(10,padY,20,
                                                     padY+padHeight,0,1,1);
                                                /* Update position. */
                                                padY += 20;
                                                break;
                                };
                };
                /* If the ball is still on the display... */
                if (ballX > -16) {
                        /* Compute where the ball will go next. */
                        newX = ballX+ballXV;
                        newY = ballY+ballYV;
                        /* Check and adjust for running into top or bottom. */
                        if ((newY < 10) || (newY > 174)) {
                                ballYV = -ballYV;
                                newY = ballY+ballYV;
                        };
                        /* Check and adjust for running into the right wall. */
                        if (newX > 614) {
                                ballXV = -ballXV;
                                newX = ballX+ballXV;
                        /* Check and adjust for running into the paddle. */
                        } else if ((padY <= (ballY+15)) &&
                                   ((padY+padHeight) >= ballY) &&
                                   (ballX >= 20) &&
                                   (newX < 20)) {
                                /* Reverse direction. */
                                ballXV = -ballXV;
                                newX = ballX+ballXV;
                                /* Check for edge of paddle hit. */
                                if (ballY < padY) {
                                        if (--ballYV == 0) ballYV--;
                                } else if ((ballY+15) > (padY+padHeight)) {
                                        if (++ballYV == 0) ballYV++;
                                };
                                /* If he's gotten five, speed it up. */
                                if (((++goodHits) % 5) == 0) {
                                        if (ballXV > 0) ballXV++;
                                        else ballXV--;
                                        if (ballYV > 0) ballYV++;
                                        else ballYV--;
                                };
                        };
                        /* Draw the new ball. */
                        rect(newX,newY,newX+16,newY+16,ballPattern,0,XOR);
                        /* Erase the old ball. */
                        rect(ballX,ballY,ballX+16,ballY+16,ballPattern,0,XOR);
                        /* Update the ball position. */
                        ballX = newX; ballY = newY;
                /* Otherwise, the ball is off the screen -- he missed. */
                } else {
                        /* Put the ball ack on the screen, and reset hits. */
                        ballX = 431;
                        ballY = 97;
                        ballXV = ballYV = 1;
                        goodHits = 0;
                        /* Draw the ball. */
                        rect(ballX,ballY,ballX+16,ballY+16,ballPattern,0,1);
                };
        };

        /* Reset the display mode to text. */
        setMod(2);
}

