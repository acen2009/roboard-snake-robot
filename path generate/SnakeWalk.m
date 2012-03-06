function [x,y] = SnakeWalk(a,motorNum,wave)
%     a=0;%(-1：左轉，0：直走，1：右轉)
    rad=pi/180;
    gain=1;
    time=360;
    b=0;
    c=0;
    r=time*gain;
    preTime=0;
    afterTime=preTime;
    extraTime=(motorNum+1-2)*time/motorNum+afterTime;
    error=-0;

    
    if(a==-1)
        filename='L_';
    elseif(a==0)
        filename='M_';
    elseif(a==1)
        filename='R_';
    end
    for i=1:1:motorNum+1
        for t=1:1:time*gain+extraTime
            x(i,t)=0;
            y(i,t)=0;
        end
    end

    for i=1:1:motorNum+2
        for t=1:1:time*gain+extraTime+preTime+error
            if(i==1)
                if(t==1)
                    x(1,t)=0;
                    y(1,t)=a*((r^2-x(1,t)^2)^(1/2)-time*gain);
                elseif(t>time*gain)
                    x(1,t)=x(1,t-1);
                    y(1,t)=y(1,t-1);
                else
                    x(1,t)=x(1,t-1)+1;
                    y(1,t)=a*((r^2-x(1,t)^2)^(1/2)-time*gain);
                end
                phi(1,t)=atan2(y(1,t),x(1,t))*180/pi;
                position(1,t)=round((811-211)*(phi(1,t)+90)/(90-(-90))+211);
            elseif(mod(i,2)==0)%i 為奇數

                if(t>preTime)
                    xt=t-preTime;
                    yt=sin(xt*gain*rad)*wave;
                    x(i,t)=xt*cos(phi(1,t-preTime)*rad)+yt*sin(phi(1,t-preTime)*rad);%x=cos(phi(2,t)*rad)
                    y(i,t)=xt*sin(phi(1,t-preTime)*rad)-yt*cos(phi(1,t-preTime)*rad); %y=sin(phi(2,t)*rad)

                    if(t==1+preTime)
                        phi(i,t)=atan2(y(i,t),x(i,t))*180/pi-phi(1,t-preTime);
                    else
                        phi(i,t)=atan2((y(i,t)-y(i,t-1)),(x(i,t)-x(i,t-1)))*180/pi-phi(1,t);
                    end                   
                else
                        xt=1;
                        yt=sin(xt*gain*rad)*wave;
                        startX=xt*cos(phi(1,1)*rad)+yt*sin(phi(1,1)*rad);%x=cos(phi(2,t)*rad)
                        startY=xt*sin(phi(1,1)*rad)-yt*cos(phi(1,1)*rad); %y=sin(phi(2,t)*rad)
                        startPhi=atan2(startY,startX)*180/pi-phi(1,1);
                        phi(i,t)=t*startPhi/preTime;

                end
                position(i,t)=round((811-211)*(phi(i,t)+90)/(90-(-90))+211);
            else
                startIndex=(i-2)*time/motorNum;%起始位置

                %-sss----------OOO--------------------eee-------------------
                %1---startIndex---time*gain+startIndex---time*gain+extraTime
                if(t<=startIndex)
                    phi(i,t)=phi(2,time*gain+extraTime+preTime+error-startIndex+t);  
                else
                    phi(i,t)=phi(2,t-startIndex+preTime);
                end
                position(i,t)=round((811-211)*(phi(i,t)+90)/(90-(-90))+211);
            end
        end
    end

    
    for i=2:1:motorNum+2
        for t=1:1:time*gain+extraTime+preTime+error
            if(i==2||i==6||i==10)
                phi(i,t)=-1*phi(i,t)/5;
            elseif(i==4||i==8||i==12)
                phi(i,t)=phi(i,t)/5;
            end
            position(i,t)=round((811-211)*(phi(i,t)+90)/(90-(-90))+211);
        end
    end

    %%
    %輸出Angle.txt
%     filename='Angle'
    fid = fopen([filename 'Angle.txt'], 'w');
    for t=1:1:time*gain+extraTime+error
        for i=2:1:motorNum+2
            if(mod(i,2)==0)%i 為奇數
                fprintf(fid, '%10.6f ',phi(i,t));
            else
                fprintf(fid, '%10.6f ',phi(i,t));
            end
        end
        fprintf(fid, '\r\n');
    end
    fclose(fid);
    % dos('Angle.txt'); % 開啟 squareRootTable.txt


    %%
    %輸出Position.txt
    fid = fopen([filename 'Position.txt'], 'w');
    for t=1:1:time*gain+extraTime+error
        for i=2:1:motorNum+2
            if(mod(i,2)==0)%i 為奇數
                fprintf(fid, '%d ',position(i,t));
            else
                fprintf(fid, '%d ',position(i,t));
            end
        end
        fprintf(fid, '\r\n');
        tArray(t)=t;
    end
    fclose(fid);
    % dos('Position.txt'); % 開啟 squareRootTable.txt
    %%
    %輸出X_Y.txt
    fid = fopen([filename 'X_Y.txt'], 'w');
    for t=1:1:time*gain+extraTime
        for i=2:1:motorNum+1
            fprintf(fid, '%.2f %.2f ',x(i,t),y(i,t));
        end
        fprintf(fid, '\r\n');
        tArray(t)=t;
    end
    fclose(fid);
    % dos('Position.txt'); % 開啟 squareRootTable.txt

    %%
    xmin=-500;
    xmax=500;
    ymin=-500;
    ymax=500;
    %%顯示曲線圖
    hold on
    figure(3);
    plot(x(1,:),y(1,:),'b-.');
    % axis([xmin xmax ymin ymax])
    % % subplot(4,1,4);
    figure(1);
    plot(x(2,:),y(2,:),'black-.');
    % axis([xmin xmax ymin ymax])
    hold off
    % dos('Angle.txt'); % 開啟 squareRootTable.txt
    % plot(phi);
end
