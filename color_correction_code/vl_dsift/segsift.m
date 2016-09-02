function segsift(image1, image2, imgname)
str1 = 'H:\intrinsic_image_work_record\data\';

imagea = imread(image1);
imageb = imread(image2);
[imgh, imgw, channels] = size(imagea);

%分别得到路径，名称和后缀名
[pathstr1, name1, ext1] = fileparts(image1);
[pathstr2, name2, ext2] = fileparts(image2);
match_string = '\matches_';
resultname = '\result_';
txt = '.txt';
matchpath = [str1,imgname,match_string,name2,txt];
resultpath = [str1, imgname, resultname, name2, ext1];
fid_match = fopen(matchpath,'wt');
matchpoint = fopen('H:\intrinsic_image_work_record\data\Piano\matchpoint.txt','wt');

imga = single(rgb2gray(imagea));
imgb = single(rgb2gray(imageb));

binsize = 64;
height = floor(imgh / binsize);
width = floor(imgw / binsize);

img1x = [];
img1y = [];
img2x = [];
img2y = [];

for i = 1 : height
    for j = 1 : width
        bina = imga((i-1)*binsize+1:i*binsize, (j-1)*binsize+1:j*binsize);
        for k = 1 : width
            fprintf(matchpoint, '%d %d %d\n', i, j, k);
            
            binb = imgb((i-1)*binsize+1:i*binsize, (k-1)*binsize+1:k*binsize);
            
            [fa, da] = vl_sift(bina);
            [fb, db] = vl_sift(binb);
            
            [matches, scores] = vl_ubcmatch(da, db);
            [drop, perm] = sort(scores, 'descend'); %降序排列
            matches = matches(:, perm);%将matches按照score排个序
           
            [height, matchnum] = size(matches);
            fprintf(matchpoint, '%d\n', matchnum);
            
            if matchnum > 0 
                xa = ones(1, matchnum);
                ya = ones(1, matchnum);
                xb = ones(1, matchnum);
                yb = ones(1, matchnum);

                for m = 1: matchnum
                    xa(1,m) = fix( fa(1, matches(1,m))) + (j-1)*binsize;
                    ya(1,m) = fix( fa(2, matches(1,m))) + (i-1)*binsize;

                    xb(1,m) = fix( fb(1, matches(2,m))) + (k-1)*binsize;
                    yb(1,m) = fix( fb(2, matches(2,m))) + (i-1)*binsize;
                    fprintf(matchpoint, '%d %d %d %d\n', xa(1, m), ya(1, m), xb(1, m), yb(1, m));
                end

                img1x = [img1x xa];
                img1y = [img1y ya];
                img2x = [img2x xb];
                img2y = [img2y yb];    
            end
        end        
    end
end

temp = [img1x' img1y' img2x' img2y'];
temp = unique(temp, 'rows');
[matchnum, th] = size(temp);

%去掉不靠谱的点
k = 1;
for i = 1:matchnum
    if(abs(temp(i, 1)-temp(i, 3))<500)
        matchmax(k,:) = temp(i, :);
        k = k + 1;
    end
end

%显示匹配结果
img = [imagea, imageb];
figure;
imshow(img);
hold on
for i = 1:k-1
    plot([matchmax(i, 1), matchmax(i, 3) + imgw], [matchmax(i, 2), matchmax(i, 4)]);
end
saveas(gcf, resultpath);
hold off

%保存匹配点
fprintf(fid_match, '%d\n', k);
for i = 1:k-1
    fprintf(fid_match,'%d %d %d %d\n', matchmax(i, 1), matchmax(i, 2), matchmax(i, 3), matchmax(i, 4));
end
fclose(fid_match);
end

