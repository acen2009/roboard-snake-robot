
clear all;
clc;
clf;
hold on;
motorNum=10;
wave=45;
[x1,y1] = SnakeWalk(-1,motorNum,wave);
[x2,y2] = SnakeWalk(0,motorNum,wave);
[x3,y3] = SnakeWalk(1,motorNum,wave);