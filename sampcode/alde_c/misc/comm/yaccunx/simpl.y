

































































%term ID END
%left '+'
%left '*'
%left '('

%%
exp:      exp '+' term          = { 1; }
        | term                  = { 2; }

term:     term '*' fact         = { 3; }
        | fact                  = { 4; }

fact:     '(' exp ')'           = { 5; }
        | ID                    = { 6; }

%%
 