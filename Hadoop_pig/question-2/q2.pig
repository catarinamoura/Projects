Q1= LOAD 'user-links-small.txt.gz' AS (user_a_q1:int,user_b_q1:int);
Q2= LOAD 'user-wall-small.txt.gz' AS (user_a_q2:int,user_b_q2:int,time:int);
A= COGROUP Q1 BY user_a_q1,Q2 BY user_a_q2;
B= FOREACH A GENERATE group AS user, COUNT(Q1) AS friends, COUNT(Q2) AS posts;
C= GROUP B BY friends;
D= FOREACH C GENERATE COUNT(B) AS number_of_users,group AS friends, $1.posts AS posts; 
E= FOREACH D GENERATE friends,AVG(posts);
F = ORDER E BY friends ASC;
STORE F INTO 'q2_out';

