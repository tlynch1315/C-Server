Project 02 - README
===================

Members
-------
- Tommy Lynch(tlynch2@nd.edu)
- Zac Cendejas(zcendeja@nd.edu)
- Will Fritz(wfritz@nd.edu)

Summary
-------
1.    This project proved to take a lot of time between all memebers of the group. Zac completed the entiriety of Thor by himself (thanks Zac you're the best). After this part was comleted, all of spidey was evenly distributed among the members of the team. Each person wrote certain initial files, but after we got everything to compile, each person manipulated little parts of every file in an effort to combat bugs. We did this by each working in our own repository and if one member fixed a bug, he would push it to the master and the other two would pull the changes in. This worked smoothly due to the fact that we were all with eachother during the completion of the entire project. 

    At first we had a lot of porblems just compiling spidey. We eventually fixed these issues and got all of the helper files to compile with little to no warnings. Hwoever, this did not produce a the server we intended to create. The majority of time spent on this project was spent trying to get features to work correctly. This part of the process went through a lot of trial and error combined with researching info on the related topics. Our largest bug was that when hovering over one of the directories without clinking for a short period of time, the server would produce NULL info for a lot of the request variables. This often caused the program to exit. However, if the program was set to forking mode, the server would just continue to run whenever this happened. Also, if the porgram was run through the student machinces it often di not kill the entire server. However, this was still a very large bug we needed to fix.

Latency
-------
2. Describe how your group went about measuring the average latency of the different types of requests. You should have a markdown table produced by your experiments that you analyze and explain.
~ We measured the latency by creating a shell script that ran the thor.py script with a thousand requests for the main spidey server and each of the three folders in it. The script then outputted the final line of thor.py that displays the average time of the thousand requests. We did this three times running 1 core, 2 cores, and 4 cores respectively. We also used the unix time command so that the time it took to process a thousand requests for each use of cores was also outputted. For the first group we ran spidey on student00 without forking and ran the shell script from Zac's personal computer. We then did the tests again using forking.

Latency of Single

| Cores | Time to process 1000 (Homepage) | Average of Homepage | Average of HTML | Average of Scripts | Average of Text |
|-------|---------------------------------|---------------------|-----------------|--------------------|-----------------|
|   1   |            8.203s               |       .00801s       |     .00895s     |       .01094s      |     .01228s     |
|   2   |            9.396s               |       .00927s       |     .01618s     |       .01144s      |     .01100s     |
|   4   |           13.405s               |       .01318s       |     .01290s     |       .01289s      |     .01285s     |

Latency of Forking

| Cores | Time to process 1000 (Homepage) | Average of Homepage | Average of HTML | Average of Scripts | Average of Text |
|-------|---------------------------------|---------------------|-----------------|--------------------|-----------------|
|   1   |        8.194s                   |       .00801s       |     .00991s     |       .00887s      |     .00849s     |
|   2   |       11.694s                   |       .01117s       |     .01276s     |       .01043s      |     .01165s     |
|   4   |       16.919s                   |       .01460s       |     .01264s     |       .01316s      |     .01438s     |

From this we can see that forking produces a faster response than handling one request a time. Also based on this output using multiple cores causes the process to take longer. This makes since for single mode because the server can only handle one request a time so the time spent splitting the requests is wasted. However you should expect using multiple cores to produce a faster result for the forking mode. That isn't true for a thousand requests because a single core is still the fastest. However we guessed that this is just because a thousand isn't a large enough sample size to have an impact. We ran the script again with ten thousand requests and that caused for four cores to be the fastest performer, although it took over a minute to run each stage of the script overall. 

Throughput
----------

Analysis
--------
See comments following respective tables

Conclusion
----------

Something that we all learned from doing the project in general is exactly how a web server works. The interactions with the client and the server is something we use everyday but up until now, it was not appreciated. There is a lot that goes into hosting a page due to how frequent requests are usually made. Also, I think we all learned how to debug a little better. THe project forced us to really dive into the code and place debugging statements at strategic locations. This is a skill we all know will be valuable in the future as we continue to develop and work on code bases.

Contributions
-------------
Even contribution over every file as a lot of the initial files were changed. We were all with eachother during the entirety of the project discussing each aspect of the code as will tried to fix bugs.
