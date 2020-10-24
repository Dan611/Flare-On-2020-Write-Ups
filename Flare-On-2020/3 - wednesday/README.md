# Challenge 3 - wednesday

This challenge provides a fully fuctioning video game, along with the following prompt:
```txt
██╗    ██╗███████╗██████╗ ███╗   ██╗███████╗███████╗██████╗  █████╗ ██╗   ██╗
██║    ██║██╔════╝██╔══██╗████╗  ██║██╔════╝██╔════╝██╔══██╗██╔══██╗╚██╗ ██╔╝
██║ █╗ ██║█████╗  ██║  ██║██╔██╗ ██║█████╗  ███████╗██║  ██║███████║ ╚████╔╝ 
██║███╗██║██╔══╝  ██║  ██║██║╚██╗██║██╔══╝  ╚════██║██║  ██║██╔══██║  ╚██╔╝  
╚███╔███╔╝███████╗██████╔╝██║ ╚████║███████╗███████║██████╔╝██║  ██║   ██║   
 ╚══╝╚══╝ ╚══════╝╚═════╝ ╚═╝  ╚═══╝╚══════╝╚══════╝╚═════╝ ╚═╝  ╚═╝   ╚═╝   

                        --- BE THE WEDNESDAY ---

                                   S
                                   M
                                   T
                                  DUDE
                                   T
                                   F
                                   S

                --- Enable accelerated graphics in VM ---
                  --- Attach sound card device to VM ---
                    --- Only reverse mydude.exe ---
                       --- Enjoy it my dudes ---
```

After some initial testing of the game, we can determine that points are earned by ducking under "Sunday", "Monday", or "Tuesday" blocks, and jumping over "Thursday", "Friday", or "Saturday" blocks.

For reference, a Sunday and Monday block looks like the following:
![down_1.png](:/4a75334564914a2889fd1805173306ae)

Points increase for successful dodges, but you must actively play the game to progress. Lets check the assembly for the collision detection:
![29c779a0d785056f5ad075d254bf9d98.png](../_resources/c878ed1fcbe64ca8a858d3ff80c881e8.png)

Inside the second function listed, there is a branch where the code will either play a noise and terminate, or increase a counter:
![cd176077fe6ebb9ff5a32865bbd2015c.png](../_resources/eed3700b6f784f8b991cfe3f3666465d.png)

Let's try changing this code to only take the left branch:
![a11299762191c0f52904a31d3a83be29.png](../_resources/c15fccd6a3b6415780733f4534e8e15d.png)

If we try out the game now, we can dodge all blocks just by holding down. Applying a speedhack to make the game progress faster gives us the flag:

![dude.gif](../_resources/ab25c3e9f01a4654bd6df31c4211a4d7.gif)

Flag: `1t_i5_wEdn3sd4y_mY_Dud3s@flare-on.com`
