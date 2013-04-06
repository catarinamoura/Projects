A= LOAD 'user-links-small.txt.gz' AS(user_a:int,user_b:int);
B= GROUP A BY user_a;
C= FOREACH B GENERATE COUNT(A) AS count;
D= GROUP C BY count;
E= FOREACH D GENERATE COUNT(C) AS friend,group AS user;
F= ORDER E BY friend ASC;
STORE F INTO 'q1_out';


 
