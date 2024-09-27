I = imread(['example.png']);

I = imgaussfilt(I, 3);
I = imgaussfilt(I, 3);

star_x = 275:375;
star_y = 550:650;
background = min(min(I))
power = 6;

COG_x = 0;
COG_y = 0;
I_sum = 0;

for i = star_x
    for j = star_y
        COG_x = COG_x + i * double(I(i,j) - background)^power;
        COG_y = COG_y + j * double(I(i,j) - background)^power;
        I_sum = I_sum + double(I(i,j) - background)^power;
    end
end

COG_x = COG_x / I_sum
COG_y = COG_y / I_sum


I = I(star_x,star_y);
x = 0:size(I,2)-1;
y = 0:size(I,1)-1;

[X,Y] = meshgrid(x,y);                          % Coordinate Matrices (Not Necessary)
figure(1)
meshc(X, Y, I)                              % Mesh Plot
grid on
xlabel('X')
ylabel('Y')
zlabel('Intensity')
colormap(jet)      